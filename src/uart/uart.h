#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stddef.h>

// This is the "Blueprint" for our UART object
typedef struct {
    uint32_t base_address;
} Uart;

// List the "Menu" options
Uart uart_new(uint32_t base_address);
int os_uart_getc();
void uart_put(Uart *self, uint8_t c);
void os_uart_putc(char c);
void os_uart_puts(const char *s);
void os_uart_init();
void uart_write_str(Uart *self, const char *s);
void mmio_write(uint32_t address, uint32_t offset, uint8_t value);
uint8_t mmio_read(uint32_t address, uint32_t offset);
void uart_puthex(uint32_t val);
void os_uart_printf(const char *fmt, ...);
void __attribute__((noreturn)) os_panic(const char *message, const char *file, uint32_t line);
#endif