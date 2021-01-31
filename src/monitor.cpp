#include "../includes/monitor.hpp"
#include "../includes/utils.hpp"

Monitor::Monitor() {
  this->queue = vector<int>();
  this->waitingSignal = vector<bool>(NUM_THREADS - 1, false);
  this->rajChoice = -1;
  this->isWaitingForDeadlockSignal = false;
  this->initializeConditions();

  pthread_mutex_init(&this->monitorMutex, NULL);
  pthread_mutex_init(&this->queueMutex, NULL);
  pthread_mutex_init(&this->waitingMutex, NULL);
  pthread_mutex_init(&this->deadlockMutex, NULL);
  pthread_cond_init(&this->deadlockConditional, NULL);
}

/************************************************
 * Threads conditional variables methods
 ***********************************************/

void Monitor::initializeConditions() {
  pthread_cond_init(&this->threadsCondition.c1, NULL);
  pthread_cond_init(&this->threadsCondition.c2, NULL);
  pthread_cond_init(&this->threadsCondition.c3, NULL);
  pthread_cond_init(&this->threadsCondition.c4, NULL);
  pthread_cond_init(&this->threadsCondition.c5, NULL);
  pthread_cond_init(&this->threadsCondition.c6, NULL);
  pthread_cond_init(&this->threadsCondition.c7, NULL);
  pthread_cond_init(&this->threadsCondition.c8, NULL);
}

void Monitor::waitSignal(int id) {
  switch(id) {
    case 0:
      pthread_cond_wait(&this->threadsCondition.c1, &this->monitorMutex);
      break;
    case 1:
      pthread_cond_wait(&this->threadsCondition.c2, &this->monitorMutex);
      break;
    case 2:
      pthread_cond_wait(&this->threadsCondition.c3, &this->monitorMutex);
      break;
    case 3:
      pthread_cond_wait(&this->threadsCondition.c4, &this->monitorMutex);
      break;
    case 4:
      pthread_cond_wait(&this->threadsCondition.c5, &this->monitorMutex);
      break;
    case 5:
      pthread_cond_wait(&this->threadsCondition.c6, &this->monitorMutex);
      break;
    case 6:
      pthread_cond_wait(&this->threadsCondition.c7, &this->monitorMutex);
      break;
    case 7:
      pthread_cond_wait(&this->threadsCondition.c8, &this->monitorMutex);
      break;
  }
}

void Monitor::signalCondition(int id) {
  pthread_mutex_lock(&this->monitorMutex);
  switch(id) {
    case 0:
      pthread_cond_signal(&this->threadsCondition.c1);
      break;
    case 1:
      pthread_cond_signal(&this->threadsCondition.c2);
      break;
    case 2:
      pthread_cond_signal(&this->threadsCondition.c3);
      break;
    case 3:
      pthread_cond_signal(&this->threadsCondition.c4);
      break;
    case 4:
      pthread_cond_signal(&this->threadsCondition.c5);
      break;
    case 5:
      pthread_cond_signal(&this->threadsCondition.c6);
      break;
    case 6:
      pthread_cond_signal(&this->threadsCondition.c7);
      break;
    case 7:
      pthread_cond_signal(&this->threadsCondition.c8);
      break;
  }
  pthread_mutex_unlock(&this->monitorMutex);
}

/************************************************
 * Waiting signal methods
 ***********************************************/

void Monitor::setWaitingSignal(int id){
  pthread_mutex_lock(&this->waitingMutex);
  // cout << "estou aguardando " << getNameById(id) << endl;
  this->waitingSignal[id] = true;
  pthread_mutex_unlock(&this->waitingMutex);

  return;
}

void Monitor::unsetWaitingSignal(int id){
  pthread_mutex_lock(&this->waitingMutex);
  // cout << "saí!!! " << getNameById(id) << endl;
  this->waitingSignal[id] = false;
  pthread_mutex_unlock(&this->waitingMutex);

  return;
}

void Monitor::waitForIdListening(int id) {
  while(true) {
    pthread_mutex_lock(&this->waitingMutex);
    bool isIdWaiting = this->waitingSignal[id];
    pthread_mutex_unlock(&this->waitingMutex);

    if (isIdWaiting) break;
  }

  return;
}

/************************************************
 * Queue Methods
 ***********************************************/

