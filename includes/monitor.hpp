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
    void getLock(int id);
    void unlock(int id);
    void setRajChoice(int id);
    void calculateNextToUse();
    bool checkForDeadlock();
    Monitor();

  private:
    pthread_mutex_t monitorMutex;
    pthread_mutex_t queueMutex;
    pthread_mutex_t threadMutex;
    pthread_mutex_t waitingMutex;
    int rajChoice;

    /* Threads conditional variables */
    conditions threadsCondition;
    void initializeConditions();
    void waitCondition(int id);
    void signalCondition(int id);

    /* Queue */
    vector<int> queue;
    int getQueueSize();
    int queueGetFirstElement();
    void queuePushBackElement(int id);
    void queueEraseElement(int id);

    /* Waiting signal queue */
    vector<bool> waitingSignal;
    void setWaitingSignal(int id);
    void unsetWaitingSignal(int id);
};

#endif