#ifndef MONARCH_TIME_H
#define MONARCH_TIME_H 1

#ifdef __unix__
  #ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
  #endif /** _POSIX_C_SOURCE  */

  #include <time.h>

  #define MONARCHTIME_OS_CLOCK_SPEC struct timespec
  #define MCMCHTIME_SPEC_GETSECS(tm) ((tm).tv_sec)
  #define MCMCHTIME_SPEC_GETNANOS(tm) ((tm).tv_nsec)
  #define MCMCHTIME_NULL_TIME_SPEC {0LL, 0LL}
#endif /** __unix__  */

#ifdef MONARCHTIME_OS_CLOCK_SPEC
  #include "monarch/stat.h"

  typedef MONARCHTIME_OS_CLOCK_SPEC McmchTimeSpec;

  mcmchst mcmchTimeGetMonotanic(McmchTimeSpec* spec);
#endif /** MONARCHTIME_OS_CLOCK_SPEC  */

#endif /** MONARCH_TIME_H  */