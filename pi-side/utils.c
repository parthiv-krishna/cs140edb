#include "mini-rpi.h"
#include "uart.h"


////////////
// Memory //
////////////
void put32(volatile void *addr, uint32_t v) {
    asm volatile ("str %0, [%1]" : : "r" (v), "r" (addr) :);
}
void PUT32(uint32_t addr, uint32_t v) {
    put32((volatile void *)addr, v);
}

uint32_t get32(const volatile void *addr) {
    uint32_t ret;
    asm volatile ("ldr %0, [%1]" : "=r" (ret) : "r" (addr) :);
    return ret;
}
uint32_t GET32(uint32_t addr) {
    return get32((const volatile void *)addr);
}

///////////////
// ASM stuff //
///////////////
void prefetch_flush(void) {
    unsigned r = 0;
    asm volatile ("mcr p15, 0, %0, c7, c5, 4" :: "r" (r));
}

void dev_barrier(void) {
    asm volatile ("mcr p15, 0, r0, c7, c10, 4" : : :);
}

//////////
// UART //
//////////
int uart_gets(char *in, unsigned nbytes) {
    int i = 0; 
    for (; i < nbytes - 1; ++i) {
        char byte = uart_getc();
        if (byte == '\n') {
            break;
        }
        in[i] = byte;
    }
    in[i] = 0;
    return i;
}

void uart_puts(char *s) {
    uart_flush_tx(); // probably overkill
    while (*s) {
        uart_putc(*s++);
    }
    uart_flush_tx(); // make sure everything is sent 
}

void debugger_print(char *str) {
    uart_puts("CS140EDB:");
    uart_puts(str);
    uart_puts("\n");
    uart_flush_tx();
}

void panic(const char *msg) {
    uart_puts("PANIC: `");
    uart_puts(msg);
    uart_puts("` was not true!\n");
    uart_flush_tx();
    while(1);
    // does not return
}