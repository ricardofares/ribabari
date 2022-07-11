CC=gcc
CFLAGS=-I. -lcurses -lmenu -lpthread -lm
SOURCES = $(wildcard *.c) $(wildcard */*c)

all: 
	$(CC) -o main $(SOURCES) $(CFLAGS)

run: all
	./main
