#ifdef __linux
#include <time.h>

#include "monarch/utilities.h"

mcmchtime mcmchCurrentTime() {
  return (mcmchtime)time(NULL);
}

void mcmchStrftime(char* buffer, size_t maxlen, const char* format, const mcmchtime time) {
  struct tm* time_info = localtime(&time);
  strftime(buffer, maxlen, "%Y/%m/%dT%H:%M:%S", time_info);
}
#endif