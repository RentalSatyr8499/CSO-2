#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void writeoutput(const char *command, const char *out_path, const char *err_path){
    pid_t p = fork();
    if (p == 0){
        int out = open(out_path, O_WRONLY | O_CREAT | O_APPEND, 0);
        dup2(out, STDOUT_FILENO);
        
        int err = open(err_path, O_WRONLY | O_CREAT | O_APPEND);
        dup2(err, STDERR_FILENO);
        
        execl( "/bin/sh", "sh", "-c", command, (char *)NULL );
    }

    int status;
    waitpid(p, &status, 0);
 
    return;
}

void writegetoutput(){
    return;
}
void parallelwriteoutput(int count, const char **argv_base, const char *out_path){
    // step 1: build argv
    char **argv = NULL;
    int i = 0;
    while (argv_base[i] != NULL){
        argv = realloc(argv, (i+1)*sizeof(char*));
        argv[i] = strdup(argv_base[i]);
        i++;
    }
    
    argv = realloc(argv, (i+2) * sizeof(char*)); // space for child index and NULL terminator 
    argv[i] = NULL; 
    argv[i+1] = NULL; 
    pid_t childPIDs[count];
    pid_t p = 1;

    // step 2: run commands 
    int original_stdout = dup(STDOUT_FILENO); // redirect to out.txt
    int out = open(out_path, O_WRONLY | O_CREAT | O_APPEND, 0);
    dup2(out, STDOUT_FILENO); 
    
    for (int j = 0; j < count; j++){ // run count children
        p = fork();
        if (p == 0){ // child branch   
            char *idx_str = malloc(16); // fill in child index
            snprintf(idx_str, 16, "%d", j);
            argv[i] = idx_str;
            argv[i+1] = NULL;

            execv(argv_base[0], argv);
            
            free(idx_str);
            close(out);
            perror("execv");
            _exit(1);            
        } else {
            childPIDs[j] = p; // save child PID so we can wait on it later
        }
    }

    // step 3. clean up
    for (int k = 0; k < count; k++){
        waitpid(childPIDs[k], NULL, 0); // wait on all the childPIDs we stored earlier
    }

    close(out); // restore stdout
    dup2(original_stdout, STDOUT_FILENO);

    for (int k = 0; k <= i; k++){ // free dynamic memory
        free(argv[k]);
    }
    free(argv);
    return;
}