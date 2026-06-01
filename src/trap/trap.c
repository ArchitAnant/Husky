#include "trap.h"
#include <uart/uart.h>

uint32_t m_trap(uint32_t epc, uint32_t tval, uint32_t cause, uint32_t hart, uint32_t status, TrapFrame *frame){
    (void) status;
    (void) frame;
    int is_async = (cause >> 31) & 1;
    uint32_t cause_code = cause & 0x7FFFFFFF;
    uint32_t return_epc = epc;
    
    if(is_async){
        switch(cause_code){
            case 3:
                os_uart_printf("[+] Software Interrupt from hart %d\r\n", hart);
                break;
            case 7: {
                // We're stepping away from QEMU CLINT hardcoding for Pico 2
                // Timer handling depends on RP2350 specific timers
                os_uart_printf("[+] Timer Interrupt from hart %d\r\n", hart);
                break;
            }
            case 8:
                os_uart_printf("[+] External Interrupt from hart %d\r\n", hart);
                break;
            default:
               os_panic("Unhandled async trap", __FILE__, __LINE__);
               break;
        }
    }else{
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
