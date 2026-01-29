CC = gcc
CFLAGS = -Wall -g

all: split

split: main.o split.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c split.h
split.o: split.c

clean:
	rm -f *.o split

.PHONY: all clean
