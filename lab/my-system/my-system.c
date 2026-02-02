#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int my_system(const char *command){
    pid_t p = fork();
    if (p == 0){
        execl(
            "/bin/sh", // executable path
            "sh", // first arg
            "-c", // flag for command string
            command, 
            (char *)NULL // terminator
        );
    }
    int status;
    waitpid(p, &status, 0);
    return status;
};