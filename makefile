CC=g++
CFLAGS=-Wall -lsodium -lsfml-graphics -lsfml-window -lsfml-system 

all: 
	$(CC) -o minesweeper main.cpp $(CFLAGS)
