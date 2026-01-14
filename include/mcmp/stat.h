#ifndef MCMP_STAT_H
#define MCMP_STAT_H

#if defined(__GNUC__) || defined(__clang__)
  #define MCMPST_THREAD __thread
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
  #define MCMPST_THREAD _Thread_local
#endif /** ...  */

/** Tipo para definir estados de funções na API "monarch".
  * 
  * Valores padrões de estados (genéricos):
  *   MCMPST_SUCCESS --------+ Caso a operação seja um sucesso. 
  *   MCMPST_NOT_IMPLEMENTED + Caso este erro não tenha sido devidamente implementado.
  *   MCMPST_OVERFLOW -------+ Caso haja um overflow na operação designada.
  *   MCMPST_OUT_OF_MEM -----+ Caso tenha excedido o limite da memória para realizar tal operação.
  *   MCMPST_SPECIFIC -------+ Caso tenha ocorrido um erro específico de SO.
  *   MCMPST_NOT_ABSPATH ----+ Caso o argumento passado não seja uma pasta absoluta.
  *   MCMPST_INVALIGN -------+ Caso o alinhamento não seja válido.
  *   MCMPST_NOINITZ --------+ Caso ocorra um erro na inicialização, ou o objeto não foi devidamente iniciado.
  *   MCMPST_OVTIMEOUT ------+ Caso ocorra um timeout.
  *
  * OBS: Caso o estado for maior que 0, é um erro específico da função.  */
typedef int mcmpst;

#define MCMPST_SUCCESS          0
#define MCMPST_NOT_IMPLEMENTED -1
#define MCMPST_OVERFLOW        -2
#define MCMPST_OUT_OF_MEM      -3
#define MCMPST_SPECIFIC        -4
#define MCMPST_NOT_ABSPATH     -5
#define MCMPST_INVALIGN        -6
#define MCMPST_NOINITZ         -7
#define MCMPST_ISEMPTY         -8
#define MCMPST_OVTIMEOUT       -9
#define MCMPST_CRITICAL        -10
#define MCMPST_INVOP           -11
#define MCMPST_ALROP           -12
#define MCMPST_NOAVLE          -13
#define MCMPST_AMBNOP          -14
#define MCMPST_RETRYOP         -15

#endif /** MCMP_STAT_H  */