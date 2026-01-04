#ifdef __linux
#include "monarch/monarch.h"
#include "monarch/statos.h"

#include <stddef.h>
#include <malloc.h>

mcmchenv mcmchInitilizeEnvironment() {
  _mcmchenv* env = (_mcmchenv*)malloc(sizeof(_mcmchenv));
  if (env == NULL) {
    MCMCHST_SETRET(MCMCHST_OUT_OF_MEM);
    return NULL;
  }
  mcmchNormalizeEnv((mcmchenv)env);
  env->logfd = -1;
  return (mcmchenv)env;
}

mcmchst mcmchSetEnvLogFileName(mcmchenv env, char* logfile) {
  _mcmchenv_comm* _env = (_mcmchenv_comm*)env;
  if (logfile[0] != '/')
    MCMCHST_RETURN(MCMCHST_NOT_ABSPATH);
  _env->logfilepath = logfile;
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

mcmchst mcmchSetEnvLogLevel(mcmchenv env, mcmchloglevel level) {
  ((_mcmchenv_comm*)env)->loglevel = level;
  MCMCHST_RETURN(MCMCHST_SUCCESS);
}

#endif /** __linux  */