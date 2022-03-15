#include "uart.h"


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

void uart_puts(const char *s) {
    uart_flush_tx(); // probably overkill
    while (*s) {
        uart_putc(*s++);
    }
    uart_flush_tx(); // make sure everything is sent 
}

void debugger_print(const char *str) {
    uart_puts("CS140EDB:");
    uart_puts(str);
}

void debugger_println(const char *str) {
    debugger_print(str);
    uart_putc('\n');
    uart_flush_tx();
}

void uart_printf(char fmt, uint32_t val) {
    int base = 10;
    switch (fmt) {
    case 's':
        uart_puts((char *) val);
        break;
    case 'c':
        uart_putc((char) val);
        break;
    case 'x':
        uart_puts("0x");
        uart_print_int(val, 16);
        break;
    case 'b':
        uart_puts("0b");
        uart_print_int(val, 2);
        break;
    case 'd':
    case 'i':
        if ((int) val < 0) {
            uart_putc('-');
            val *= -1;
        }
    case 'u':
        uart_print_int(val, 10);
        break;
    default:
        uart_puts("<Invalid format spec>");
        break;
    }
}
