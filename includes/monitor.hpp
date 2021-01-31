#ifndef MONITOR_HPP
#define MONITOR_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <pthread.h>
#include <unistd.h>

using namespace std;

/**
 * Struct contendo as variáveis de condição para cada um dos 8 personagens do problema. Fazer
 * um vetor de 8 posições se provou ineficaz por um problema ao enviar o sinal.
 */
typedef struct conditions {
  pthread_cond_t c1;
  pthread_cond_t c2;
  pthread_cond_t c3;
  pthread_cond_t c4;
  pthread_cond_t c5;
  pthread_cond_t c6;
  pthread_cond_t c7;
  pthread_cond_t c8;
} conditions;

/**
 * Classe contendo todos os métodos do monitor do forno. Esse monitor é responsável por escolher o próximo
 * a esquentar a comida, delegar acesso e checar por deadlock.
 */
class Monitor {
  public:
    /**
     * Método que controla o acesso ao forno. Irá inserir o personagem que pediu acesso na fila de espera, e retirá-lo
     * quando for selecionado.
     * 
     * @param {int} id - Identificador do personagem.
     */
    void getLock(int id);

    /**
     * Método que libera o forno. Irá também chamar a função para calcular a próxima pessoa a ser selecionada para esquentar
     * a comida.
     *
     * @param {int} id - Identificador do personagem.
     */
    void unlock(int id);

    /**
     * Método chamado por Raj quando o mesmo detectou um deadlock.
     * 
     * @param {int} id - Identificador do personagem selecionado por Raj, randomicamente, para usar o forno.
     */
    void setRajChoice(int id);

    /**
     * Método que calcula a próxima pessoa a utilizar o forno. Leva em conta todos os personagens na fila
     * de espera.
     */
    void calculateNextToUse();

    /**
     * Método que verifica se há deadlock na fila de espera.
     */
    bool checkForDeadlock();
    Monitor();

  private:
    /**
     * Mutex usado para controlar acesso ao forno.
     */
    pthread_mutex_t monitorMutex;

    /**
     * Mutex usado para controlar acesso à fila de espera. Duas ou mais threads não podem acessar esse
     * espaço de memória ao mesmo tempo.
     */
    pthread_mutex_t queueMutex;

    /**
     * Mutex usado para controlar acesso ao vetor de todas as threads esperando por um sinal para utilizar o forno.
     */
    pthread_mutex_t waitingMutex;

    /**
     * Mutex usado para controlar acesso às variáveis relacionadas com o deadlock.
     */
    pthread_mutex_t deadlockMutex;

    /**
     * Variável de condição que será sinalizada quando Raj achar um deadlock. O sinal será aguardado no cálculo do próximo
     * personagem a utilizar o forno.
     */
    pthread_cond_t deadlockConditional;

    /**
     * Escolha do Raj quando um deadlock for encontrado.
     */
    int rajChoice;

    /**
     * Booleano que indica se a função que calcula o próximo personagem a usar o forno encontrou um deadlock e está esperando
     * por um sinal de Raj.
     */
    bool isWaitingForDeadlockSignal;

    /************************************************
     * Threads conditional variables
     ***********************************************/
    /**
     * Variáveis de condição dos 8 personagens. Todos esperarão um sinal antes de começar a utilizar o forno.
     */
    conditions threadsCondition;

    /**
     * Método de inicialização das variáveis de condição.
     */
    void initializeConditions();

    /**
     * Método que irá aguardar por um sinal na variável de condição do personagem passado como parâmetro.
     * 
     * @param {int} id - Identificador do personagem.
     */
    void waitSignal(int id);

    /**
     * Método que emitirá um sinal para a variável de condição do personagem passado como parâmetro.
     * 
     * @param {int} id - Identificador do personagem.
     */
    void signalCondition(int id);

    /************************************************
     * Queue
     ***********************************************/
    /**
     * Fila de espera para utilizar o forno
     */
    vector<int> queue;

    /**
     * Método que retorna o tamanho da fila da espera.
     */
    int getQueueSize();

    /**
     * Método que retorna o primeiro elemento da fila da espera.
     */
    int queueGetFirstElement();

    /**
     * Método que insere um elemento na fila da espera.
     * 
     * @param {int} id - Identificador do personagem.
     */
    void queuePushBackElement(int id);

    /**
     * Método que remove o elemento passado como parâmetro da fila de espera.
     * 
     * @param {int} id - Identificador do personagem a ser removido da fila.
     */
    void queueEraseElement(int id);

    /************************************************
     * Waiting signal queue
     ***********************************************/
    /**
     * Vetor representando quais personagens estão esperando um sinal em sua variável de condição para
     * começar a utilizar o forno. waitingSignal[i] = true se, e somente se, o personagem cujo identificado é i
     * está esperando por um sinal de sua variável de condição.
     */
    vector<bool> waitingSignal;

    /**
     * Método usado para definir que um personagem está esperando pelo sinal.
     *
     * @param {int} id - Identificador do personagem que está agora esperando pelo sinal.
     */
    void setWaitingSignal(int id);

    /**
     * Método usado para definir que um personagem não está mais esperando pelo sinal.
     * 
     * @param {int} id - Identificador do personagem.
     */
    void unsetWaitingSignal(int id);

    /**
     * Método que bloqueia a execução do código enquanto o personagem passado como parâmetro não estiver esperando
     * por um sinal para usar o forno.
     * 
     * @param {int} id - Identificador do personagem.
     */
    void waitForIdListening(int id);
};

#endif