#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include "mini-rpi.h"
#include "uart.h"
#include "interrupts.h"
#include "breakpt-watchpt.h"

void debugger_print(char *str);
void debugger_println(char *str);


/**
 * @brief Called by debugger-start.S
 * 
 * @param target_dst destination of user code (i.e. original_code_start)
 * @param target_src start of current user code location (i.e. original_code_end)
 */
void notmain(uint32_t *target_dst, uint32_t *target_src);

/**
 * @brief Copies user program to the location it expects to be linked to
 * 
 * @param dst location user code should be copied
 * @param src location containing size of user code and then user code
 */
void move_user_program(uint32_t *dst, uint32_t *src);

/**
 * @brief Switches to user mode, clears registers, branches to code. does not return
 * 
 * @param code address to branch to
 */
void enter_user_mode(uint32_t *code);

// main REPL
void debugger_shell(uint32_t *regs);

#endif