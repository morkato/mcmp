#ifdef __linux

#ifndef _POSIX_C_SOURCE
  #define _POSIX_C_SOURCE 200809L
#endif /** _POSIX_C_SOURCE  */

#include <monarch/statos.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>

#include "monarch/util/routine.h"

#if _POSIX_C_SOURCE >= 200809L
  #include <monarch/time.h>
  
  static void* mcmchSignalRoutineHandler(void* arg) {
    McmchSignalRoutine* routine = (McmchSignalRoutine*)arg;
    McmchSignalLock*    lock = routine->lock;
    McmchTimeSpec       spec = MCMCHTIME_NULL_TIME_SPEC;
    int                 ret;
    while (routine->running) {
      mcmchSignalLockBlock(lock);
      while ((ret = mcmchSignalLockTimedWait(lock, &spec)) != MCMCHST_SUCCESS) {
        if (!routine->running || ret != MCMCHST_OVTIMEOUT) {
          if (ret != MCMCHST_OVTIMEOUT) {
            routine->handler(lock, MCMCHSIGNALROUTINE_ERROR, routine->arg);
            // mcmchstperr("mcmchSignalLockTimedWait");
          }
          routine->handler(lock, MCMCHSIGNALROUTINE_SHUTDOWN, routine->arg);
          mcmchSignalLockRelease(lock);
          return NULL;
        }
        mcmchTimeGetMonotanic(&spec);
        MCMCHTIME_SPEC_GETNANOS(spec) += 500000000L;
        if (MCMCHTIME_SPEC_GETNANOS(spec) >= 1000000000L) {
          MCMCHTIME_SPEC_GETNANOS(spec) -= 1000000000L;
          MCMCHTIME_SPEC_GETSECS(spec) += 1L;
        }
      }
      if (!routine->running) {
        routine->handler(lock, MCMCHSIGNALROUTINE_SHUTDOWN, routine->arg);
        mcmchSignalLockRelease(lock);
        break;  
      }
      routine->handler(lock, MCMCHSIGNALROUTINE_CONTINUE, routine->arg);
    }
    return NULL;
  }

  mcmchst mcmchSignalLockInitilize(McmchSignalLock* lock) {
    pthread_condattr_t condattrs;
    pthread_condattr_init(&condattrs);
    pthread_condattr_setclock(&condattrs, CLOCK_MONOTONIC);
    pthread_mutex_init(&(lock->mutex), NULL);
    pthread_cond_init(&(lock->cond), &condattrs);
    pthread_condattr_destroy(&condattrs);
    MCMCHST_RETURN(MCMCHST_SUCCESS);
  }

  mcmchst mcmchSignalRoutineCreate(McmchSignalRoutine* routine, McmchSignalLock* lock, void(*handler)(McmchSignalLock*, int, void*), void* arg) {
    routine->arg = arg;
    routine->lock = lock;
    routine->handler = handler;
    routine->running = 1;
    return mcmchRoutineMake((McmchRoutine*)routine, mcmchSignalRoutineHandler, routine);
  }
#endif /** _POSIX_C_SOURCE  */

mcmchst mcmchRoutineMake(McmchRoutine* routine, McmchRoutineHandler handler, void* arg) {
  routine->handler = handler;
  routine->arg = arg;
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchRoutineLink(McmchRoutinePipeline* pipeline, const McmchRoutine* routine) {
  pthread_create(pipeline, NULL, routine->handler, routine->arg);
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

void mcmchRoutineJoin(const McmchRoutinePipeline* routine, void** result) {
  pthread_join(*routine, result);
}

mcmchst mcmchSignalLockBlock(McmchSignalLock* lock) {
  pthread_mutex_lock(&(lock->mutex));
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchSignalLockWait(McmchSignalLock* lock) {
  int status = pthread_cond_wait(&(lock->cond), &(lock->mutex));
  if (status != 0)
    MCMCHST_RETURN(MCMCHST_NOT_IMPLEMENTED);
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchSignalLockTimedWait(McmchSignalLock* lock, McmchTimeSpec* timer) {
  int status =  pthread_cond_timedwait(&(lock->cond), &(lock->mutex), (struct timespec*)timer);
  if (status == ETIMEDOUT)
    MCMCHST_RETURN(MCMCHST_OVTIMEOUT);
  MCMCHST_RETURN(status == 0 ? MCMCHST_SUCCESS : MCMCHST_NOT_IMPLEMENTED);
}

mcmchst mcmchSignalLockRelease(McmchSignalLock* lock) {
  pthread_mutex_unlock(&(lock->mutex));
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchSignalLockDestroy(McmchSignalLock* lock) {
  pthread_cond_destroy(&(lock->cond));
  pthread_mutex_destroy(&(lock->mutex));
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

void mcmchSignalDispatch(McmchSignalLock* lock) {
  pthread_cond_signal(&(lock->cond));
}

void mcmchSignalAllDispatch(McmchSignalLock* lock) {
  pthread_cond_broadcast(&(lock->cond));
}

void mcmchSignalRoutineInterrompt(McmchSignalRoutine* routine) {
  routine->running = 0;
}

#endif // __linux