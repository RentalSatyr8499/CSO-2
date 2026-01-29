# xaq7pj
# I worked alone on this assignment
CC = gcc
CFLAGS = -Wall -fPIC
LDFLAGS = -Wall -Wl,-rpath . -L.
LDLIBS = -lsay

all: guesser
guesser: guesser.o libsay.so
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
libsay.so: grunt.o cheer.o
	$(CC) -shared $(CFLAGS) -o $@ $^

guesser.o: guesser.c say.h
grunt.o: grunt.c say.h 
cheer.o: cheer.c say.h

clean: 
	rm -f guesser.o grunt.o cheer.o guesser libsay.so

.PHONY: all clean