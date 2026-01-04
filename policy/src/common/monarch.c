#include "monarch/monarch.h"
#include "monarch/log.h"

#include <malloc.h>
#include <stddef.h>

mcmchenv selected_mch_current_env = NULL;

void mcmchNormalizeEnv(mcmchenv env) {
  _mcmchenv_comm* comm = (_mcmchenv_comm*)env;
  comm->logfilepath = NULL;
  comm->loglevel = MCMCHLOG_INFO;
}

void mcmchEnvSetCurrentContext(mcmchenv env) {
  selected_mch_current_env = env;
}

void mcmchEnvClose(mcmchenv env) {
  mcmchlog(env, MCMCHLOG_INFO, 0, "Closing environment context.");
  mcmchCloseLogFile(env);
  free(env);
}