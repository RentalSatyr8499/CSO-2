#include "tlb.h"
#include <string.h>
#include <stdio.h>

#define NUMWAYS 4
#define NUMSETS 16
#define INDEXBITS 4

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


size_t tlb_hit(tlb_set *set, size_t va, int way){
    for (int i = 0; i < NUMWAYS; i ++){
        int *lru = &set->lru_statuses[i];
        tlb_entry entry = set->entries[i];

        if ((i != way)
         &&(entry.valid_bit == 1)
         &&(*lru < set->lru_statuses[way])){
            *lru += 1;
        }
    }

    set->lru_statuses[way] = 1;

    return (*set).entries[way].ppn | (va & ((1ULL << POBITS) - 1));
}

size_t tlb_miss(tlb_set *set, size_t va){
    size_t vpn = va >> POBITS;
    size_t tag = vpn >> INDEXBITS;
    size_t ppn = translate((va >> POBITS) << POBITS);
    if (ppn == (size_t)-1) return ppn;

    // "victim selection"
    int wayToWrite = -1;
    int maxLRU = -1;
    for (int i = 0; i < NUMWAYS; i++){ 
        if (set->lru_statuses[i] == 0) {
            wayToWrite = i; 
            break;
        }
        if (set->lru_statuses[i] > maxLRU) {
            maxLRU = set->lru_statuses[i];
            wayToWrite = i;
        }
    }

    for (int i = 0; i < NUMWAYS; i++){
        if ((i != wayToWrite) && (set->entries[i].valid_bit == 1)){
            set->lru_statuses[i] += 1;
        }
    }

    set->lru_statuses[wayToWrite] = 1;
    set->entries[wayToWrite] = (tlb_entry){ tag, ppn, 1 };

    return ppn | (va & ((1ULL << POBITS) - 1));
}

size_t tlb_translate(size_t va){
    size_t vpn = va >> POBITS;
    size_t tag = vpn >> INDEXBITS;
    tlb_set *set = &tlb[vpn & ((1ULL << INDEXBITS) - 1)];

    for (int i = 0; i < NUMWAYS; i++){
        tlb_entry entry = (*set).entries[i];
        if ((entry.tag == tag)
          &&(entry.valid_bit == 1)){
            return tlb_hit(set, va, i);
        }
    }

    return tlb_miss(set, va);
};

void tlb_clear(){
    for (int i = 0; i < NUMSETS; i++){
        for (int j = 0; j < NUMWAYS; j++){
            tlb[i].entries[j] = (tlb_entry){0,0};
        }
        memset(tlb[i].lru_statuses, 0, sizeof(tlb[i].lru_statuses)); // new lru_statuses
    }

};
int tlb_peek(size_t va){
    size_t vpn = va >> POBITS;
    size_t tag = vpn >> INDEXBITS;

    tlb_set *set = &tlb[vpn & ((1ULL << INDEXBITS) - 1)];
    for (int i = 0; i < NUMWAYS; i++){
        tlb_entry entry = (*set).entries[i];
        if ((entry.valid_bit == 1) 
         && (entry.tag == tag)){            
            return (*set).lru_statuses[i];
        }
    }
    return 0;
};
