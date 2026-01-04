#include "monarch/util/chunkedqueue.h"
#include <monarch/statos.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

McmchChunkedQueue mcmchChunkedQueueCreate(const size_t length, const size_t alignment) {
  const size_t totalchunks = (length + (alignment - 1UL)) / alignment;
  const size_t normalized_length = totalchunks * alignment;
  const size_t chunksize = sizeof(struct McmchChunkInfo) + (alignment * sizeof(uintptr_t));
  McmchChunkedQueue queue = (McmchChunkedQueue)malloc(
    sizeof(struct McmchChunkedQueueHeader)
    + (totalchunks * chunksize)
  );
  if (queue == NULL) {
    MCMCHST_SETRET(MCMCHST_OUT_OF_MEM);
    return NULL;
  } else if (normalized_length != length) {
    printf("[Monarch] WARNING: Queue size requested (%zu) required padding to align chunks. "
           "Allocating %zu total items (Slack: %zu items).\n", length, normalized_length, (normalized_length - length));
  }
  MCH_CQ_ALIGNMENT(queue) = alignment;
  MCH_CQ_LENGTH(queue) = normalized_length;
  MCH_CQ_CHUNKSIZE(queue) = chunksize;
  for (size_t i = 0UL; i < totalchunks; ++i) {
    struct McmchChunkInfo* chunk = MCH_CQ_CHUNK(queue, i);
    chunk->head = 0UL;
    chunk->tail = 0UL;
  }
  return queue;
}

mcmchst mcmchChunkedQueueDestroy(const McmchChunkedQueue cq) {
  free(cq);
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchChunkedQueueTake(McmchChunkedQueue cq, const size_t chunkidx, uintptr_t* item) {
  struct McmchChunkInfo* chunk = MCH_CQ_CHUNK(cq, chunkidx);
  size_t index;
  size_t next;
  do {
    index = __atomic_load_n(&(chunk->tail), __ATOMIC_ACQUIRE);
    next = (index + 1) % MCH_CQ_ALIGNMENT(cq);
    if (__atomic_load_n(&(chunk->head), __ATOMIC_ACQUIRE) == index)
      MCMCHST_RETURN(MCMCHST_OVERFLOW);
  } while (!__atomic_compare_exchange_n(
    &(chunk->tail),
    &index,
    next,
    1,
    __ATOMIC_ACQ_REL,
    __ATOMIC_RELAXED
  ));
  memcpy(item, MCH_CQ_ITEM(cq, chunkidx, index), sizeof(uintptr_t));
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchChunkedQueuePush(McmchChunkedQueue cq, const size_t chunkidx, const uintptr_t item) {
  struct McmchChunkInfo* chunk = MCH_CQ_CHUNK(cq, chunkidx);
  size_t index;
  size_t next;
  do {
    index = __atomic_load_n(&(chunk->head), __ATOMIC_ACQUIRE);
    next = (index + 1) % MCH_CQ_ALIGNMENT(cq);
    if (__atomic_load_n(&(chunk->tail), __ATOMIC_ACQUIRE) == next)
      MCMCHST_RETURN(MCMCHST_OVERFLOW);
  } while (!__atomic_compare_exchange_n(
    &(chunk->head),
    &index,
    next,
    1,
    __ATOMIC_RELEASE,
    __ATOMIC_RELAXED
  ));
  memcpy(MCH_CQ_ITEM(cq, chunkidx, index), &item, sizeof(uintptr_t));
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}