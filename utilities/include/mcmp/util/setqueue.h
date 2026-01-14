#ifndef MCMP_UTIL_SETQUEUE_H
#define MCMP_UTIL_SETQUEUE_H 1
#include <mcmp/stat.h>
#include <stdint.h>
#include <stddef.h>

#define MCMP_SQU_CAST(squ) ((mcmpsetqueue_header_t*)(squ))
#define MCMP_SQU_GET_BITSLENGTH(squ) \
  ((size_t)(MCMP_SQU_CAST(squ)->length))
#define MCMP_SQU_GET_BYTESLENGTH(squ) \
  ((size_t)(MCMP_SQU_CAST(squ)->byteslength))
#define MCMP_SQU_GET_HEAD(squ) \
  (MCMP_SQU_CAST(squ)->head)
#define MCMP_SQU_GET_TAIL(squ) \
  (MCMP_SQU_CAST(squ)->tail)
#define MCMP_SQU_GET_VALUES_BITMAP(squ) \
  ((uint8_t*)(MCMP_SQU_CAST(squ) + 1))
#define MCMP_SQU_GET_READY_BITMAP(squ) \
  (MCMP_SQU_GET_VALUES_BITMAP(squ) + MCMP_SQU_GET_BYTESLENGTH(squ))
#define MCMP_SQU_GET_ITEMS(squ) \
  ((mcmpsetqueue_index_t*)(MCMP_SQU_GET_READY_BITMAP(squ) + MCMP_SQU_GET_BYTESLENGTH(squ)))
#define MCMP_SQU_GET_ITEM(squ,ind) \
  (MCMP_SQU_GET_ITEMS(squ) + ind)

typedef uintptr_t mcmpsetqueue_index_t;
typedef struct __attribute__((packed, aligned(64ULL))) mcmpsetqueue_header_t {
  size_t byteslength;
  size_t length;
  size_t head;
  size_t tail;
  const uint8_t __padding[32];
} mcmpsetqueue_header_t;
typedef void* mcmpsetqueue;

_Static_assert(sizeof(mcmpsetqueue_header_t) == 64UL, "a");

mcmpsetqueue  mcmpSetQueueCreate  (const size_t length);
void          mcmpSetQueueDestroy (const mcmpsetqueue squ);
mcmpst        mcmpSetQueuePush    (mcmpsetqueue squ, const mcmpsetqueue_index_t idx);
mcmpst        mcmpSetQueueTake    (mcmpsetqueue squ, mcmpsetqueue_index_t* ind);
#endif // MCMP_UTIL_SETQUEUE_H