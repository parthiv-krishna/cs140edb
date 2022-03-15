#define INT_STACK 0x70000000 // todo: change this

#define MK_TRAMPOLINE(offset, function) \
    sub lr, lr, offset; \
    mov sp, #INT_STACK; \
    sub sp, sp, #(17 * 4); \
    stmia sp, {r0-r14}^; \
    str lr, [sp, #(15 * 4)]; @ store lr at array[15] \
    mov r0, sp; \
    mrs r1, spsr; \
    str r1, [sp, #(16 * 4)]; @ store spsr at array[16] \
    mov r2, lr; \
    bl function; \
    ldmia sp, {r0-r15}^

.align 5
.globl _interrupt_table
_interrupt_table:
    b reset_asm
    b undefined_instruction_asm
    b software_interrupt_asm
    b prefetch_abort_asm
    b data_abort_asm
    b interrupt_asm
    b interrupt_asm

undefined_instruction_asm:
    MK_TRAMPOLINE(#4, undefined_instruction_vector)

prefetch_abort_asm:
    MK_TRAMPOLINE(#4, prefetch_abort_vector)

data_abort_asm:
    MK_TRAMPOLINE(#8, data_abort_vector)

interrupt_asm: 
    MK_TRAMPOLINE(#4, interrupt_vector)

software_interrupt_asm:
reset_asm:
    MK_TRAMPOLINE(#4, unexpected_interrupt)