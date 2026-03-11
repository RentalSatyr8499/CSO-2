#include <stdio.h>
#include <assert.h>
#include "mlpt.h"
#include "config.h"
#include <stdalign.h>


int main(void) {
       ptbr = 0;
       // --- Test 1: empty page table should return ~0 for any VA ---
       alignas(4096) static size_t testing_page_table[512];
       ptbr = (size_t)&testing_page_table[0];

       size_t r1 = translate(0x1234);
       printf("Test 1 (empty PT): translate(0x1234) = 0x%zx (expected ~0)\n", r1);


       // --- Test 2: map virtual page 3 to a physical page ---
       alignas(4096) static char data_for_page_3[4096];

       size_t phys_addr = (size_t)&data_for_page_3[0];
       size_t phys_ppn  = phys_addr >> 12;          // extract physical page number
       size_t pte       = (phys_ppn << 12) | 1;     // encode PTE with valid bit

       testing_page_table[3] = pte;                 // install mapping

       size_t r2 = translate(0x3000);               // offset = 0
       printf("Test 2: translate(0x3000) = 0x%zx (expected %p)\n",
              r2, &data_for_page_3[0]);


       // --- Test 3: same mapping, nonzero offset ---
       size_t r3 = translate(0x3045);               // offset = 0x45
       printf("Test 3: translate(0x3045) = 0x%zx (expected %p)\n",
              r3, &data_for_page_3[0x45]);


       return 0;
}

    /*
    // 0 pages have been allocated
    assert(ptbr == 0);

    allocate_page(0x456789abc000);
    // 5 pages have been allocated: 4 page tables and 1 data
    assert(ptbr != 0);

    allocate_page(0x456789abc000);
    // no new pages allocated (still 5)

    int *p1 = (int *)translate(0x456789abcd00);
    *p1 = 0xaabbccdd;
    short *p2 = (short *)translate(0x456789abcd02);
    printf("%04hx\n", *p2); // prints "aabb\n"

    assert(translate(0x456789ab0000) == 0xFFFFFFFFFFFFFFFF);

    allocate_page(0x456789ab0000);
    // 1 new page allocated (now 6; 4 page table, 2 data)

    assert(translate(0x456789ab0000) != 0xFFFFFFFFFFFFFFFF);

    allocate_page(0x456780000000);
    // 2 new pages allocated (now 8; 5 page table, 3 data) */
