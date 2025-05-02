# Makefile for echo server & client

CC = gcc
CFLAGS = -Wall -O2
OBJS = csapp.o echo.o

all: echoserveri echoclient

echoserveri: echoserveri.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -pthread

echoclient: echoclient.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -pthread

clean:
	rm -f *.o echoserveri echoclient