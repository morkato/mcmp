#ifdef __linux
#include "monarch/server/broadcast.h"
#include <monarch/util/bitmap.h>
#include <monarch/statos.h>
#include <sys/socket.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef __LINUX_OS_MONARCH_ADDRESS_IPV4_IPV6_SIZE
  #define __LINUX_OS_MONARCH_ADDRESS_IPV4_IPV6_SIZE 32ULL
#endif

#define MCH_BC_CAST(bc)              ((struct McmchBroadcastHeader*)(bc))
#define MCH_BC_GET_LENGTH(bc)        (MCH_BC_CAST(bc)->full_length)
#define MCH_BC_GET_BITMAP_LENGTH(bc) (MCH_BC_CAST(bc)->bitmap_length)
#define MCH_BC_GET_BITMAP(bc)        ((uint8_t*)(MCH_BC_CAST(bc) + 1))
#define MCH_BC_GET_CLIENTS(bc)       ((struct McmchClientImpl*)(MCH_BC_GET_BITMAP(bc) + MCH_BC_GET_BITMAP_LENGTH(bc)))
#define MCH_BC_GET_CLIENT(bc,i)      (MCH_BC_GET_CLIENTS(bc) + i)

typedef char McmchAddressInput[__LINUX_OS_MONARCH_ADDRESS_IPV4_IPV6_SIZE];

struct __attribute__((packed, aligned(64ULL))) McmchClientImpl {
  McmchAddressInput address;
  McmchBroadcast    broadcast;
};

struct McmchBroadcastHeader {
  size_t full_length;
  size_t bitmap_length;
};

McmchBroadcast mcmchBroadcastCreate(const size_t capacity) {
  const size_t bitmap_length = (capacity + 7ULL) / 8ULL;
  McmchBroadcast broadcast = (McmchBroadcast)malloc(
    sizeof(struct McmchBroadcastHeader)
    + bitmap_length
    + (sizeof(struct McmchClientImpl) * capacity)
  );
  if (broadcast == NULL) {
    MCMCHST_SETRET(MCMCHST_OUT_OF_MEM);
    return NULL;
  }
  MCH_BC_CAST(broadcast)->full_length = capacity;
  MCH_BC_CAST(broadcast)->bitmap_length = bitmap_length;
  mcmchBitmapCleanOrder(MCH_BC_GET_BITMAP(broadcast), MCH_BC_GET_BITMAP_LENGTH(broadcast));
  return broadcast;
}

McmchClient mcmchBroadcastAddClient(McmchBroadcast broadcast, const McmchAddress address, const size_t addrlen) {
  const bitidx_t ni = mcmchBitmapNextAvaibleBit(MCH_BC_GET_BITMAP(broadcast), 0, MCH_BC_GET_BITMAP_LENGTH(broadcast));
  struct McmchClientImpl* client;
  if (ni == -1 || ni >= MCH_BC_GET_LENGTH(broadcast)) {
    MCMCHST_SETRET(MCMCHST_OVERFLOW);
    return NULL;
  }
  client = MCH_BC_GET_CLIENT(broadcast, ni);
  memcpy(client->address, address, addrlen);
  client->broadcast = broadcast;
  mcmchBitmapEnableBit(MCH_BC_GET_BITMAP(broadcast), ni);
  return (McmchClient)client;
}

mcmchst mcmchBroadcastDelClient(McmchBroadcast broadcast, const McmchClient client) {
  const bitidx_t ni = ((struct McmchClientImpl*)client) - MCH_BC_GET_CLIENTS(client);
  if (ni < 0 || ni >= MCH_BC_GET_LENGTH(broadcast))
    MCMCHST_RETURN(MCMCHST_OVERFLOW);
  else if (mcmchBitmapIsDisabledBit(MCH_BC_GET_BITMAP(broadcast), ni))
    MCMCHST_RETURN(MCMCHST_NOAVLE);
  mcmchBitmapDisableBit(MCH_BC_GET_BITMAP(broadcast), ni);
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchBroadcastDestroy(const McmchBroadcast broadcast) {
  free(broadcast);
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}
#endif /** __linux  */