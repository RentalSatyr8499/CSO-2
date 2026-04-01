#define _POSIX_C_SOURCE 199309L
#include <stdio.h> // fopen, fread, fclose, printf, fseek, ftell
#include <math.h> // log, exp
#include <stdlib.h> // free, realloc
#include <time.h> // struct timespec, clock_gettime, CLOCK_REALTIME
#include <errno.h>

typedef double (*geomean_fn)(unsigned char *, size_t);

typedef struct {
    geomean_fn fn;
    const char *label;
} labelled_fn;


// computes the geometric mean of a set of values.
// You should use OpenMP to make faster versions of this.
// Keep the underlying sum-of-logs approach.
double geomean(unsigned char *s, size_t n) {
    double answer = 0;
    for(int i=0; i<n; i+=1) {
        if (s[i] > 0) answer += log(s[i]) / n;
    }
    return exp(answer);
}

double g1(unsigned char *s, size_t n) {
    double answer = 0;
    # pragma omp parallel for
    for(int i=0; i<n; i+=1) {
        if (s[i] > 0){
            double result = log(s[i]) / n;
            #pragma omp atomic update
            answer += result;
        }
    }
    return exp(answer);
}

double g2(unsigned char *s, size_t n) {
    double answer = 0;
    int j = 0;

    #pragma omp parallel
    while (1) {
        int i;
        #pragma omp atomic capture
        i = j++;
        if (i >= n) break;
        if (s[i] > 0) {
            double result = log(s[i]) / n;
            #pragma omp atomic update
            answer += result;
        }
    }
    return exp(answer);
}

double g3(unsigned char *s, size_t n) {
    double answer = 0;
    int j = 0;
    int k = 100; 

    #pragma omp parallel
    while (1) {
        int i;
        #pragma omp atomic capture
        i = j += k;
        if (i - k >= n) break;
        
        double large_task = 0;
        for (int l = i - k; l < i && l < n; l++) {
            if (s[l] > 0) large_task += log(s[l]) / n;
        } // iterate through the small tasks
        
        #pragma omp atomic update
        answer += large_task;
    }
    return exp(answer);
}

double g4(unsigned char *s, size_t n) {
    double answer = 0;
    # pragma omp parallel for reduction(+:answer)
    for(int i=0; i<n; i+=1) {
        if (s[i] > 0){
            double result = log(s[i]) / n;
            answer += result;
        }
    }
    return exp(answer);
}

double g5(unsigned char *s, size_t n) {
    double answer = 0;
    int j = 0;
    int k = 100; 

    #pragma omp parallel reduction(+:answer)
    while (1) {
        int i;
        #pragma omp atomic capture
        i = j += k;
        if (i - k >= n) break;
        
        double large_task = 0;
        for (int l = i - k; l < i && l < n; l++) {
            if (s[l] > 0) large_task += log(s[l]) / n;
        } // iterate through the small tasks
        
        answer += large_task;
    }
    return exp(answer);
}

double g6(unsigned char *s, size_t n) {
    double answer = 0;

    # pragma omp parallel 
    {
        double local_answer = 0;
        #pragma omp for nowait
        for(int i=0; i<n; i+=1) {
            if (s[i] > 0){
                double result = log(s[i]) / n;
                local_answer += result;
            }
        }
        #pragma omp atomic update
        answer += local_answer;
    }

    return exp(answer);
}

const int NUM_TESTS = 7;
labelled_fn tests[] = {
    { geomean, "original function geomean()" },
    { g1,      "even split + atomic update g1()" },
    { g2,      "task queue + atomic update g2()" },
    { g3,      "task queue with larger tasks + atomic update g3()" }, 
    { g4,      "even split + reduction(op:) g4()" }, 
    { g5,      "task queue with larger tasks + reduction(op:) g5()" }, 
    { g6,      "even split + manual many-to-few g6()" }, 

};

/// nanoseconds that have elapsed since 1970-01-01 00:00:00 UTC
long long nsecs() {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return t.tv_sec*1000000000 + t.tv_nsec;
}

/// reads arguments and invokes geomean; should not require editing
int main(int argc, char *argv[]) {
    // step 1: get the input array (the bytes in this file)
    char *s = NULL;
    size_t n = 0;

    for (int i = 1; i < argc; i += 1) {
        FILE *f = fopen(argv[i], "rb");
        if (f) {
            fseek(f, 0, SEEK_END);
            size_t size = ftell(f);
            fseek(f, 0, SEEK_SET);

            s = realloc(s, n + size);
            fread(s + n, 1, size, f);
            fclose(f);

            n += size;
        } else {
            errno = 0;
        }
    }

    // open results file
    FILE *out = fopen("results.txt", "w");
    if (!out) {
        perror("results.txt");
        free(s);
        return 1;
    }

    int W = 2;
    int K = 20;
    // run each labelled function
    for (int t = 0; t < NUM_TESTS; t++) {

        // warm-up runs
        for (int w = 0; w < W; w++) {
            tests[t].fn((unsigned char*) s, n);
        }

        // measured runs
        long long total_time = 0;
        double last_answer = 0;

        for (int r = 0; r < K; r++) {
            long long t0 = nsecs();
            last_answer = tests[t].fn((unsigned char*) s, n);
            long long t1 = nsecs();
            total_time += (t1 - t0);
        }

        long long avg_time = total_time / K;

        fprintf(out, "%s: %lld ns to process %zd characters: %g\n",
                tests[t].label, avg_time, n, last_answer);
    }


    fclose(out);
    free(s);

    return 0;
}
