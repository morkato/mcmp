#ifdef __unix__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>

#include <monarch/util/bitmap.h>
#include <monarch/protocol.h>
#include <monarch/statos.h>
#include <monarch/log.h>

#include "monarch/server/server.h"

#ifndef MONARCH_MAX_EVENTS_EPOLL
  #define MONARCH_MAX_EVENTS_EPOLL 8UL
#endif /** MONARCH_MAX_EVENTS_EPOLL  */

struct McmchServer {
  struct sockaddr_in  server_addr4;
  struct sockaddr_in6 server_addr6;
  int                 listen4fd;
  int                 listen6fd;
  int                 epollfd;
  struct epoll_event  event;
  struct epoll_event  events[MONARCH_MAX_EVENTS_EPOLL];
};

struct McmchProtoCryptoInfo {
  unsigned char shared[crypto_box_BEFORENMBYTES];
  uint64_t      sv_nonce;
  uint64_t      nonce;
};

// McmchClient mcmchServerAcceptConnection(McmchServer server, McmchBroadcast broadcast) {
//   struct sockaddr_in addr;
//   McmchClient algnc = NULL;
//   int clientfd = accept(server->listenfd, (struct sockaddr*)&(addr), &(server->socklen));
//   server->event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
//   if (epoll_ctl(server->epollfd, EPOLL_CTL_ADD, clientfd, &(server->event)) == -1) {
//     mcmchlog(mcmchenvcur, MCMCHLOG_ERROR, 0, "Erro ao adicionar cliente ao epoll: %s", strerror(errno));
//     close(clientfd);
//     MCMCHST_SETRET(MCMCHST_NOT_IMPLEMENTED);
//     return NULL;
//   }
//   mcmchlog(mcmchenvcur, MCMCHLOG_INFO, 0, "Um cliente foi conectado sobre o descritor: %i.", clientfd);
//   algnc = mcmchClientBroadcastAdd(broadcast, &client);
//   if (algnc == NULL) {
//     mcmchlog(mcmchenvcur, MCMCHLOG_INFO, 0, "Erro ao tentar registrar a conexão. Conexão dropada.");
//     epoll_ctl(server->epollfd, EPOLL_CTL_DEL, client.ctfd, NULL);
//     close(client.ctfd);
//     MCMCHST_SETRET(MCMCHST_NOT_IMPLEMENTED);
//     return NULL;
//   }
//   MCMCHST_SETRET(MCMCHST_SUCCESS);
//   return algnc;
// }

static void __priv__mcmch__load_ipv4_server(McmchServer server, const int port) {
  int optval = 1;
  server->listen4fd = socket(AF_INET, SOCK_DGRAM, 0);
  server->server_addr4.sin_family = AF_INET;
  server->server_addr4.sin_port = htons(port);
  server->server_addr4.sin_addr.s_addr = INADDR_ANY;
  setsockopt(server->listen4fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  bind(server->listen4fd, (struct sockaddr*)&(server->server_addr4), sizeof(server->server_addr4));
}

static void __priv__mcmch__load_ipv6_server(McmchServer server, const int port) {
  int optval = 1;
  server->listen4fd = socket(AF_INET, SOCK_DGRAM, 0);
  memset(&(server->server_addr6), 0, sizeof(server->server_addr6));
  server->server_addr6.sin6_family = AF_INET6;
  server->server_addr6.sin6_port = htons(port);
  server->server_addr6.sin6_addr = in6addr_any;
  setsockopt(server->listen6fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  bind(server->listen6fd, (struct sockaddr*)&(server->server_addr6), sizeof(server->server_addr6));
}

McmchServer mcmchServerCreate(const int port) {
  struct epoll_event e4;
  struct epoll_event e6;
  McmchServer server = (McmchServer)malloc(sizeof(struct McmchServer));
  server->epollfd = -1;
  server->listen4fd = -1;
  server->listen6fd = -1;
  server->epollfd = epoll_create1(0);
  server->event.events = EPOLLIN;
  __priv__mcmch__load_ipv4_server(server, port);
  __priv__mcmch__load_ipv6_server(server, port);
  e4 = server->event;
  e4.data.fd = server->listen4fd;
  epoll_ctl(server->epollfd, EPOLL_CTL_ADD, server->listen4fd, &e4);
  e6.data.fd = server->listen6fd;
  epoll_ctl(server->epollfd, EPOLL_CTL_ADD, server->listen6fd, &e6);
  return server;
}

void __protected__mcmch_ack_accept_client(
  const McmchAddress                     address,
  const McmchProtoAcceptConnectionDgram* dgram
) {
  const struct sockaddr_storage* addr = (struct sockaddr_storage*)address;
  const unsigned char* pkey = dgram->pkey;
  struct McmchProtoCryptoInfo crypto;
  unsigned char        skey[crypto_box_SECRETKEYBYTES];
  unsigned char        fkey[32UL];

  crypto_box_keypair(NULL, skey);
  if (crypto_scalarmult(crypto.shared, skey, pkey) != 0)  {
    /** TODO: Chave pública inválida!  */
    return;
  }
  
  memcpy(&(crypto.nonce), dgram->nonce, sizeof(dgram->nonce));
  crypto.nonce = be64toh(crypto.nonce);
  crypto.sv_nonce = 0;
  crypto_generichash(fkey, sizeof(fkey), (unsigned char*)&crypto, sizeof(crypto), NULL, 0);

  /** TODO: Continuar handshake, enviar nonce do servidor, derivar chave simétrica */
}

void __protected__mcmch_proccess_dgram(
  const McmchProtoDgram dgram,
  McmchAddress          address,
  const ssize_t         size
);

mcmchst mcmchServerUpdate(McmchServer server, McmchBroadcast broadcast, const int nfds) {
  McmchProtoDgram dgram;
  struct sockaddr_storage address;
  for (int i = 0; i < nfds; ++i) {
    struct epoll_event event = server->events[i];
    int                clientfd = event.data.fd;
    socklen_t          addrlen = sizeof(address);
    ssize_t            size = recvfrom(
      clientfd,
      dgram,
      sizeof(dgram),
      0,
      (struct sockaddr*)&address,
      &addrlen
    );
    __protected__mcmch_proccess_dgram(dgram, (McmchAddress)&address, size);
  }
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchServerClose(McmchServer server) {
  close(server->listen4fd);
  close(server->listen6fd);
  close(server->epollfd);
  free(server);
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}
#endif