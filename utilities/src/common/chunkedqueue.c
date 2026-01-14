#include "mcmp/util/chunkedqueue.h"
#include <mcmp/statos.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

McmpChunkedQueue mcmpChunkedQueueCreate(const size_t length, const size_t alignment) {
  const size_t totalchunks = (length + (alignment - 1UL)) / alignment;
  const size_t normalized_length = totalchunks * alignment;
  const size_t chunksize = sizeof(struct McmpChunkInfo) + (alignment * sizeof(uintptr_t));
  McmpChunkedQueue queue = (McmpChunkedQueue)malloc(
    sizeof(struct McmpChunkedQueueHeader)
    + (totalchunks * chunksize)
  );
  if (queue == NULL) {
    MCMPST_SETRET(MCMPST_OUT_OF_MEM);
    return NULL;
  } else if (normalized_length != length) {
    printf("[Monarch] WARNING: Queue size requested (%zu) required padding to align chunks. "
           "Allocating %zu total items (Slack: %zu items).\n", length, normalized_length, (normalized_length - length));
  }
  MCMP_CQ_ALIGNMENT(queue) = alignment;
  MCMP_CQ_LENGTH(queue) = normalized_length;
  MCMP_CQ_CHUNKSIZE(queue) = chunksize;
  for (size_t i = 0UL; i < totalchunks; ++i) {
    struct McmpChunkInfo* chunk = MCMP_CQ_CHUNK(queue, i);
    chunk->head = 0UL;
    chunk->tail = 0UL;
  }
  return queue;
}

mcmpst mcmpChunkedQueueDestroy(const McmpChunkedQueue cq) {
  free(cq);
  MCMPST_RETURN(MCMPST_SUCCESS);
}

mcmpst mcmpChunkedQueueTake(McmpChunkedQueue cq, const size_t chunkidx, uintptr_t* item) {
  struct McmpChunkInfo* chunk = MCMP_CQ_CHUNK(cq, chunkidx);
  size_t index;
  size_t next;
  do {
    index = __atomic_load_n(&(chunk->tail), __ATOMIC_ACQUIRE);
    next = (index + 1) % MCMP_CQ_ALIGNMENT(cq);
    if (__atomic_load_n(&(chunk->head), __ATOMIC_ACQUIRE) == index)
      MCMPST_RETURN(MCMPST_OVERFLOW);
  } while (!__atomic_compare_exchange_n(
    &(chunk->tail),
    &index,
    next,
    1,
    __ATOMIC_ACQ_REL,
    __ATOMIC_RELAXED
  ));
  memcpy(item, MCMP_CQ_ITEM(cq, chunkidx, index), sizeof(uintptr_t));
  MCMPST_RETURN(MCMPST_SUCCESS);
}

mcmpst mcmpChunkedQueuePush(McmpChunkedQueue cq, const size_t chunkidx, const uintptr_t item) {
  struct McmpChunkInfo* chunk = MCMP_CQ_CHUNK(cq, chunkidx);
  size_t index;
  size_t next;
  do {
    index = __atomic_load_n(&(chunk->head), __ATOMIC_ACQUIRE);
    next = (index + 1) % MCMP_CQ_ALIGNMENT(cq);
    if (__atomic_load_n(&(chunk->tail), __ATOMIC_ACQUIRE) == next)
      MCMPST_RETURN(MCMPST_OVERFLOW);
  } while (!__atomic_compare_exchange_n(
    &(chunk->head),
    &index,
    next,
    1,
    __ATOMIC_RELEASE,
    __ATOMIC_RELAXED
  ));
  memcpy(MCMP_CQ_ITEM(cq, chunkidx, index), &item, sizeof(uintptr_t));
  MCMPST_RETURN(MCMPST_SUCCESS);
}