#ifdef __unix__
#include "monarch/routines/monitor.h"
#include "monarch/routines/worker.h"
#include <monarch/util/atomicbitmap.h>
#include <monarch/util/setqueue.h>
#include <monarch/statos.h>
#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>

struct __attribute__((aligned(64UL))) McmchWorkerExecutorHeader {
  McmchMonitor              monitor;
  McmchWorkerMonitorHandler handle;
  mcmchsetqueue             queue;
  size_t                    bitmap_length;
  size_t                    length;
  int                       failstry;
};

struct __attribute__((aligned(32UL))) McmchWorkerMonitor {
  McmchMonitor        monitor;
  McmchWorkerExecutor worker;
  void*               taskdata;
  int                 finished;
};

_Static_assert(sizeof(struct McmchWorkerExecutorHeader) == 64UL, "Alinhamento inválido.");
_Static_assert(sizeof(struct McmchWorkerMonitor) == 32UL, "Alinhamento inválido.");

#define MCMCHWORKER_EXECUTOR_CAST(ex) ((struct McmchWorkerExecutorHeader*)(ex))
#define MCMCHWORKER_EXECUTOR_GET_MASTER_MONITOR(ex) (MCMCHWORKER_EXECUTOR_CAST(ex)->monitor)
#define MCMCHWORKER_EXECUTOR_GET_HANDLER(ex) (MCMCHWORKER_EXECUTOR_CAST(ex)->handle)
#define MCMCHWORKER_EXECUTOR_GET_QUEUE(ex) (MCMCHWORKER_EXECUTOR_CAST(ex)->queue)
#define MCMCHWORKER_EXECUTOR_GET_BITMAP_LENGTH(ex) (MCMCHWORKER_EXECUTOR_CAST(ex)->bitmap_length)
#define MCMCHWORKER_EXECUTOR_GET_LENGTH(ex) (MCMCHWORKER_EXECUTOR_CAST(ex)->length)
#define MCMCHWORKER_EXECUTOR_GET_FAILTRY(ex) (MCMCHWORKER_EXECUTOR_CAST(ex)->failstry)
#define MCMCHWORKER_EXECUTOR_GET_BITMAP(ex) ((mcmchbitmap)(MCMCHWORKER_EXECUTOR_CAST(ex) + 1))
#define MCMCHWORKER_EXECUTOR_GET_MONITORS(ex) ((struct McmchWorkerMonitor*)(MCMCHWORKER_EXECUTOR_GET_BITMAP(ex) + MCMCHWORKER_EXECUTOR_GET_BITMAP_LENGTH(ex)))
#define MCMCHWORKER_EXECUTOR_GET_MONITOR(ex,m) (MCMCHWORKER_EXECUTOR_GET_MONITORS(ex) + (m))
#define MCMCHWORKER_EXECUTOR_GET_MONITOR_POS(mptr) ((struct McmchWorkerMonitor*)(mptr) - MCMCHWORKER_EXECUTOR_GET_MONITORS(((struct McmchWorkerMonitor*)(mptr))->worker))

static int __priv__mcmch_worker_monitor_try_disable_bit(struct McmchWorkerMonitor* monitor, const size_t maxtries) {
  for (size_t tries = 0; tries < maxtries; ++tries) {
    if (mcmchAtomicBitmapTryDisableBit(MCMCHWORKER_EXECUTOR_GET_BITMAP(monitor->worker), MCMCHWORKER_EXECUTOR_GET_MONITOR_POS(monitor)) == MCMCHST_SUCCESS)
      return 1;
  }
  return 0;
}

