#ifndef MONARCH_STATOS_H
#define MONARCH_STATOS_H
#include "monarch/stat.h" /** MCMCHST_THREAD  */

#ifdef MCMCHST_THREAD
  typedef struct mcmchst_soexc_frame_t {
    char        message[128];
    int         status;
    int         length;
  } mcmchst_soexc_frame_t;

  extern MCMCHST_THREAD mcmchst_soexc_frame_t mchst_so_frame_v;
  extern MCMCHST_THREAD mcmchst               mchst_last_ret;

  /** O Instante de erro específico para SO.
    * Verificado por outras funções da API.  */
  #define MCMCHST_ERRFRAME mchst_so_frame_v
  #define mcmchstret       mchst_last_ret

  #define MCMCHST_SETRET(st) \
    (mcmchstret = (st))
  #define MCMCHST_RETURN(st) \
    do { \
      mcmchstret = (st); \
      return (st); \
    } while (0)

  /** Depura o erro relacionado ao frame do estado.
    * Caso não reconheça o nome da função, não fará nada.
    * 
    * @param funcname Nome da função no qual deu erro.  */
  void        mcmchstperr (const char* funcname);
  /** Retorna uma string referente ao nome do erro.
    * Caso não reconheça o status, retornará uma string inválida.
    * 
    * @param funcname Nome da função no qual deu erro.  */
  const char* mcmchstrerr (const mcmchst st);
#else
  #define MCMCHST_RETURN(st) \
    do { \
      return (st); \
    } while (0)
#endif /** MCMCHST_THREAD  */
#endif /** MONARCH_STATOS_H  */