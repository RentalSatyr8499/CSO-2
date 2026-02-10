#include "fork_run.h"
#include <stdio.h>

int main(int argc, const char *argv[]) {
    printf("Hi!\n");
    writeoutput("echo 1 2 3; sleep 2; echo 5 5", "out.txt", "err.txt");
    printf("Bye!\n");
}
