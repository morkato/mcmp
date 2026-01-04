#ifndef MONARCH_UTIL_SETQUEUE_H
#define MONARCH_UTIL_SETQUEUE_H 1
#include <monarch/stat.h>
#include <stdint.h>
#include "monarch/util/bitmap.h"
#include "monarch/util/queue.h"

#define MCMCHSETQUEUE_CAST(queue) ((mcmchsetqueue_header_t*)(queue))
#define MCMCHSETQUEUE_GET_BITSLENGTH(queue) \
  ((size_t)(MCMCHSETQUEUE_CAST(queue)->bitslength))
#define MCMCHSETQUEUE_GET_BYTESLENGTH(queue) \
  ((size_t)(MCMCHSETQUEUE_CAST(queue)->byteslength))
#define MCMCHSETQUEUE_GET_BITMAP(queue) \
  ((uint8_t*)((uint8_t*)(queue) + sizeof(mcmchsetqueue_header_t)))
#define MCMCHSETQUEUE_GET_QUEUE(queue) \
  (MCMCHSETQUEUE_GET_BITMAP(queue) + MCMCHSETQUEUE_GET_BYTESLENGTH(queue))

typedef uintptr_t mcmchsetqueue_index_t;
typedef struct mcmchsetqueue_header_t {
  size_t bitslength;
  size_t byteslength;
} mcmchsetqueue_header_t;
typedef void* mcmchsetqueue;

mcmchsetqueue mcmchSetQueueCreate  (const size_t bitslength);
void          mcmchSetQueueDestroy (const mcmchsetqueue queue);
mcmchst       mcmchSetQueuePush    (mcmchsetqueue queue, const mcmchsetqueue_index_t idx);
mcmchst       mcmchSetQueueTake    (mcmchsetqueue queue, mcmchsetqueue_index_t* ind);
#endif // MONARCH_UTIL_SETQUEUE_H