// #include <monarch/util/consumer.h>
// #include <monarch/util/routine.h>
#include <monarch/util/bitmap.h>
#include <monarch/util/setqueue.h>
#include <monarch/routines/monitor.h>
#include <monarch/routines/worker.h>
#include <monarch/routines/lock.h>
// #include <monarch/util/pool.h>
#include <monarch/statos.h>
#include <unistd.h>
#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// McmchConsumerRoutine routine;
// McmchSignalLock      lock;

// static mcmchst get_handler(void* ctx, void** taked) {
//   mcmchSetQueueTake((mcmchsetqueue)ctx, (void*)taked);
//   if (mcmchstret != MCMCHST_SUCCESS)
//     return MCMCHST_SUCCESS;
//   return MCMCHCONSUMER_TAKED;
// }

// static void proccess(void* k) {
//   printf("Meu slot: %u no pool: 0x%p Obtive: %lu.\n", mcmchpoolslot, mcmchpool, (uint64_t)k);
//   sleep(2);
// }

void proccess(void* arg) {
  printf("[Monitor/%lu] Processaria: %lu.\n", mcmchmonitor, (uint64_t)arg);
  sleep(10);
}

int procced[100];

void handler(void* arg) {
  uintptr_t job = (uintptr_t)arg;
  usleep((rand() % 400 + 100) * 1000);
  procced[job] = 1;
}

int main(void) {
  srand(time(NULL));
  
  printf("=== Stress test McmchWorkerExecutor ===\n");
  size_t total_jobs = 1000;
  size_t pool_size = 16;

  mcmchsetqueue queue = mcmchSetQueueCreate(total_jobs);
  McmchWorkerExecutor executor = mcmchWorkerCreate(handler, queue, pool_size);
  mcmchWorkerStart(executor);

  size_t step = 50;
  for (size_t i = 0; i < total_jobs; i += step) {
    for (size_t j = 0; j < step && (i + j) < total_jobs; j++) {
      mcmchSetQueuePush(queue, i + j);
    }
    printf("[Main] Beat enviado (%zu jobs na fila)\n", i + step);
    mcmchWorkerBeat(executor);
    usleep(200 * 1000);
  }

  printf("[Main] Esvaziando a fila...\n");
  while (!MCMCHQUEUE_ISEMPTY(MCMCHSETQUEUE_GET_QUEUE(queue))) {
    usleep(500000);
    mcmchWorkerBeat(executor);
  }

  mcmchWorkerDestroy(executor);
  mcmchSetQueueDestroy(queue);

  printf("[Main] Stress test finalizado com sucesso.\n");
  return 0;
}

// int main() {
//   mcmchsetqueue queue = mcmchSetQueueCreate(16);
//   McmchWorkerExecutor executor = mcmchWorkerCreate(proccess, queue, 16UL);
//   mcmchWorkerStart(executor);
//   mcmchSetQueuePush(queue, 2);
//   mcmchSetQueuePush(queue, 5);
//   mcmchWorkerBeat(executor);
//   sleep(2);
//   printf("Beat!\n");
//   mcmchSetQueuePush(queue, 7);
//   mcmchSetQueuePush(queue, 3);
//   mcmchSetQueuePush(queue, 15);
//   mcmchWorkerBeat(executor);
//   sleep(1);
//   mcmchWorkerDestroy(executor);
//   mcmchSetQueueDestroy(queue);
//   // mcmchSignalLockInitilize(&lock);  
//   // McmchExecutionPool pool = mcmchExecutionPoolCreate("generic-consumer", 8U);
//   // mcmchConsumerRoutineCreate(
//   //   &routine, &lock,
//   //   get_handler, proccess, queue
//   // );
//   // mcmchConsumerExcutionPoolLink(&routine, pool);
//   // mcmchExecutionPoolStart(pool);
//   // mcmchSetQueuePush(queue, 5);
//   // mcmchSignalDispatch(&lock);
//   // mcmchSetQueuePush(queue, 5);
//   // mcmchSignalDispatch(&lock);
//   // mcmchSetQueuePush(queue, 2);
//   // mcmchSignalDispatch(&lock);
//   // mcmchSetQueuePush(queue, 3);
//   // mcmchSignalDispatch(&lock);
//   // sleep(5);
//   // printf("-------------------\n");
//   // routine.running = 0;
//   // mcmchExecutionPoolDestroy(pool);
//   return 0;
// }