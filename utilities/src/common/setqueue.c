#include <monarch/statos.h>
#include <stdatomic.h>
#include <string.h>
#include <malloc.h>
#include "monarch/util/setqueue.h"

mcmchsetqueue mcmchSetQueueCreate(const size_t bitslength) {
  const size_t            byteslength = (bitslength + 7UL) / 8UL;
  mcmchsetqueue_header_t* header = NULL;
  mcmchqueue_header*      queue_header = NULL;
  mcmchsetqueue           queue = (mcmchsetqueue)malloc(
    sizeof(mcmchsetqueue_header_t)
    + byteslength
    + sizeof(mcmchqueue_header)
    + (sizeof(mcmchsetqueue_index_t) * bitslength)
  );
  if (queue == NULL)
    return NULL;
  header = (mcmchsetqueue_header_t*)queue;
  header->bitslength = bitslength;
  header->byteslength = byteslength;
  queue_header = (mcmchqueue_header*)MCMCHSETQUEUE_GET_QUEUE(queue);
  queue_header->length = bitslength;
  queue_header->itsize = sizeof(mcmchsetqueue_index_t);
  queue_header->head = 0UL;
  queue_header->tail = 0UL;
  return queue;
}

void mcmchSetQueueDestroy(const mcmchsetqueue queue) {
  free(queue);
}

mcmchst mcmchSetQueuePush(mcmchsetqueue queue, const mcmchsetqueue_index_t idx) {
  const size_t bitslength = MCMCHSETQUEUE_GET_BITSLENGTH(queue);
  mcmchqueue   pqueue = MCMCHSETQUEUE_GET_QUEUE(queue);
  void*        offset = MCMCHQUEUE_PAYLOADPTR(pqueue);
  size_t       index;
  size_t       next;
  if (idx >= bitslength)
    MCMCHST_RETURN(MCMCHST_NOT_IMPLEMENTED);
  else if (mcmchBitmapIsEnabledBit(MCMCHSETQUEUE_GET_BITMAP(queue), idx))
    MCMCHST_RETURN(MCMCHST_NOT_IMPLEMENTED);
  do {
    index = atomic_load(&MCMCHQUEUE_HEAD(pqueue));
    next = (index + 1) % MCMCHQUEUE_LENGTH(pqueue);
    if (next == atomic_load(&MCMCHQUEUE_TAIL(pqueue))) {
      MCMCHST_RETURN(MCMCHST_NOT_IMPLEMENTED);
    }
  } while (!atomic_compare_exchange_weak(&MCMCHQUEUE_HEAD(pqueue), &index, next));
  memcpy(offset + (index * MCMCHQUEUE_ITEM_SIZE(pqueue)), &idx, MCMCHQUEUE_ITEM_SIZE(pqueue));
  mcmchBitmapEnableBit(MCMCHSETQUEUE_GET_BITMAP(queue), idx);
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchSetQueueTake(mcmchsetqueue queue, mcmchsetqueue_index_t* ind) {
  const size_t bitslength = MCMCHSETQUEUE_GET_BITSLENGTH(queue);
  mcmchqueue   pqueue = MCMCHSETQUEUE_GET_QUEUE(queue);
  void*        offset = MCMCHQUEUE_PAYLOADPTR(pqueue);
  size_t       index;
  size_t       next;
  do {
    index = atomic_load(&MCMCHQUEUE_TAIL(pqueue));
    next = (index + 1) % MCMCHQUEUE_LENGTH(pqueue);
    if (index == atomic_load(&MCMCHQUEUE_HEAD(pqueue)))
      MCMCHST_RETURN(MCMCHST_ISEMPTY);
    memcpy(ind, offset + (index * MCMCHQUEUE_ITEM_SIZE(pqueue)), MCMCHQUEUE_ITEM_SIZE(pqueue));
    mcmchBitmapDisableBit(MCMCHSETQUEUE_GET_BITMAP(queue), *ind);
  } while (!atomic_compare_exchange_weak(&MCMCHQUEUE_TAIL(pqueue), &index, next));
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}