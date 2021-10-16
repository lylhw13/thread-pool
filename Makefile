CFLAGS = -g
CC = gcc

LIBS = -lpthread

PROM = test.out

all: $(PROM)

test.out: thread-pool.h thread-pool.c ./test/test.c 
	$(CC) $(CFLAGS) -o ./test/test.out thread-pool.c ./test/test.c $(LIBS)

clear:
	rm $(PROM)