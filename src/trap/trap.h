#ifndef TRAP_H
#define TRAP_H
#include <stdint.h>

typedef struct {
    uint32_t regs[32]; 
    uint32_t fregs[32]; 
    uint32_t satp; 
    void*    trap_stack; 
} TrapFrame;

// The m_trap function is the main trap handler for the operating system. It is called whenever a trap occurs, and it takes several parameters that provide information about the trap and the state of the CPU at the time of the trap. The function can be used to handle different types of traps (e.g., interrupts, exceptions, etc.) and to perform necessary actions based on the cause of the trap and the state of the CPU.
/*
epc -> the program counter at the time of the trap, which can be used to determine where the trap occurred in the code.
tval -> a value that provides additional information about the trap, such as the address that caused
cause -> the reason for the trap, which can be used to determine the type of trap that occurred (e.g., an interrupt, an exception, etc.)
hart -> the hardware thread (or core) that was executing when the trap occurred, which can  be useful for handling traps in a multi-core system.
status -> the status register at the time of the trap, which can provide information about the state of the CPU and the trap.
frame -> a pointer to a TrapFrame structure that can be used
*/
uint32_t m_trap(uint32_t epc, uint32_t tval, uint32_t cause, uint32_t hart, uint32_t status, TrapFrame *frame);

#endif