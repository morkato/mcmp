#include <monarch/server.h>
#include <stdio.h>

int main() {
  // struct sockaddr_in sime;
  // struct sockaddr_in siother;
  // int                simefd;
  // int                clientfd;
  // socklen_t          silen;
  // char               buffer[512];
  // int                recvlen;
  // silen = sizeof(siother);
  // simefd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  // if (simefd == -1) {
  //   printf("Error to open a socket file descriptor!\n");
  //   return 1;
  // }
  // memset((char*)&sime, 0, sizeof(sime));
  // sime.sin_family = AF_INET;
  // sime.sin_port = htons(5500);
  // sime.sin_addr.s_addr = htonl(INADDR_ANY);
  // if (bind(simefd, (struct sockaddr*)&sime, sizeof(sime))) {
  //   printf("Error to binding a socket file descriptor!\n");
  //   close(simefd);
  //   return 1;
  // }
  // if (listen(simefd, 1) != 0) {
  //   printf("Error to declare listen in file descriptor!\n");
  //   close(simefd);
  //   return -1;
  // }
  // printf("Socket listening on port 5500.\n");
  // while (1) {
  //   clientfd = accept(simefd, (struct sockaddr*)&siother, &silen);
  //   if (clientfd == -1)
  //     break;
  //   printf("Connection accepted from %s:%d\n",
  //     inet_ntoa(siother.sin_addr),
  //     ntohs(siother.sin_port));
  //   write(clientfd, "Um oi do servidor!\n", 19);
  //   sleep(5);
  //   close(clientfd);
  // }
  // close(simefd);
  // return 0;
}