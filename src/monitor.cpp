#include "../includes/monitor.hpp"
#include "../includes/utils.hpp"

Monitor::Monitor() {
  this->queue = vector<int>();
  this->waitingSignal = vector<bool>(NUM_THREADS - 1, false);
  this->rajChoice = -1;
  this->isWaitingForDeadlockSignal = false;
  this->isOvenBusy = false;
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
  this->waitingSignal[id] = true;
  pthread_mutex_unlock(&this->waitingMutex);

  return;
}

void Monitor::unsetWaitingSignal(int id){
  pthread_mutex_lock(&this->waitingMutex);
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

int Monitor::queuePushBackElement(int id) {
  int queueSize;
  pthread_mutex_lock(&this->queueMutex);
  cout << getNameById(id) << " quer usar o forno" << endl;
  this->queue.push_back(id);
  queueSize = this->queue.size();
  pthread_mutex_unlock(&this->queueMutex);

  return queueSize;
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
  /* Vamos guardar a informação que diz se o forno estava ou não ocupado quando o personagem entrou na fila. Se sim, e a
  fila estava vazia, então esse personagem deve passar direto e usar o forno. */
  int size = this->queuePushBackElement(id);
  bool wasOvenBusy = this->isOvenBusy;

  pthread_mutex_lock(&this->monitorMutex);
  if (size > 1 || wasOvenBusy) {
    /* Se temos mais de uma pessoa na fila, vamos esperar que o personagem atual seja escolhido pelo nosso algoritmo
    de escolha. Isso será comunicado por um sinal na variável de condição. */
    this->setWaitingSignal(id);
    this->waitSignal(id);
  }

  // Marcamos o forno como ocupado
  this->isOvenBusy = true;

  // O personagem foi escolhido, podemos removê-lo da fila de espera.
  this->queueEraseElement(id);
}

void Monitor::unlock(int id) {
  this->isOvenBusy = false;

  pthread_mutex_unlock(&this->monitorMutex);

  if (this->getQueueSize() >= 1) {
    // Vamos calcular o próximo a utilizar somente se tivermos alguém na fila.
    this->calculateNextToUse();
  }
}

void Monitor::calculateNextToUse() {
  if (this->checkForDeadlock().size()) {
    // Achamos um deadlock, vamos esperar para que o Raj escolha o próximo a esquentar a comida.

    pthread_mutex_lock(&this->deadlockMutex);

    // Sinalizamos que estamos esperando.
    this->isWaitingForDeadlockSignal = true;
    pthread_cond_wait(&this->deadlockConditional, &this->deadlockMutex);
    pthread_mutex_unlock(&this->deadlockMutex);

    /* Esperamos que o personagem escolhido por Raj esteja esperando um sinal na sua variável de condição para entrar na fila. */
    this->waitForIdListening(this->rajChoice);
    this->unsetWaitingSignal(this->rajChoice);
    this->signalCondition(this->rajChoice);
    return;
  }

  vector<int> indexes(NUM_THREADS);
  vector<pair<int,int> > couples;
  map<int, int> frequence;

  pthread_mutex_lock(&this->queueMutex);
  for(int i=0;i<this->queue.size();i++) {
    int thread = this->queue[i];
    indexes[thread] = i;
    frequence[thread]++;
  }
  pthread_mutex_unlock(&this->queueMutex);
  
  for(int x: {0, 2, 4}) {
    if (frequence[x] && frequence[x + 1]) {
      couples.push_back(make_pair(x, x+1));
    }
  }

  int next;
  if (!couples.size()) {
    // Se não temos casais, podemos só ordenar a fila e pegar o primeiro elemento, aquele com maior prioridade (e menor id).
    pthread_mutex_lock(&this->queueMutex);
    sort(this->queue.begin(), this->queue.end());
    pthread_mutex_unlock(&this->queueMutex);

    next = this->queueGetFirstElement();
  }

  if (couples.size() == 1 || couples.size() == 2) {
    /**
     * - Se temos apenas um casal, eles irão vir na frente de todos da fila. O próximo a usar o forno é a parte do casal que chegou primeiro na fila.
     * - Se temos dois casais, temos que decidir quem colocar. Nesse caso, o casal com o menor identificador vai ser selecionado, eles terão maior prioridade. * Por construção, o casal com menor id é aquele na primeira posição do vetor couples.
     */

    if (indexes[couples[0].first] < indexes[couples[0].second]) {
      next = couples[0].first;
    } else {
      next = couples[0].second;
    }
  }

  /* Esperamos que o personagem escolhido por nós esteja esperando um sinal na sua variável de condição para entrar na fila. */
  this->waitForIdListening(next);
  this->unsetWaitingSignal(next);
  this->signalCondition(next);
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

vector<int> Monitor::checkForDeadlock() {
  int menInCycle = 0;
  int womenInCycle = 0;
  vector<int> deadlockParts;

  pthread_mutex_lock(&this->queueMutex);
  for(int i=0;i<this->queue.size();i++) {
    int thread = this->queue[i];

    if(thread == 0 || thread == 2 || thread == 4) {
      /* Incrementaremos essa variável sempre que vermos Sheldon, Horward ou Leonard na fila. Se o valor final dela for 3, por exemplo, sabemos que podemos ter um deadlock se as namoradas não estiverem presentes. */
      menInCycle++;
    }

    if(thread == 1 || thread == 3 || thread == 5) {
      /* Incrementaremos essa variável sempre que vermos Penny, Bernardette ou Amy na fila. Como, sozinhas, as namoradas respeitarão
      as mesmas regras que seus respectivos namorados, caso estejam apenas elas na fila teremos um deadlock. */
      womenInCycle++;
    }
  }
  pthread_mutex_unlock(&this->queueMutex);

  if (menInCycle == 3 && !womenInCycle) {
    // Sheldon, Horward e Leonard na fila, sem nenhuma namorada.
    deadlockParts.push_back(0);
    deadlockParts.push_back(2);
    deadlockParts.push_back(4);
  }
  if (womenInCycle == 3 && !menInCycle) {
    // Penny, Bernardette e Amy na fila, sem nenhum namorado.
    deadlockParts.push_back(1);
    deadlockParts.push_back(3);
    deadlockParts.push_back(5);
  }
  if (womenInCycle == 3 && menInCycle == 3) {
    // Penny, Bernardette e Amy na fila, com todos os namorados. É como se tivéssemos somente os namorados. 
    for(int x: {0, 1, 2, 3, 4, 5}) {
      deadlockParts.push_back(x);
    }
  }

  return deadlockParts;
}