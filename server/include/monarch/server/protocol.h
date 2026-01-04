#ifndef MONARCH_PROTOCOL_H
#define MONARCH_PROTOCOL_H 1
#include "monarch/server/server.h"

#define MCMCHPROTO_BUFFER_SIZE 32

#if defined(__unix__)
  typedef struct mcmchproto_queue_payload_unix {
    mcmchclient client;
    char        protobuf[MCMCHPROTO_BUFFER_SIZE];
    void*       content;
    size_t      size;
  } mcmchproto_queue_payload_unix;
#endif // defined(__unix__)

// void mcmchProtoPingClient (const mcmchserv server, const mcmchclient client);
#endif // MONARCH_PROTOCOL_H