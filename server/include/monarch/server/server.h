#ifndef MONARCH_SERVER_H
#include <unistd.h>
#include <stdint.h>

#include "monarch/server/broadcast.h"
#include "monarch/server/client.h"
#include <monarch/util/bitmap.h>
#include <monarch/monarch.h>

typedef struct McmchServer* McmchServer;

McmchServer mcmchServerCreate (const int port);
mcmchst     mcmchServerUpdate (McmchServer serv, McmchBroadcast broadcast, const int nfds);
mcmchst     mcmchServerClose  (McmchServer serv);
// mcmchst     mcmchServerSetWriterDispatcher (McmchServer serv, void* wrdr);

mcmchst mcmchClientBroadcastFlush (const McmchServer server, McmchBroadcast broadcast);
mcmchst mcmchClientClose          (const McmchServer server, const McmchClient client);

#endif // MONARCH_SERVER_H