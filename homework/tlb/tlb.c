#include "tlb.h"
#define NUMWAYS 4
#define NUMSETS 16

typedef struct {
    size_t tag;
    size_t ppn;
    int valid_bit;
} tlb_entry;
typedef struct {
    tlb_entry entries[NUMWAYS];
    int lru_statuses[NUMWAYS];
} tlb_set;
tlb_set tlb[NUMSETS];

void create_empty_tlb(){
    for (int i = 0; i < NUMSETS; i++){
        for (int j = 0; j < NUMWAYS; j++){
            tlb[i].entries[j] = (tlb_entry){0,0};
        }
        tlb[i].lru_statuses = (int[]){0, 0, 0, 0}; // new lru_statuses
    }
}

size_t tlb_translate(size_t va){
    if (tlb == NULL){
        create_empty_tlb();
    }

    // more translation logic
};

void write_entry(tlb_set *set, size_t tag, size_t ppn){
    int wayToWrite = 0;
    for (int i = 0; i < NUMWAYS; i++){
        if ((set->lru_statuses[i] == NUMWAYS)||(set->lru_statuses[i] == 0)){
            set->lru_statuses[i] = 1;
            wayToWrite = i;
        } else {
            set->lru_statuses[i] += 1;
        }
    }
    set->entries[wayToWrite] = (tlb_entry){
        tag,
        ppn,
        1
    };
}
void tlb_clear();
int tlb_peek(size_t va);
