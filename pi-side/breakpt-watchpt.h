#ifndef BREAKPOINT_WATCHPOINT_H
#define BREAKPOINT_WATCHPOINT_H

#include "mini-rpi.h"
#include "debug-coprocessors.h"
#include "uart.h"

// setup breakpt and watchpt
void breakpt_watchpt_init(void);

void breakpt_print_active(void);

void watchpt_print_active(void);

int breakpt_get_id(uint32_t *addr);

int watchpt_get_id(uint32_t *addr);

// set a breakpoint on addr
// returns 1 if successful or 0 if failed
int breakpt_set(uint32_t *addr);

// set a watchpoint on addr
// returns 1 if successful or 0 if failed
int watchpt_set(uint32_t *addr);

void breakpt_disable(uint32_t *addr);

void watchpt_disable(uint32_t *addr);

void breakpt_singlestep_start(uint32_t caller_pc);

void breakpt_singlestep_stop(void);

#endif // BREAKPOINT_WATCHPOINT_H