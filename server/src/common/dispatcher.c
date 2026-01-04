#ifndef _POSIX_C_SOURCE
  #define _POSIX_C_SOURCE 200809L
#endif

#include <monarch/util/routine.h>
#include <monarch/util/bitmap.h>
#include <monarch/monarch.h>
#include <monarch/statos.h>
#include "monarch/server/dispatcher.h"
#include "monarch/server/clients.h"

#include <malloc.h>
#include <string.h>
#include <time.h>

McmchDispatcherWrite mcmchDispatcherCreateWriterInstance(mcmchclient_broadcast broadcast, const size_t bitslen) {
  McmchDispatcherWrite wrdr = (McmchDispatcherWrite)malloc(sizeof(McmchDispatcherWriterHandler));
  if (wrdr == NULL)
    return NULL;
  wrdr->queue = mcmchSetQueueCreate(bitslen);
  if (wrdr->queue == NULL) {
    free(wrdr);
    return NULL;
  }
  wrdr->broadcast = broadcast;
  wrdr->flags = 0;
  if (mcmchSignalLockInitilize(&(wrdr->lock)) != MCMCHST_SUCCESS) {
    free(wrdr->queue);
    free(wrdr);
    return NULL;
  } else if (mcmchRoutineCreate(&(wrdr->routine), mcmchDispatcherWriterRuntineHandler, wrdr) != MCMCHST_SUCCESS) {
    mcmchSignalLockDestroy(&(wrdr->lock));
    free(wrdr->queue);
    free(wrdr);
    return NULL;
  }
  return wrdr;
}

void mcmchDispatcherWriterStart(McmchDispatcherWrite wrdr) {
  MCMCHENV_SETFLAG(wrdr->flags, MCMCHDISPATCHER_FLAG_STARTED);
  mcmchSignalDispatch(&(wrdr->lock));
}

void mcmchDispatcherDestroyWriterInstance(const McmchDispatcherWrite wrdr) {
  MCMCHENV_SETFLAG(wrdr->flags, MCMCHDISPATCHER_FLAG_CLOSURE);
  mcmchSignalDispatch(&(wrdr->lock));
  mcmchRoutineJoin(&(wrdr->routine), NULL);
  mcmchSignalLockDestroy(&(wrdr->lock));
  free(wrdr->queue);
  free(wrdr); 
}

mcmchst mcmchDispatcherWriterNotify(McmchDispatcherWrite wrdr, const mcmchclient client) {
  
}

void* mcmchDispatcherWriterRuntineHandler(void* arg) {
  mcmchclient_broadcast broadcast = MCMCHDISPATCHER_GET_BROADCAST(arg);
  McmchSignalLock*      lock = MCMCHDISPATCHER_GET_LOCK(arg);
  mcmchsetqueue         queue = MCMCHDISPATCHER_GET_SETQUEUE(arg);
  mcmchclient           client;
  mcmchsetqueue_index_t worker;
  mcmchSignalLockBlock(lock);
  printf("Na thread!\n");
  while (!MCMCHENV_HASFLAG(MCMCHDISPATCHER_GET_FLAGS(arg), MCMCHDISPATCHER_FLAG_STARTED)) {
    mcmchSignalLockWait(lock);
    if (MCMCHENV_HASFLAG(MCMCHDISPATCHER_GET_FLAGS(arg), MCMCHDISPATCHER_FLAG_CLOSURE) || mcmchstret != 0) {
      if (mcmchstret != 0) /** TODO: Erro específico, depende do SO.  */
        (void)0;
      mcmchSignalLockRelease(lock);
      return NULL;
    }
  }
  mcmchSignalLockRelease(lock);
  printf("Na rotína! (Iniciado)!\n");
  while (!MCMCHENV_HASFLAG(MCMCHDISPATCHER_GET_FLAGS(arg), MCMCHDISPATCHER_FLAG_CLOSURE)) {
    mcmchSignalLockBlock(lock);
    MCMCHENV_CLEARFLAG(MCMCHDISPATCHER_GET_FLAGS(arg), MCMCHDISPATCHER_FLAG_WORKING);
    if (!MCMCHENV_HASFLAG(MCMCHDISPATCHER_GET_FLAGS(arg), MCMCHDISPATCHER_FLAG_ONDISPATCH)) {
      mcmchSignalLockWait(lock);
      if (MCMCHENV_HASFLAG(MCMCHDISPATCHER_GET_FLAGS(arg), MCMCHDISPATCHER_FLAG_CLOSURE) || mcmchstret != 0) {
        if (mcmchstret != 0) /** TODO: Erro específico, depende do SO.  */
          (void)0;
        mcmchSignalLockRelease(lock);
        return NULL;
      }
      mcmchSignalLockRelease(lock);
      continue;
    }
    MCMCHENV_SETFLAG(MCMCHDISPATCHER_GET_FLAGS(arg), MCMCHDISPATCHER_FLAG_WORKING);
    MCMCHENV_CLEARFLAG(MCMCHDISPATCHER_GET_FLAGS(arg), MCMCHDISPATCHER_FLAG_ONDISPATCH);
    mcmchSignalLockRelease(lock);
    while (MCMCHQUEUE_NONEMPTY(MCMCHSETQUEUE_GET_QUEUE(queue)) && !MCMCHENV_HASFLAG(MCMCHDISPATCHER_GET_FLAGS(arg), MCMCHDISPATCHER_FLAG_CLOSURE)) {
      mcmchSetQueueTake(queue, &worker);
      if (mcmchstret != MCMCHST_SUCCESS)
        break;
      printf("Trabalhador: %lu\n", worker);
    }
  }
  return NULL;
}