#include "debugger.h"

#define USER_MODE 0b10000

void move_user_program(uint32_t *dst, uint32_t *src) {
    // src is pointer to int containing length of user program
    unsigned user_len = *src / sizeof(uint32_t);
    // next word is start of user code
    uint32_t *user_code = src + 1;
    for (unsigned i = 0; i < user_len; i++) {
        dst[i] = user_code[i];
    }
}

void notmain(uint32_t *target_dst, uint32_t *target_src) {
    move_user_program(target_dst, target_src);

    uart_init();

    debugger_print("Hello from debugger");

    // user program is wherever the bootloader would have put it
    // if we did not inject the debugger code

    debugger_print("About to enter user code");

    // setup breakpoint at target_dst

    // flush regs, enter user mode

    // prefetch flush

    enter_user_mode(target_dst); // setup user mode and branch to target
}