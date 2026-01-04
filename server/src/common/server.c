#include "monarch/server/server.h"
#include <monarch/protocol.h>
#include <endian.h>
#include <string.h>
#include <stdint.h>

void __protected__mcmch_ack_accept_client(
  const McmchAddress                     address,
  const McmchProtoAcceptConnectionDgram* dgram
);

static inline void __priv__mcmch_handle_ack(
  const uint32_t subop,
  const McmchProtoDgram dgram,
  McmchAddress   address,
  const ssize_t  size
) {
  switch (subop) {
    case MCH_PROTO_ACK_ACCEPT_CONN:
      if (size < sizeof(McmchProtoAcceptConnectionDgram))
        /** TODO: Adicionar um método.  */
        return;
      __protected__mcmch_ack_accept_client(address, (const McmchProtoAcceptConnectionDgram*)dgram);
      break;
    default:
      break;
  }
}

static inline void __priv__mcmch_handle_rck(
  const uint32_t subop,
  const McmchProtoDgram dgram,
  McmchAddress   address,
  const ssize_t  size
) {
  
}

static inline void __priv__mcmch_handle_fragment(
  const uint32_t subop,
  const McmchProtoDgram dgram,
  McmchAddress   address,
  const ssize_t  size
) {
  
}

static inline void __priv__mcmch_handle_request(
  const uint32_t subop,
  const McmchProtoDgram dgram,
  McmchAddress   address,
  const ssize_t  size
) {
  
}

void __protected__mcmch_proccess_dgram(
  const McmchProtoDgram dgram,
  McmchAddress          address,
  const ssize_t         size
) {
  McmchProtoBaseDgram* base = (McmchProtoBaseDgram*)dgram;
  uint64_t             sigversion;
  uint32_t             operation;

  if (size < sizeof(McmchProtoBaseDgram))
    /** TODO: Datagrama inválido. Decidir como tratar.  */
    return;
  memcpy(&sigversion, base->sigversion, sizeof(base->sigversion));
  if (be64toh(sigversion) != MCH_PROTO_HEADER_SIGV1)
    /** TODO: Logar para assinaturas inválidas [DROP]. */
    return;
  memcpy(&operation, base->operation, sizeof(base->operation));
  operation = be32toh(operation);
  switch (operation & 0xFF000000) {
    case MCH_PROTO_OPC_ACK:
      __priv__mcmch_handle_ack(operation & 0x00FFFFFF, dgram, address, size);
      break;
    case MCH_PROTO_OPC_RCK:
      __priv__mcmch_handle_rck(operation & 0x00FFFFFF, dgram, address, size);
      break;
    case MCH_PROTO_OPC_FRAGMENT:
      __priv__mcmch_handle_fragment(operation & 0x00FFFFFF, dgram,  address, size);
      break;
    case MCH_PROTO_OPC_REQUEST:
      __priv__mcmch_handle_request(operation & 0x00FFFFFF, dgram, address, size);
      break;
  }
}