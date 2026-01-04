#ifndef MCMCH_PROTOCOL_H
#define MCMCH_PROTOCOL_H 1

#include <sodium.h>

#define MCH_PROTO_SIGNATURE 0x004D4350 /** MCP  */
#define MCH_PROTO_VERSION   0x0000000000 /** v1  */

#define MCH_PROTO_HEADER_SIGV1  0x4D43500000000000
#define MCH_PROTO_HEADER_SIZE   128UL
#define MCH_PROTO_PAYLOAD_SIZE  1024UL
#define MCH_PROTO_DGRAM_MAXSIZE (MCH_PROTO_HEADER_SIZE + MCH_PROTO_PAYLOAD_SIZE)
#define MCH_PROTO_MAKE_OPC(opc, c) (((uint32_t)(opc) << 24) | ((uint32_t)(c) & 0x00FFFFFF))

#define MCH_PROTO_OPC_ACK         0x00
#define MCH_PROTO_OPC_RCK         0x01
#define MCH_PROTO_OPC_FRAGMENT    0x02
#define MCH_PROTO_OPC_REQUEST     0x03

#define MCH_PROTO_ACK_ACCEPT_CONN 0x000000

#define MCH_PROTO_RCK_REJECT_CONN 0x000000

typedef char McmchProtoDgram[MCH_PROTO_DGRAM_MAXSIZE];
typedef struct McmchProtoBaseDgram {
  char sigversion[8];
  char operation[4];
} McmchProtoBaseDgram;

typedef struct McmchProtoAcceptConnectionDgram {
  McmchProtoBaseDgram family;
  unsigned char       pkey[crypto_box_PUBLICKEYBYTES];
  unsigned char       nonce[8];
} McmchProtoAcceptConnectionDgram;

_Static_assert(sizeof(struct McmchProtoBaseDgram) < MCH_PROTO_HEADER_SIZE, "...");
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

#endif /** MCMCH_PROTOCOL_H  */