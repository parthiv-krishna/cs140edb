// #include "uart.h"

void rpi_reboot(void);

void notmain(void) {
    // uart_puts("about to dereference null");
    
    volatile int uh_oh = *(int *)0;
    uh_oh = *(int *)0;
    uh_oh = *(int *)0;
    uh_oh = *(int *)0;
    uh_oh = *(int *)0;
    uh_oh = *(int *)0;
    uh_oh = *(int *)0;
    uh_oh = *(int *)0;
    uh_oh = *(int *)0;
    uh_oh = *(int *)0;
    uh_oh = *(int *)0;
    uh_oh = *(int *)0;
    uh_oh = *(int *)0;

    
}