CC      = gcc
CFLAGS  = -Wall -Wextra -g -fsanitize=thread
LDFLAGS = -pthread

# Build both by default
all: pool-example pool-test

# ---- Binaries ----

pool-example: pool-example.o threadpool.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

pool-test: pool-test.o threadpool.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ---- Objects ----

pool-example.o: pool-example.c pool.h
	$(CC) $(CFLAGS) -c $<

pool-test.o: pool-test.c pool.h
	$(CC) $(CFLAGS) -c $<

threadpool.o: threadpool.c pool.h
	$(CC) $(CFLAGS) -c $<

# ---- Cleanup ----

clean:
	rm -f *.o pool-example pool-test
