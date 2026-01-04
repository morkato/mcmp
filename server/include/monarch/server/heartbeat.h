#ifndef MONARCH_HEARTBEAT_H
#include "monarch/server/server.h"
#include "monarch/server/broadcast.h"
#include <monarch/routines/monitor.h>
#include <monarch/util/bitmap.h>
#include <monarch/stat.h>
#include <stdint.h>

#define MCMCHHEARTBEAT_CAST(hb) ((McmchHeartBeatHeader*)(hb))
#define MCMCHHEARTBEAT_GET_BITMAP_LENGTH(hb) (MCMCHHEARTBEAT_CAST(hb)->length)
#define MCMCHHEARTBEAT_GET_BITMAP(heartbeat) (((uint8_t*)(heartbeat)) + sizeof(McmchHeartBeatHeader))
#define MCMCHHEARTBEAT_GET_BROADCAST(hb) (&(MCMCHHEARTBEAT_CAST(hb)->broadcast))

#define MCMCHHEARTBEAT_SETFLAG(hb, fg)   MCMCHENV_SETFLAG(MCMCHHEARTBEAT_CAST(hb)->flags, fg)
#define MCMCHHEARTBEAT_HASFLAG(hb, fg)   MCMCHENV_HASFLAG(MCMCHHEARTBEAT_CAST(hb)->flags, fg)
#define MCMCHHEARTBEAT_CLEARFLAG(hb, fg) MCMCHENV_CLEARFLAG(MCMCHHEARTBEAT_CAST(hb)->flags, fg)

#define MCMCHHEARTBEAT_RUNNING   0
#define MCMCHHEARTBEAT_AVAILABLE 1

#define MCH_HBT_CAST(hb) ((McmchHeartBeatHeader*)(hb))
#define MCH_HBT_GET_BROADCAST(hb) (MCH_HBT_CAST(hb)->broadcast)
#define MCH_HBT_GET_MONITOR(hb) (MCH_HBT_CAST(hb)->monitor)
#define MCH_HBT_GET_MONITOR_LOCK(hb) (MCH_HBT_CAST(hb)->lock)
#define MCH_HBT_GET_BITMAP(heartbeat) (((uint8_t*)(heartbeat)) + sizeof(McmchHeartBeatHeader))
#define MCH_HBT_GET_BITMAP_LENGTH(hb) (MCH_BC_GET_BITMAP_LENGTH(MCH_HBT_GET_BROADCAST(hb)))

#define MCH_HBT_SETFLAG(hb, fg)   MCMCHENV_SETFLAG(MCH_HBT_CAST(hb)->flags, fg)
#define MCH_HBT_HASFLAG(hb, fg)   MCMCHENV_HASFLAG(MCH_HBT_CAST(hb)->flags, fg)
#define MCH_HBT_CLEARFLAG(hb, fg) MCMCHENV_CLEARFLAG(MCH_HBT_CAST(hb)->flags, fg)

#define MCH_HBT_RUNNING   0
#define MCH_HBT_AVAILABLE 1

typedef struct McmchHeartBeatHeader {
  McmchMonitor     monitor;
  McmchBroadcast   broadcast;
  McmchMonitorLock lock;
  uint8_t          flags;
} McmchHeartBeatHeader;
typedef struct McmchHeartbeat* McmchHeartbeat;

McmchHeartbeat mcmchHeartbeatCreateInstance    (McmchBroadcast broadcast, McmchMonitorPool pool);
mcmchst        mcmchHeartbeatDestroyInstance (const McmchHeartbeat heartbeat);
mcmchst        mcmchHeartbeatBeat            (const McmchHeartbeat heartbeat);
// mcmchst          mcmchHeartbeatCall             (const mcmchclient_broadcast brodcast, mcmchbitmap result);
// mcmchst          mcmchHeartbeatRuntineHandler   (McmchSignalLock* lock, int status, mcmchst* arg);
mcmchst          mcmchHeartbeatProcessIfAvaible (const McmchServer server, McmchHeartbeat heartbeat);

#endif // MONARCH_HEARTBEAT_H