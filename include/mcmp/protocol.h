#ifndef MCMP_PROTOCOL_H
#define MCMP_PROTOCOL_H 1

#include <sodium.h>

#define MCMP_SIGNATURE 0x4D434D50 /** MCMP  */
#define MCMP_VERSION   0x00000000 /** v1  */

#define MCMP_HEADER_SIGV1  0x4D434D5000000000
#define MCMP_HEADER_SIZE   64UL
#define MCMP_PAYLOAD_SIZE  1024UL
#define MCMP_DGRAM_MAXSIZE (MCMP_HEADER_SIZE + MCMP_PAYLOAD_SIZE)
#define MCMP_MAKE_OPC(opc, c) (((uint32_t)(opc) << 24) | ((uint32_t)(c) & 0x00FFFFFF))

#define MCMP_OPC_ACK         0x00
#define MCMP_OPC_RCK         0x01
#define MCMP_OPC_FRAGMENT    0x02
#define MCMP_OPC_REQUEST     0x03

#define MCMP_ACK_ACCEPT_CONN 0x000000

#define MCMP_RCK_REJECT_CONN 0x000000

typedef uint8_t McmpDgram[MCMP_DGRAM_MAXSIZE];
typedef uint8_t* McmpDgramHeader;
typedef uint8_t* McmpDgramPayload;
typedef struct McmpBaseDgram {
  char sigversion[8];
  char operation[4];
} McmpBaseDgram;

typedef struct McmpAcceptConnectionDgram {
  McmpBaseDgram       family;
  unsigned char       pkey[crypto_box_PUBLICKEYBYTES];
  unsigned char       nonce[8];
} McmpAcceptConnectionDgram;

_Static_assert(sizeof(struct McmpBaseDgram) < MCMP_HEADER_SIZE, "...");
_Static_assert(crypto_box_PUBLICKEYBYTES == 0x20, "O crypto_box_PUBLICKEYBYTES excedeu 32 BYTES!");

// /** Structure (BIG-EDIAN) */
// typedef struct __attribute__((packed, aligned(MCH_PROTO_HEADER_SIZE))) McmchProtoReceivedHeader {
//   char sigversion[8];
//   char clientrel[8];
//   char lookup[8];
//   char requestid[4];
//   char protoop[4];
//   char length[8];
//   char fragment[4];
//   char utildata[MCH_PROTO_HEADER_SIZE - 44UL];
// } McmchProtoReceivedHeader;
// typedef char McmchProtoPayload[MCH_PROTO_PAYLOAD_SIZE];
// typedef struct McmchProtoDgram {
//   McmchProtoReceivedHeader header;
//   McmchProtoPayload payload;
// } McmchProtoDgram;

#endif /** MCMP_PROTOCOL_H  */