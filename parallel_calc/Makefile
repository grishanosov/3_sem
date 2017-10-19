CC = gcc
CFLAGS = -Wall -lpthread 

all: test

test: matrix.o main.o
	$(CC) $(CFLAGS) -o test matrix.o main.o

main.c matrix.c: matrix.h

-include *.d

.PHONY: all test clean

clean: 
	rm -rf *.d *.o test

