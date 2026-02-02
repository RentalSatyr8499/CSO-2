CC = gcc
CFLAGS = -Wall -g

all: my-system

my-system: main.o my-system.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c my-system.h
my-system.o: my-system.c

clean:
	rm -f *.o my-system
.PHONY: all clean