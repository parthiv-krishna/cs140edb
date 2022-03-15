#define STACK_ADDR 0x8000000

@ linker hack so we can force this file in the first portion of 
@ the linked binary.
.section ".text.boot"

@ this is where we start.  if you look in any '.list' file, you 
@ better see code at the first addresses (in our case: starting
@ at 0x8000).
.globl _start
_start:
    mov sp, #STACK_ADDR @ initialize stack pointer
    mov fp, #0          @ clear frame pointer reg.  don't think needed.
    @ bl notmain        @ we could jump right to notmain
    bl _cstart          @ call our code to do initialization.
    bl rpi_reboot       @ if they return just reboot.

.globl put32
.globl PUT32
put32:
PUT32:
    str r1,[r0]     @ store r1 into address held in r0
    bx lr           @ return


.globl get32
.globl GET32
get32:
GET32:
    ldr r0,[r0]     @ load address held in r0 into r0
    bx lr           @ return


.globl BRANCHTO
BRANCHTO:
    bx r0

#ifdef RPI_FP_ENABLED

.globl m4add
m4add:
    vmov s0,r0
    vmov s1,r1
    vmov s2,r2
    vmov s3,r3
    vadd.f32 s4,s0,s1
    vadd.f32 s5,s2,s3
    vmov r0,s5
    bx lr

.globl m4add2
m4add2:
    vmov s0,r0
    vmov s1,r1
    vmov s2,r2
    vmov s3,r3
    vadd.f32 s4,s0,s1
    vadd.f32 s5,s2,s3
    vmov r0,s4
    bx lr

.globl m4vmov
m4vmov:
    vmov s4,r0
    vmov s5,r0
    bx lr

#endif

