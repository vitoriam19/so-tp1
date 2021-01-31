CC := g++
SRC_DIR := src
INC_DIR := includes
FLAGS  := -Wall -Werror
all: tp.out

tp.out:	$(SRC_DIR)/threadManipulations.o $(SRC_DIR)/monitor.o $(SRC_DIR)/utils.o
	$(CC) $(FLAGS) main.cpp $(SRC_DIR)/threadManipulations.o $(SRC_DIR)/monitor.o $(SRC_DIR)/utils.o -o tp.out -lpthread

$(SRC_DIR)/threadManipulations.o: $(SRC_DIR)/threadManipulations.cpp 
	$(CC) $(FLAGS) -c $(SRC_DIR)/threadManipulations.cpp -o $(SRC_DIR)/threadManipulations.o

$(SRC_DIR)/monitor.o: $(SRC_DIR)/monitor.cpp 
	$(CC) $(FLAGS) -c $(SRC_DIR)/monitor.cpp -o $(SRC_DIR)/monitor.o

$(SRC_DIR)/utils.o: $(SRC_DIR)/utils.cpp 
	$(CC) $(FLAGS) -c $(SRC_DIR)/utils.cpp -o $(SRC_DIR)/utils.o

run:
	./tp.out

clean:
	rm $(SRC_DIR)/threadManipulations.o
	rm $(SRC_DIR)/monitor.o
	rm $(SRC_DIR)/utils.o
	rm ./tp.out

	