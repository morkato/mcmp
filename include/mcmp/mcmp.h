#ifndef MCMP_H
#define MCMP_H

#define MCMPENV_SETFLAG(env, flag)   ((env) |= (1 << flag))
#define MCMPENV_CLEARFLAG(env, flag) ((env) &= ~(1 << (flag)))
#define MCMPENV_HASFLAG(env, flag)   (((env) & (1 << flag)) != 0)

#include <stdint.h>
#include <stddef.h>

/**

typedef uint8_t  mcmploglevel;
typedef uint32_t mcmplogflags;
typedef void*    mcmpenv;

typedef struct _mcmpenv_comm {
  char* logfilepath;
  mcmploglevel loglevel;
} _mcmpenv_comm;

extern mcmpenv selected_mp_current_env;

#define mcmpenvcur (selected_mp_current_env)

mcmpenv mcmpInitilizeEnvironment ();
void    mcmpNormalizeEnv         (mcmpenv env);
void    mcmpEnvSetCurrentContext (mcmpenv env);
mcmpst  mcmpSetEnvLogFileName    (mcmpenv env, char* logfile);
mcmpst  mcmpSetEnvLogLevel       (mcmpenv env, mcmploglevel level);
void    mcmpEnvClose             (mcmpenv env);

*/

#endif // MCMP_H