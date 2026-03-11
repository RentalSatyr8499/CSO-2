#define _XOPEN_SOURCE 700
#include "config.h"
#include <stddef.h>
#include "mlpt.h"
#include <stdint.h>
#include <stdlib.h>

#include <stdio.h>

size_t ptbr;
size_t allocation_count = 0;

uint64_t* getPTEptr(size_t ppn, size_t va, int currLevel){
    uint64_t *ptePtr = (uint64_t *) ppn;

    int shift = POBITS + (LEVELS - 1 - currLevel) * (POBITS - 3); // 8-byte PTEs, asssumes currLevel starts at 0
    size_t mask = (1ULL << (POBITS - 3)) - 1;
    size_t index = (va >> shift) & mask;
    ptePtr += index;

    return ptePtr; 
}

size_t traversePTEs(size_t ppn, size_t va, int currLevel){
    uint64_t pte = *((uint64_t *)getPTEptr(ppn, va, currLevel)); 
    
    if ((pte & 1) == 0){ 
        return -1;
    } else if (currLevel + 1 == LEVELS){
        return ((pte >> POBITS) << POBITS);
    } else {
        return traversePTEs(((pte >> POBITS) << POBITS), va, currLevel + 1);
    }
}

size_t translate(size_t va){
    if (ptbr == 0){
        return -1;
    } 

    size_t pageBase = traversePTEs(ptbr, va, 0);
    if (pageBase == -1){
        return -1;
    }
    
    size_t physicalAddress = (pageBase | (((1 << POBITS)-1) & va));
    return physicalAddress;
}

void* initialize_page(){
    void* x;
    posix_memalign(&x, 1ULL << POBITS, 1ULL << POBITS); // 1ULL << POBITS gives 2^POBITS
    allocation_count++;
    return x;
}
void zero_validbits(size_t pa){
    for (int i = 0; i < (1ULL << (POBITS - 3)); i++){
        *((uint64_t*)pa + i) = 0;
    }
}
size_t traverseAndAllocate(size_t ppn, size_t va, int currLevel){
    uint64_t *ptePtr = getPTEptr(ppn, va, currLevel);

    if ((*ptePtr & 1) == 0){ 
        *ptePtr = ((size_t) initialize_page()) | 1;
        
        if (currLevel + 1 == LEVELS){
            return 1;
        } else {
            zero_validbits((*ptePtr >> POBITS) << POBITS);
        }
    } else if (currLevel + 1 == LEVELS){
        return 0;
    } 
    return traverseAndAllocate(((*ptePtr >> POBITS) << POBITS), va, currLevel + 1);
}
int allocate_page(size_t start_va){
    if (ptbr == 0){
        ptbr = (size_t) initialize_page();
    }

    if (start_va & ((1ULL << POBITS) - 1)) {
        return -1;
    }
    return traverseAndAllocate(ptbr, start_va, 0);
}
