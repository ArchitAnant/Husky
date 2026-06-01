#include "page.h"
#include <uart/uart.h>
#include <stddef.h>

// Drop QEMU linker references:
// extern uint64_t _bss_end;
// extern uint64_t _stack_end;
// extern uint64_t _heap_start;
// extern uint64_t _heap_size;

// Pre-allocate 128KB of the Pico's SRAM for our page allocator pool
#define POOL_SIZE (128 * 1024)
static uint8_t physical_memory_pool[POOL_SIZE] __attribute__((aligned(PAGE_SIZE)));

static PageDescriptor *ptr_to_descriptaors; // pointer to the start of the page descriptors array
static uint32_t total_pages; // total number of pages available in the system
static uint32_t actual_heap_start; // actual start of the heap after the page descriptors

uint32_t page_get_heap_start(void) {
    return actual_heap_start;
}

PageDescriptor* page_get_descriptors(void) {
    return ptr_to_descriptaors;
}

void page_init(void){
    uint32_t h_start = (uint32_t)physical_memory_pool;
    uint32_t h_size  = (uint32_t)POOL_SIZE;
    
    h_start = (h_start + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    total_pages = (uint32_t)(h_size / PAGE_SIZE); 
    
    ptr_to_descriptaors = (PageDescriptor*)h_start; 
    
    uint32_t descriptors_size = total_pages * sizeof(PageDescriptor); 
    uint32_t descriptors_pages = (descriptors_size + PAGE_SIZE - 1) / PAGE_SIZE; 
    
    actual_heap_start = (h_start + (descriptors_pages * PAGE_SIZE) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    
    uint32_t overhead = actual_heap_start - h_start; 
    total_pages -= (uint32_t)(overhead / PAGE_SIZE); 
    
    for(uint32_t i = 0; i < total_pages; i++){
        ptr_to_descriptaors[i].flags = 0; 
        ptr_to_descriptaors[i].block_size = 0; 
    }
    os_uart_puts("[+] Page Allocator Initialized\r\n");
}

void* page_alloc(int n){
    if (n <= 0) return NULL;
    uint32_t i = 0;

    while(i <= total_pages - n){
       if (ptr_to_descriptaors[i].flags & PAGE_TAKEN)
       {
            i += ptr_to_descriptaors[i].block_size;
            continue;
       }
       
       int found = 1;
       uint32_t j;

       for(j= i; j < i+n; j++){
            if (ptr_to_descriptaors[j].flags & PAGE_TAKEN)
            {
                found = 0;
                break;
            }
       }

        if(found){
            for(j = i; j < i+n; j++){
                ptr_to_descriptaors[j].flags |= PAGE_TAKEN;
            }
            ptr_to_descriptaors[i].block_size = n; 
            return (void*)(actual_heap_start + (i * PAGE_SIZE)); 
        }
        else {
            i = j; 
        }
    }
    os_uart_puts("Out of memory!\n");
    return NULL; 
}

void page_free(void* p){
    if (p == NULL) return;
    uint32_t addr = (uint32_t)p;
    uint32_t i = (uint32_t)((addr - actual_heap_start) / PAGE_SIZE);

    if (!(ptr_to_descriptaors[i].flags & PAGE_TAKEN)) {
        os_uart_printf("ERROR: Double free or invalid pointer at index %d!\r\n", i);
        return;
    }

    uint32_t block_size = ptr_to_descriptaors[i].block_size; 

    os_uart_printf("Freeing page at index: %d\r\n", i);
    for(uint32_t k = 0; k < block_size; k++){
        ptr_to_descriptaors[i+k].flags &= ~PAGE_TAKEN; 
        ptr_to_descriptaors[i+k].block_size = 0; 
    }
}

int get_page_index_from_address(void* addr) {
    uint32_t address = (uint32_t)addr;
    if (address < actual_heap_start) {
        return -1; 
    }
    return (int)((address - actual_heap_start) / PAGE_SIZE);
}