static void __priv__mcmch_worker_monitor_run_cleanup_monitors(McmchWorkerExecutor executor) {
  const mcmchbitmap bitmap = MCMCHWORKER_EXECUTOR_GET_BITMAP(executor);
  printf("[Monitor/Main] Monitores zumbies detectados. Identificando e limpando...\n");

  for (bitidx_t bitidx = mcmchAtomicBitmapNextOccupiedBit(bitmap, 0, MCMCHWORKER_EXECUTOR_GET_BITMAP_LENGTH(executor)); 
       bitidx != -1 && bitidx < MCMCHWORKER_EXECUTOR_GET_LENGTH(executor);
       bitidx = mcmchAtomicBitmapNextOccupiedBit(bitmap, bitidx + 1, MCMCHWORKER_EXECUTOR_GET_BITMAP_LENGTH(executor))) {
    struct McmchWorkerMonitor* act = MCMCHWORKER_EXECUTOR_GET_MONITOR(executor, bitidx);
    if (!act->finished)
      continue;
    else if (!__priv__mcmch_worker_monitor_try_disable_bit(act, 10UL)) {
      /** O monitor pai falhou em conseguir limpar o monitor neste momento, e tentará numa próxima interação.  */
      printf("[Monitor/Main] Não foi possível limpar o monitor: %lu no momento. Tentarei novamente na próxima interação.\n", MCMCHWORKER_EXECUTOR_GET_MONITOR_POS(act));
      __atomic_store_n(
        &(MCMCHWORKER_EXECUTOR_GET_FAILTRY(executor)),
        1,
        __ATOMIC_RELEASE
      );
      break;
    }
    printf("[Monitor/Main] O monitor: %lu era um zumbie e foi limpo com sucesso.\n", MCMCHWORKER_EXECUTOR_GET_MONITOR_POS(act));
  }
}

static void __priv__mcmch_worker_monitor_beat(mcmchst st, void* arg) {
  McmchWorkerExecutor executor = (McmchWorkerExecutor)arg;
  mcmchbitmap         bitmap   = MCMCHWORKER_EXECUTOR_GET_BITMAP(executor);
  mcmchsetqueue       queue    = MCMCHWORKER_EXECUTOR_GET_QUEUE(executor);

  if (__atomic_exchange_n(
    &(MCMCHWORKER_EXECUTOR_GET_FAILTRY(executor)),
    0,
    __ATOMIC_ACQ_REL
  )) __priv__mcmch_worker_monitor_run_cleanup_monitors(executor);
  
  for (bitidx_t bitidx = mcmchAtomicBitmapNextAvaibleBit(bitmap, 0, MCMCHWORKER_EXECUTOR_GET_BITMAP_LENGTH(executor)); 
       bitidx != -1 && bitidx < MCMCHWORKER_EXECUTOR_GET_LENGTH(executor);
       bitidx = mcmchAtomicBitmapNextAvaibleBit(bitmap, bitidx + 1, MCMCHWORKER_EXECUTOR_GET_BITMAP_LENGTH(executor))) {
    struct McmchWorkerMonitor* act = MCMCHWORKER_EXECUTOR_GET_MONITOR(executor, bitidx);
    if (!act->finished) {
      mcmchMonitorPing(act->monitor);
      continue;
    } 
    mcmchSetQueueTake(queue, (mcmchsetqueue_index_t*)&(act->taskdata));
    if (mcmchstret != MCMCHST_SUCCESS)
      break;
    act->finished = 0;
    mcmchMonitorPing(act->monitor);
  }
}

static void __priv__mcmch_worker_monitor_sub_secure_op(struct McmchWorkerMonitor* monitor) {
  const McmchWorkerExecutor executor = monitor->worker;
  McmchWorkerMonitorHandler handler = MCMCHWORKER_EXECUTOR_GET_HANDLER(executor);
  handler(monitor->taskdata);
  monitor->finished = 1;
}

static void __priv__mcmch_worker_monitor_sub_secure_clean(struct McmchWorkerMonitor* monitor) {
  const McmchWorkerExecutor executor = monitor->worker;
  int                       expected = 0;
  if (!__priv__mcmch_worker_monitor_try_disable_bit(monitor, 10UL))
    __atomic_compare_exchange_n(
      &MCMCHWORKER_EXECUTOR_GET_FAILTRY(executor),
      &expected,
      1,
      0,
      __ATOMIC_ACQ_REL,
      __ATOMIC_RELAXED
    );
}

