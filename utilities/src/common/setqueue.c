#include "mcmp/util/setqueue.h"
#include "mcmp/util/atomicbitmap.h"

#include <mcmp/statos.h>
#include <string.h>
#include <stdlib.h>

mcmpsetqueue mcmpSetQueueCreate(const size_t length) {
  const size_t           byteslength = (length + 7UL) / 8UL;
  mcmpsetqueue_header_t* header = NULL;
  mcmpsetqueue           squ = (mcmpsetqueue)malloc(  
    sizeof(mcmpsetqueue_header_t)
    + (byteslength * 2)
    + (sizeof(mcmpsetqueue_index_t) * length)
  );
  if (squ == NULL) {
    MCMPST_SETRET(MCMPST_OUT_OF_MEM);
    return NULL;
  }
  header = (mcmpsetqueue_header_t*)squ;
  header->byteslength = byteslength;
  header->length = length;
  header->head = 0UL;
  header->tail = 0UL;
  memset(MCMP_SQU_GET_VALUES_BITMAP(squ), 0, byteslength * 2);
  return squ;
}

void mcmpSetQueueDestroy(const mcmpsetqueue squ) {
  free(squ);
}

mcmpst mcmpSetQueuePush(
  mcmpsetqueue               squ,
  const mcmpsetqueue_index_t ind
) {
  const size_t  bitslength = MCMP_SQU_GET_BITSLENGTH(squ);
  const uint8_t mask_set = (1 << (ind % 8));
  uint8_t       mask_ready;
  uint8_t*      word_set;
  uint8_t*      word_ready;
  uint8_t       expected_hashset;
  uint8_t       expected_ready_value;
  size_t        tail;
  size_t        index;
  size_t        next;
  if (ind >= bitslength)
    MCMPST_RETURN(MCMPST_OVERFLOW);
  word_set = MCMP_SQU_GET_VALUES_BITMAP(squ) + (ind / 8);
  expected_hashset = __atomic_load_n(word_set, __ATOMIC_ACQUIRE);
  if (expected_hashset & mask_set)
    MCMPST_RETURN(MCMPST_ALROP);
  else if (!__atomic_compare_exchange_n(
    word_set,
    &expected_hashset,
    expected_hashset | mask_set,
    0,
    __ATOMIC_ACQ_REL,
    __ATOMIC_RELAXED
  )) MCMPST_RETURN(MCMPST_RETRYOP);
  do {
    tail = __atomic_load_n(
      &MCMP_SQU_GET_TAIL(squ),
      __ATOMIC_ACQUIRE
    );
    index = __atomic_load_n(
      &MCMP_SQU_GET_HEAD(squ),
      __ATOMIC_ACQUIRE
    );
    next = (index + 1) % bitslength;
    if (next == tail && mcmpAtomicBitmapIsEnabledBit(MCMP_SQU_GET_READY_BITMAP(squ), tail)) {
      __atomic_fetch_and(word_set, ~mask_set, __ATOMIC_RELEASE);
      MCMPST_RETURN(MCMPST_OVERFLOW);
    }
  } while (!__atomic_compare_exchange_n(
    &MCMP_SQU_GET_HEAD(squ),
    &index,
    next,
    0,
    __ATOMIC_ACQ_REL,
    __ATOMIC_RELAXED
  ));
  word_ready = MCMP_SQU_GET_READY_BITMAP(squ) + (index / 8);
  mask_ready = (1 << (index % 8));
  __atomic_store_n(MCMP_SQU_GET_ITEM(squ, index), ind, __ATOMIC_RELEASE);
  do {
    expected_ready_value = __atomic_load_n(word_ready, __ATOMIC_ACQUIRE);
  } while (!__atomic_compare_exchange_n(
    word_ready,
    &expected_ready_value,
    expected_ready_value | mask_ready,
    0,
    __ATOMIC_ACQ_REL,
    __ATOMIC_RELAXED
  ));
  MCMPST_RETURN(MCMPST_SUCCESS);
}

mcmpst mcmpSetQueueTake(mcmpsetqueue squ, mcmpsetqueue_index_t* ind) {
  const size_t bitslength = MCMP_SQU_GET_BITSLENGTH(squ);
  uint8_t      mask_ready;
  uint8_t      mask_set;
  uint8_t*     word_set;
  uint8_t*     word_ready;
  uint8_t      expected_hashset;
  uint8_t      expected_ready_value;
  size_t       index;
  size_t       next;
  do {
    index = __atomic_load_n(&MCMP_SQU_GET_TAIL(squ), __ATOMIC_ACQUIRE);
    if (index == __atomic_load_n(
      &MCMP_SQU_GET_HEAD(squ),
      __ATOMIC_ACQUIRE
    )) MCMPST_RETURN(MCMPST_ISEMPTY);
    word_ready = MCMP_SQU_GET_READY_BITMAP(squ) + (index / 8);
    mask_ready = (1 << (index % 8));
    if ((__atomic_load_n(word_ready, __ATOMIC_ACQUIRE) & mask_ready) == 0)
      MCMPST_RETURN(MCMPST_RETRYOP);
    next = (index + 1) % bitslength;
  } while (!__atomic_compare_exchange_n(
    &MCMP_SQU_GET_TAIL(squ),
    &index,
    next,
    0,
    __ATOMIC_ACQ_REL,
    __ATOMIC_RELAXED
  ));
  *ind = MCMP_SQU_GET_ITEMS(squ)[index];
  word_set = MCMP_SQU_GET_VALUES_BITMAP(squ) + ((*ind) / 8);
  mask_set = (1 << (*ind) % 8);
  do {
    expected_hashset = __atomic_load_n(word_set, __ATOMIC_ACQUIRE);
  } while (!__atomic_compare_exchange_n(
    word_set,
    &expected_hashset,
    expected_hashset & (~mask_set),
    0,
    __ATOMIC_ACQ_REL,
    __ATOMIC_RELAXED
  ));
  do {
    expected_ready_value = __atomic_load_n(word_ready, __ATOMIC_ACQUIRE);
  } while (!__atomic_compare_exchange_n(
    word_ready,
    &expected_ready_value,
    expected_ready_value & (~mask_ready),
    0,
    __ATOMIC_ACQ_REL,
    __ATOMIC_RELAXED
  ));
  MCMPST_RETURN(MCMPST_SUCCESS);
}