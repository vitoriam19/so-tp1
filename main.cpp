#include "includes/threadManipulations.hpp"

using namespace std;

int main (int argc, char *argv[]) {
	if (argc != 2) {
		perror("Erro! Passe apenas um parâmetro, representando quantas vezes os personagens vão tentar usar o forno");
		exit(-1);
	}

	int timesToRun = atoi(argv[1]);

	createThreads(timesToRun);
}