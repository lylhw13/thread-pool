CFLAGS = -g
CC = gcc

LIBS = -lpthread

PROM = test

all: $(PROM)

test: thread-poll.h thread-poll.c test.c 
	$(CC) $(CFLAGS) -o test thread-poll.c test.c $(LIBS)

clear:
	rm $(PROM)