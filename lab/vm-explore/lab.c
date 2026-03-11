#define _GNU_SOURCE
#include "util.h"
#include <stdio.h>      // for printf
#include <stdlib.h>     // for atoi (and malloc() which you'll likely use)
#include <sys/mman.h>   // for mmap() which you'll likely use
#include <stdalign.h>

alignas(4096) volatile char global_array[4096 * 32];

void labStuff(int which) {
    if (which == 0) {
        /* do nothing */
    } else if (which == 1) {
        for (int i = 0; i < 10*4096; i += 4096){ // default page size is 4096?
            global_array[i] = '&';
        }
    } else if (which == 2) {
        int *myArray = malloc(1000000);
        for (int i = 0; i < 1000000/sizeof(int); i += 4096/sizeof(int)){ // touch every page in the mb
            myArray[i] = 1;
        }
    } else if (which == 3) {
        char *ptr;
        ptr = mmap(
            NULL, /* "hint address" */
            1048576, /* length */
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0
        );
        for (int i = 0; i < 131072; i += 4096){ // touch until you get 131072 rss
            ptr[i] = 0xC;
        }
    } else if (which == 4) {
        char *ptr;
        ptr = mmap(
            (void *) 0x5555557bc000, // 5555555bbfff rounds to 5555555bc000
            4096, /* length */
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0
        );
        ptr[0] = 0xC; // 4096 is only one page
    } else if (which == 5) {
        char *ptr;
        ptr = mmap(
            (void *) 0x5655555bc000, 
            4096, 
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0
        );
        ptr[0] = 0xC; 
    }
}

int main(int argc, char **argv) {
    int which = 0;
    if (argc > 1) {
        which = atoi(argv[1]);
    } else {
        fprintf(stderr, "Usage: %s NUMBER\n", argv[0]);
        return 1;
    }
    printf("Memory layout:\n");
    print_maps(stdout);
    printf("\n");
    printf("Initial state:\n");
    force_load();
    struct memory_record r1, r2;
    record_memory_record(&r1);
    print_memory_record(stdout, NULL, &r1);
    printf("---\n");

    printf("Running labStuff(%d)...\n", which);

    labStuff(which);

    printf("---\n");
    printf("Afterwards:\n");
    record_memory_record(&r2);
    print_memory_record(stdout, &r1, &r2);
    print_maps(stdout);
    return 0;
}
