#include "mini-rpi.h"
#include "debugger.h"

void put_hex(unsigned x) {
    while (x) {
        uart_putc((x % 10) + '0');
        x /= 10;
    }
}

void init_interrupts() {
    dev_barrier();
    extern uint32_t _interrupt_table;
    uint32_t *int_table = &_interrupt_table;
    asm volatile("MCR p15, 0, %0, c12, c0, 0" : : "r"(int_table) :);
    dev_barrier();
    void *result = 0;
    asm volatile ("MRC p15, 0, %0, c12, c0, 0" : "=r" (result) : :);
}


void undefined_instruction_vector(uint32_t *regs) {
    debugger_print("undefined instruction");
    debugger_shell(regs);
}

void prefetch_abort_vector(uint32_t *regs) {
    debugger_print("breakpoint triggered? or single step");
    debugger_shell(regs);
}

void data_abort_vector(uint32_t *regs) {
    debugger_print("watchpoint triggered");
    debugger_shell(regs);
}

void interrupt_vector(uint32_t *regs) {
    debugger_print("possibly interrupt line?");
    debugger_shell(regs);
}

void unexpected_interrupt(uint32_t *regs) {
    debugger_print("unexpected interrupt. this really shouldn't happen");
}
