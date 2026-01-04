#include "monarch/server/broadcast.h"
#include "monarch/server/heartbeat.h"
#include "monarch/server/client.h"
#include <monarch/routines/monitor.h>
#include <monarch/statos.h>
#include <monarch/log.h>

void __mcmch__heartbeat_plataform_heartbeat_heandler(mcmchst st, void* arg);

McmchHeartbeat mcmchHeartbeatCreateInstance(McmchBroadcast boradcast, McmchMonitorPool pool) {
  McmchHeartbeat heartbeat = (McmchHeartbeat)malloc(
    sizeof(McmchHeartBeatHeader)
    + MCH_BC_GET_BITMAP_LENGTH(boradcast)
  );
  if (heartbeat == NULL) {
    MCMCHST_SETRET(MCMCHST_OUT_OF_MEM);
    return NULL;
  }
  MCH_HBT_CAST(heartbeat)->broadcast = boradcast;
  MCH_HBT_CAST(heartbeat)->flags = 0;
  mcmchBitmapCleanOrder(MCMCHHEARTBEAT_GET_BITMAP(heartbeat), MCH_HBT_GET_BITMAP_LENGTH(heartbeat));
  mcmchMonitorCreate(&(MCH_HBT_GET_MONITOR(heartbeat)), pool, __mcmch__heartbeat_plataform_heartbeat_heandler, heartbeat);
  return heartbeat;
}

mcmchst mcmchHeartbeatDestroyInstance(const McmchHeartbeat heartbeat) {
  mcmchMonitorDestroy(MCH_HBT_GET_MONITOR(heartbeat));
  free(heartbeat);
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchHeartbeatBeat(const McmchHeartbeat heartbeat) {
  return mcmchMonitorPing(MCH_HBT_GET_MONITOR(heartbeat));
}

mcmchst mcmchHeartbeatProcessIfAvaible(const McmchServer server, McmchHeartbeat heartbeat) {
  size_t            length = MCH_HBT_GET_BITMAP_LENGTH(heartbeat);
  mcmchbitmap       bitmap = MCH_HBT_GET_BITMAP(heartbeat);
  McmchBroadcast    broadcast = MCH_HBT_GET_BROADCAST(heartbeat);
  McmchMonitorLock* lock   = &(MCH_HBT_GET_MONITOR_LOCK(heartbeat));
  if (mcmchMonitorLockTryAcquire(lock, 16ULL) != MCMCHST_SUCCESS)
    MCMCHST_RETURN(MCMCHST_NOAVLE);
  if (!MCH_HBT_HASFLAG(heartbeat, MCH_HBT_AVAILABLE)) {
    mcmchMonitorLockRelease(lock);
    MCMCHST_RETURN(MCMCHST_NOT_IMPLEMENTED);
  }
  for (bitidx_t bitidx = mcmchBitmapNextOccupiedBit(bitmap, 0, length);
       bitidx != -1;
       bitidx = mcmchBitmapNextOccupiedBit(bitmap, bitidx + 1, length)
  ) {
    mcmchlog(mcmchenvcur, MCMCHLOG_DEBUG, 0, "O cliente escrito no bit: %li será dropado. Por inatividade.", bitidx);
    McmchClient client = /* Obtendo cliente. */ (void*)0;
    mcmchClientClose(server, client);
    mcmchBroadcastDelClient(broadcast, client);
    mcmchBitmapDisableBit(bitmap, bitidx);
  }
  printf("Processo do heartbeat disparado!\n");
  MCMCHHEARTBEAT_CLEARFLAG(heartbeat, MCMCHHEARTBEAT_AVAILABLE);
  mcmchMonitorLockRelease(lock);
}

void mcmchHeartbeatCall(const mcmchclient_broadcast broadcast, mcmchbitmap result) {
  const mcmchbitmap slots   = MCMCHCLIENT_BROADCAST_BITMAP(broadcast);
  const size_t      length  = MCMCHCLIENT_BROADCAST_BITMAP_LENGTH(broadcast);
  mcmchclient       client  = NULL;
  bitidx_t slot = mcmchBitmapNextOccupiedBit(slots, 0, length);
  for (; slot != -1; slot = mcmchBitmapNextOccupiedBit(slots, slot + 1, length)) {
    client = MCMCHCLIENT_GETBY_SLOT(broadcast, slot);
    if (!MCMCHCLIENT_ISALIVE(client)) {
      mcmchBitmapEnableBit(result, slot);
      continue;
    }
    MCMCHCLIENT_CLEARFLAG(client, MCMCHCLIENT_FLAG_ALIVE);
  }
}

void mcmchHeartbeatRuntineHandler(McmchSignalLock* lock, int status, void* arg) {
  if (status != MCMCHSIGNALROUTINE_CONTINUE) {
    printf("Foi diaparado o fechamento da runtine do heartbeat!\n");
    return;
  }
  printf("Heartbeat disparado!\n");
  mcmchHeartbeatCall(MCMCHHEARTBEAT_GET_BROADCAST(arg), MCMCHHEARTBEAT_GET_BITMAP(arg));
  MCMCHHEARTBEAT_SETFLAG(arg, MCMCHHEARTBEAT_AVAILABLE);
  mcmchSignalLockRelease(lock);
  return;
}