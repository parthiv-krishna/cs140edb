#ifndef __UART_H__
#define __UART_H__

#include "mini-rpi.h"

// 2.1, p8
// XXX: go through and do the bitfields for these.
struct aux_periphs {
    volatile unsigned
        /* <aux_mu_> regs */
        io,     // p11
        ier,

#       define CLEAR_TX_FIFO    (1 << 1)
#       define CLEAR_RX_FIFO    (1 << 2)
#       define CLEAR_FIFOs  (CLEAR_TX_FIFO|CLEAR_RX_FIFO)
        // dwelch does not write the low bit?
#       define IIR_RESET        ((0b11 << 6) | 1)
        iir,

        lcr,
        mcr,
        lsr,
        msr,
        scratch,

#       define RX_ENABLE (1 << 0)
#       define TX_ENABLE (1 << 1)
        cntl,

        stat,
        baud;
};

// never use this directly!
static inline struct aux_periphs *uart_get(void) { return (void*)0x20215040; }

// initialize
void uart_init(void);
// disable
void uart_disable(void);

// get one byte from the uart
int uart_getc(void);
// put one byte on the uart
void uart_putc(unsigned c);

// returns -1 if no byte, the value otherwise.
int uart_getc_async(void);

// 0 = no data, 1 = at least one byte
int uart_has_data(void);
// 0 = no space, 1 = space for at least 1 byte
int uart_can_putc(void);

// flush out the tx fifo
void uart_flush_tx(void);

////////////////////
// uart-helpers.c //
////////////////////

int uart_gets(char *in, unsigned nbytes);

void uart_puts(const char *s);

void debugger_print(const char *str);
void debugger_println(const char *str);

static const char *HEX = "0123456789abcdef";
// inline so we can use mod
static inline void uart_print_int(uint32_t val, int base) {
    char buf[32];
    int msd = 0;
    for (int i = 0; i < sizeof(buf); i++) {
        int next = val % base;
        if (next != 0) {
            msd = i;
        }
        buf[i] = HEX[next];
        val /= base;
    }
    for (int i = msd; i >= 0; i--) {
        uart_putc(buf[i]);
    }
}

void uart_printf(char fmt, uint32_t val);

char *parse_token(char **s);

int find_in_arr(char *c, const char **arr);
int find_in_s(char c, const char *s);

int parse_int(char *expr);

uint32_t parse_expr(char *expr, int get_addr, uint32_t *regs);

int process_input(char *line, uint32_t *regs);

#endif
