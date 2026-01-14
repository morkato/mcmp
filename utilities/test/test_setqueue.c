#include <mcmp/util/setqueue.h>
#include <mcmp/util/bitmap.h>
#include <stddef.h>
#include <stdio.h>

int main() {
  mcmpsetqueue queue = mcmpSetQueueCreate(16UL);
  mcmpsetqueue_index_t ind;
  if (queue == NULL)
    return 1;
  mcmpSetQueuePush(queue, 0);
  mcmpSetQueuePush(queue, 2);
  mcmpSetQueuePush(queue, 5);
  mcmpSetQueuePush(queue, 7);
  mcmpSetQueuePush(queue, 15);
  mcmpSetQueuePush(queue, 13);
  puts("READY BITMAP: ");
  for (size_t idx = 0; idx < 16UL; ++idx) {
    if (idx % 8 == 0)
      putchar(' ');
    putchar(mcmpBitmapIsEnabledBit(MCMP_SQU_GET_READY_BITMAP(queue), idx) ? '1' : '0');
  }
  putchar('\n');
  puts("VALUES BITMAP: ");
  for (size_t idx = 0; idx < 16UL; ++idx) {
    if (idx % 8 == 0)
      putchar(' ');
    putchar(mcmpBitmapIsEnabledBit(MCMP_SQU_GET_VALUES_BITMAP(queue), idx) ? '1' : '0');
  }
  putchar('\n');
  while (MCMP_SQU_GET_HEAD(queue) != MCMP_SQU_GET_TAIL((queue))) {
    mcmpSetQueueTake(queue, &ind);
    printf("Taked: %lu\n", ind);
    puts("READY BITMAP: ");
    for (size_t idx = 0; idx < 16UL; ++idx) {
      if (idx % 8 == 0)
        putchar(' ');
      putchar(mcmpBitmapIsEnabledBit(MCMP_SQU_GET_READY_BITMAP(queue), idx) ? '1' : '0');
    }
    putchar('\n');
    puts("VALUES BITMAP: ");
    for (size_t idx = 0; idx < 16UL; ++idx) {
      if (idx % 8 == 0)
        putchar(' ');
      putchar(mcmpBitmapIsEnabledBit(MCMP_SQU_GET_VALUES_BITMAP(queue), idx) ? '1' : '0');
    }
    putchar('\n');
  }
  return 0;
}