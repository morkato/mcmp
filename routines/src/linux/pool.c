#ifdef __linux

#include "monarch/routines/pool.h"
#include <monarch/statos.h>

#include <signal.h>
#include <malloc.h>

MCMCHST_THREAD sigset_t __priv__mcmch_poolmask;

void* __mcmch__priv_monitor_handler(void* arg) {
  McmchExecutionPool pool = (McmchExecutionPool)arg;
  sigemptyset(&mcmchpoolmask);
  sigaddset(&mcmchpoolmask, SIGUSR1);
  pthread_sigmask(SIG_BLOCK, &mcmchpoolmask, NULL);
  mcmchpool = pool;
  mcmchpoolslot = 0;
  
}

void* __mcmch__priv_pool_routine_entry(void* arg) {
    McmchExecutionPoolIdentifier* act = (McmchExecutionPoolIdentifier*)arg;
    McmchExecutionPool            pool = act->start;
    McmchExecutionPoolIdentifier* identifiers = MCMCHPOOL_GET_IDENTIFIERS(pool);
    McmchPoolSize                 slot = (uint16_t)(act - identifiers);
    void*                         ret;
    
    int      recvsig;
    if (act < identifiers) {
      /** TODO: Tratar o erro de forma mais indomática. */
      printf("Erro: Ponteiro anterior ao início do array. Abortando...\n");
      return NULL;
    } else if (slot >= MCMCHPOOL_GET_LENGTH(pool)) {
      /** TODO: Tratar o erro de forma mais indomática. */
      printf("Vazamento de memória! Índice %u fora do limite %u. Abortando...\n", slot, MCMCHPOOL_GET_LENGTH(pool));
      return NULL;
    }
    mcmchpool = pool;
    mcmchpoolslot = slot;
    sigemptyset(&mcmchpoolmask);
    sigaddset(&mcmchpoolmask, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &mcmchpoolmask, NULL);
    while (mcmchBitmapIsDisabledBit(MCMCHPOOL_GET_BITMAP(mcmchpool), slot))
      sigwait(&mcmchpoolmask, &recvsig);
    ret = MCMCHPOOL_GET_ROUTINE(mcmchpool)->handler(MCMCHPOOL_GET_ROUTINE(mcmchpool)->arg);
    while (mcmchBitmapIsEnabledBit(MCMCHPOOL_GET_BITMAP(mcmchpool), slot))
      sigwait(&mcmchpoolmask, &recvsig);
    printf("[SHUTDOWN] [%s] Meu slot: %u no pool: 0x%p\n", MCMCHPOOL_GET_POOLNAME(mcmchpool), mcmchpoolslot, mcmchpool);
    return ret;
  }

  mcmchst mcmchPoolWaitSignal() {
    int recv;
    int status = sigwait(&mcmchpoolmask, &recv);
    if (status != 0)
      MCMCHST_RETURN(MCMCHST_NOT_IMPLEMENTED);
    MCMCHST_RETURN(MCMCHST_SUCCESS);
  }

  void mcmchExecutionPoolStart(McmchExecutionPool pool) {
    McmchExecutionPoolIdentifier* identifiers = MCMCHPOOL_GET_IDENTIFIERS(pool);
    mcmchbitmap                   bitmap = MCMCHPOOL_GET_BITMAP(pool);
    for (McmchPoolSize i = 0; i < MCMCHPOOL_GET_LENGTH(pool); ++i) {
      McmchExecutionPoolIdentifier* act = identifiers + i;
      mcmchBitmapEnableBit(bitmap, i);
      pthread_kill((pthread_t)(act->pipeline), SIGUSR1);
    }
  }

  void mcmchExecutionPoolDestroy(McmchExecutionPool pool) {
    McmchExecutionPoolIdentifier* identifiers = MCMCHPOOL_GET_IDENTIFIERS(pool);
    mcmchbitmap                   bitmap = MCMCHPOOL_GET_BITMAP(pool);
    for (McmchPoolSize i = 0; i < MCMCHPOOL_GET_LENGTH(pool); ++i) {
      McmchExecutionPoolIdentifier* act = identifiers + i;
      mcmchBitmapDisableBit(bitmap, i);
      pthread_kill((pthread_t)(act->pipeline), SIGUSR1);
      mcmchRoutineJoin(&(act->pipeline), NULL);
    }
    free(pool);
  }
#endif /** __linux  */