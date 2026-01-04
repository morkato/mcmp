#ifndef MONARCH_ROUTINES_LOCK_H
#define MONARCH_ROUTINES_LOCK_H 1
#include <monarch/stat.h>
#include <stdint.h>

typedef struct McmchLockRegistry* McmchLockRegistry;
typedef uintptr_t                 McmchLock;

mcmchst mcmchLockCreate  (McmchLock* lock, McmchLockRegistry reg);
mcmchst mcmchLockAcquire (McmchLock lock);
mcmchst mcmchLockRelease (McmchLock lock);
mcmchst mcmchLockDestroy (McmchLock lock);

#endif /** MONARCH_ROUTINES_LOCK_H  */