CC=gcc
CFLAGS=-lcurses -lmenu

all: 
	$(CC) -o main main.c terminal/terminal.c $(CFLAGS)

run: all
	./main
