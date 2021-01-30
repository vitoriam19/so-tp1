CC := g++
SRC_DIR := src
INC_DIR := includes
FLAGS  := -Wall -Werror
all: tp.out

tp.out:	$(SRC_DIR)/threadManipulations.o
	$(CC) $(FLAGS) main.cpp $(SRC_DIR)/threadManipulations.o -o tp.out -lpthread

$(SRC_DIR)/threadManipulations.o: $(SRC_DIR)/threadManipulations.cpp 
	$(CC) $(FLAGS) -c $(SRC_DIR)/threadManipulations.cpp -o $(SRC_DIR)/threadManipulations.o

run:
	./tp.out

clean:
	rm $(SRC_DIR)/threadManipulations.o
	rm ./tp.out

	