#include <monarch/statos.h>
#include "monarch/server/client.h"
#include "monarch/server/server.h"

#ifdef MCMCHCLIENT_ST
  mcmchclient_broadcast mcmchClientCreateBroadcast(const size_t length) {
    size_t                        blen   = (length + 7UL) / 8UL;
    mcmchclient_broadcast_header* header = NULL;
    mcmchclient_broadcast broadcast = (mcmchclient_broadcast)malloc(
      sizeof(mcmchclient_broadcast_header)
      + blen
      + (sizeof(MCMCHCLIENT_ST) * length)
    );
    if (broadcast == NULL) {
      MCMCHST_SETRET(MCMCHST_OUT_OF_MEM);
      return NULL;
    }
    header = (mcmchclient_broadcast_header*)broadcast;
    header->fullength = length;
    header->bitmap_length = blen;
    mcmchBitmapCleanOrder(MCMCHCLIENT_BROADCAST_BITMAP(broadcast), blen);
    return broadcast;
  }

  void mcmchClientDestroyBroadcast(const mcmchclient_broadcast broadcast) {
    free(broadcast);
  }

  mcmchclient mcmchClientBroadcastAdd(mcmchclient_broadcast broadcast, const mcmchclient client) {
    mcmchbitmap       bitmap = MCMCHCLIENT_BROADCAST_BITMAP(broadcast);
    size_t            length = MCMCHCLIENT_BROADCAST_BITMAP_LENGTH(broadcast);
    bitidx_t          slot   = mcmchBitmapNextAvaibleBit(bitmap, 0, length);
    MCMCHCLIENT_ST*   written;
    if (slot == -1) {
      MCMCHST_SETRET(MCMCHST_OVERFLOW);
      return NULL;
    }
    mcmchBitmapEnableBit(bitmap, slot);
    written = (MCMCHCLIENT_ST*)MCMCHCLIENT_GETBY_SLOT(broadcast, slot);
    *written = *(MCMCHCLIENT_ST*)(client);
    return written;
  }

  mcmchst mcmchClientBroadcastDel(mcmchclient_broadcast broadcast, const mcmchclient client) {
    uintptr_t dif = ((uintptr_t)client - (uintptr_t)MCMCHCLIENT_BROADCAST_CLIENTS(broadcast));
    size_t    idx;
    if ((dif % sizeof(MCMCHCLIENT_ST)) != 0)
      MCMCHST_RETURN(MCMCHST_INVALIGN);
    idx = dif / sizeof(MCMCHCLIENT_ST);
    mcmchBitmapDisableBit(MCMCHCLIENT_BROADCAST_BITMAP(broadcast), (const bitidx_t)idx);
    MCMCHST_RETURN(MCMCHST_SUCCESS);
  }

  mcmchst mcmchClientBroadcastFlush(const McmchServer server, mcmchclient_broadcast broadcast) {
    mcmchbitmap bitmap = MCMCHCLIENT_BROADCAST_BITMAP(broadcast);
    size_t      length = MCMCHCLIENT_BROADCAST_BITMAP_LENGTH(broadcast);
    for (bitidx_t bitidx = mcmchBitmapNextOccupiedBit(bitmap, 0, length); bitidx != -1; bitidx = mcmchBitmapNextOccupiedBit(bitmap, bitidx + 1, length)) {
      mcmchclient client = MCMCHCLIENT_GETBY_SLOT(broadcast, bitidx);
      mcmchClientClose(server, client);
      mcmchBitmapDisableBit(bitmap, bitidx);
    }
    MCMCHST_RETURN(MCMCHST_SUCCESS);
  }
#endif /** MCMCHCLIENT_ST  */