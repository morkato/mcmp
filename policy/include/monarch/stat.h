#ifndef MONARCH_STAT_H
#define MONARCH_STAT_H

#if defined(__GNUC__) || defined(__clang__)
  #define MCMCHST_THREAD __thread
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
  #define MCMCH_THREAD _Thread_local
#endif /** ...  */

/** Tipo para definir estados de funções na API "monarch".
  * 
  * Valores padrões de estados (genéricos):
  *   MCMCHST_SUCCESS --------+ Caso a operação seja um sucesso. 
  *   MCMCHST_NOT_IMPLEMENTED + Caso este erro não tenha sido devidamente implementado.
  *   MCMCHST_OVERFLOW -------+ Caso haja um overflow na operação designada.
  *   MCMCHST_OUT_OF_MEM -----+ Caso tenha excedido o limite da memória para realizar tal operação.
  *   MCMCHST_SPECIFIC -------+ Caso tenha ocorrido um erro específico de SO.
  *   MCMCHST_NOT_ABSPATH ----+ Caso o argumento passado não seja uma pasta absoluta.
  *   MCMCHST_INVALIGN -------+ Caso o alinhamento não seja válido.
  *   MCMCHST_NOINITZ --------+ Caso ocorra um erro na inicialização, ou o objeto não foi devidamente iniciado.
  *   MCMCHST_OVTIMEOUT ------+ Caso ocorra um timeout.
  *
  * OBS: Caso o estado for maior que 0, é um erro específico da função.  */
typedef int mcmchst;

#define MCMCHST_SUCCESS          0
#define MCMCHST_NOT_IMPLEMENTED -1
#define MCMCHST_OVERFLOW        -2
#define MCMCHST_OUT_OF_MEM      -3
#define MCMCHST_SPECIFIC        -4
#define MCMCHST_NOT_ABSPATH     -5
#define MCMCHST_INVALIGN        -6
#define MCMCHST_NOINITZ         -7
#define MCMCHST_ISEMPTY         -8
#define MCMCHST_OVTIMEOUT       -9
#define MCMCHST_CRITICAL        -10
#define MCMCHST_INVOP           -11
#define MCMCHST_ALROP           -12
#define MCMCHST_NOAVLE          -13

#endif /** MONARCH_STAT_H  */