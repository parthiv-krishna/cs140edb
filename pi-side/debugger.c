#include "debugger.h"

// returns 1 if we should return to the program
int process_input(char *line, uint32_t *regs) {
    char *cmd = parse_token(&line);
    char *expr;
    switch (cmd[0]) {
        case 'c':
            breakpt_singlestep_stop();
            return 1;
        case 'p':;
            char format = line[1];
            expr = parse_token(&line);
            uint32_t val = parse_expr(expr, 0, regs);
            debugger_print(expr);
            uart_puts(" = ");
            uart_printf(cmd[1], val);
            uart_putc('\n');
            break;
        case 'k':
            expr = parse_token(&line);
            uint32_t *dst = (uint32_t *)parse_expr(expr, 1, regs);
            expr = parse_token(&line);
            uint32_t src = parse_expr(expr, 0, regs);
            *dst = src;
            debugger_print("Wrote ");
            uart_printf('x', src);
            uart_puts(" to *");
            uart_printf('x', (uint32_t)dst);
            uart_putc('\n');
            break;
        case 'b':
            expr = parse_token(&line);
            uint32_t *break_addr = (uint32_t *) parse_expr(expr, 0, regs);
            if (breakpt_set(break_addr)) {
                debugger_print("Successfully set breakpoint #");
                int id = breakpt_get_id(break_addr);
                uart_printf('d', id);
                uart_puts(" to ");
            } else {
                debugger_print("Unable to set breakpoint on ");
            }
            uart_printf('x', (uint32_t) break_addr);
            uart_putc('\n');
            breakpt_print_active();
            break;
        case 'w':
            expr = parse_token(&line);
            uint32_t *watch_addr = (uint32_t *)parse_expr(expr, 0, regs);
            if (watchpt_set(watch_addr)) {
                debugger_print("Successfully set watchpoint #");
                int id = watchpt_get_id(watch_addr);
                uart_printf('d', id);
                uart_puts(" to ");
            } else {
                debugger_print("Unable to set watchpoint on ");
            }
            uart_printf('x', (uint32_t) watch_addr);
            uart_putc('\n');
            watchpt_print_active();
            break;
        case 's':
            breakpt_singlestep_start(regs[15]);
            return 1;
        case 'q':
            debugger_println("DONE!!!");
            rpi_reboot();
        default:
            debugger_println("Invalid command. Use 'h' for a list of commands");
    }
    return 0;
}

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

void debugger_main(uint32_t *target_dst, uint32_t *target_src) {
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
    breakpt_set(target_dst);


    prefetch_flush();

    // user program is wherever the bootloader would have put it
    // if we did not inject the debugger code
    move_user_program(target_dst, target_src);

    debugger_println("About to enter user code");

    // flush regs, enter user mode
    enter_user_mode(target_dst); // setup user mode and branch to target
}