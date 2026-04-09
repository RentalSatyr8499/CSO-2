#define _GNU_SOURCE
#include "life.h"
#include <pthread.h>

typedef struct {
    LifeBoard *even_board;
    LifeBoard *odd_board;
    int num_steps;
    int y_start, y_end;
    pthread_barrier_t *barrier;
} ThreadInfo;

void update_cell(int x, int y, LifeBoard *source, LifeBoard *desination){ 
    int live_in_window = 0;
    for (int y_offset = -1; y_offset <= 1; y_offset += 1)
        for (int x_offset = -1; x_offset <= 1; x_offset += 1)
            if (LB_get(source, x + x_offset, y + y_offset))
                live_in_window += 1;
    LB_set(desination, x, y,
        live_in_window == 3 ||
        (live_in_window == 4 && LB_get(source, x, y))
    );
}

void *worker_thread(void* _info){
    ThreadInfo *info = (ThreadInfo*)_info;
    LifeBoard *source_board;
    LifeBoard *destination_board;
    for (int s = 0; s < info->num_steps; s++){

        if (s % 2 == 0){
            source_board = info->even_board;
            destination_board = info->odd_board;
        } else {
            source_board = info->odd_board;
            destination_board = info->even_board;
        }

        for (int y = info->y_start; y < info->y_end; y++){
            for (int x = 1; x < info->even_board->width - 1; x++){
                update_cell(x, y, source_board, destination_board);
            }
        }
        pthread_barrier_wait(info->barrier);
    }
    return NULL;
}
void simulate_life_parallel(int threads, LifeBoard *state, int steps) {
    LifeBoard *next_state = LB_new(state->width, state->height);
    pthread_barrier_t barrier; pthread_barrier_init(&barrier, NULL, threads);
    pthread_t thread[threads];
    ThreadInfo info[threads];

    int chunk_size = (state->height - 2) / threads;
    for (int i = 0; i < threads; i++){
        info[i] = (ThreadInfo){
            state,
            next_state,
            steps,
            chunk_size*i, chunk_size*(i+1), // y_start and y_end
            &barrier
        };
    }
    info[0].y_start = 1;
    info[threads-1].y_end = state->height-2; // remainder goes to last thread

    for (int i = 0; i < threads; i++){
        pthread_create(&thread[i], NULL, worker_thread, &info[i]);
    }
    for (int i = 0; i < threads; i++){
        pthread_join(thread[i], NULL);
    }

    if (steps % 2 != 0){ // if there were an odd number of steps, final state needs to be copied from next state buffer
        LB_swap(state, next_state);
    }

    pthread_barrier_destroy(&barrier);
    LB_del(next_state);
}
