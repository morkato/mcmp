#ifndef MCMP_LOG_H
#define MCMP_LOG_H
#include "mcmp/mcmp.h"

#define MCMPLOG_MAX_LINE_SIZE 4096

#define MCMPLOG_TRACE 0
#define MCMPLOG_DEBUG 1
#define MCMPLOG_INFO  2
#define MCMPLOG_WARN  3
#define MCMPLOG_ERROR 4

mcmpst mcmpOpenLogFile  (mcmpenv env);
mcmpst mcmpCloseLogFile (mcmpenv env);
mcmpst mcmplogWriteLine (const mcmpenv env, const mcmploglevel level, const char* line);
mcmpst mcmplogFormatted (const mcmpenv env, const mcmploglevel level, const mcmplogflags flags, const char* message);
mcmpst mcmplog          (const mcmpenv env, const mcmploglevel level, const mcmplogflags flags, const char* message, ...);
#endif // MCMP_LOG_H