CC = gcc
CFLAGS = -Wall -g

all: fork_run

fork_run: main.o fork_run.o
	$(CC) $(CFLAGS) $^ -o $@
	chmod 777 fork_run

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c fork_run.h
fork_run.o: fork_run.c

clean:
	rm -f *.o fork_run out.txt err.txt
.PHONY: all clean