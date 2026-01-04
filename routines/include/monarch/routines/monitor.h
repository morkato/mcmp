#ifndef MONARCH_ROUTINES_MONITOR_H
#define MONARCH_ROUTINES_MONITOR_H 1
#include <monarch/stat.h>
#include <stdint.h>
#include <stddef.h>

typedef void     (*McmchMonitorHandler)(mcmchst stat, void* arg);
typedef struct McmchMonitorPoolHeader {
  size_t length;
}* McmchMonitorPoolHeader;
typedef struct McmchMonitorPool* McmchMonitorPool;
typedef uintptr_t McmchMonitor;
typedef McmchMonitor McmchMonitorLock;

#define MCMCHMONITOR_POOL_CAST(p) ((McmchMonitorPoolHeader)(p))
#define MCMCHMONITOR_POOL_GET_LENGTH(p) (MCMCHMONITOR_POOL_CAST(p)->length)

extern McmchMonitor                __protected__mcmch__main_monitor;
extern MCMCHST_THREAD McmchMonitor __protected__mcmch__running_monitor;

#define mcmchmonitor_main (__protected__mcmch__main_monitor)
#define mcmchmonitor (__protected__mcmch__running_monitor)

mcmchst mcmchMonitorCreate       (McmchMonitor* monitor, McmchMonitorPool pool, McmchMonitorHandler handler, void* ctx);
mcmchst mcmchMonitorStart        (McmchMonitor monitor);
mcmchst mcmchMonitorPing         (const McmchMonitor monitor);
mcmchst mcmchMonitorDestroy      (const McmchMonitor monitor);

mcmchst mcmchMonitorCreatePool   (McmchMonitorPool* pool, size_t length);

mcmchst mcmchMonitorLockTryAcquire (McmchMonitorLock* lock, const size_t ntries);
mcmchst mcmchMonitorLockAcquire    (McmchMonitorLock* lock);
mcmchst mcmchMonitorLockRelease    (McmchMonitorLock* lock);
#endif /** MONARCH_ROUTINES_MONITOR_H  */