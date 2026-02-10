#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

void writeoutput(const char *command, const char *out_path, const char *err_path){
    pid_t p = fork();
    if (p == 0){
        int out = open(out_path, O_WRONLY | O_CREAT);
        dup2(out, STDOUT_FILENO);
        
        int err = open(err_path, O_WRONLY | O_CREAT);
        dup2(err, STDERR_FILENO);
        
        execl( "/bin/sh", "sh", "-c", command, (char *)NULL );
    }

    int status;
    waitpid(p, &status, 0);
 
    return;
}

void parallelwriteoutput(int count, const char **argv_base, const char *out_path){
    // step 1: build argv
    char **argv = malloc((len(argv_base)+1)*sizeof(char**)); // one extra spot for child index
    int i = 0;
    while (argv_base[i] != NULL){
        argv[i] = argv_base[i];
        i++;
    }
    argv[i+1] = NULL; // skip over the second to last spot for the child index

    pid_t childPIDs[count];
    pid_t p = 1;
    for (int j = 0; j < count; j++){
        p = fork();
        int out = open(out_path, O_WRONLY | O_CREAT);
        dup2(out, STDOUT_FILENO); // redirect output to out.txt
        if (p == 0){ // child branch
            argv[i] = itoa(j); // fill in child index
            execv(
                argv_base[0], 
                argv
            );
        } else {
            childPIDs[j] = p; // save child PID so we can wait on it later
        }
    }

    int status;
    for (int k = 0; k < count; k++){
        waitpid(childPIDs[k], &status, 0); // wait on all the childPIDs we stored earlier
    }

    return;
}