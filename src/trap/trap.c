#include "trap.h"
#include <uart/uart.h>
#include "hardware/irq.h"

extern void irq_handler_chain(void); 

uint32_t m_trap(uint32_t epc, uint32_t tval, uint32_t cause, uint32_t hart, uint32_t status, TrapFrame *frame){
    (void) status;
    (void) frame;
    int is_async = (cause >> 31) & 1;
    uint32_t cause_code = cause & 0x7FFFFFFF;
    uint32_t return_epc = epc;
    
    if (is_async) {
        // On Pico 2 (Hazard3), hardware IRQs map to cause_code 16 and up.
        if (cause_code >= 16) {
            uint irq_num = cause_code - 16;
            
            // Ask the Pico SDK for the correct handler (e.g. TinyUSB's handler)
            irq_handler_t sdk_handler = irq_get_vtable_handler(irq_num);
            
            if (sdk_handler) {
                sdk_handler(); // Let the SDK clear the hardware flag and queue data!
            } else {
                os_uart_printf("[!] Unhandled Hardware IRQ: %d\r\n", irq_num);
            }
        } 
        else if (cause_code == 11) {
            // Fallback: Standard RISC-V Machine External Interrupt
            // Read the Hazard3 meicontext register to get the IRQ
            uint32_t meicontext;
            asm volatile("csrr %0, 0xbe5" : "=r"(meicontext)); 
            uint irq_num = meicontext & 0xFFF;
            
            irq_handler_t sdk_handler = irq_get_vtable_handler(irq_num);
            if (sdk_handler) sdk_handler();
        }
        else {
            os_uart_printf("[+] Async trap %d from hart %d\r\n", cause_code, hart);
        }
    } else {
        switch(cause_code){
            case 2:
                os_uart_printf("Illegal instruction CPU#%d -> 0x%x: 0x%x\r\n", hart, epc, tval);
                os_panic("Illegal Instruction Trap", __FILE__, __LINE__);
                break;
            case 8:
                os_uart_printf("E-call from User mode! CPU#%d -> 0x%x\r\n", hart, epc);
                return_epc += 4;
                break;
            case 9:
                os_uart_printf("E-call from Supervisor mode! CPU#%d -> 0x%x\r\n", hart, epc);
                return_epc += 4;
                break;
            case 11:
                os_uart_printf("E-call from Machine mode! CPU#%d -> 0x%x\r\n", hart, epc);
                os_panic("Machine E-Call", __FILE__, __LINE__);
                break;
            case 12:
                os_uart_printf("Instruction page fault CPU#%d -> 0x%x: 0x%x\r\n", hart, epc, tval);
                return_epc += 4;
                break;
            case 13:
                os_uart_printf("Load page fault CPU#%d -> 0x%x: 0x%x\r\n", hart, epc, tval);
                return_epc += 4;
                break;
            case 15:
                os_uart_printf("Store/AMO page fault CPU#%d -> 0x%x: 0x%x\r\n", hart, epc, tval);
                return_epc += 4;
                break;
            default:
                os_uart_printf("Unhandled sync trap: Cause %d CPU#%d -> 0x%x: 0x%x\r\n", cause_code, hart, epc, tval);
                os_panic("Unhandled sync trap", __FILE__, __LINE__);
                break;
        }
    }
    return return_epc;
}
