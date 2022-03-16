#define INT_STACK 0x9000000 

#define MK_TRAMPOLINE(offset, function) \
    sub lr, lr, offset; \
    mov sp, #INT_STACK; \
    sub sp, sp, #(17 * 4); \
    stmia sp, {r0-r14}^; \
    str lr, [sp, #(15 * 4)]; \
    mov r0, sp; \
    mrs r1, spsr; \
    str r1, [sp, #(16 * 4)]; \
    bl function; \
    ldmia sp, {r0-r15}^

.globl branchto
branchto:
    bx r0

.globl enter_user_mode
enter_user_mode:

    cpsie ia, 0b10000 @ user mode, interrupts on
    mov r1, #0
    mcr p15, 0, r1, c7, c5, 4 @ prefetch flush

    @ Zero registers
    mov r1, #0
    mov r2, #0
    mov r3, #0
    mov r4, #0
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
    mov r9, #0
    mov r10, #0
    mov r11, #0
    mov r12, #0
    mov lr, #0

    msr cpsr, r2 @ clear CPSR (the bits we can clear)

    bx r0 @ r0 contains code


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
