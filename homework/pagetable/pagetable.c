#define _XOPEN_SOURCE 700
#include "config.h"
#include <stddef.h>
#include "mlpt.h"
#include <stdint.h>
#include <stdlib.h>

#include <stdio.h>

size_t ptbr;
size_t allocation_count = 0;

// Given a PPN, a virtual address, and a particular level return a pointer to the page table entry at that level.
uint64_t* get_PTE_pointer(size_t physical_page_base_address, size_t virtual_address, int current_level){
    /* STEP 1: begin with a pointer to the beginning of the physical page */
    uint64_t *pte_pointer = (uint64_t *) physical_page_base_address; 



    /* STEP 2: move the pointer forward by a distance derived from the VA */
    int lower_bits_to_discard = POBITS + // page offset bits
                (LEVELS - 1 - current_level) // number of levels not yet traversed 
                * (POBITS - 3); // width of VPN indices (PTEs are always 2^3 bytes) (asssumes current_level starts at 0)
    size_t vpn_index_mask = (1ULL << (POBITS - 3)) - 1; // allows us to mask off unwanted upper bits
    size_t index = (virtual_address >> lower_bits_to_discard) & vpn_index_mask;

    pte_pointer += index;



    return pte_pointer; 
}

// Recursively traverses the multi-level pagetable, return -1 if an invalid page is encountered, and the final PPN otherwise.
size_t traverse_PTEs(size_t physical_page_base_address, size_t virtual_address, int current_level){
    uint64_t pte = *((uint64_t *)get_PTE_pointer(physical_page_base_address, virtual_address, current_level)); //retrieve the actual PTE itself via casting and dereferencing

    if ((pte & 1) == 0){ 
        return -1; // if valid bit is 1, return -1
    } else if (current_level + 1 == LEVELS){
        return ((pte >> POBITS) << POBITS); // if we're at the last level, return the PPN field of the PTE
    } else {
        return traverse_PTEs(((pte >> POBITS) << POBITS), virtual_address, current_level + 1); // otherwise, traverse one level further into the multi-level pagetable
    }
}

// Given a virtual address, returns its corresponding physical address according to the pagetable configuration designated in config.h.
size_t translate(size_t virtual_address){
    if (ptbr == 0){
        return -1;
    } 

    size_t physical_page_base = traverse_PTEs(ptbr, virtual_address, 0);
    if (physical_page_base == -1){
        return -1; // if the VA translates to an invalid physical page, return -1
    }   
    size_t physical_page_offset = (((1ULL << POBITS)-1) & virtual_address); // mask the non-page-offset bits off the VA
    
    size_t physical_address = (physical_page_base | physical_page_offset); 
    return physical_address;
}

// Initializes a page, zeroes it out, and returns a pointer to it.
void* initialize_page(){
    /* STEP 1: allocate da page */
    void* resulting_page_pointer;
    posix_memalign(
        &resulting_page_pointer, 
        1ULL << POBITS, // 1ULL << POBITS gives 2^POBITS
        1ULL << POBITS
    ); 

    /* STEP 2: zero out da page*/
    int num_PTEs = 1ULL << (POBITS - 3); // 1ULL << x gives 2^x; POBITS - 3 is the exponent of the number of PTEs per page, assuming PTEs are 2^3 bytes in size
    for (int i = 0; i < (num_PTEs); i++){
        *((uint64_t*)resulting_page_pointer + i) = 0; // zero out the current PTE
    }
    
    allocation_count++; // for testing purposes

    return resulting_page_pointer;
}

// Recursively traverses the multi-level page table, allocating new pages as needed.
size_t traverse_and_allocate(size_t physical_page_base_address, size_t virtual_address, int current_level){
    uint64_t *ptePtr = get_PTE_pointer(physical_page_base_address, virtual_address, current_level);

    if ((*ptePtr & 1) == 0){  // if the valid bit is 0...
        *ptePtr = ((size_t) initialize_page()) | 1; // allocate a new page, and update the PTE to point to it
        
        if (current_level + 1 == LEVELS){
            return 1; // if that was the last level of the multi-level pagetable, then return
        }
    } else if (current_level + 1 == LEVELS){
        return 0; // if we're at the last level of the of the multi-level pagetable, we still haven't encountered an invalid bit, meaning the VPN was already valid. Return 0.
    } 
    return traverse_and_allocate(((*ptePtr >> POBITS) << POBITS), virtual_address, current_level + 1); // traverse one level further into the multi-level pagetable
}

// Given a virtual address, allocates a page in physical memory for it.
int allocate_page(size_t start_va){
    int page_offset_mask = (1ULL << POBITS) - 1;
    if (start_va & page_offset_mask) { 
        return -1; // if the VA isn't at the start of a physical page, return -1
    }
    if (ptbr == 0){
        ptbr = (size_t) initialize_page(); // if not already allocated, allocate the root page table
    }
    return traverse_and_allocate(ptbr, start_va, 0); // recurse into the multi-level pagetable
}


size_t is_table_valid(size_t physical_address){
    int is_valid = 1;

    int num_PTEs = 1ULL << (POBITS - 3); // 1ULL << x gives 2^x; POBITS - 3 is the exponent of the number of PTEs per page, assuming PTEs are 2^3 bytes in size
    for (int i = 0; i < (num_PTEs); i++){
        is_valid &= (*((uint64_t*)physical_address + i) & 1);
    }

    return is_valid;
}
// Recursively traverses the multi-level page table, deallocating along the way.
size_t traverse_and_deallocate(size_t physical_page_base_address, size_t virtual_address, int current_level){
    uint64_t *ptePtr = get_PTE_pointer(physical_page_base_address, virtual_address, current_level);

    size_t pte = *ptePtr;
    if ((pte & 1) == 0){
        return 1; // if page isn't allocated in the first place, return 1
    }
    *ptePtr = 0; // zero out the page table entry

    size_t result = traverse_and_deallocate(((pte >> POBITS) << POBITS), virtual_address, current_level + 1); // traverse one level further into the multi-level pagetable
    
    if (current_level == LEVELS){ // assuming current_level = 0 at the root page table
        // if at the data page, free it
        free((void *)physical_page_base_address);
        return 0; 
    } else if (!is_table_valid((pte >> POBITS) << POBITS)){
        // if the we just deallocated the last entry in the page table, deallocate the page table itself as well
        free((void *)((pte >> POBITS) << POBITS));
    }

    return result;
}

// Given a virtual address, deallocates that page.
int deallocate_page(size_t start_va){
    int page_offset_mask = (1ULL << POBITS) - 1;
    if (start_va & page_offset_mask) { 
        return -1; // if the VA isn't at the start of a physical page, return -1
    }
    
    return traverse_and_deallocate(ptbr, start_va, 0); // recurse into the multi-level pagetable
}