#include "mcmp/mcmp.h"
#include "mcmp/log.h"

#include <malloc.h>
#include <stddef.h>

mcmpenv selected_mp_current_env = NULL;

void mcmpNormalizeEnv(mcmpenv env) {
  _mcmpenv_comm* comm = (_mcmpenv_comm*)env;
  comm->logfilepath = NULL;
  comm->loglevel = MCMPLOG_INFO;
}

void mcmpEnvSetCurrentContext(mcmpenv env) {
  selected_mp_current_env = env;
}

void mcmpEnvClose(mcmpenv env) {
  mcmplog(env, MCMPLOG_INFO, 0, "Closing environment context.");
  mcmpCloseLogFile(env);
  free(env);
}