#ifndef MCMP_QUEUE_H
#define MCMP_QUEUE_H 1
#include <stdint.h>
#include <stddef.h>

#define MCMP_QU_CASTHEADER(queue)  ((mcmpqueue_header*)(queue))
#define MCMP_QU_PAYLOADPTR(queue)  ((queue) + sizeof(mcmpqueue_header))
#define MCMP_QU_LENGTH(queue)      (MCMP_QU_CASTHEADER(queue)->length)
#define MCMP_QU_ITEM_SIZE(queue)   (MCMP_QU_CASTHEADER(queue)->itsize)
#define MCMP_QU_HEAD(queue)        (MCMP_QU_CASTHEADER(queue)->head)
#define MCMP_QU_TAIL(queue)        (MCMP_QU_CASTHEADER(queue)->tail)
#define MCMP_QU_ITEM(queue, index) (MCMP_QU_PAYLOADPTR(queue) + (MCMP_QU_ITEM_SIZE(queue) * index))

#define MCMP_QU_ISEMPTY(queue)  (MCMP_QU_CASTHEADER(queue)->head == MCMP_QU_CASTHEADER(queue)->tail)
#define MCMP_QU_NONEMPTY(queue) (!MCMP_QU_ISEMPTY(queue))

typedef struct mcmpqueue_header {
  size_t length;
  size_t itsize;
  size_t head;
  size_t tail;
} mcmpqueue_header;

typedef void* mcmpqueue;

mcmpqueue  mcmpQueueCreate  (const size_t item_size, const size_t length);
void       mcmpQueueDestroy (const mcmpqueue queue);
int        mcmpQueuePush    (mcmpqueue queue, void* item);
#endif /** MCMP_QUEUE_H  */