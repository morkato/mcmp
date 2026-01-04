#ifdef __linux
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <time.h>

#include "monarch/statos.h"
#include "monarch/log.h"

mcmchst mcmchOpenLogFile(mcmchenv env) {
  _mcmchenv* _env = (_mcmchenv*)env;
  _mcmchenv_comm* _comm = (_mcmchenv_comm*)env;
  const char* logfilepath = _comm->logfilepath;
  int logfd = open(logfilepath, O_CREAT | O_WRONLY | O_APPEND, 0644);
  if (logfd == -1) {
    fprintf(stderr, "Error on open logfile\n");
    MCMCHST_RETURN(MCMCHST_NOT_IMPLEMENTED);
  }
  _env->logfd = logfd;
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchCloseLogFile(mcmchenv env) {
  _mcmchenv* _env = (_mcmchenv*)env;
  int logfd = _env->logfd;
  if (logfd == -1)
    MCMCHST_RETURN(MCMCHST_NOT_IMPLEMENTED);
  mcmchlog(env, MCMCHLOG_INFO, 0, "Closing log file.");
  fsync(logfd);
  close(logfd);
  _env->logfd = -1;
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchlogWriteLine(const mcmchenv env, const mcmchloglevel level, const char* line) {
  const _mcmchenv* _env = (_mcmchenv*)env;
  const _mcmchenv_comm* _comm = (_mcmchenv_comm*)env;
  size_t line_length = strlen(line);
  if (_env == NULL || level >= _comm->loglevel)
    write(STDOUT_FILENO, line, line_length);
  if (_env != NULL && _env->logfd != -1)
    write(_env->logfd, line, line_length);
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}
#endif // __linux
