#ifndef MCMP_PROTO_H
#define MCMP_PROTO_H 1
#include "mcmp/stat.h"
#include <stdint.h>

#define MCMPPROTO_HEADER_SIZE 0x40

typedef uint64_t McmpProtoSequenceGenerator;
typedef uint64_t McmpProtoRequestId;
typedef uint64_t McmpProtoOperation;
typedef uint32_t McmpProtoFragmentInd;
typedef uint16_t McmpProtoFragmentSize;
typedef uint16_t McmpProtoRequestLookup;

typedef struct __attribute__((packed, aligned(8))) McmpProtoHeader {
  McmpProtoRequestId     reqid;
  McmpProtoRequestLookup lookup;
  McmpProtoOperation     op;
  McmpProtoFragmentInd   fragind;
  McmpProtoFragmentSize  fragsize;
  /** Padding  */
  uint8_t                 reserved[38];
} McmpProtoHeader;

mcmpst mcmpProtoCreateRequest (McmpProtoSequenceGenerator* gen, McmpProtoHeader* req,
                                 uint16_t slot, McmpProtoRequestLookup lookup, McmpProtoOperation op);
mcmpst mcmpProtoSequenceNext  (McmpProtoSequenceGenerator* gen);

#endif /** MCMP_PROTO_H  */