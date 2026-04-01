#include "tlb.h"
#include <string.h>
#include <stdio.h>

#define NUMWAYS 4
#define NUMSETS 16
#define INDEXBITS 4

#define DEBUG1 0
#define DEBUG2 1

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


void write_missed_entry(tlb_set *set, size_t tag, size_t ppn){
    int wayToWrite = -1;
    for (int i = 0; i < NUMWAYS; i++){
        int *lru = &set->lru_statuses[i];
        if ((*lru == NUMWAYS)||(*lru == 0)){
            if (wayToWrite == -1) {
                *lru = 1;
                wayToWrite = i;
            }
        } else {
            *lru += 1;
        }
    }

    set->entries[wayToWrite] = (tlb_entry){
        tag,
        ppn,
        1
    };


    if (DEBUG1) printf("wrote to way %d.\n", wayToWrite);

    if (DEBUG2){
        printf("set %d's tags and lru bits: ", (int) (set - tlb));
        for (int i = 0; i < NUMWAYS; i++){
            printf("(%lx, %d) ", set->entries[i].tag, set->lru_statuses[i]);
        }
        printf("\n");
    }
}

size_t tlb_translate(size_t va){
    if (DEBUG2) printf("---------\ntranslating va %lx\n", va);
    size_t vpn = va >> POBITS;
    size_t tag = vpn >> INDEXBITS;

    tlb_set *set = &tlb[vpn & ((1ULL << INDEXBITS) - 1)];
    for (int i = 0; i < NUMWAYS; i++){
        tlb_entry entry = (*set).entries[i];
        if ((entry.tag == tag)&&(entry.valid_bit == 1)){
          
            if (DEBUG1) printf("translated address %zx: cache hit in way %d, found ppn %zx for tag %zx (%zx). returning %llx\n", va, i, (*set).entries[i].ppn, entry.tag, tag, (*set).entries[i].ppn | (va & ((1ULL << POBITS) - 1)));

            return (*set).entries[i].ppn | (va & ((1ULL << POBITS) - 1));
        }
    }

    size_t ppn = translate((va >> POBITS) << POBITS);

    if (DEBUG2) printf("writing to set %ld the tag %lx and ppn %lx\n", set - tlb, tag, ppn);
    write_missed_entry(set, tag, ppn);

    if (DEBUG1) printf("translated address %zx: cache miss. wrote ppn %zx for vpn %zx. returning pa of %llx\n", va, ppn, (va >> POBITS) << POBITS, ppn | (va & ((1ULL << POBITS) - 1)));

    return ppn | (va & ((1ULL << POBITS) - 1));
};

void tlb_clear(){
    for (int i = 0; i < NUMSETS; i++){
        for (int j = 0; j < NUMWAYS; j++){
            tlb[i].entries[j] = (tlb_entry){0,0};
        }
        memset(tlb[i].lru_statuses, 0, sizeof(tlb[i].lru_statuses)); // new lru_statuses
    }

    if (DEBUG1) printf("cleared tlb\n");
};
int tlb_peek(size_t va){
    if (DEBUG2) printf("---------\npeeking address %lx\n", va);

    size_t vpn = va >> POBITS;
    size_t tag = vpn >> INDEXBITS;

    tlb_set *set = &tlb[vpn & ((1ULL << INDEXBITS) - 1)];
    for (int i = 0; i < NUMWAYS; i++){
        tlb_entry entry = (*set).entries[i];
        if ((entry.valid_bit == 1) 
         && (entry.tag == tag)){

            if (DEBUG1) printf("peeked at set %lld, way %d. found lru value %d\n", vpn & ((1ULL << INDEXBITS)-1), i, (*set).lru_statuses[i]);
            
            return (*set).lru_statuses[i];
        }
        if (DEBUG2) printf("searching set %lld, way %d holds a tag of %lx (!= %lx)\n", vpn & ((1ULL << INDEXBITS)-1), i, (*set).entries[i].tag, tag);
    }

    if (DEBUG1) printf("tried to find tag %zx in set %lld, but couldn't finy any. returning 0.\n", tag, vpn & ((1ULL << INDEXBITS)-1));

    return 0;
};
