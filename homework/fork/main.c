#include "fork_run.h"
#include <stdio.h>

int main(int argc, const char *argv[]) {
    
    printf("Test 1\n");
    printf("Hi!\n");
    writeoutput("echo 1 2 3; sleep 2; echo 5 5", "out.txt", "err.txt");
    printf("Bye!\n");

    printf("Test 2\n");
    const char *argv_base[] = {
        "/bin/echo", "running", NULL
    };
    parallelwriteoutput(2, argv_base, "out1.txt");

    printf("Test 3\n");
    const char *argv_base1[] = { "./args.py", "first", "second", "third", NULL };
    parallelwriteoutput(3, argv_base1, "out2.txt");

}
