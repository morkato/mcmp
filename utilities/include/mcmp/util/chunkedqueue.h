#ifndef MCMP_UTIL_CHUNKED_QUEUE_H
#include <mcmp/stat.h>
#include <stdint.h>
#include <stddef.h>

typedef struct __attribute__((aligned(64UL))) McmpChunkedQueueHeader {
  size_t alignment;
  size_t length;
  size_t chunk_total_size;
  const char __padding[40];
} McmpChunkedQueueHeader;
typedef struct McmpChunkInfo {
  size_t head;
  size_t tail;
} McmpChunkInfo;
typedef struct McmpChunkedQueue* McmpChunkedQueue;

#define MCMP_CQ_CAST(ptr)       ((struct McmpChunkedQueueHeader*)ptr)
#define MCMP_CQ_ALIGNMENT(ptr)  (MCMP_CQ_CAST(ptr)->alignment)
#define MCMP_CQ_LENGTH(ptr)     (MCMP_CQ_CAST(ptr)->length)
#define MCMP_CQ_CHUNKSIZE(ptr)  (MCMP_CQ_CAST(ptr)->chunk_total_size)
#define MCMP_CQ_CHUNKSPTR(ptr)  ((char*)(MCMP_CQ_CAST(ptr) + 1))
#define MCMP_CQ_CHUNK(ptr,ch)   ((struct McmpChunkInfo*)(MCMP_CQ_CHUNKSPTR(ptr) + (ch * MCMP_CQ_CHUNKSIZE(ptr))))
#define MCMP_CQ_TAIL(ptr,ch)    (MCMP_CQ_CHUNK(ptr,ch)->tail)
#define MCMP_CQ_HEAD(ptr,ch)    (MCMP_CQ_CHUNK(ptr,ch)->head)
#define MCMP_CQ_ISEMPTY(ptr,ch) (MCMP_CQ_HEAD(ptr,ch) == MCMP_CQ_TAIL(ptr,ch))
#define MCMP_CQ_ITEMS(ptr,ch)   ((char*)(MCMP_CQ_CHUNK(ptr,ch) + 1))
#define MCMP_CQ_ITEM(ptr,ch,it) (MCMP_CQ_ITEMS(ptr,ch) + (sizeof(uintptr_t) * it))

McmpChunkedQueue  mcmpChunkedQueueCreate  (const size_t length, const size_t alignment);
mcmpst            mcmpChunkedQueueDestroy (const McmpChunkedQueue cq);
mcmpst            mcmpChunkedQueueTake    (McmpChunkedQueue cq, const size_t chunkidx, uintptr_t* item);
mcmpst            mcmpChunkedQueuePush    (McmpChunkedQueue cq, const size_t chunkidx, const uintptr_t item);
#endif /** MCMP_UTIL_CHUNKED_QUEUE_H  */