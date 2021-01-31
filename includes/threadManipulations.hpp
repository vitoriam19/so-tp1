#ifndef THREADMANIPULATIONS_HPP
#define THREADMANIPULATIONS_HPP

#include <iostream>
#include <vector>
#include <pthread.h>
#include <unistd.h>

using namespace std;

/**
 * Função a ser executada por todos os personagens do problema, com exceção do Raj. Basicamente, o personagem vai
 * tentar acessar o forno pelo número de vezes que é passado para ele. Após esquentar algo, vai comer e depois trabalhar.
 * Cada uma dessas ações toma um tempo aleatório entre 3 e 6 segundos.
 * 
 * @param {void *} id - Inteiro que representa o identificador do personagem dessa thread.
 */
void *runThreads(void *id);

/**
 * Função a ser executada pelo Raj. Basicamente, irá procurar por um deadlock de 5 em 5 segundos. Para tal, usará o método
 * checkForDeadlock da classe Monitor.
 */
void *runRaj(void *);

/**
 * Função que cria o array de threads e inicializa cada uma passando a função a ser executada como
 * parâmetro. Também instancia a classe Monitor a ser usada pelas threads.
 */
void createThreads();

#endif