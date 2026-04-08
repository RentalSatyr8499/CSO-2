#include <stdio.h>
#include "netsim.h"

int next_sequence_num = 1;
int total_sequence_count = 0;
int current_timer_id = 0;
char last_ack[5] = {0, 0, 1, 0, 0};
char get[5];

void fill_checksum(size_t len, void *_data){
    char *data = _data;
    char checksum = 0;
    for (int i = 1; i < len; i++){
        checksum ^= data[i];
    }
    data[0] = checksum;
}
int verify_packet(int len, void* _data){
    char *data = _data;
    char checksum = 0;
    for (int i = 1; i < len; i++){
        checksum ^= data[i];
    }

    return (checksum == data[0]) ? 0 : -1;
}
void new_ack(void* _data){
    char ack[5] = {0, 'A', 'C', 'K', ((char *)_data)[1]};
    fill_checksum(5, &ack);

    for (int i = 0; i < 5; i++){
        last_ack[i] = ack[i];
    }
}
void send_ack(){
    send(5, last_ack);
    if (total_sequence_count == 0 ||  next_sequence_num <= total_sequence_count) {
        current_timer_id = setTimeout(send_ack, 1000, NULL);
    }
}
void request_next(void* _data){
    new_ack((char *)_data);
    send_ack();

    current_timer_id = setTimeout(send_ack, 1000, NULL);
}
void recvd(size_t len, void* _data) {
    char *data = _data;
    if (verify_packet(len, data) == -1){
        if (total_sequence_count == 0)
            send(5, get);
        else
            send_ack();
        return;
    }
    if (data[1] == next_sequence_num){
        clearTimeout(current_timer_id);
        next_sequence_num += 1;
        total_sequence_count = data[2];

        fwrite(data+3,1,len-3,stdout);
        fflush(stdout);

        if (next_sequence_num != data[2]+1){
            request_next(data);
        }
    } 
}
void send_get(void* arg){
    char data[5];
    data[1] = 'G'; data[2] = 'E'; data[3] = 'T'; data[4] = *((int *)arg);
    fill_checksum(5, data);

    send(5, data);
    for (int i = 0; i < 5; i ++){
        get[i] = data[i];
    }
}
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s n\n    where n is a number between 0 and 3\n", argv[0]);
        return 1;
    }

    send_get((void *)&argv[1][0]);
    current_timer_id = setTimeout(send_get, 1000, (void *)&argv[1][0]);

    waitForAllTimeoutsAndMessagesThenExit();
}

