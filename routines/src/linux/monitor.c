#ifdef __unix__

#ifndef MCMCHMONITOR_STATIC_MONITORS
  #define MCMCHMONITOR_STATIC_MONITORS (512UL)
#endif /** MCMCHMONITOR_STATIC_MONITORS  */

#define MCMCHMONITOR_STATIC_SLOTS ((MCMCHMONITOR_STATIC_MONITORS + 7UL) / 8UL)

#if defined(_MSC_VER)
  #include <intrin.h>
  #define CPU_RELAX() _mm_pause()
#elif defined(__GNUC__) || defined(__clang__)
  #if defined(__i386__) || defined(__x86_64__)
    #include <immintrin.h>
    #define CPU_RELAX() _mm_pause()
  #elif defined(__arm__)
    #define CPU_RELAX() __builtin_arm_yield()
  #elif defined(__aarch64__)
    #define CPU_RELAX() __builtin_aarch64_yield()
  #endif
#endif

#include "monarch/routines/monitor.h"
#include <monarch/util/bitmap.h>
#include <monarch/statos.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>

struct McmchMonitorUnixImpl {
  McmchMonitorPool    pool;
  pthread_t           pipeline;
  void*               ctx;
  int                 running;
  McmchMonitorHandler beat;
};

static struct McmchMonitorUnixImpl monitors[MCMCHMONITOR_STATIC_MONITORS];
static char                        slots[MCMCHMONITOR_STATIC_SLOTS];
struct McmchMonitorUnixImpl __protected__mcmch__main_monitor_unix = {0ULL, 0ULL, 0ULL, 1, 0ULL};
McmchMonitor                __protected__mcmch__main_monitor = (McmchMonitor)(&__protected__mcmch__main_monitor_unix);
MCMCHST_THREAD McmchMonitor __protected__mcmch__running_monitor;

#define MCMCHMONITOR_GET_SO_CONTEXT(m) ((struct McmchMonitorUnixImpl*)(m))

static void* __priv__mcmch_monitor_handler(void* arg) {
  struct McmchMonitorUnixImpl* monitor = (struct McmchMonitorUnixImpl*)arg;
  sigset_t                     waitset;
  int                          sig;
  mcmchmonitor = (McmchMonitor)monitor;
  sigemptyset(&waitset);
  sigaddset(&waitset, SIGUSR1);
  pthread_sigmask(SIG_BLOCK, &waitset, NULL);
  while (monitor->running) {
    int status = sigwait(&waitset, &sig);
    if (!monitor->running)
      break;
    monitor->beat(MCMCHST_SUCCESS, monitor->ctx);
    if (status == -1) {
      if (errno == EAGAIN)
        monitor->beat(MCMCHST_OVTIMEOUT, monitor->ctx);
      else if (errno == EINTR)
        continue;
      else {
        monitor->beat(MCMCHST_CRITICAL, monitor->ctx);
        break;
      }
    }
  }
  return NULL;
}

static mcmchst mcmchMonitorCreateStatic(McmchMonitor* monitor, McmchMonitorHandler handler, void* ctx) {
  bitidx_t slot = mcmchBitmapNextAvaibleBit(slots, 0, MCMCHMONITOR_STATIC_SLOTS);
  if (slot >= MCMCHMONITOR_STATIC_MONITORS || slot == -1)
    MCMCHST_RETURN(MCMCHST_OVERFLOW);
  *monitor = (McmchMonitor)(monitors + slot);
  MCMCHMONITOR_GET_SO_CONTEXT(*monitor)->pool = NULL;
  MCMCHMONITOR_GET_SO_CONTEXT(*monitor)->ctx = ctx;
  MCMCHMONITOR_GET_SO_CONTEXT(*monitor)->beat = handler;
  MCMCHMONITOR_GET_SO_CONTEXT(*monitor)->running = 0;
  mcmchBitmapEnableBit(slots, slot);
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchMonitorCreate(McmchMonitor* monitor, McmchMonitorPool pool, McmchMonitorHandler handler, void* ctx) {
  if (pool == NULL)
    return mcmchMonitorCreateStatic(monitor, handler, ctx);
  MCMCHST_RETURN(MCMCHST_NOT_IMPLEMENTED);
}

mcmchst mcmchMonitorStart(McmchMonitor monitor) {
  MCMCHMONITOR_GET_SO_CONTEXT(monitor)->running = 1;
  pthread_create((pthread_t*)&(MCMCHMONITOR_GET_SO_CONTEXT(monitor)->pipeline), NULL, __priv__mcmch_monitor_handler, (void*)monitor);
}

mcmchst mcmchMonitorPing(const McmchMonitor monitor) {
  pthread_kill(MCMCHMONITOR_GET_SO_CONTEXT(monitor)->pipeline, SIGUSR1);
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchMonitorDestroy(const McmchMonitor monitor) {
  MCMCHMONITOR_GET_SO_CONTEXT(monitor)->running = 0;
  pthread_kill((pthread_t)MCMCHMONITOR_GET_SO_CONTEXT(monitor)->pipeline, SIGUSR1);
  pthread_join((pthread_t)MCMCHMONITOR_GET_SO_CONTEXT(monitor)->pipeline, NULL);
  if (MCMCHMONITOR_GET_SO_CONTEXT(monitor)->pool == NULL)
    mcmchBitmapDisableBit(slots, (struct McmchMonitorUnixImpl*)monitor - monitors);
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchMonitorCreatePool(McmchMonitorPool* pool, size_t length) {

}

mcmchst mcmchMonitorLockAcquire(McmchMonitorLock* lock) {
  McmchMonitorLock expected = 0;

  if (__atomic_compare_exchange_n(
    lock,
    &expected,
    ((mcmchmonitor == 0UL) ? mcmchmonitor_main : mcmchmonitor),
    0,
    __ATOMIC_ACQUIRE,
    __ATOMIC_RELAXED
  )) {
    MCMCHST_RETURN(MCMCHST_SUCCESS);
  }
  MCMCHST_RETURN(MCMCHST_NOAVLE);
}

mcmchst mcmchMonitorLockRelease(McmchMonitorLock* lock) {
  McmchMonitorLock expected = (mcmchmonitor == 0UL) ? mcmchmonitor_main : mcmchmonitor;

  if (__atomic_compare_exchange_n(
    lock,
    &expected,
    0UL,
    0,
    __ATOMIC_RELEASE,
    __ATOMIC_RELAXED
  )) {
    MCMCHST_RETURN(MCMCHST_SUCCESS);
  }
  MCMCHST_RETURN(MCMCHST_NOAVLE);
}

#ifdef CPU_RELAX
  mcmchst mcmchMonitorLockTryAcquire(McmchMonitorLock* lock, const size_t ntries) {
    size_t tries = 0ULL;
    size_t delay = 0ULL;
    do {
      if (mcmchMonitorLockAcquire(lock) == MCMCHST_SUCCESS)
        MCMCHST_RETURN(MCMCHST_SUCCESS);
      delay = ((1ULL << (tries / 2ULL)) - 1) % 1024ULL;
      for (size_t i = 0ULL; i < delay; ++i)
        CPU_RELAX();
    } while (tries++ < ntries);
    MCMCHST_RETURN(MCMCHST_NOAVLE);
  }
#endif /** CPU_RELAX  */

#endif /** __unix__  */