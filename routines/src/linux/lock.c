#ifdef __unix__
#include <monarch/util/atomicbitmap.h>
#include <monarch/statos.h>
#include <pthread.h>
#include "monarch/routines/lock.h"

#define OS_PAGE_MEMORY_SIZE 4096UL
#define CACHE_LINE_SIZE     64UL

struct __attribute__((aligned(64UL))) McmchLockRegistryHeader_t {
  size_t length;
};

struct __attribute__((aligned(CACHE_LINE_SIZE))) McmchLockProtected_t {
  pthread_mutex_t mutex;
  char            padding_to_reg[CACHE_LINE_SIZE - sizeof(pthread_mutex_t)];
};

struct __attribute__((aligned(CACHE_LINE_SIZE))) McmchLockImpl_t {
  struct McmchLockProtected_t st;
  McmchLockRegistry           registry;
};

_Static_assert(OS_PAGE_MEMORY_SIZE % CACHE_LINE_SIZE == 0, "");

#define REGISTRIES_STATIC_SIZE (OS_PAGE_MEMORY_SIZE / sizeof(struct McmchLockProtected_t))
#define SLOTS_STATIC_SIZE      (REGISTRIES_STATIC_SIZE / 8)

static struct McmchLockProtected_t registries[REGISTRIES_STATIC_SIZE];
static uint8_t                     slots[SLOTS_STATIC_SIZE];

mcmchst mcmchLockCreate(McmchLock* lock, McmchLockRegistry reg) {
  bitidx_t slot;
  if (reg != NULL)
    MCMCHST_RETURN(MCMCHST_NOT_IMPLEMENTED);
  for (size_t tries = 0; tries < 100; ++tries) {
    slot = mcmchAtomicBitmapNextAvaibleBit(slots, 0, SLOTS_STATIC_SIZE);
    if (slot == -1)
      MCMCHST_RETURN(MCMCHST_OVERFLOW);
    else if (mcmchAtomicBitmapTryEnableBit(slots, slot) == MCMCHST_SUCCESS) {
      *lock = (McmchLock)(registries + slot);
      pthread_mutex_init((pthread_mutex_t*)(*lock), NULL);
      MCMCHST_RETURN(MCMCHST_SUCCESS);
    }
  }
  MCMCHST_RETURN(MCMCHST_OVERFLOW);
}

mcmchst mcmchLockAcquire(McmchLock lock) {
  pthread_mutex_lock((pthread_mutex_t*)lock);
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}
mcmchst mcmchLockRelease(McmchLock lock) {
  pthread_mutex_unlock((pthread_mutex_t*)lock);
  MCMCHST_RETURN(MCMCHST_SUCCESS); 
}

mcmchst mcmchLockDestroy(McmchLock lock) {
  if (!(lock >= (McmchLock)registries && lock < (McmchLock)(registries + REGISTRIES_STATIC_SIZE)))
    MCMCHST_RETURN(MCMCHST_NOT_IMPLEMENTED);
  bitidx_t slot = registries - (struct McmchLockProtected_t*)lock;
  pthread_mutex_destroy((pthread_mutex_t*)lock);
  mcmchBitmapDisableBit(slots, slot);
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

#endif /** __unix__  */