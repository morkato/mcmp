#ifndef MONARCH_DISPATCHER_H
#define MONARCH_DISPATCHER_H 1
#include <stdint.h>
#include <malloc.h>

#include "monarch/server/broadcast.h"

#ifdef MCMCHCLIENT_T
  #include <monarch/util/setqueue.h>
  #include <monarch/util/routine.h>
  #include <monarch/stat.h>

  #define MCMCHDISPATCHER_GET_BROADCAST(wrdr)  (((McmchDispatcherWriterHandler*)(wrdr))->broadcast)
  #define MCMCHDISPATCHER_GET_LOCK(wrdr)       (&(((McmchDispatcherWriterHandler*)(wrdr))->lock))
  #define MCMCHDISPATCHER_GET_FLAGS(wrdr)      (((McmchDispatcherWriterHandler*)(wrdr))->flags)
  #define MCMCHDISPATCHER_GET_SETQUEUE(wrdr)   (((McmchDispatcherWriterHandler*)(wrdr))->queue)

  #define MCMCHDISPATCHER_FLAG_STARTED    0
  #define MCMCHDISPATCHER_FLAG_CLOSURE    1
  #define MCMCHDISPATCHER_FLAG_ONDISPATCH 2
  #define MCMCHDISPATCHER_FLAG_WORKING    3

  typedef struct McmchDispatcherWriterHandler {
    McmchBroadcast  broadcast;
    mcmchsetqueue   queue;
    McmchRoutine    routine;
    McmchSignalLock lock;
    int             flags;
  } McmchDispatcherWriterHandler;

  typedef McmchDispatcherWriterHandler* McmchDispatcherWrite;

  McmchDispatcherWrite mcmchDispatcherCreateWriterInstance  (McmchBroadcast broadcast);
  void                 mcmchDispatcherWriterStart           (McmchDispatcherWrite wrdr);
  void                 mcmchDispatcherDestroyWriterInstance (const McmchDispatcherWrite wrdr);
  mcmchst              mcmchDispatcherWriterNotify          (McmchDispatcherWrite wrdr, const McmchClient client);
  void*                mcmchDispatcherWriterRuntineHandler  (void* arg);
#endif /** MCMCHCLIENT_T  */

#endif // MONARCH_DISPATCHER_H