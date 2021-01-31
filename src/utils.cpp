#include "../includes/utils.hpp"

#define pb push_back

string getNameById(int id) {
  vector<string> characters;

  // Adiciona os personagens ao vetor
  characters.pb("Sheldon");
  characters.pb("Howard");
  characters.pb("Leonard");
  characters.pb("Stuart");
  characters.pb("Kripke");
  characters.pb("Amy");
  characters.pb("Bernadette");
  characters.pb("Penny");

  // Retorna o personagem correspondente ao id
  return characters[id];
}

int getAleatoryNumber(int smaller, int bigger){
  return rand()%(bigger-smaller+1) + smaller;
}