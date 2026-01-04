#ifndef MONARCH_LOG_H
#define MONARCH_LOG_H
#include "monarch.h"

#define MCMCHLOG_MAX_LINE_SIZE 4096

#define MCMCHLOG_TRACE 0
#define MCMCHLOG_DEBUG 1
#define MCMCHLOG_INFO  2
#define MCMCHLOG_WARN  3
#define MCMCHLOG_ERROR 4

mcmchst mcmchOpenLogFile  (mcmchenv env);
mcmchst mcmchCloseLogFile (mcmchenv env);
mcmchst mcmchlogWriteLine (const mcmchenv env, const mcmchloglevel level, const char* line);
mcmchst mcmchlogFormatted (const mcmchenv env, const mcmchloglevel level, const mcmchlogflags flags, const char* message);
mcmchst mcmchlog          (const mcmchenv env, const mcmchloglevel level, const mcmchlogflags flags, const char* message, ...);
#endif // MONARCH_LOG_H