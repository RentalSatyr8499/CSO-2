#include <stdio.h>
#include <assert.h>
#include "mlpt.h"
#include "config.h"
#include <stdalign.h>

extern size_t allocation_count;

int main(void) {

    ptbr = 0;
    allocation_count = 0;

    printf("---- allocate_page tests ----\n");

    // Test A: first call
    allocate_page(0x3000);

    printf("allocations after first call: %zu\n", allocation_count);
    assert(ptbr != 0);

#if LEVELS == 1
    assert(allocation_count == 2);
#endif


    // Test B: same page again
    allocate_page(0x3000);

    printf("allocations after duplicate call: %zu\n", allocation_count);

#if LEVELS == 1
    assert(allocation_count == 2);
#endif


    // Test C: new virtual page
    allocate_page(0x4000);

    printf("allocations after new page: %zu\n", allocation_count);

#if LEVELS == 1
    assert(allocation_count == 3);
#endif


    // Check translation works
    int *p = (int *)translate(0x3000);
    *p = 1234;

    int *p_check = (int *)translate(0x3000);
    printf("stored value: %d\n", *p_check);
    assert(*p_check == 1234);


    printf("---- translate tests ----\n");

    // --- Test 1: empty page table ---
    ptbr = 0;

    alignas(4096) static size_t testing_page_table[512];
    ptbr = (size_t)&testing_page_table[0];

    size_t r1 = translate(0x1234);
    printf("Test 1 (empty PT): translate(0x1234) = 0x%zx (expected ~0)\n", r1);


    // --- Test 2: manual mapping ---
    alignas(4096) static char data_for_page_3[4096];

    size_t phys_addr = (size_t)&data_for_page_3[0];
    size_t phys_ppn  = phys_addr >> 12;
    size_t pte       = (phys_ppn << 12) | 1;

    testing_page_table[3] = pte;

    size_t r2 = translate(0x3000);
    printf("Test 2: translate(0x3000) = 0x%zx (expected %p)\n",
           r2, &data_for_page_3[0]);


    // --- Test 3: offset ---
    size_t r3 = translate(0x3045);
    printf("Test 3: translate(0x3045) = 0x%zx (expected %p)\n",
           r3, &data_for_page_3[0x45]);

    return 0;
}