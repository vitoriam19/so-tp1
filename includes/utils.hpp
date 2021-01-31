#ifndef UTILS_HPP
#define UTILS_HPP
#define NUM_THREADS 9 // Uma thread para cada personagem + a thread do Raj

#include <iostream>
#include <vector>

using namespace std;

/**
 * Função que retorna o nome do personagem dado seu id
 *
 * @param {int} id - Id do personagem a ser identificado
 */
string getNameById(int id);

/**
 * Função que retorna um número aleatório
 *
 * @param {int} smaller - Menor valor que pode ser escolhido
 * @param {int} bigger - Maior valor que pode ser escolhido
 */
int getAleatoryNumber(int smaller, int bigger);

#endif