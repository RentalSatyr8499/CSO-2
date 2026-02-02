#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>

typedef struct { // compiler doesn't optimize volatile stuff away
    volatile double d;
    volatile int i;
} sink; 
sink mySink;

/*      method overhead benchmark      */
__attribute__((noinline))
void baseTime(){
    // hejjj
};

/*      scenario 1      */
__attribute__((noinline))
void emptyCallHelper(){
    // hey
}
__attribute__((noinline))
void emptyCall(){
    baseTime();
};

/*      scenario 2      */
__attribute__((noinline))
void randomCall(){
    mySink.d = drand48();
};

/*      scenario 3      */
__attribute__((noinline))
void parentPID(){
    mySink.i = getppid();
}; 

/*      scenario 4      */
void setupForkToParent(){
    int s;
    waitpid(
        -1, // wait for ANY children 
        &s, 
        WNOHANG // return immediately if there are no children
    );
}
__attribute__((noinline))
void forkToParent(){
    pid_t p = fork();
    if (p == 0) _exit(0);
};

/*      scenario 5      */
void setupWaitpidExited(){
    pid_t p  = fork();
    if (p == 0){
        _exit(0);
    } else {
        nanosleep(&(struct timespec){
            0,  // number of seconds
            1000000   // num ns
        }, NULL);
    }
}; 
__attribute__((noinline))
void waitpidExited(){
    int s;
    waitpid(-1, &s, 0);
};

/*      scenario 6      */
void immediateExit(){
    pid_t p = fork();
    if (p == 0) _exit(0);
    int s;
    waitpid(p, &s, 0);
}; 

/*      scenario 7      */
__attribute__((noinline))
void execTrue(){
    mySink.i = system("/bin/true");
}; 

/*      scenario 8      */
void dirOps(){
    mkdir("/tmp/xaq7pj-timing", 0700);
    rmdir("/tmp/xaq7pj-timing");
} 

