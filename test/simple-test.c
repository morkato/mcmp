#ifdef __linux
#include <mcisid/mcisidv1.h>
#include <monarch/monarch.h>
#include <monarch/bitmap.h>
#include <monarch/store.h>
#include <monarch/uid.h>
#include <monarch/log.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
  const char* passedRootdir = getenv("MONARCH_ROOTDIR");
  const char* passedLogfilename = getenv("MONARCH_SAVE_LOGFILE");
  const char* pwd = getenv("PWD");
  if (passedRootdir == NULL || passedLogfilename == NULL) {
    fprintf(stderr, "It's necessary pass MONARCH_ROOTDIR and MONARCH_SAVE_LOGFILE environment, to configure application context.\n");
    return 1;
  }
  char rootdir[4096];
  char logfilename[4096];
  if (passedRootdir[0] == '/')
    memcpy(rootdir, passedRootdir, strlen(passedRootdir) + 1);
  else
    snprintf(rootdir, 4096, "%s/%s", pwd, passedRootdir);
  if (passedLogfilename[0] == '/')
    memcpy(logfilename, passedLogfilename, strlen(passedLogfilename) + 1);
  else
    snprintf(logfilename, 4096, "%s/%s", pwd, passedLogfilename);
  mcmchenv env = mcmchInitilizeEnvironment();  
  mcmchSetEnvRootDir(env, rootdir);
  mcmchSetEnvLogFileName(env, logfilename);
  mcmchOpenLogFile(env);
  mcmchSetEnvLogLevel(env, MCMCHLOG_DEBUG);
  mcmchGenerateStore(env);
  mcmchOpenIdStore(env);
  mcmchEnvClose(env);
  return 0;
}
#endif