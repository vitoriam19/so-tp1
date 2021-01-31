#include "../includes/monitor.hpp"
#include "../includes/utils.hpp"

Monitor::Monitor() {
  this->rajChoice = -1;
  this->queue = vector<int>();
  this->waitingSignal = vector<bool>(NUM_THREADS - 1, false);
  this->initializeConditions();

  pthread_mutex_init(&this->monitorMutex, NULL);
  pthread_mutex_init(&this->queueMutex, NULL);
  pthread_mutex_init(&this->waitingMutex, NULL);
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

void Monitor::waitCondition(int id) {
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
  cout << "estou aguardando " << getNameById(id) << endl;
  this->waitingSignal[id] = true;
  pthread_mutex_unlock(&this->waitingMutex);
}

void Monitor::unsetWaitingSignal(int id){
  pthread_mutex_lock(&this->waitingMutex);
  cout << "saí!!! " << getNameById(id) << endl;
  this->waitingSignal[id] = false;
  pthread_mutex_unlock(&this->waitingMutex);
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

  pthread_mutex_lock(&this->monitorMutex);
  // usleep(2000); // Easy way to create a deadlock
  if (this->getQueueSize() > 1) {
    this->setWaitingSignal(id);
    this->waitCondition(id);
    this->unsetWaitingSignal(id);
  }

  this->queueEraseElement(id);
}

void Monitor::unlock(int id) {
  pthread_mutex_unlock(&this->monitorMutex);

  if (this->getQueueSize() >= 1) {
    this->calculateNextToUse();
  }
}

void Monitor::calculateNextToUse() {
  if (this->rajChoice != -1) {
    this->signalCondition(this->rajChoice);
    this->rajChoice = -1;

    return;
  }

  if (this->checkForDeadlock()) {
    cout << " deadlock found " << endl;
    return;
  }

  pthread_mutex_lock(&this->queueMutex);
  sort(this->queue.begin(), this->queue.end());
  pthread_mutex_unlock(&this->queueMutex);

  int first = this->queueGetFirstElement();

  while(true) {
    pthread_mutex_lock(&this->waitingMutex);
    bool element = this->waitingSignal[first];
    pthread_mutex_unlock(&this->waitingMutex);

    if (element) break;
  }

  cout << "o prox eh " << getNameById(first) << endl;
  this->signalCondition(first);
}

void Monitor::setRajChoice(int id) {
  cout << "liberando o " << getNameById(id) << endl; 
  this->rajChoice = id;
  // this->signalCondition(id);
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