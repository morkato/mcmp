#include "monarch/statos.h"
#include "monarch/time.h"

mcmchst mcmchTimeGetMonotanic(McmchTimeSpec* spec) {
  clock_gettime(CLOCK_MONOTONIC, (struct timespec*)spec);
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}