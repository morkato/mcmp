#include <monarch/statos.h>
#include <pthread.h>
#include <malloc.h>

#include "monarch/routines/pool.h"

#ifdef MCMCHROUTINE_PIPELINE_OST
  MCMCHST_THREAD McmchExecutionPool __priv__mcmch_pool = NULL;
  MCMCHST_THREAD McmchPoolSize      __priv__mcmch_pool_slot = 0U;

  void* __mcmch__priv_monitor_handler(void* arg);
  void* __mcmch__priv_pool_routine_entry(void* arg);

  McmchExecutionPool mcmchExecutionPoolCreate(const char* name, McmchPoolSize nrots) {
    McmchRoutine             routine;
    __McmchExecution_header* header = NULL;
    McmchExecutionPool       pool = (McmchExecutionPool)malloc(
      sizeof(__McmchExecution_header)
      + sizeof(McmchExecutionPoolIdentifier) * nrots
      + ((nrots + 7) / 8)
    );
    if (pool == NULL) {
      MCMCHST_SETRET(MCMCHST_OUT_OF_MEM);
      return NULL;
    }
    mcmchRoutineMake(&routine, __mcmch__priv_monitor_handler, pool);
    header = MCMCHPOOL_CAST_HEADER(pool);
    header->poolname = name;
    header->length = nrots;
    mcmchBitmapCleanOrder(MCMCHPOOL_GET_BITMAP(pool), ((nrots + 7) / 8));
    mcmchRoutineLink(&(header->monitor), &routine);
    MCMCHST_SETRET(MCMCHST_SUCCESS);
    return pool;
  }

  void mcmchExecutionPoolLink(const McmchRoutine* routine, McmchExecutionPool pool) {
    __McmchExecution_header* header = (__McmchExecution_header*)pool;
    McmchExecutionPoolIdentifier* identifiers = (McmchExecutionPoolIdentifier*)(header + 1);
    header->routine = routine;
    for (size_t i = 0; i < header->length; ++i) {
      McmchExecutionPoolIdentifier* act = identifiers + i;
      act->start = pool;
      pthread_create((pthread_t*)&(act->pipeline), NULL, __mcmch__priv_pool_routine_entry, act);
    }
  }
#endif /** MCMCHROUTINE_PIPELINE_OST  */