CFLAGS = -g
CC = gcc

LIBS = -lpthread

PROM = test

all: $(PROM)

test: thread-pool.h thread-pool.c test.c 
	$(CC) $(CFLAGS) -o test thread-pool.c test.c $(LIBS)

clear:
	rm $(PROM)