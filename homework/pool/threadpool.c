#include <pthread.h>
#include "pool.h"

#define MAXTASKS 200

typedef struct Task {
    task_fn fn;
    void *arg;
    void *result;

    int id;
    struct Task *prev;
    struct Task *next;
} Task;
typedef struct TaskQueue {
    Task *head;
    Task *tail;
    
    int id_counter;
    pthread_mutex_t lock;
    pthread_cond_t task_ready;
} TaskQueue;

// globalssss
TaskQueue task_queue;
pthread_t *thread_pool;

void *worker_thread(void* _info){
    pthread_mutex_lock(&task_queue.lock);
    // finish
}
void pool_setup(int threads){
    pthread_mutex_lock(&task_queue.lock);
    pthread_mutex_init(&task_queue.lock, NULL);
    pthread_cond_init(&task_queue.task_ready, NULL);
    task_queue.id_counter = 0;
    task_queue.head = NULL;
    task_queue.tail = NULL;
    pthread_mutex_unlock(&task_queue.lock);

    thread_pool = malloc(sizeof(pthread_t) * threads);
    for (int i = 0; i < threads; i++){
        pthread_create(&thread_pool[i], NULL, worker_thread, NULL);
    }
};

int pool_submit_task(task_fn task, void *argument){
    pthread_mutex_lock(&task_queue.lock);
    int id = task_queue.id_counter ++;
    Task *old_tail = task_queue.tail->next;
    Task *new_task = malloc(sizeof(Task));
    *new_task = (Task){
        task,           // taskfn
        argument,       // arg
        NULL,           // result
        id,             // id
        old_tail,       // prev
        NULL            // next
    };
    if (task_queue.head == NULL) task_queue.head = new_task;
    pthread_cond_signal(&task_queue.task_ready);
    pthread_mutex_unlock(&task_queue.lock);

    return id;
}

void *pool_get_task_result(int task_id){

}

void pool_stop(void){
    pthread_cond_destroy(&task_queue.task_ready);
    pthread_mutex_destroy(&task_queue.lock);
}