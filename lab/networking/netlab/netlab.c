#include <stdio.h>
#include "netsim.h"

int next_sequence_num = 1;
int current_timer_id = 0;
char last_ack[5] = {0, 0, 1, 0, 0};

void fill_checksum(size_t len, void *_data){
    char *data = _data;
    char checksum = 0;
    for (int i = 1; i < len; i++){
        checksum ^= data[i];
    }
    data[0] = checksum;
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
}
int verify_packet(int len, void* _data){
    char *data = _data;
    char checksum = 0;
    for (int i = 1; i < len; i++){
        checksum ^= data[i];
    }

    return (checksum == data[0]) ? 0 : -1;
}
void request_next(void* _data){
    new_ack((char *)_data);
    send_ack();

    current_timer_id = setTimeout(send_ack, 1000, NULL);
}
void recvd(size_t len, void* _data) {
    char *data = _data;

    if (data[1] == next_sequence_num){
        /*
        if (verify_packet(len, data) == -1){
            send_ack();
            return;
        }
        */
        clearTimeout(current_timer_id);
        next_sequence_num += 1;

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

