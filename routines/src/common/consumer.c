#include "monarch/routines/consumer.h"

#ifdef MCMCHCONSUMER_DEFINED
  #include "monarch/util/routine.h"
  #include "monarch/util/bitmap.h"
  #include <monarch/statos.h>
  #include <monarch/time.h>
  #include <malloc.h>
  #include <stdio.h>
  #include <signal.h>

  void* ___priv_consumer_handler_dispatcher(void* arg) {
    McmchConsumerRoutine* consumer  = (McmchConsumerRoutine*)arg;
    McmchSignalLock*      lock      = consumer->lock;
    McmchTimeSpec         nextframe = MCMCHTIME_NULL_TIME_SPEC;
    void*                 taked;
    mcmchst               ret;

    while (consumer->running) {
      mcmchSignalLockBlock(lock);
      ret = consumer->key(consumer->ctx, &taked);
      while (ret != MCMCHCONSUMER_TAKED && (ret = mcmchSignalLockTimedWait(lock, &nextframe)) != MCMCHST_SUCCESS) {
        if (!consumer->running || ret != MCMCHST_OVTIMEOUT) {
          if (ret != MCMCHST_OVTIMEOUT) (void)0;
            // mcmchstperr("mcmchSignalLockTimedWait");  
          break;
        }
        mcmchTimeGetMonotanic(&nextframe);
        MCMCHTIME_SPEC_GETNANOS(nextframe) += 500000000L;
        if (MCMCHTIME_SPEC_GETNANOS(nextframe) >= 1000000000L) {
          MCMCHTIME_SPEC_GETNANOS(nextframe) -= 1000000000L;
          MCMCHTIME_SPEC_GETSECS(nextframe) += 1L;
        }
        ret = consumer->key(consumer->ctx, &taked);
      }
      if (!consumer->running) {
        mcmchSignalLockRelease(lock);
        break;
      } else if (ret == MCMCHST_SUCCESS) {
        ret = consumer->key(consumer->ctx, &taked);
      }
      mcmchSignalLockRelease(lock);
      if (ret == MCMCHCONSUMER_TAKED) {
        consumer->handler(taked);
      }
    }
    return NULL;
  }

  void* __priv_consumer_handler_dispatcher(void* arg) {
    McmchConsumerRoutine* consumer  = (McmchConsumerRoutine*)arg;
    void*                 taked;
    mcmchst               ret;
    while (consumer->running) {
      do {
        ret = consumer->key(consumer->ctx, &taked);
        mcmchPoolWaitSignal();
      } while (ret != MCMCHCONSUMER_TAKED);
      consumer->handler(taked);
    }
    return NULL;
  }

  mcmchst mcmchConsumerRoutineCreate(
    McmchConsumerRoutine*      routine,
    McmchSignalLock*           lock,
    McmchConsumerGetKeyHandler key,
    McmchConsumerHandler       handler,
    void*                      ctx
  ) {
    routine->key = key;
    routine->handler = handler;
    routine->ctx = ctx;
    routine->lock = lock;
    routine->running = 1;
    return mcmchRoutineMake(
      (McmchRoutine*)routine,
      __priv_consumer_handler_dispatcher,
      routine
    );
  }

  void mcmchConsumerExcutionPoolLink(const McmchConsumerRoutine* routine, McmchExecutionPool pool) {
    mcmchExecutionPoolLink((const McmchRoutine*)routine, pool);
  }
#endif /** MCMCHCONSUMER_DEFINED  */