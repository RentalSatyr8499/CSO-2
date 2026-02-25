#include "config.h"
#include <stddef.h>
#include "mlpt.h"


size_t translate(size_t va){
    // mask off po
    // traverse ptes
    size_t ppn = traversePTEs(ptbr, va, 1);
    // concatenate with offset
    // access to get final value
}
size_t traversePTEs(size_t ppn, size_t vpn, int currLevel){
    size_t ptePtr = ppn;
    ptePtr += (vpn >> POBITS + LEVELS - currLevel)*8; // 8-byte PTEs, uh vpn calculation is wrong
    size_t pte = (int *)ptePtr; // this is wrong, pte needs to be cast to a pointer to an 8-byte value

    if (pte & 1 != 1){ 
        return -1;
    } else if (currLevel = LEVELS){
        return ((pte >> POBITS) << POBITS);
    } else {
        return traversePTEs(pte >> POBITS, vpn, currLevel + 1);
    }
}
