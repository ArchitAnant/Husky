#include <stdint.h>
#include <uart/uart.h>
#include <init/shell/shell.h>
#include <mm/pages/page.h>
#include <mm/kmalloc/kmalloc.h>
#include <trap/trap.h>
#include "pico/stdlib.h"

void blink_test() {
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1) {
        gpio_put(LED_PIN, 1);
        sleep_ms(500);

        gpio_put(LED_PIN, 0);
        sleep_ms(500);
    }
}


// Test function for trap
void trigger_page_fault() {
        volatile int *ptr = (int *)0x0;  // invalid address
        *ptr = 10;  // will crash
}

// Test for malloc and free
void test_malloc_complex() {
    os_uart_puts("\r\n--- Starting Complex Malloc Test ---\r\n");

    // 1. Test: Multiple Small Allocations
    os_uart_puts("[1] Allocating 3 small blocks (A, B, C)...\r\n");
    char *a = (char *)kmalloc(100);
    char *b = (char *)kmalloc(100);
    char *c = (char *)kmalloc(100);

    if (a && b && c) {
        os_uart_printf("  A: 0x%x, B: 0x%x, C: 0x%x\r\n", (uint32_t)a, (uint32_t)b, (uint32_t)c);
    }

    // 2. Test: Slicing & Hole Reuse
    os_uart_puts("[2] Freeing B and allocating D (size 50) to see if it slices B's hole...\r\n");
    kfree(b);
    char *d = (char *)kmalloc(50);
    os_uart_printf("  D (50 bytes) allocated at: 0x%x (Should match B's old address)\r\n", (uint32_t)d);

    // 3. Test: The "Healing" (Coalescing) Test
    // If we free A and then free D (which is in B's old spot), 
    // and then free C... they should all merge back into one giant block.
    os_uart_puts("[3] Freeing A, D, and C to trigger merging...\r\n");
    kfree(a);
    kfree(d);
    kfree(c);

    os_uart_puts("[4] Allocating a large block (300 bytes) that requires the merged space...\r\n");
    char *large = (char *)kmalloc(300);
    if (large) {
        os_uart_printf("  Large block allocated at: 0x%x (Success! Merging works)\r\n", (uint32_t)large);
        kfree(large);
    } else {
        os_uart_puts("  FAILED: Merging did not create a 300-byte block.\r\n");
    }

    // 4. Test: Cross-Page Allocation
    os_uart_puts("[5] Requesting 5000 bytes (Crosses 4KB page boundary)...\r\n");
    void *big_ptr = kmalloc(5000);
    if (big_ptr) {
        os_uart_printf("  Big block (5000 bytes) at: 0x%x\r\n", (uint32_t)big_ptr);
        kfree(big_ptr);
    } else {
        os_uart_puts("  FAILED: Could not allocate across page boundary.\r\n");
    }

    // 5. Test: Stress Test - Exhausting the Leftover Array
    os_uart_puts("[6] Stress test: 10 small allocations...\r\n");
    void *ptrs[10];
    for(int i=0; i<10; i++) {
        ptrs[i] = kmalloc(16);
    }
    os_uart_puts("  Freeing stress test blocks...\r\n");
    for(int i=0; i<10; i++) {
        kfree(ptrs[i]);
    }

    os_uart_puts("--- Complex Malloc Test Finished ---\r\n\r\n");
}

// int main(void) {
//     // void trigger_illegal_instruction() {
//     //     asm volatile(".word 0xFFFFFFFF");  // invalid opcode
//     // }
//     // void trigger_ecall() {
//     //     asm volatile("ecall");
//     // }

//     // os_uart_puts("[+] Starting OS\r\n[+] Init UART\r\n");
//    os_uart_init();
    
//     // This loops infinitely UNTIL you run the `screen` command on your Mac
//     while (os_uart_getc() == -1) {
//         sleep_ms(10);
//     }
    
//     // Now it's safe to print; the Mac is definitely listening
//     os_uart_puts("[+] Starting OS\r\n[+] Init UART\r\n");
//     os_uart_puts("[+] Init Shell\r\n");
//     shell_init();
    
//     page_init();
//     os_uart_puts("[+] Init Page Allocator\r\n");
//     // void* page1 = page_alloc(1);
//     // os_uart_printf("Allocated page at: 0x%x\r\n", (uint64_t)page1);
//     // void *p2 = page_alloc(1);
//     // os_uart_puts("\r\n");
//     // os_uart_printf("Allocated page at: 0x%x\r\n", (uint32_t)p2);
//     // page_free(page1);
//     // os_uart_puts("\r\n");
//     // os_uart_printf("Freed page at: 0x%x\r\n", (uint32_t)page1);
//     void *page1 = page_alloc(4);
//     os_uart_printf("Allocated 4 contiguous pages starting at: 0x%x\r\n", (uint32_t)page1);
//     void *page2 = page_alloc(4);
//     os_uart_printf("Allocated 4 contiguous pages starting at: 0x%x\r\n", (uint32_t)page2);
//     void *page3 = page_alloc(4);
//     os_uart_printf("Allocated 4 contiguous pages starting at: 0x%x\r\n", (uint32_t)page3);
//     page_free(page3);
//     page_free(page2);
//     page_free(page1);
    
//     test_malloc_complex();
    
    
//     os_uart_puts("\r\n");
    
//     // blink_test(); // Comment this out if you want to see the UART output
//     while (1) {
//         shell_update();
//         //  trigger_illegal_instruction();
//         // trigger_ecall();
//         // trigger_page_fault();

//     }
// }
int main(void) {
    os_uart_init(); // Initializes stdio
    
    while (1) {
        os_uart_puts("HELLO FROM NATIVE USB!\r\n");
        sleep_ms(1000);
    }
}