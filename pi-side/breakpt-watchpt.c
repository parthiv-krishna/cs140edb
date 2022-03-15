#include "breakpt-watchpt.h"

// in .h so they can be printed
uint32_t *breakpts[BREAKPT_MAX];
uint32_t *watchpts[WATCHPT_MAX];
unsigned int breakpts_mask;
unsigned int watchpts_mask;

int find_first_available(char mask) {
    int id = 0;
    while (mask & 1) {
        mask >>= 1;
        id++;
    }
    return id;
}

// setup handlers: enable cp14
void breakpt_watchpt_init(void) {
    breakpts_mask = 0; // all start disabled
    watchpts_mask = 0;
    cp14_enable();

    watchpt_set_helper(WATCHPT_MAX, NULL); // null protection
}

// set a breakpoint on <addr>
int breakpt_set(uint32_t *addr) {
    int id = find_first_available(breakpts_mask);
    // max reserved for singlestep
    if (id >= BREAKPT_MAX) {
        return 0;
    }
    breakpt_set_helper(id, addr);
    breakpts[id] = addr;
    breakpts_mask = bit_set(breakpts_mask, id);
    return 1;
}

// set a watchpoint on <addr>
int watchpt_set(uint32_t *addr) {
    int id = find_first_available(watchpts_mask);
    // max reserved for nullptr
    if (id >= WATCHPT_MAX) {
        return 0;
    }
    watchpt_set_helper(id, addr);
    watchpts[id] = addr;
    watchpts_mask = bit_set(watchpts_mask, id);
    return 1;
}
