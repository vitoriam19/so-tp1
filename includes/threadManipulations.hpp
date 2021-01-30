#ifndef THREADMANIPULATIONS_HPP
#define THREADMANIPULATIONS_HPP

#include <iostream>
#include <vector>

using namespace std;

/**
 * Estrutura que vai representar os argumentos de uma thread (de um personagem)
 * @property {int} thread_id - representa o id identificador do personagem
 * @property {string} thread_characters_name - representa o nome do personagem que é representado por essa thread
 */
typedef struct Thread_data {
    int thread_id;
    string thread_character_name;
} thread_data;

/**
 * Função que inicializa e preenche uma thread data
 * @param {int} id - recebe o id da thread
 * @param {string} name - recebe o nome do personagem correspondente à thread
 */ 
thread_data initializeThreadData (int id, string name);

/**
 * Função que preenche o struct thread_data e cria a thread para cada personagem 
 * passando o struct como argumento para a função PrintHello
 */
void createThreads();

/**
 * Função que executa a thread printando o id e o nome - para testar 
 * @param {void} character - struct com os dados do personagem
 */
void *PrintHello(void *character);
#endif