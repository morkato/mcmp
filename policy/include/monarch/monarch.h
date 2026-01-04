#ifndef MONARCH_H
#define MONARCH_H

#define MCMCHENV_SETFLAG(env, flag)   ((env) |= (1 << flag))
#define MCMCHENV_CLEARFLAG(env, flag) ((env) &= ~(1 << (flag)))
#define MCMCHENV_HASFLAG(env, flag)   (((env) & (1 << flag)) != 0)

#include "monarch/stat.h"
#include <stdint.h>
#include <stddef.h>

typedef uint8_t  mcmchloglevel;
typedef uint32_t mcmchlogflags;
typedef void*    mcmchenv;

typedef struct _mcmchenv_comm {
  char* logfilepath;
  mcmchloglevel loglevel;
} _mcmchenv_comm;
#if defined(__linux)
  typedef struct _mcmchenv {
    _mcmchenv_comm comm;
    int logfd;
  } _mcmchenv;
#elif defined(__WIN32)
  #include <windows.h>
  typedef struct _mcmchenv {
    _mcmchenv_comm comm;
    HANDLE loghandle;
  } _mcmchenv;
#endif

extern mcmchenv selected_mch_current_env;

#define mcmchenvcur selected_mch_current_env

mcmchenv mcmchInitilizeEnvironment ();
void     mcmchNormalizeEnv         (mcmchenv env);
void     mcmchEnvSetCurrentContext (mcmchenv env);
mcmchst  mcmchSetEnvLogFileName    (mcmchenv env, char* logfile);
mcmchst  mcmchSetEnvLogLevel       (mcmchenv env, mcmchloglevel level);
void     mcmchEnvClose             (mcmchenv env);
#endif // MONARCH_H