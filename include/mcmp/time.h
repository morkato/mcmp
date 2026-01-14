#ifndef MCMP_TIME_H
#define MCMP_TIME_H 1

#ifdef __unix__
  #ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
  #endif /** _POSIX_C_SOURCE  */

  #include <time.h>

  #define MCMPTIME_OS_CLOCK_SPEC struct timespec
  #define MCMPTIME_SPEC_GETSECS(tm) ((tm).tv_sec)
  #define MCMPTIME_SPEC_GETNANOS(tm) ((tm).tv_nsec)
  #define MCMPTIME_NULL_TIME_SPEC {0LL, 0LL}
#endif /** __unix__  */

#ifdef MCMPTIME_OS_CLOCK_SPEC
  #include "mcmp/stat.h"

  typedef MCMPTIME_OS_CLOCK_SPEC McmpTimeSpec;

  mcmpst mcmpTimeGetMonotanic(McmpTimeSpec* spec);
#endif /** MCMPTIME_OS_CLOCK_SPEC  */

#endif /** MCMP_TIME_H  */