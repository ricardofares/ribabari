CC=gcc
CFLAGS=-I. -lncurses -lmenu

all: main.c
	$(CC) -o main main.c $(CFLAGS)

run: all
	./main
