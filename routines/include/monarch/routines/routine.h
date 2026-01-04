#ifndef MONARCH_UTIL_ROUTINE_H
#define MONARCH_UTIL_ROUTINE_H 1
#include <monarch/time.h>
#include <monarch/stat.h>

typedef void* (*McmchRoutineHandler)(void*);
typedef struct McmchRoutine_payload_t {
  McmchRoutineHandler handler;
  void* arg;
} McmchRoutine_payload_t;

#ifdef __unix__
  #include <pthread.h>

  typedef struct McmchSignalLock_unix_t {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
  } McmchSignalLock_unix_t;

  #define MCMCHROUTINE_SIGNALROUTINE_LOCK_T McmchSignalLock_unix_t
  #define MCMCHROUTINE_PIPELINE_OST         pthread_t
#endif /** __unix__  */

#ifdef MCMCHROUTINE_PIPELINE_OST
  typedef MCMCHROUTINE_PIPELINE_OST McmchRoutinePipeline;
  typedef McmchRoutine_payload_t    McmchRoutine;

  /** Faz uma rotina.
    * Retorna `MCMCHST_SUCCESS` caso a requisição seja aceita.
    * Retorna diferente de `MCMCHST_SUCCESS` caso haja um erro específico de SO.
    * 
    * @param routine         Parâmentro para rotína, específico para cada SO.
    * @param routine_handler O ponto de entrada da rotína.
    * @param arg             Um argumento alinhado à heap passado para o ponto de entrada da rotína.  */
  mcmchst mcmchRoutineMake (McmchRoutine* routine, McmchRoutineHandler handler, void* arg);
  /** Linka com a rotína com uma linha.
    * Retorna `MCMCHST_SUCCESS` caso a requisição seja aceita.
    * Retorna diferente de `MCMCHST_SUCCESS` caso haja um erro específico de SO.
    * 
    * @param pipeline Parâmentro para linha, específico para cada SO.
    * @param routine  A rotína para ser linkada à linha.  */
  mcmchst mcmchRoutineLink (McmchRoutinePipeline* pipeline, const McmchRoutine* routine);
  /** Entra na tarefa da linha.
    * 
    * @param pipeline Linha passada para ponto de entrada.
    * @param result   Resultado esperado para o retorno da linha.  */
  void    mcmchRoutineJoin (const McmchRoutinePipeline* pipeline, void** result);

  #ifdef MCMCHROUTINE_SIGNALROUTINE_LOCK_T
    #define MCMCHSIGNALROUTINE_CONTINUE  0
    #define MCMCHSIGNALROUTINE_SHUTDOWN  1
    #define MCMCHSIGNALROUTINE_ERROR    -1 

    typedef MCMCHROUTINE_SIGNALROUTINE_LOCK_T McmchSignalLock;
    typedef void (*McmchSignalRoutineHandler)(McmchSignalLock*, int, void*);
    
    /** Inicializa um lock.
      * Retorna `MCMCHST_SUCCESS` caso a requisição seja aceita.
      * 
      * @param lock O lock para ser inializado.  */
    mcmchst mcmchSignalLockInitilize (McmchSignalLock* lock);
    /** Destroi um lock.
      * Retorna `MCMCHST_SUCCESS` caso a requisição seja aceita.
      * OBS: A destruição de um lock pode levar a inconsistências caso alguma rotína esteja consumindo-o.
      * 
      * @param lock O lock para ser destruído.  */
    mcmchst mcmchSignalLockDestroy   (McmchSignalLock* lock);
    /** Bloqueia/Adquire o lock para o contexto da thread.
      * Retorna `MCMCHST_SUCCESS` caso a requisição seja aceita.
      * 
      * @param lock O lock para ser bloqueado.  */
    mcmchst mcmchSignalLockBlock     (McmchSignalLock* lock);
    /** Espera até que um sinal seja chamado no lock.
      * Retorna `MCMCHST_SUCCESS` caso algum tipo de sinal tenha sido despachado.
      * OBS: O estado de "espera" tende ser imprevisível e instável, e pode retornar sucesso mesmo se nenhum sinal tenha sido despachado.
      * 
      * @param lock O lock usado para a runtine esperar.  */
    mcmchst mcmchSignalLockWait      (McmchSignalLock* lock);
    /** Espera até que um sinal seja chamado no lock.
      * Retorna `MCMCHST_SUCCESS` caso algum tipo de sinal tenha sido despachado.
      * OBS: O estado de "espera" tende ser imprevisível e instável, e pode retornar sucesso mesmo se nenhum sinal tenha sido despachado.
      * 
      * @param lock O lock usado para a runtine esperar.  */
    mcmchst mcmchSignalLockTimedWait (McmchSignalLock* lock, McmchTimeSpec* timer);
    /** Libera o lock para o contexto da rotína.
      * Retorna `MCMCHST_SUCCESS` caso a requisição seja aceita.
      * 
      * @param lock O lock para ser liberado.  */
    mcmchst mcmchSignalLockRelease   (McmchSignalLock* lock);
    /** Despacha um sinal para uma rotína que está esperando-o.
      * 
      * @param lock O lock para despachar um sinal.  */
    void    mcmchSignalDispatch      (McmchSignalLock* lock);
    /** Despacha um sinal para todas rotínas que está esperando-o.
      * 
      * @param lock O lock para despachar um sinal.  */
    void    mcmchSignalAllDispatch   (McmchSignalLock* lock);
    
    typedef struct McmchSignalRoutine {
      McmchRoutine              routine;
      McmchSignalLock*          lock;
      int                       running;
      McmchSignalRoutineHandler handler;
      void*                     arg;
    } McmchSignalRoutine;
    
    /** Cria uma rotína especial para sinais.
      * Retorna `MCMCHST_SUCCESS` caso a requisição seja aceita.
      * 
      * @param routine A rotína de sinal para ser iniciada.
      * @param lock    O lock de sincronização dos sinais desta rotína.
      * @param handler O ponto de entrada desta rotína.
      * @param arg     Um argumento alinhado à heap passado para o ponto de entrada da rotína.  */
    mcmchst mcmchSignalRoutineMake      (McmchSignalRoutine* routine, McmchSignalLock* lock, McmchSignalRoutineHandler handler, void* arg);
    /** Sinaliza para a rotína com sinais fechar.
      * OBS: Está função não força o fechamento da rotína, apenas sinaliza que seu processo terminou. O fechamento seguro depende da sua implementação.
      * OBS: Pode ser necessário despachar um sinal no lock atribuído à rotína para que ela feche corretamente sem esperar.
      * 
      * @param routine Rotína com sinais para ser interrompida.  */
    void    mcmchSignalRoutineInterrompt (McmchSignalRoutine* routine);
  #endif /** MCMCHROUTINE_SIGNALROUTINE_LOCK_T  */
#endif /** MCMCHROUTINE_ROUTINE_T  */

#endif /** MONARCH_UTIL_ROUTINE_H  */