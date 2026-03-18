# Yeeva's implementation of virtual memory
* This library gives two methods: 
    * `translate()`, which translates a given virtual address, return `-1` if the address is invalid. 
    * `allocate_page()`, allocates a new page at the given virtual address (assuming it's an address pointing to the beginning of a page). It returns `1` on success, `-1` on fail, and `0` if there is already a page there.
* You can customize the multi-level page table using the two following settings in `config.h`:
    * `LEVELS`: chooses how many page tables the OS traverses through before reaching a physical page of data. If you know a lot of virtual memory will go unused, choose a high `LEVELS` value.
    * `POBITS`: chooses the number of bits used for the page offset. The more bits, the bigger each page is.