int Monitor::getQueueSize() {
  pthread_mutex_lock(&this->queueMutex);
  int size = this->queue.size();
  pthread_mutex_unlock(&this->queueMutex);

  return size;
}

void Monitor::queuePushBackElement(int id) {
  pthread_mutex_lock(&this->queueMutex);

  cout << getNameById(id) << " quer usar o forno" << endl;

  this->queue.push_back(id);
  pthread_mutex_unlock(&this->queueMutex);
}

void Monitor::queueEraseElement(int id) {
  pthread_mutex_lock(&this->queueMutex);

  vector<int>::iterator it = find(
    this->queue.begin(),
    this->queue.end(),
    id
  );
  this->queue.erase(it);

  pthread_mutex_unlock(&this->queueMutex);
}

int Monitor::queueGetFirstElement() {
  pthread_mutex_lock(&this->queueMutex);
  int first = this->queue[0];
  pthread_mutex_unlock(&this->queueMutex);

  return first;
}

/************************************************
 * General Methods
 ***********************************************/

void Monitor::getLock(int id) {
  this->queuePushBackElement(id);
  int size = this->getQueueSize();

  pthread_mutex_lock(&this->monitorMutex);

  // if (id == 3 || id == 4) {
  //   this->queueEraseElement(id);

  //   return;
  // }
  if (size > 1) {
    /* Se temos mais de uma pessoa na fila, vamos esperar que o personagem atual seja escolhido pelo nosso algoritmo
    de escolha. Isso será comunicado por um sinal na variável de condição. */
    this->setWaitingSignal(id);
    this->waitSignal(id);
    this->unsetWaitingSignal(id);
  }

  // O personagem foi escolhido, podemos removê-lo da fila de espera.
  this->queueEraseElement(id);
}

void Monitor::unlock(int id) {
  pthread_mutex_unlock(&this->monitorMutex);

  if (this->getQueueSize() >= 1) {
    // Vamos calcular o próximo a utilizar somente se tivermos alguém na fila.
    this->calculateNextToUse();
  }
}

void Monitor::calculateNextToUse() {
  if (this->checkForDeadlock()) {
    // Achamos um deadlock, vamos esperar para que o Raj escolha o próximo a esquentar a comida.

    pthread_mutex_lock(&this->deadlockMutex);

    // Sinalizamos que estamos esperando.
    this->isWaitingForDeadlockSignal = true;
    pthread_cond_wait(&this->deadlockConditional, &this->deadlockMutex);
    pthread_mutex_unlock(&this->deadlockMutex);

    /* Esperamos que o personagem escolhido por Raj esteja esperando um sinal na sua variável de condição para
    entrar na fila. */
    this->waitForIdListening(this->rajChoice);
    this->signalCondition(this->rajChoice);
    return;
  }

  pthread_mutex_lock(&this->queueMutex);
  sort(this->queue.begin(), this->queue.end());
  pthread_mutex_unlock(&this->queueMutex);

  int first = this->queueGetFirstElement();

  /* Esperamos que o personagem escolhido por nós esteja esperando um sinal na sua variável de condição para
    entrar na fila. */
  this->waitForIdListening(first);
  // cout << "o prox eh " << getNameById(first) << endl;
  this->signalCondition(first);
}

void Monitor::setRajChoice(int id) {
  /* Antes de sinalizar que o Raj escolheu alguém, vamos esperar que a função que calcula o próximo a usar o forno
  ache o deadlock e espere por uma solução para o mesmo. */
  while(1) {
    pthread_mutex_lock(&this->deadlockMutex);
    if(this->isWaitingForDeadlockSignal) {
      this->isWaitingForDeadlockSignal = false;
      break;
    }
    pthread_mutex_unlock(&this->deadlockMutex);
  }

  // Setamos qual foi a escolha de Raj e sinalizamos que vamos liberar o deadlock.
  this->rajChoice = id;
  pthread_cond_signal(&this->deadlockConditional);
  pthread_mutex_unlock(&this->deadlockMutex);
}

bool Monitor::checkForDeadlock() {
  int hasDeadlock = 0;

  pthread_mutex_lock(&this->queueMutex);
  for(int i=0;i<this->queue.size();i++) {
    int thread = this->queue[i];

    if(thread == 0 || thread == 1 || thread == 2) {
      hasDeadlock++;
    }
  }
  pthread_mutex_unlock(&this->queueMutex);

  return hasDeadlock == 3;
}