CC = g++
CFLAGS = -std=c++17 -O3

all: game

game: minesweeper.cc
	$(CC) $(CFLAGS) minesweeper.cc -o game

clean:
	rm -rf ./*.o game
