#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
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
    emptyCallHelper();
};

/*      scenario 2      */
void randomCall(){
    mySink.d = drand48();
};

/*      scenario 3      */
void parentPID(){
    mySink.i = getppid();
}; 

/*      scenario 4      */
void setupForkToParent(){
    waitpid(
        -1, // wait for ANY children 
        &mySink.i, 
        WNOHANG // return immediately if there are no children
    );
}
void forkToParent(){
    pid_t p = fork();
    if (p == 0) exit(0);
};

/*      scenario 5      */
void setupWaitpidExited(){
    pid_t p  = fork();
    if (p == 0){
        exit(0);
    } else {
        nanosleep(&(struct timespec){
            1,  // number of seconds
            0   // num ns
        }, NULL);
    }
}; 
void waitpidExited(){
    waitpid(-1, &mySink.i, WNOHANG);
};

/*      scenario 6      */
void immediateExit(){
    pid_t p = fork();
    if (p == 0) exit(0);
    waitpid(p, &mySink.i, 0);
}; 

/*      scenario 7      */
void execTrue(){
    system("/bin/true");
}; 

/*      scenario 8      */
void dirOps(){
    mkdir("../../../../../tmp/xaq7pj-timing", 0700);
    rmdir("../../../../../tmp/xaq7pj-timing");
} 

