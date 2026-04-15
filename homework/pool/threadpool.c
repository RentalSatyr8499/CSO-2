#include <pthread.h>
#include "pool.h"
#include <stdbool.h>
#include <stdlib.h>

#define MAXTASKS 200

typedef struct Task {
    task_fn fn;
    void *arg;
    void *result;
    bool done;
} Task;
typedef struct TaskQueue {
    Task *queue[200];
    int tail_index;
    int head_index;

    pthread_mutex_t lock;
    pthread_cond_t task_ready;
    pthread_cond_t task_completed;
    int completed_prefix;
    bool is_stopping;
} TaskQueue;
typedef struct ThreadPool {
    pthread_t *pool;
    int num_threads;
} ThreadPool;

// globalssss
TaskQueue task_queue;
ThreadPool thread_pool;

void advance_completed_prefix(int i){
    if ((task_queue.completed_prefix == i-1)&&
        (i < task_queue.tail_index)&&
        (task_queue.queue[i]->done)){
        task_queue.completed_prefix += 1;
        advance_completed_prefix(i+1);
    } 
}
void *worker_thread(void* _info){
    while(1){
        pthread_mutex_lock(&task_queue.lock);
        while ((task_queue.head_index == task_queue.tail_index) && (!task_queue.is_stopping)){
            pthread_cond_wait(&task_queue.task_ready, &task_queue.lock);
        }
        if (task_queue.is_stopping){
        pthread_mutex_unlock(&task_queue.lock);
            break;
        }
        int id = task_queue.head_index;
        task_queue.head_index += 1;
        pthread_mutex_unlock(&task_queue.lock);

        task_fn fn = task_queue.queue[id]->fn;
        void *arg = task_queue.queue[id]->arg;
        void *result = malloc(sizeof(void *));
        result = fn(arg);
        
        pthread_mutex_lock(&task_queue.lock);
        task_queue.queue[id]->result = result;

        task_queue.queue[id]->done = true;
        advance_completed_prefix(id);
        pthread_cond_broadcast(&task_queue.task_completed);
        pthread_mutex_unlock(&task_queue.lock);

    }
    
    return NULL;
}
void pool_setup(int threads){
    pthread_mutex_init(&task_queue.lock, NULL);
    pthread_mutex_lock(&task_queue.lock); 
    pthread_cond_init(&task_queue.task_ready, NULL);
    pthread_cond_init(&task_queue.task_completed, NULL);

    task_queue.is_stopping = false;
    task_queue.completed_prefix = -1; // what does this need to be intialized to?

    task_queue.head_index = 0;
    task_queue.tail_index = 0;
    pthread_mutex_unlock(&task_queue.lock);

    thread_pool.pool = malloc(sizeof(pthread_t) * threads);
    thread_pool.num_threads = threads;
    for (int i = 0; i < threads; i++){
        pthread_create(&thread_pool.pool[i], NULL, worker_thread, NULL);
    }
};
int pool_submit_task(task_fn task, void *argument){
    pthread_mutex_lock(&task_queue.lock);
    Task *new_task = malloc(sizeof(Task));
    *new_task = (Task){
        task,           // taskfn
        argument,       // arg
        NULL,           // result
        false,          // done
    };
    task_queue.queue[task_queue.tail_index] = new_task;
    int id = task_queue.tail_index ++;
    pthread_cond_signal(&task_queue.task_ready);
    pthread_mutex_unlock(&task_queue.lock);

    return id;
}
void *pool_get_task_result(int task_id){
    return task_queue.queue[task_id]->result;
}
void pool_stop(void){
    pool_wait();
    
    pthread_mutex_lock(&task_queue.lock);
    task_queue.is_stopping = true;
    pthread_cond_broadcast(&task_queue.task_ready);
    pthread_mutex_unlock(&task_queue.lock);

    for (int i = 0; i < thread_pool.num_threads; i++){
        void *result;
        pthread_join(thread_pool.pool[i], &result);
    }

    pthread_cond_destroy(&task_queue.task_ready);
    pthread_cond_destroy(&task_queue.task_completed);
    pthread_mutex_destroy(&task_queue.lock);
}
void pool_wait(void){
    // if it becomes a problem: tasks submitted after pool_wait was called but completed before pool_wait returns are also...returned?
    pthread_mutex_lock(&task_queue.lock);
    int pending_tasks_snapshot = task_queue.tail_index - 1;
    while (pending_tasks_snapshot > task_queue.completed_prefix){
        pthread_cond_wait(&task_queue.task_completed, &task_queue.lock);
    }
    pthread_mutex_unlock(&task_queue.lock);
    return;
}