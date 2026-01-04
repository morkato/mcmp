#ifndef MONARCH_SERVER_BROADCAST_H
#define MONARCH_SERVER_BROADCAST_H 1
#include "monarch/server/client.h"
#include <monarch/stat.h>
#include <stdint.h>
#include <stddef.h>

typedef struct McmchBroadcast* McmchBroadcast;
typedef struct McmchAddress*   McmchAddress;

/** Cria uma instância de broadcast.
  * Retorna um ponteiro válido caso a requisição seja um sucesso. `NULL` caso haja algum erro.
  * 
  * @param capacity O tamanho total de clientes monitorados pelo broadcast.  */
McmchBroadcast mcmchBroadcastCreate    (const size_t capacity);
/** Adiciona a cópia de um cliente para o broadcast monitorar e gerenciar.
  * Retorna a referencia da cópia alinhada ao broadcast caso a requisição seja um sucesso. `NULL` caso haja algum erro.
  * OBS: O cliente retornado é uma referencia da sua localização dentro do broadcast, não sendo uma referencia do parâmetro passado!
  * 
  * @param broadcast O broadcast para adicionar o cliente.
  * @param address    O cliente para ser copiado no broadcast.  */
McmchClient    mcmchBroadcastAddClient (McmchBroadcast broadcast, const McmchAddress address, const size_t addrlen);
/** Remove o cliente do monitoramento do broadcast.
  * Retorna `MCMCHST_SUCESS` caso a requisição seja aceita.
  * OBS: O cliente passado deve estar alinhado ao broadcast, isto é, um ponteiro dentro do broadcast.
  * 
  * @param broadcast O broadcast para remover o cliente.
  * @param client    O cliente para ser removido no broadcast.  */
mcmchst        mcmchBroadcastDelClient (McmchBroadcast broadcast, const McmchClient client);
/** Adiciona a cópia de um cliente para o broadcast monitorar e gerenciar.
  * Retorna a referencia da cópia alinhada ao broadcast caso a requisição seja um sucesso. `NULL` caso haja algum erro.
  * OBS: O cliente retornado é uma referencia da sua localização dentro do broadcast, não sendo uma referencia do parâmetro passado!
  * 
  * @param broadcast O broadcast para adicionar o cliente.
  * @param address    O cliente para ser copiado no broadcast.  */
McmchClient    mcmchBroadcastWatchClient (McmchBroadcast broadcast, const McmchClient client);
/** Destroí uma instância de broadcast.
  * Retorna `MCMCHST_SUCESS` caso a requisição seja aceita.
  * 
  * @param broadcast O broadcast a ser destruído.  */
mcmchst        mcmchBroadcastDestroy   (const McmchBroadcast broadcast);

#ifdef MCMCHCLIENT_T
  #include <monarch/util/bitmap.h>
  #include <monarch/stat.h>

  typedef struct McmchBroadcastOpaque* McmchBroadcast;
  typedef struct McmchBroadcastHeader {
    size_t full_length;
    size_t bitmap_length;
  } McmchBroadcastHeader;

  /** Cria uma instância de broadcast.
    * Retorna um ponteiro válido caso a requisição seja um sucesso. `NULL` caso haja algum erro.
    * 
    * @param length O tamanho total de clientes monitorados pelo broadcast.  */
  McmchBroadcast mcmchBroadcastCreate               (const size_t length);
  /** Adiciona a cópia de um cliente para o broadcast monitorar e gerenciar.
    * Retorna a referencia da cópia alinhada ao broadcast caso a requisição seja um sucesso. `NULL` caso haja algum erro.
    * OBS: O cliente retornado é uma referencia da sua localização dentro do broadcast, não sendo uma referencia do parâmetro passado!
    * 
    * @param broadcast O broadcast para adicionar o cliente.
    * @param client    O cliente para ser copiado no broadcast.  */
  McmchClient    mcmchBroadcastAddClient            (McmchBroadcast broadcast, const McmchClient client);
  /** Remove o cliente do monitoramento do broadcast.
   * Retorna `MCMCHST_SUCESS` caso a requisição seja aceita.
   * OBS: O cliente passado deve estar alinhado ao broadcast, isto é, um ponteiro dentro do broadcast.
   * 
   * @param broadcast O broadcast para remover o cliente.
   * @param client    O cliente para ser removido no broadcast.  */
  mcmchst        mcmchBroadcastDelClient            (McmchBroadcast broadcast, const McmchClient client);
   /** Destroí uma instância de broadcast.
    * Retorna `MCMCHST_SUCESS` caso a requisição seja aceita.
    * 
    * @param broadcast O broadcast a ser destruído.  */
  mcmchst        mcmchBroadcastDestroy              (const McmchBroadcast broadcast);

  #define MCMCHBROADCAST_CAST(bc)              ((McmchBroadcastHeader*)(bc))
  #define MCMCHBROADCAST_GET_LENGTH(bc)        (MCMCHBROADCAST_CAST(bc)->full_length)
  #define MCMCHBROADCAST_GET_BITMAP_LENGTH(bc) (MCMCHBROADCAST_CAST(bc)->bitmap_length)
  #define MCMCHBROADCAST_GET_BITMAP(bc)        (((uint8_t*)(bc)) + sizeof(McmchBroadcastHeader))
  #define MCMCHBROADCAST_GET_CLIENTS(bc)       (MCMCHBROADCAST_GET_BITMAP(bc) + MCMCHBROADCAST_GET_BITMAP_LENGTH(bc))

  #define MCH_BC_CAST(bc)              ((McmchBroadcastHeader*)(bc))
  #define MCH_BC_GET_LENGTH(bc)        (MCH_BC_CAST(bc)->full_length)
  #define MCH_BC_GET_BITMAP_LENGTH(bc) (MCH_BC_CAST(bc)->bitmap_length)
  #define MCH_BC_GET_BITMAP(bc)        (((uint8_t*)(bc)) + sizeof(McmchBroadcastHeader))
  #define MCH_BC_GET_CLIENTS(bc)       (MCH_BC_GET_BITMAP(bc) + MCH_BC_GET_BITMAP_LENGTH(bc))
#endif /** MCMCHCLIENT_T  */

#endif /** MONARCH_SERVER_BROADCAST_H  */