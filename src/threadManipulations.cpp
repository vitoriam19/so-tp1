#include "../includes/threadManipulations.hpp"
#include "../includes/utils.hpp"
#include "../includes/monitor.hpp"

/**
 * Instância global da classe monitor a ser usada pelas threads.
 */
Monitor *monitor;

void *runThreads(void *id) {
  int characterId = (long int)id;

  monitor->getLock(characterId);

  cout << getNameById(characterId) << " começa a esquentar algo" << endl;
  usleep(1 * 1e6);
  cout << getNameById(characterId) << " vai comer" << endl;

  monitor->unlock(characterId);

  usleep(getAleatoryNumber(3,6) * 1e6);
  cout << getNameById(characterId) << " voltou para o trabalho" << endl;
  usleep(getAleatoryNumber(3,6) * 1e6);

  pthread_exit(NULL);
}

void *runRaj(void *) {
  //para checar a cada 5 segundos
  usleep(1 * 1e6);
  if(monitor->checkForDeadlock()){
    int id = getAleatoryNumber(1,3);
    cout << "Raj detectou um deadlock, liberando " << getNameById(id) << endl;
    monitor->setRajChoice(id);
  }

  pthread_exit(NULL);
}

void createThreads() {
  monitor = new Monitor();
  pthread_t threads[NUM_THREADS];
  int td;

  // Cria threads de todos os 8 personagens, com exceção do Raj.
  for(long int i=0;i<5;i++){
    td = pthread_create(&threads[i], NULL, runThreads, (void *)i);
    if (td){
      printf("ERROR; return code from pthread_create() is %d\n", td);
      return;
    }
  }
  // Cria thread do Raj
  td = pthread_create(&threads[NUM_THREADS-1], NULL, runRaj, NULL);
  if (td){
    printf("ERROR; return code from pthread_create() is %d\n", td);
    return;
  }

  pthread_exit(NULL);
}