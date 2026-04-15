#include "lab.h"
#include <string.h>
#include <time.h>

void tiny_sleep(long microseconds) {
    struct timespec ts;
    ts.tv_sec  = microseconds / 1000000;
    ts.tv_nsec = (microseconds % 1000000) * 1000;
    nanosleep(&ts, NULL);
}

/* forward function for "attack 0" case.
   change this code to implement your attack
 */
void forward_attack_0(struct message *message) {
    if (message->data[5] == '1')
        message = new_message('A', 'B', "PAY $10001000 TO M", true, false);
    send_message(message);
}

/* forward function for "attack 1" case.
   change this code to implement your attack
 */
void forward_attack_1(struct message *message) {
    if (message->from == 'A')
        message = new_message('A', 'B', "PAY $5000719 TO M", true, false);
    send_message(message);
}

/* forward function for "attack 2" case.
   change this code to implement your attack
 */
void forward_attack_2(struct message *message) {
    send_message(message);
    if (message->from == 'A'){
        tiny_sleep(100);
        send_message(message);
    }
}

/* forward function for "attack 3" case.
   change this code to implement your attack
 */
struct message *messages_3[3];
int messages_3_counter = 0;
void forward_attack_3_helper(struct message *message){
    if (messages_3_counter < 3) {
        messages_3[messages_3_counter] = malloc(sizeof(struct message));
        memcpy(messages_3[messages_3_counter], message, sizeof(struct message));
        messages_3_counter ++;
        
    } else {
        send_message(messages_3[2]);
        send_message(messages_3[1]);
        send_message(messages_3[2]);
    }
}
void forward_attack_3(struct message *message) {
    if (message->from == 'A'){
        forward_attack_3_helper(message);
        tiny_sleep(1000000);
    } else {
        send_message(message);
    }
}


/* forward function for "attack 4" case.
   change this code to implement your attack */
void forward_attack_4(struct message *message) {
    send_message(message);
}

/* forward function for "attack 5" case.
   I did not intend this one to be possible. */
void forward_attack_5(struct message *message) {
    send_message(message);
}
