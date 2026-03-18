CC = gcc
CFLAGS = -Wall -g

all: tester

tester: tester.o pagetable.o
	$(CC) $(CFLAGS) -o $@ $^


tester.o: tester.c mlpt.h
	$(CC) $(CFLAGS) -c $< -o $@

pagetable.o: pagetable.c mlpt.h config.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f tester.o pagetable.o tester
