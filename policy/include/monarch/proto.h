#ifndef MONARCH_PROTO_H
#define MONARCH_PROTO_H 1
#include "monarch/stat.h"
#include <stdint.h>

#define MCMCHPROTO_HEADER_SIZE 0x40

typedef uint64_t McmchProtoSequenceGenerator;
typedef uint64_t McmchProtoRequestId;
typedef uint64_t McmchProtoOperation;
typedef uint32_t McmchProtoFragmentInd;
typedef uint16_t McmchProtoFragmentSize;
typedef uint16_t McmchProtoRequestLookup;

typedef struct __attribute__((packed, aligned(8))) McmchProtoHeader {
  McmchProtoRequestId     reqid;
  McmchProtoRequestLookup lookup;
  McmchProtoOperation     op;
  McmchProtoFragmentInd   fragind;
  McmchProtoFragmentSize  fragsize;
  /** Padding  */
  uint8_t                 reserved[38];
} McmchProtoHeader;

mcmchst mcmchProtoCreateRequest (McmchProtoSequenceGenerator* gen, McmchProtoHeader* req,
                                 uint16_t slot, McmchProtoRequestLookup lookup, McmchProtoOperation op);
mcmchst mcmchProtoSequenceNext  (McmchProtoSequenceGenerator* gen);

#endif /** MONARCH_PROTO_H  */