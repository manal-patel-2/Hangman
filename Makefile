CFLAGS = -g -Wall
CC = g++


all: hangman


clean:
	rm -f core *.o a.out hangman



hangman: hangman.o functions.o functions.h
	$(CC) $(CFLAGS) -o $@ $@.o

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@
