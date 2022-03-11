.section ".text.boot"

start:
    # copy self to __code_start__
    sub r0, pc, #8
    ldr r1, =__code_start__
    ldr r2, =__prog_end__
    
copy_loop:
    ldr r3, [r0]
    str r3, [r1]





done:
    ldr pc, =notmain