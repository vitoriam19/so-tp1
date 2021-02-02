#include "../includes/utils.hpp"

#define pb push_back

string getNameById(int id) {
  vector<string> characters;

  // Adiciona os personagens ao vetor
  characters.pb("Sheldon"); // 0
  characters.pb("Amy"); // 1
  characters.pb("Howard"); // 2
  characters.pb("Bernadette"); // 3
  characters.pb("Leonard"); // 4
  characters.pb("Penny"); // 5
  characters.pb("Stuart"); // 6
  characters.pb("Kripke"); // 7

  // Retorna o personagem correspondente ao id
  return characters[id];
}

int getAleatoryNumber(int smaller, int bigger){
  return rand()%(bigger-smaller+1) + smaller;
}