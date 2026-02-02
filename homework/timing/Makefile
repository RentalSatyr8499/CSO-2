CC = gcc
CFLAGS = -Wall -g -Og

all: gettimings

gettimings: gettimings.o toMeasure.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

gettimings.o: gettimings.c toMeasure.h
toMeasure.o: toMeasure.c

clean:
	rm -f *.o gettimings
.PHONY: all clean