#ifndef MONARCH_UTIL_H
#define MONARCH_UTIL_H
#include <stdio.h>

#if defined(__unix)
  #include <unistd.h>
  #include <time.h>
  typedef time_t mcmchtime;

  #define MCMCHUTIL_SLEEP_MILIS(ms) usleep(ms * 1000)
#elif defined(_WIN32)
  #include <windows.h>
  typedef _FILETIME mcmchtime;

  #define MCMCHUTIL_SLEEP_MILIS(ms) Sleep(ms)
#endif

mcmchtime mcmchCurrentTime ();
void      mcmchStrftime    (char* buffer, size_t maxlen, const char* format, const mcmchtime time);
#endif