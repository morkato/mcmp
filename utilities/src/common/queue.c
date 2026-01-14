#include <malloc.h>
#include <string.h>

#include "mcmp/util/queue.h"

mcmpqueue mcmchQueueCreate(const size_t item_size, const size_t length) {
  size_t             abslength = item_size * length;
  mcmpqueue_header*  header    = NULL;
  mcmpqueue          queue     = (mcmpqueue)malloc(sizeof(mcmpqueue_header) + abslength);
  if (queue == NULL)
    return NULL;
  header = (mcmpqueue_header*)queue;
  header->length = length;
  header->itsize = item_size;
  header->head = 0UL;
  header->tail = 0UL;
  return queue;
}

void mcmchQueueDestroy(const mcmpqueue queue) {
  free(queue);
}

int mcmchQueuePush(mcmpqueue queue, void* item) {
  size_t item_size = MCMP_QU_ITEM_SIZE(queue);
  void* offset = MCMP_QU_PAYLOADPTR(queue);
  size_t index = __atomic_load_n(&MCMP_QU_HEAD(queue), __ATOMIC_ACQUIRE);
  size_t next = (index + 1) % MCMP_QU_LENGTH(queue);
  if (next == __atomic_load_n(&MCMP_QU_TAIL(queue), __ATOMIC_ACQUIRE))
    return -1;
  memcpy(offset + (index * item_size), item, item_size);
  __atomic_store_n(&MCMP_QU_HEAD(queue), next, __ATOMIC_RELEASE);
  return index;
}