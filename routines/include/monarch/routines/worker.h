#ifndef MONARCH_ROUTINES_WORKER_H
#define MONARCH_ROUTINES_WORKER_H 1
#include <monarch/util/setqueue.h>
#include <monarch/stat.h>
#include <stddef.h>

typedef void                           (*McmchWorkerMonitorHandler)(void*);
typedef struct McmchWorkerExecutorHeader McmchWorkerExecutorHeader;
typedef struct McmchWorkerExecutor*      McmchWorkerExecutor;

McmchWorkerExecutor mcmchWorkerCreate  (McmchWorkerMonitorHandler handler, mcmchsetqueue queue, size_t length);
mcmchst             mcmchWorkerStart   (McmchWorkerExecutor executor);
mcmchst             mcmchWorkerBeat    (const McmchWorkerExecutor executor);
mcmchst             mcmchWorkerDestroy (const McmchWorkerExecutor executor);
#endif /** MONARCH_ROUTINES_WORKER_H  */