#include "mini-rpi.h"
#include "debugger.h"

void debugger_shell(uint32_t *regs) {
    while (1) {
        char line[512];
        uart_putc('>');
        uart_gets(line, sizeof(line));
        if (process_input(line, regs)) {
            break;
        }
    }
    uart_flush_tx();
}


void move_user_program(uint32_t *dst, uint32_t *src) {
    // src is pointer to int containing length of user program
    unsigned user_len = *src / sizeof(uint32_t);
    // next word is start of user code
    volatile uint32_t *user_code = src + 1;
    for (unsigned i = 0; i < user_len; i++) {
        dst[i] = user_code[i];
    }
}
    
uint32_t regs[17] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

void notmain(uint32_t *target_dst, uint32_t *target_src) {
    uint32_t sp;
    asm volatile("mov %0, pc" : "=r"(sp) : :);
    debugger_print("sp=");
    uart_printf('x', sp);
    uart_puts("; td=");
    uart_printf('x', (uint32_t)target_dst);
    uart_puts("; ts=");
    uart_printf('x', (uint32_t)target_src);
    uart_putc('\n');
    uart_flush_tx();

    uart_init();

    debugger_println("Hello from debugger");

    // user program is wherever the bootloader would have put it
    // if we did not inject the debugger code
    init_interrupts();

    breakpt_watchpt_init();

    // setup breakpoint at target_dst
    breakpt_set0(target_dst);

    prefetch_flush();

    // user program is wherever the bootloader would have put it
    // if we did not inject the debugger code
    move_user_program(target_dst, target_src);

    debugger_println("About to enter user code");

    // flush regs, enter user mode
    enter_user_mode(target_dst); // setup user mode and branch to target
}