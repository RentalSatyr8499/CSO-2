#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

void writeoutput(const char *command, const char *out_path, const char *err_path){
    int out = open(out_path, O_WRONLY | O_CREAT);
    dup2(out, STDOUT_FILENO);
    
    int err = open(err_path, O_WRONLY | O_CREAT);
    dup2(err, STDERR_FILENO);
    
    pid_t p = fork();
    if (p == 0){
        execl( "/bin/sh", "sh", "-c", command, (char *)NULL );
    }

    int status;
    waitpid(p, &status, 0);

    dup2(STDOUT_FILENO, out);
    dup2(STDERR_FILENO, err);
    
    return;
};
void parallelwriteoutput(int count, const char **argv_base, const char *out_path){
    return;
};