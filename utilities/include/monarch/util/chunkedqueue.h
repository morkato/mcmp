#ifndef MONARCH_UTIL_CHUNKED_QUEUE_H
#include <monarch/stat.h>
#include <stdint.h>
#include <stddef.h>

typedef struct __attribute__((aligned(64UL))) McmchChunkedQueueHeader {
  size_t alignment;
  size_t length;
  size_t chunk_total_size;
  const char __padding[40];
} McmchChunkedQueueHeader;
typedef struct McmchChunkInfo {
  size_t head;
  size_t tail;
} McmchChunkInfo;
typedef struct McmchChunkedQueue* McmchChunkedQueue;

#define MCH_CQ_CAST(ptr)       ((struct McmchChunkedQueueHeader*)ptr)
#define MCH_CQ_ALIGNMENT(ptr) (MCH_CQ_CAST(ptr)->alignment)
#define MCH_CQ_LENGTH(ptr)     (MCH_CQ_CAST(ptr)->length)
#define MCH_CQ_CHUNKSIZE(ptr)  (MCH_CQ_CAST(ptr)->chunk_total_size)
#define MCH_CQ_CHUNKSPTR(ptr)  ((char*)(MCH_CQ_CAST(ptr) + 1))
#define MCH_CQ_CHUNK(ptr,ch)   ((struct McmchChunkInfo*)(MCH_CQ_CHUNKSPTR(ptr) + (ch * MCH_CQ_CHUNKSIZE(ptr))))
#define MCH_CQ_TAIL(ptr,ch)    (MCH_CQ_CHUNK(ptr,ch)->tail)
#define MCH_CQ_HEAD(ptr,ch)    (MCH_CQ_CHUNK(ptr,ch)->head)
#define MCH_CQ_ISEMPTY(ptr,ch) (MCH_CQ_HEAD(ptr,ch) == MCH_CQ_TAIL(ptr,ch))
#define MCH_CQ_ITEMS(ptr,ch)   ((char*)(MCH_CQ_CHUNK(ptr,ch) + 1))
#define MCH_CQ_ITEM(ptr,ch,it) (MCH_CQ_ITEMS(ptr,ch) + (sizeof(uintptr_t) * it))

McmchChunkedQueue mcmchChunkedQueueCreate  (const size_t length, const size_t alignment);
mcmchst           mcmchChunkedQueueDestroy (const McmchChunkedQueue cq);
mcmchst           mcmchChunkedQueueTake    (McmchChunkedQueue cq, const size_t chunkidx, uintptr_t* item);
mcmchst           mcmchChunkedQueuePush    (McmchChunkedQueue cq, const size_t chunkidx, const uintptr_t item);
#endif /** MONARCH_UTIL_CHUNKED_QUEUE_H  */