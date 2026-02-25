#include "config.h"
#include <stddef.h>
#include "mlpt.h"
#include <stdint.h>

size_t translate(size_t va){
    size_t pageBase = traversePTEs(ptbr, va, 0);
    size_t physicalAddress = (pageBase | ((1 << POBITS)-1) & va);
    return  *(size_t *)physicalAddress;
}
size_t traversePTEs(size_t ppn, size_t va, int currLevel){
    size_t ptePtr = ppn;

    int shift = POBITS + (LEVELS - 1 - currLevel) * (POBITS - 3); // 8-byte PTEs, asssumes currLevel starts at 0
    size_t mask = (1ULL << (POBITS - 3)) - 1;
    size_t index = (va >> shift) & mask;
    ptePtr += index * 8;

    uint64_t pte = *((uint64_t *)ptePtr); 

    if ((pte & 1) == 0){ 
        return -1;
    } else if (currLevel + 1 == LEVELS){
        return ((pte >> POBITS) << POBITS);
    } else {
        return traversePTEs(((pte >> POBITS) << POBITS), va, currLevel + 1);
    }
}
