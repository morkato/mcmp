#ifdef __linux
#include <sys/socket.h>
#include <stdatomic.h>
#include <malloc.h>
#include <string.h>
#include "monarch/server/protocol.h"

// void mcmchProtoPingClient(const McmchServer server, const mcmchclient client) {
//   char protobuf[32] = {0};
//   protobuf[0] = '\x01';
//   protobuf[1] = '\x00';
//   // mcmchProtoQueueAlloc(server->brodcast.wrqueue, client, protobuf, NULL, NULL);
// }
#endif // __linux 