#define _XOPEN_SOURCE 700
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "toMeasure.h"

const int NUM_ITERATIONS = 1000;
long long MEASUREMENT_OVERHEAD = 0; // set in main
long long measureFunction(void (*setup)(), void (*fxn)()) {
    if (setup){
        setup();
    }
    struct timespec initialTime;
    struct timespec finalTime;
    long long runningTotal = 0;
    for (int i = 0; i < NUM_ITERATIONS; i ++){
        clock_gettime(CLOCK_MONOTONIC, &initialTime);
        fxn();
        clock_gettime(CLOCK_MONOTONIC, &finalTime);
        runningTotal += 
            (finalTime.tv_sec - initialTime.tv_sec)*1000000000 
            + (finalTime.tv_nsec - initialTime.tv_nsec);
    }
    
    return (long long)(runningTotal / NUM_ITERATIONS) - MEASUREMENT_OVERHEAD;
}

int main(int argc, char* argv[]) {
    MEASUREMENT_OVERHEAD = measureFunction(NULL, randomCall); // for some reason I've found that measureFunction needs to warm up first before measuring the actual overhead
    MEASUREMENT_OVERHEAD = measureFunction(NULL, baseTime);
    
    void (*scenarios[8][2])() = {
        { NULL, emptyCall },
        { NULL, randomCall },
        { NULL, parentPID },
        { setupForkToParent, forkToParent },
        { setupWaitpidExited, waitpidExited },
        { NULL, immediateExit },
        { NULL, execTrue },
        { NULL, dirOps },
    };
    if (argc > 1){
        if  ((strlen(argv[1]) == 1) && 
            (argv[1][0] >= '1') && 
            (argv[1][0] <= '8')){

            void (**scenario)() = scenarios[atoi(argv[1]) - 1];
            long long time = measureFunction(scenario[0], scenario[1]);
            
            printf("Over %d iterations, scenario %s took an average of %lld nanoseconds\n", NUM_ITERATIONS, argv[1], time);
            
            return 0;
        } else if (strcmp(argv[1], ".")== 0){
            FILE *file = fopen("timings.txt", "w");
            long long time;
            for (int i = 0; i < 8; i++){
                time = measureFunction(scenarios[i][0], scenarios[i][1]);
                fprintf(file, "Scenario %d: %lld nanoseconds\n", i+1, time);
            }
            printf("wrote to timings.txt ♥\n");
            fclose(file);

            return 0;
        }
    }
    printf("unrecognized input ♥\n");
    return 0;
}