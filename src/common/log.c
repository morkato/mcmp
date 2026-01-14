#include <stdarg.h>

#include "mcmp/utilities.h"
#include "mcmp/log.h"

static const char* loglevels[] = {"TRACE", "DEBUG", "INFO ", "WARN ", "ERROR"};

mcmpst mcmplogFormatted(const mcmpenv env, const mcmploglevel level, const mcmplogflags flags, const char* message) {
  const mcmptime now = mcmchCurrentTime();
  char formatted_time[64];
  char fullmessage[MCMPLOG_MAX_LINE_SIZE];
  mcmchStrftime(formatted_time, sizeof(formatted_time), "%Y/%m/%dT%H:%M:%S", now);
  snprintf(fullmessage, MCMPLOG_MAX_LINE_SIZE, "[%s] [%s] : %s\n", formatted_time, loglevels[level], message);
  return mcmplogWriteLine(env, level, fullmessage);
}

mcmpst mcmchlog(const mcmpenv env, const mcmploglevel level, const mcmplogflags flags, const char* message, ...) {
  char formattedmessage[MCMPLOG_MAX_LINE_SIZE - 100];
  va_list args;
  va_start(args, message);
  vsnprintf(formattedmessage, sizeof(formattedmessage), message, args);
  va_end(args);
  return mcmplogFormatted(env, level, flags, formattedmessage);
}