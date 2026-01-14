#ifdef __linux
#include "mcmp/mcmp.h"
#include "mcmp/statos.h"

#include <stddef.h>
#include <malloc.h>

typedef struct _mcmpenv {
  _mcmpenv_comm comm;
  int logfd;
} _mcmpenv;

mcmpenv mcmpInitilizeEnvironment() {
  _mcmpenv* env = (_mcmpenv*)malloc(sizeof(_mcmpenv));
  if (env == NULL) {
    MCMPST_SETRET(MCMPST_OUT_OF_MEM);
    return NULL;
  }
  mcmchNormalizeEnv((mcmchenv)env);
  env->logfd = -1;
  return (mcmchenv)env;
}

mcmpst mcmpSetEnvLogFileName(mcmpenv env, char* logfile) {
  _mcmpenv_comm* _env = (_mcmpenv_comm*)env;
  if (logfile[0] != '/')
    MCMPST_RETURN(MCMPST_NOT_ABSPATH);
  _env->logfilepath = logfile;
  MCMPST_RETURN(MCMPST_SUCCESS);
}

mcmpst mcmpSetEnvLogLevel(mcmpenv env, mcmploglevel level) {
  ((_mcmpenv_comm*)env)->loglevel = level;
  MCMPST_RETURN(MCMPST_SUCCESS);
}

#endif /** __linux  */