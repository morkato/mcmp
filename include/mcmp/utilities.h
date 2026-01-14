#ifndef MCMP_UTIL_H
#define MCMP_UTIL_H
#include <stdio.h>

#if defined(__unix)
  #include <unistd.h>
  #include <time.h>
  typedef time_t mcmptime;

  #define MCMPUTIL_SLEEP_MILIS(ms) usleep(ms * 1000)
#elif defined(_WIN32)
  #include <windows.h>
  typedef _FILETIME mcmchtime;

  #define MCMCHUTIL_SLEEP_MILIS(ms) Sleep(ms)
#endif

mcmptime mcmchCurrentTime ();
void      mcmchStrftime    (char* buffer, size_t maxlen, const char* format, const mcmptime time);
#endif