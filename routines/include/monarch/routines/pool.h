#ifndef MONARCH_UTIL_POOL_H
#define MONARCH_UTIL_POOL_H 1

#include <monarch/util/setqueue.h>
#include <monarch/util/bitmap.h>
#include <monarch/stat.h>
#include <stdint.h>
#include <stddef.h>

#include "monarch/routines/routine.h"

#ifdef MCMCHROUTINE_PIPELINE_OST
  typedef uint32_t McmchPoolSize;
  typedef struct __McmchExecution_header {
    const char*   poolname;
    McmchRoutinePipeline monitor;
    McmchPoolSize length;
  } __McmchExecution_header;
  typedef struct McmchExecutionPool* McmchExecutionPool;
  typedef struct McmchExecutionPoolIdentifier {
    McmchExecutionPool   start;
    McmchRoutinePipeline pipeline;
    void*                taskdata;
  } McmchExecutionPoolIdentifier;

  extern MCMCHST_THREAD McmchExecutionPool __priv__mcmch_pool;
  extern MCMCHST_THREAD McmchPoolSize      __priv__mcmch_pool_slot;
  #ifdef __unix__
    #include <signal.h>
    extern MCMCHST_THREAD sigset_t __priv__mcmch_poolmask;

    #define mcmchpoolmask (__priv__mcmch_poolmask)
  #endif /** __unix__  */

  #define mcmchpool (__priv__mcmch_pool)
  #define mcmchpoolslot (__priv__mcmch_pool_slot)

  #define MCMCHPOOL_CAST_HEADER(pl) ((__McmchExecution_header*)(pl))
  #define MCMCHPOOL_GET_POOLNAME(pl) (MCMCHPOOL_CAST_HEADER(pl)->poolname)
  #define MCMCHPOOL_GET_QUEUE(pl) (MCMCHPOOL_CAST_HEADER(pl)->queue)
  #define MCMCHPOOL_GET_ROUTINE(pl) (MCMCHPOOL_CAST_HEADER(pl)->routine)
  #define MCMCHPOOL_GET_LENGTH(pl) (MCMCHPOOL_CAST_HEADER(pl)->length)
  #define MCMCHPOOL_GET_IDENTIFIERS(pl) ((McmchExecutionPoolIdentifier*)(MCMCHPOOL_CAST_HEADER(pl) + 1))
  #define MCMCHPOOL_GET_BITMAP(pl) ((mcmchbitmap)(MCMCHPOOL_GET_IDENTIFIERS(pl) + MCMCHPOOL_GET_LENGTH(pl)))

  McmchExecutionPool mcmchExecutionPoolCreate  (const char* name, McmchPoolSize nrots);
  void               mcmchExecutionPoolLink    (const McmchRoutine* routine, McmchExecutionPool pool);
  mcmchst            mcmchPoolWaitSignal       ();
  void               mcmchExecutionPoolStart   (McmchExecutionPool pool);
  void               mcmchExecutionPoolDestroy (McmchExecutionPool pool);
#endif /** MCMCHROUTINE_PIPELINE_OST  */
#endif /** MONARCH_UTIL_POOL_H  */