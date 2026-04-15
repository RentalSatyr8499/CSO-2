#include <pthread.h>
#include "pool.h"

#define MAXTASKS 200

typedef struct Task {
    task_fn fn;
    void *arg;
    void *result;
} Task;
typedef struct TaskQueue {
    Task *queue[200];
    int tail_index;
    int head_index;

    pthread_mutex_t lock;
    pthread_cond_t task_ready;
} TaskQueue;

// globalssss
TaskQueue task_queue;
pthread_t *thread_pool;

void *worker_thread(void* _info){
    pthread_mutex_lock(&task_queue.lock);
    while (task_queue.head_index == task_queue.tail_index){
        pthread_cond_wait(&task_queue.task_ready, &task_queue.lock);
    }
    int id = task_queue.head_index;
    task_queue.head_index += 1;
    pthread_mutex_unlock(&task_queue.lock);

    task_fn fn = task_queue.queue[id]->fn;
    void *arg = task_queue.queue[id]->arg;
    task_queue.queue[id]->result = fn(arg); // i suppose because this is not under the task queue lock it's unsafe, but the logic of the program should guarantee only one thread modifies a given Task's result at a time. And adding the lock will impact performance, too, right? - updating the result needs to be scheduled with all other task queue stuff.

}
void pool_setup(int threads){
    pthread_mutex_lock(&task_queue.lock);
    pthread_mutex_init(&task_queue.lock, NULL);
    pthread_cond_init(&task_queue.task_ready, NULL);
    task_queue.head_index = 0;
    task_queue.tail_index = 0;
    pthread_mutex_unlock(&task_queue.lock);

    thread_pool = malloc(sizeof(pthread_t) * threads);
    for (int i = 0; i < threads; i++){
        pthread_create(&thread_pool[i], NULL, worker_thread, NULL);
    }
};

int pool_submit_task(task_fn task, void *argument){
    pthread_mutex_lock(&task_queue.lock);
    Task *new_task = malloc(sizeof(Task));
    *new_task = (Task){
        task,           // taskfn
        argument,       // arg
        NULL,           // result
    };
    task_queue.queue[task_queue.tail_index] = new_task;
    int id = task_queue.tail_index ++;
    pthread_cond_signal(&task_queue.task_ready);
    pthread_mutex_unlock(&task_queue.lock);

    return id;
}

void *pool_get_task_result(int task_id){
    return (void *)(task_queue.queue[task_id]);
}

void pool_stop(void){
    pthread_cond_destroy(&task_queue.task_ready);
    pthread_mutex_destroy(&task_queue.lock);
}