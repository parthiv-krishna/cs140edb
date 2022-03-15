#include "mini-rpi.h"
#include "debugger.h"

void init_interrupts() {
    dev_barrier();
    extern uint32_t* _interrupt_table;
    asm volatile("MCR p15, 0, %0, c12, c0, 0" : : "r"(_interrupt_table) :);
    dev_barrier();
}


void undefined_instruction_vector(uint32_t *regs) {
    debugger_print("undefined instruction");
    debugger_shell(regs);
}

void prefetch_abort_vector(uint32_t *regs) {
    debugger_println("breakpoint triggered? or single step");
    debugger_shell(regs);
}

void data_abort_vector(uint32_t *regs) {
    debugger_println("watchpoint triggered");
    debugger_shell(regs);
}

void interrupt_vector(uint32_t *regs) {
    debugger_println("possibly interrupt line?");
    debugger_shell(regs);
}

void unexpected_interrupt(uint32_t *regs) {
    debugger_println("unexpected interrupt. this really shouldn't happen");
}
