#ifdef __linux
#include <netinet/in.h>
#include <malloc.h>

#include <monarch/util/bitmap.h>
#include <monarch/statos.h>
#include "monarch/server/clients.h"
#include "monarch/server/server.h"

mcmchst mcmchClientClose(const McmchServer server, const mcmchclient client) {
  epoll_ctl(server->epollfd, EPOLL_CTL_DEL, MCMCHCLIENT_GET_FD(client), NULL);
  close(MCMCHCLIENT_GET_FD(client));
}

bitidx_t mcmchClientGetBitIndex(const mcmchclient_broadcast broadcast, const int ctfd) {
  mcmchbitmap bitmap = MCMCHCLIENT_BROADCAST_BITMAP(broadcast);
  size_t      length = MCMCHCLIENT_BROADCAST_BITMAP_LENGTH(broadcast);
  for (bitidx_t bitidx = mcmchBitmapNextOccupiedBit(bitmap, 0, length); bitidx != -1; bitidx = mcmchBitmapNextOccupiedBit(bitmap, bitidx + 1, length)) {
    if (MCMCHCLIENT_GET_FD(MCMCHCLIENT_GETBY_SLOT(broadcast, bitidx)) == ctfd)
      return bitidx;
  }
  return -1;
}

#endif /** __linux  */