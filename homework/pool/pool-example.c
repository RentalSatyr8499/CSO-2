#include <stdio.h>
#include <time.h>
#include "pool.h"

#define THREADS 2
#define TASKS_BEFORE_WAIT 3
#define TASKS_AFTER_WAIT 3

static void *task_function(void *argument) {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 200 * 1000 * 1000;   // 200ms

    printf("task %ld starting\n", (long) argument);
    nanosleep(&ts, NULL);
    printf("task %ld finishing\n", (long) argument);

    return argument;
}

static void *int_to_pointer(int x) {
    return (void *) (long) x;
}

int main() {
    int task_ids[TASKS_BEFORE_WAIT + TASKS_AFTER_WAIT];
    pool_setup(THREADS);

    printf("submitting tasks BEFORE wait\n");
    for (int i = 0; i < TASKS_BEFORE_WAIT; i += 1) {
        task_ids[i] = pool_submit_task(task_function, int_to_pointer(i * 10));
        printf("pool_submit_task returned id %d\n", task_ids[i]);
    }

    printf("calling pool_wait()\n");
    pool_wait();
    printf("pool_wait() returned\n");

    for (int i = 0; i < TASKS_BEFORE_WAIT; i += 1) {
        printf("result for task id %d is %p (expected %p)\n",
            task_ids[i], pool_get_task_result(task_ids[i]), int_to_pointer(i * 10));
    }

    printf("submitting tasks AFTER wait\n");
    for (int i = TASKS_BEFORE_WAIT; i < TASKS_BEFORE_WAIT + TASKS_AFTER_WAIT; i += 1) {
        task_ids[i] = pool_submit_task(task_function, int_to_pointer(i * 10));
        printf("pool_submit_task returned id %d\n", task_ids[i]);
    }

    printf("calling pool_stop()\n");
    pool_stop();
    printf("pool_stop() returned\n");

    for (int i = TASKS_BEFORE_WAIT; i < TASKS_BEFORE_WAIT + TASKS_AFTER_WAIT; i += 1) {
        printf("result for task id %d is %p (expected %p)\n",
            task_ids[i], pool_get_task_result(task_ids[i]), int_to_pointer(i * 10));
    }
}
