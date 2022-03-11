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
    ldr pc, =notmain

.globl BRANCHTO
BRANCHTO:
    bx r0
