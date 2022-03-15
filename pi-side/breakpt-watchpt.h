#ifndef BREAKPOINT_WATCHPOINT_H
#define BREAKPOINT_WATCHPOINT_H

#include "mini-rpi.h"
#include "debug-coprocessors.h"

// setup breakpt and watchpt
void breakpt_watchpt_init(void);

// set a breakpoint on addr
// returns 1 if successful or 0 if failed
int breakpt_set(uint32_t *addr);

// set a watchpoint on addr
// returns 1 if successful or 0 if failed
int watchpt_set(uint32_t *addr);

#endif // BREAKPOINT_WATCHPOINT_H