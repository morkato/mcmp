#include <stdatomic.h>
#include <malloc.h>
#include <string.h>

#include "monarch/util/queue.h"

mcmchqueue mcmchQueueCreate(const size_t item_size, const size_t length) {
  size_t             abslength = item_size * length;
  mcmchqueue_header* header    = NULL;
  mcmchqueue         queue     = (mcmchqueue)malloc(sizeof(mcmchqueue_header) + abslength);
  if (queue == NULL)
    return NULL;
  header = (mcmchqueue_header*)queue;
  header->length = length;
  header->itsize = item_size;
  header->head = 0UL;
  header->tail = 0UL;
  return queue;
}

void mcmchQueueDestroy(const mcmchqueue queue) {
  free(queue);
}

int mcmchQueuePush(mcmchqueue queue, void* item) {
  size_t item_size = MCMCHQUEUE_ITEM_SIZE(queue);
  void* offset = MCMCHQUEUE_PAYLOADPTR(queue);
  size_t index = atomic_load(&MCMCHQUEUE_HEAD(queue));
  size_t next = (index + 1) % MCMCHQUEUE_LENGTH(queue);
  if (next == atomic_load(&MCMCHQUEUE_TAIL(queue)))
    return -1;
  memcpy(offset + (index * item_size), item, item_size);
  atomic_store(&MCMCHQUEUE_HEAD(queue), next);
  return index;
}