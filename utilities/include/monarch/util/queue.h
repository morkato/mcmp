#ifndef MONARCH_QUEUE_H
#define MONARCH_QUEUE_H 1
#include <stdint.h>
#include <stddef.h>

#define MCMCHQUEUE_CASTHEADER(queue)  ((mcmchqueue_header*)(queue))
#define MCMCHQUEUE_PAYLOADPTR(queue)  ((queue) + sizeof(mcmchqueue_header))
#define MCMCHQUEUE_LENGTH(queue)      (MCMCHQUEUE_CASTHEADER(queue)->length)
#define MCMCHQUEUE_ITEM_SIZE(queue)   (MCMCHQUEUE_CASTHEADER(queue)->itsize)
#define MCMCHQUEUE_HEAD(queue)        (MCMCHQUEUE_CASTHEADER(queue)->head)
#define MCMCHQUEUE_TAIL(queue)        (MCMCHQUEUE_CASTHEADER(queue)->tail)
#define MCMCHQUEUE_ITEM(queue, index) (MCMCHQUEUE_PAYLOADPTR(queue) + (MCMCHQUEUE_ITEM_SIZE(queue) * index))

#define MCMCHQUEUE_ISEMPTY(queue)  (MCMCHQUEUE_CASTHEADER(queue)->head == MCMCHQUEUE_CASTHEADER(queue)->tail)
#define MCMCHQUEUE_NONEMPTY(queue) (!MCMCHQUEUE_ISEMPTY(queue))

typedef struct mcmchqueue_header {
  size_t length;
  size_t itsize;
  size_t head;
  size_t tail;
} mcmchqueue_header;

typedef void* mcmchqueue;

mcmchqueue mcmchQueueCreate  (const size_t item_size, const size_t length);
void       mcmchQueueDestroy (const mcmchqueue queue);
int        mcmchQueuePush    (mcmchqueue queue, void* item);
#endif