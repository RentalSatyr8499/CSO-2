#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <time.h>



pthread_barrier_t barrier; // optional: to hopefully make deadlock more consistent

pthread_t philosopher[5];
pthread_mutex_t chopstick[5];

void exponentialSleep(int numAttempt){
    long minSleepTime = 50*pow(2, numAttempt-1);
    long maxSleepTime = 100*pow(2, numAttempt-1);
    long sleepTime = minSleepTime + rand() % (maxSleepTime - minSleepTime + 1);

    struct timespec ts;
    ts.tv_sec = sleepTime / 1000000;
    nanosleep(&ts, NULL);
}
int tryPickUp(void *arg) {
    int n = (int) (long)arg;

    // take two chopsticks
    pthread_mutex_lock(&chopstick[n]);
    printf("Philosopher %d got chopstick %d\n", n, n);

    if (pthread_mutex_trylock(&chopstick[(n+1)%5]) == 0) {
        printf("Philosopher %d got chopstick %d\n", n, (n+1)%5);
        return 0;
    }

    return 1;
}
void setBackDown(void *arg){
    int n = (int) (long)arg;
    printf("Philosopher %d set down chopstick %d\n", n, n);
    pthread_mutex_unlock(&chopstick[n]);
}
void *eat(void *arg){
    int n = (int) (long)arg;

    printf ("Philosopher %d is eating\n",n);
    sleep(1);
    
    // set them back down
    printf("Philosopher %d set down chopstick %d\n", n, (n+1)%5);
    pthread_mutex_unlock(&chopstick[(n+1)%5]);
    printf("Philosopher %d set down chopstick %d\n", n, n);
    pthread_mutex_unlock(&chopstick[n]);
    return NULL;
}
void *pickUpAndEat(void *arg){
    int eaten = 0;
    int numAttempt = 0;
    while (eaten != 1){
        if (tryPickUp(arg) != 0){
            setBackDown(arg);
            numAttempt += 1; 
            exponentialSleep(numAttempt);
            continue;
        } else {
            eat(arg);
            eaten = 1;
        }
    }
    return NULL;
} 


int main(int argc, const char *argv[]) {
    pthread_barrier_init(&barrier, NULL, 5);

    for(int i = 0; i < 5; i += 1)
        pthread_mutex_init(&chopstick[i], NULL);

    for(int i =0; i < 5; i += 1)
        pthread_create(&philosopher[i], NULL, pickUpAndEat, (void *)(size_t)i);

    for(int i=0; i < 5; i += 1)
        pthread_join(philosopher[i], NULL);

    for(int i=0; i < 5; i += 1)
        pthread_mutex_destroy(&chopstick[i]);

    pthread_barrier_destroy(&barrier);

    return 0;
}
