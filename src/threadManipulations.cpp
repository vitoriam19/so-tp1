#include "../includes/threadManipulations.hpp"
#include <pthread.h>

#define pb push_back
#define NUM_THREADS 8 // Uma thread para cada personagem

thread_data initializeThreadData(int id, string name){
    thread_data character;
    character.thread_id = id;
    character.thread_character_name = name;

    return character;
}

void *PrintHello(void *character)
{
  thread_data * td;
  td = (thread_data *) character;
  int thread_id = td->thread_id;
  string name = td->thread_character_name;

  cout << "Sou eu, thread " << thread_id << " personagem " << name << endl;
  pthread_exit(NULL);
}

void createThreads(){
  pthread_t threads[NUM_THREADS];
  vector<string> characters;
  thread_data character;
  int it, td;

  characters.pb("Sheldon");
  characters.push_back("Howard");
  characters.push_back("Leonard");
  characters.push_back("Stwart");
  characters.push_back("Kripke");
  characters.push_back("Amy");
  characters.push_back("Bernadette");
  characters.push_back("Penny");

  for(it = 0; it < NUM_THREADS; it++){
    character = initializeThreadData(it, characters[it]);
    cout << "Cria thread " << it << " " << "que corresponde ao personagem: " << characters[it] << endl; 
    td = pthread_create(&threads[it], NULL, PrintHello, (void *) &character);
    if (td){
      printf("ERROR; return code from pthread_create() is %d\n", td);
      return;
    }
  }
  pthread_exit(NULL);
}