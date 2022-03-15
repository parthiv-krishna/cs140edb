#include "interrupts.h"

void init_interrupts() {
    dev_barrier();
    extern uint32_t _interrupt_table;
    uint32_t *int_table = &_interrupt_table;
    asm volatile("MCR p15, 0, %0, c12, c0, 0" : : "r"(int_table) :);
    dev_barrier();
    void *result = 0;
    asm volatile ("MRC p15, 0, %0, c12, c0, 0" : "=r" (result) : :);
    assert(result == int_table);
}


void undefined_instruction_vector(uint32_t *regs) {
    debugger_print("undefined instruction");
    debugger_shell(regs);
}

void prefetch_abort_vector(uint32_t *regs) {
    uint32_t pc = regs[15];
    int id = breakpt_get_id(pc);
    debugger_print("Breakpoint #");
    uart_printf('d', id);
    uart_puts(" triggered at pc=");
    uart_printf('x', pc);
    uart_putc('\n');

    debugger_println("Active breakpoints:");
    breakpt_print_active();

    debugger_shell(regs);
    breakpt_disable(pc);
}

void data_abort_vector(uint32_t *regs) {
    uint32_t *addr = watchpt_fault_addr();
    uint32_t *pc = watchpt_fault_pc();
    uint32_t id = watchpt_get_id(addr);
    debugger_print("Watchpoint #");
    uart_printf('d', id);
    uart_puts(" at ");
    uart_printf('x', (uint32_t) addr);
    uart_puts(" triggered by pc=");
    uart_printf('x', (uint32_t) pc);
    uart_putc('\n');
    debugger_shell(regs);
    watchpt_disable(addr);
}

void interrupt_vector(uint32_t *regs) {
    debugger_println("possibly interrupt line?");
    debugger_shell(regs);
}

void unexpected_interrupt(uint32_t *regs) {
    debugger_println("unexpected interrupt. this really shouldn't happen");
}
