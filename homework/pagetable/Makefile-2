CC = gcc
CFLAGS = -Wall -g 

all: libmlpt.a

libmlpt.a: pagetable.o
	ar rcs libmlpt.a pagetable.o

pagetable.o: pagetable.c mlpt.h config.h
	$(CC) $(CFLAGS) -c $< $@

clean: 
	rm -f pagetable.o libmlpt.a

.PHONY: all clean