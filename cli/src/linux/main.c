#ifndef _POSIX_C_SOURCE
  #define _POSIX_C_SOURCE 200809L
#endif

#include <sys/socket.h>
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>

#include <monarch/server/dispatcher.h>
#include <monarch/server/heartbeat.h>
#include <monarch/server/client.h>
#include <monarch/server/server.h>
#include <monarch/util/queue.h>
#include <monarch/monarch.h>
#include <monarch/statos.h>
#include <monarch/log.h>

#define LISTEN_PORT       7767
#define MAX_EVENTS        10
#define MAX_STATIC_CLIENT 16
#define BEAT_TICK         150

static char* logfilename = "/var/local/log/monarch.log";
// static char* rootdir = "/usr/local/monarch";
volatile sig_atomic_t server_running = 1;

void run_server_signal(int signum) {
  server_running = 0;
  if (signum == SIGINT)
  putchar('\n');
}

int main_sigset(sigset_t* set, sigset_t* oldset) {
  signal(SIGINT,  run_server_signal);
  signal(SIGTERM, run_server_signal);
  sigemptyset(set);
  sigaddset(set, SIGINT);
  sigaddset(set, SIGTERM);
  if (sigprocmask(SIG_BLOCK, set, oldset)) {
    perror("sigprocmask");
    return 1;
  }
  return 0;
}

mcmchenv createEnvironmentInstanceFromContext() {
  mcmchenv env = mcmchInitilizeEnvironment();
  mcmchSetEnvLogFileName(env, logfilename);
  mcmchSetEnvLogLevel(env, MCMCHLOG_DEBUG);
  mcmchOpenLogFile(env);
  return env;
}

int run_server(int argc, const char** args) {
  sigset_t      set;
  sigset_t      oldset;
  mcmchenv      env;
  McmchServer   server;
  mcmcheartbeat heartbeat;
  McmchDispatcherWrite writer;
  mcmchclient_broadcast broadcast;
  uint64_t      beatick = 0;
  // mcmchdispatcher_wrhandler_pool writter_worker;
  if (main_sigset(&set, &oldset) != 0)
    return 1;
  mcmchlog(NULL, MCMCHLOG_INFO, 0, "Abrindo o servidor...");
  env = createEnvironmentInstanceFromContext();
  server = mcmchServerCreate(env, LISTEN_PORT);
  brodcast = mcmchClientCreateBroadcast(16UL);
  heartbeat = mcmchHeartbeatCreateInstance(brodcast, MCMCHCLIENT_BROADCAST_BITMAP_LENGTH(brodcast));
  writer = mcmchDispatcherCreateWriterInstance(brodcast, 16UL);
  if (sigprocmask(SIG_SETMASK, &oldset, NULL) < 0) {
    perror("sigprocmask");
    mcmchlog(env, MCMCHLOG_ERROR, 0, "Falha ao marcar novamente os signals, fechando o servidor de forma segura.");
    mcmchDispatcherDestroyWriterInstance(writer);
    mcmchHeartbeatDestroyInstance(heartbeat);
    mcmchClientBroadcastFlush(server, brodcast);
    mcmchClientDestroyBroadcast(brodcast);
    mcmchServerClose(server);
    mcmchEnvClose(env);
    return 1;
  }
  mcmchServerSetWriterDispatcher(server, writer);
  mcmchEnvSetCurrentContext(env);
  mcmchDispatcherWriterStart(writer);
  mcmchlog(env, MCMCHLOG_INFO, 0, "Servidor rodando na porta: [::]:%hu.", LISTEN_PORT);
  while (server_running) {
    int nfds = epoll_wait(server->epollfd, server->events, MAX_EVENTS, 100);
    if (nfds == -1) {
      if (errno == EINTR)
      continue;
      perror("epoll_wait");
      break;
    }
    if (++beatick % BEAT_TICK == 0)
      mcmchHeartbeatBeat(heartbeat);
    mcmchHeartbeatProcessIfAvaible(server, brodcast, heartbeat);
    mcmchServerUpdate(server, brodcast, nfds);
  }
  mcmchlog(env, MCMCHLOG_INFO, 0, "Fechando o processo, limpando os dados.");
  mcmchDispatcherDestroyWriterInstance(writer);
  mcmchHeartbeatDestroyInstance(heartbeat);
  mcmchClientBroadcastFlush(server, brodcast);
  mcmchClientDestroyBroadcast(brodcast);
  mcmchServerClose(server);
  mcmchEnvClose(env);
  return 0;
}

int main(int argc, const char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Not selected option!\n");
    return 1;
  }
  const char* option = argv[1];
  const char** args = argv + 2;
  const int argslen = argc - 2;
  if (strcmp(option, "serve") == 0)
    return run_server(argslen, args);
  fprintf(stderr, "Opção inválida!\n");
  mcmchqueue queue = mcmchQueueCreate(sizeof(int), 1024);
  mcmchQueueDestroy(queue);
  return 1;
}