CC = gcc
CCFLAGS = -Wall -g

all: test

test: test.o tlb.o
	$(CC) $(CCFLAGS) -o $@ $^

%.o: %.c 
	$(CC) $(CCFLAGS) -c $< -o $@

test.o: tlb.h
tlb.o: mlpt.h config.h

.PHONY: all clean

clean:
	rm -f test *.o