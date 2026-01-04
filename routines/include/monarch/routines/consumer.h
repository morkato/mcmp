#ifndef MONARCH_UTIL_CONSUMER_H
#define MONARCH_UTIL_CONSUMER_H 1
#include "monarch/routines/routine.h"
#include "monarch/routines/pool.h"

#ifdef MCMCHROUTINE_SIGNALROUTINE_LOCK_T
  #include <monarch/monarch.h>
  #include <monarch/stat.h>

  #define MCMCHCONSUMER_TAKED 1

  typedef mcmchst (*McmchConsumerGetKeyHandler)(void* ctx, void** taked);
  typedef void    (*McmchConsumerHandler)(void* taked);
  typedef struct McmchConsumerRoutine {
    McmchRoutine               routine;
    McmchSignalLock*           lock;
    McmchConsumerGetKeyHandler key;
    McmchConsumerHandler       handler;
    int                        running;
    void*                      ctx;
  } McmchConsumerRoutine;

  mcmchst mcmchConsumerRoutineCreate(McmchConsumerRoutine* routine, McmchSignalLock* lock, 
                                     McmchConsumerGetKeyHandler key, McmchConsumerHandler handler,
                                     void* ctx);

  #define MCMCHCONSUMER_DEFINED 1

  McmchExecutionPool mcmchConsumerCreateExecutionPool (McmchPoolSize nrots);
  void               mcmchConsumerExcutionPoolLink    (const McmchConsumerRoutine* routine, McmchExecutionPool pool);
  void               mcmchConsumerDestroyExcutionPool (McmchExecutionPool pool);
#endif /** MCMCHROUTINE_SIGNALROUTINE_LOCK_T  */
#endif /** MONARCH_UTIL_CONSUMER_H  */