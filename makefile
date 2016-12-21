CC=g++
CFLAGS=-Wall -lsodium

all: 
	$(CC) -o minesweeper main.cpp $(CFLAGS) -lsfml-graphics -lsfml-window -lsfml-system
