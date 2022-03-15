.section ".text.boot"

start:
    @ copy self to __code_start__
    sub r0, pc, #8              @ original_code_start
    mov r1, r0                  @ original_counter
    ldr r2, =__code_start__     @ target_code_start
    ldr r3, =__prog_end__       @ target_code_end
    mov r4, r2                  @ target_counter
    b copy_loop_cond
    
copy_loop:
    ldr r5, [r1]                @ load from original_counter
    str r5, [r4]                @ store to target_counter
    add r1, #4                  @ update original_counter
    add r4, #4                  @ update target_counter

copy_loop_cond:
    cmp r4, r3
    beq done                    @ while (target_counter != target_code_end)
    b copy_loop                 @ continue 

done:
    @ now r1 contains original_code_end
    @ and we are at the place our linker expects us to be
    @ notmain(original_code_start, original_code_end)
    mov sp, #0x9000000
    ldr pc, =notmain

.globl branchto
branchto:
    bx r0

.globl enter_user_mode
enter_user_mode:

    cpsie ia, 0b10000 @ user mode, interrupts on (maybe need aborts?)
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