static void __priv__mcmch_worker_monitor_sub(mcmchst st, void* arg) {
  struct McmchWorkerMonitor* monitor = (struct McmchWorkerMonitor*)arg;
  mcmchbitmap                bitmap   = MCMCHWORKER_EXECUTOR_GET_BITMAP(monitor->worker);
  for (size_t tries = 0; tries < 10; ++tries) {
    if (mcmchAtomicBitmapTryEnableBit(bitmap, MCMCHWORKER_EXECUTOR_GET_MONITOR_POS(monitor)) == MCMCHST_SUCCESS) {
      __priv__mcmch_worker_monitor_sub_secure_op(monitor);
      __priv__mcmch_worker_monitor_sub_secure_clean(monitor);
      return;
    }
  }
}

McmchWorkerExecutor mcmchWorkerCreate(McmchWorkerMonitorHandler handler, mcmchsetqueue queue, size_t length) {
  size_t bitmap_length = ((length + 7) / 8);
  McmchWorkerExecutor executor = (McmchWorkerExecutor)malloc(
    sizeof(struct McmchWorkerExecutorHeader)
    + bitmap_length
    + (sizeof(struct McmchWorkerMonitor) * length)
  );
  if (executor == NULL) {
    MCMCHST_SETRET(MCMCHST_OUT_OF_MEM);
    return NULL;
  }
  MCMCHWORKER_EXECUTOR_GET_LENGTH(executor) = length;
  MCMCHWORKER_EXECUTOR_GET_HANDLER(executor) = handler;
  MCMCHWORKER_EXECUTOR_GET_BITMAP_LENGTH(executor) = bitmap_length;
  MCMCHWORKER_EXECUTOR_GET_QUEUE(executor) = queue;
  mcmchMonitorCreate(&MCMCHWORKER_EXECUTOR_GET_MASTER_MONITOR(executor), NULL, __priv__mcmch_worker_monitor_beat, executor);
  mcmchMonitorStart(MCMCHWORKER_EXECUTOR_GET_MASTER_MONITOR(executor));
  return executor;
}

mcmchst mcmchWorkerBeat(const McmchWorkerExecutor executor) {
  mcmchMonitorPing(MCMCHWORKER_EXECUTOR_GET_MASTER_MONITOR(executor));
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchWorkerStart(McmchWorkerExecutor executor) {
  for (struct McmchWorkerMonitor* monitor = MCMCHWORKER_EXECUTOR_GET_MONITOR(executor, 0);
       monitor < MCMCHWORKER_EXECUTOR_GET_MONITOR(executor, MCMCHWORKER_EXECUTOR_GET_LENGTH(executor));
       monitor += 1) {
    mcmchMonitorCreate(&monitor->monitor, NULL, __priv__mcmch_worker_monitor_sub, monitor);
    mcmchMonitorStart(monitor->monitor);
    monitor->finished = 1;
    monitor->worker = executor;
  }
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchWorkerDestroy(const McmchWorkerExecutor executor) {
  McmchMonitor mainmonitor = MCMCHWORKER_EXECUTOR_GET_MASTER_MONITOR(executor);
  mcmchMonitorDestroy(mainmonitor);
  for (struct McmchWorkerMonitor* monitor = MCMCHWORKER_EXECUTOR_GET_MONITOR(executor, 0);
       monitor < MCMCHWORKER_EXECUTOR_GET_MONITOR(executor, MCMCHWORKER_EXECUTOR_GET_LENGTH(executor));
       monitor += 1) {
    mcmchMonitorDestroy(monitor->monitor);
  }
  free(executor);
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}
#endif /** __unix__  */