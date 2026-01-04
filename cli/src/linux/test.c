#include <monarch/routines/worker.h>
#include <monarch/util/setqueue.h>
#include <monarch/util/chunkedqueue.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

int tasks[1000];

void task(void* arg) {
  uintptr_t job = (uintptr_t)arg;
  usleep((rand() % 400 + 100) * 1000);
  tasks[job] = 1;
}

int main(void) {
  // srand(time(NULL));
  // mcmchsetqueue queue = mcmchSetQueueCreate(1000UL);
  // McmchWorkerExecutor executor = mcmchWorkerCreate(task, queue, 16UL);
  // mcmchWorkerStart(executor);
  // printf("[Main] Teste de estresse iniciado!\n");
  // for (size_t j = 0; j < 1000UL; ++j) {
  //   if (j % 16 == 0)
  //     mcmchWorkerBeat(executor);
  //   mcmchSetQueuePush(queue, j);
  // }
  // printf("[Main] Teste de estresse finalizado. Pulsando até acabar a fila!\n");
  // while (!MCMCHQUEUE_ISEMPTY(MCMCHSETQUEUE_GET_QUEUE(queue))) {
  //   mcmchWorkerBeat(executor);
  //   usleep(500000UL);
  // }
  // mcmchWorkerDestroy(executor);
  // mcmchSetQueueDestroy(queue);
  // printf("[Main] Validando o teste. Verificando se houve algum item perdido...!\n");
  // for (size_t i = 0; i < 1000; ++i) {
  //   if (!tasks[i]) {
  //     printf("O item: %lu foi perdido!\n", i);
  //     return 0;
  //   }
  // }
  // printf("Nenhum item foi perdido!\n");
  McmchChunkedQueue queue = mcmchChunkedQueueCreate(96UL, 6UL);
  uintptr_t val;
  mcmchChunkedQueuePush(queue, 0UL, 2UL);
  mcmchChunkedQueuePush(queue, 0UL, 5UL);
  mcmchChunkedQueuePush(queue, 1UL, 4UL);
  mcmchChunkedQueuePush(queue, 1UL, 10UL);
  mcmchChunkedQueuePush(queue, 15UL, 16UL);
  
  printf("Take Status: %i\n", mcmchChunkedQueueTake(queue, 0UL, &val));
  printf("Taked: %lu\n", val);
  printf("Take Status: %i\n", mcmchChunkedQueueTake(queue, 0UL, &val));
  printf("Taked: %lu\n", val);
  printf("Take Status: %i\n", mcmchChunkedQueueTake(queue, 1UL, &val));
  printf("Taked: %lu\n", val);
  printf("Take Status: %i\n", mcmchChunkedQueueTake(queue, 1UL, &val));
  printf("Taked: %lu\n", val);
  printf("Take Status: %i\n", mcmchChunkedQueueTake(queue, 15UL, &val));
  printf("Taked: %lu\n", val);
  printf("Take Status: %i\n", mcmchChunkedQueueTake(queue, 1UL, &val));
  printf("Take Status: %i\n", mcmchChunkedQueueTake(queue, 1UL, &val));
  mcmchChunkedQueueDestroy(queue);
  return 0;
}