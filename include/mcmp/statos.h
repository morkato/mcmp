#ifndef MCMP_STATOS_H
#define MCMP_STATOS_H
#include "mcmp/stat.h" /** MCMP_THREAD  */

#ifdef MCMPST_THREAD
  typedef struct mcmchst_soexc_frame_t {
    char        message[128];
    int         status;
    int         length;
  } mcmpst_soexc_frame_t;

  extern MCMPST_THREAD mcmpst_soexc_frame_t mcmpst_so_frame_v;
  extern MCMPST_THREAD mcmpst               mcmpst_last_ret;

  /** O Instante de erro específico para SO.
    * Verificado por outras funções da API.  */
  #define MCMPST_ERRFRAME (&mcmpst_so_frame_v)
  #define mcmpstret       (mcmpst_last_ret)

  #define MCMPST_SETRET(st) \
    (mcmpstret = (st))
#else
  #define MCMPST_ERRFRAME (NULL)
  #define mcmpstret       (MCMPST_AMBNOP)

  #define MCMPST_SETRET(st)
#endif /** MCMCHST_THREAD  */

#define MCMPST_RETURN(st) \
  do { \
    MCMPST_SETRET(st); \
    return (st); \
  } while (0)

/** Depura o erro relacionado ao frame do estado.
  * Caso não reconheça o nome da função, não fará nada.
  * 
  * @param funcname Nome da função no qual deu erro.  */
void        mcmpstperr (const char* funcname);
/** Retorna uma string referente ao nome do erro.
  * Caso não reconheça o status, retornará uma string inválida.
  * 
  * @param st Status para obter o nome.  */
const char* mcmpstrerr (const mcmpst st);

#endif /** MCMP_STATOS_H  */