#include <stdint.h>
#include <stdarg.h>
#include <pico/stdlib.h>
#include "uart.h"
#include <stdio.h> 

#define print(fmt, ...)   do { } while (0)
#define println(fmt, ...) do { } while (0)

Uart uart_new(uint32_t base_address) {
    Uart u;
    u.base_address = base_address;
    return u;
}

int os_uart_getc() {
    int c = getchar_timeout_us(0);
    return (c == PICO_ERROR_TIMEOUT) ? -1 : c;
}

void uart_put(Uart *self, uint8_t c) {
    (void)self;
    putchar((char)c);
}

void os_uart_putc(char c) {
    putchar(c);
}

void os_uart_puts(const char *s) {
    while (*s) putchar(*s++);
}

void os_uart_init() {
    stdio_init_all();
}

void uart_write_str(Uart *self, const char *s) {
    (void)self;
    while (*s) {
        putchar(*s++);
    }
}

void __attribute__((noreturn)) abort() {
    while (1) {
        __asm__ volatile("wfi");
    }
}

void __attribute__((noreturn)) os_panic(const char *message, const char *file, uint32_t line){
    os_uart_puts("\r\n--- KERNEL PANIC ---\r\n");
    os_uart_printf("Message: %s\r\n", message);
    os_uart_printf("At: %s:%d\r\n", file, line);
    os_uart_puts("----------------------\r\n");
    os_uart_puts("System halted.\r\n");
    abort();
}

void mmio_write(uint32_t address, uint32_t offset, uint8_t value) {
    volatile uint8_t *reg = (volatile uint8_t *)(address + offset);
    *reg = value;
}

uint8_t mmio_read(uint32_t address, uint32_t offset) {
    volatile uint8_t *reg = (volatile uint8_t *)(address + offset);
    return *reg;
}

void uart_puthex(uint32_t val) {
    char *hex = "0123456789abcdef";
    for (int i = 28; i >= 0; i -= 4) {
        os_uart_putc(hex[(val >> i) & 0xf]);
    }
}

void os_uart_printf(const char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    for(const char *p = fmt; *p != '\0'; p++){
        if(*p!='%'){
            os_uart_putc(*p);
            continue;
        }
        p++;
        switch(*p){
            case 's': {
                const char *str = va_arg(args, const char *);
                os_uart_puts(str);
                break;
            }
            case 'd': {
                long long num = va_arg(args, long long);
                if(num < 0){
                    os_uart_putc('-');
                    num = -num;
                }
                char buf[21];
                int i = 0;
                do {
                    buf[i++] = (num % 10) + '0';
                    num /= 10;
                } while(num > 0);
                for(int j = i-1; j >= 0; j--){
                    os_uart_putc(buf[j]);
                }
                break;
            }
            case 'x': {
                uint32_t num = va_arg(args, uint32_t);
                uart_puthex(num);
                break;
            }
            case 'c': {
                char c = (char)va_arg(args, int);
                os_uart_putc(c);
                break;
            }
            case 'p': {
                uint32_t num = va_arg(args, uint32_t);
                os_uart_puts("0x");
                uart_puthex(num);
                break;
            }
            default:
                os_uart_putc('%');
                break;
        }
    }
    va_end(args);
}
