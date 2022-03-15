#include "breakpt-watchpt.h"

// in .h so they can be printed
uint32_t *breakpts[BREAKPT_MAX];
uint32_t *watchpts[WATCHPT_MAX];
unsigned int breakpts_mask;
unsigned int watchpts_mask;

int find_first_available(unsigned int mask) {
    int id = 0;
    while ((mask >> id) & 1) {
        id++;
    }
    return id;
}

void breakpt_print_active(void) {
    for (int i = 0; i < BREAKPT_MAX; i++) {
        if ((breakpts_mask >> i) & 1) {
            uart_printf('d', i);
            uart_puts(": ");
            uart_printf('x', breakpts[i]);
            uart_puts("\n");
        }
    }
}

void watchpt_print_active(void) {
    for (int i = 0; i <= WATCHPT_MAX; i++) {
        if ((watchpts_mask >> i) & 1) {
            uart_printf('d', i);
            uart_puts(": ");
            uart_printf('x', watchpts[i]);
            uart_puts("\n");
        }
    }
}

int breakpt_get_id(uint32_t *addr) {
    for (int i = 0; i < BREAKPT_MAX; i++) {
        if ((breakpts_mask >> i) & 1) {
            if (breakpts[i] == addr) {
                return i;
            }
        }
    }
    return -1;
}

int watchpt_get_id(uint32_t *addr) {
    for (int i = 0; i <= WATCHPT_MAX; i++) {
        if ((watchpts_mask >> i) & 1) {
            if (watchpts[i] == addr) {
                return i;
            }
        }
    }
    return -1;
}

// setup handlers: enable cp14
void breakpt_watchpt_init(void) {
    breakpts_mask = 0; // all start disabled
    watchpts_mask = 0;
    cp14_enable();
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

void breakpt_disable(uint32_t *addr) {
    int id = breakpt_get_id(addr);
    if (id == -1) {
        return;
    }
    cp14_bcr_disable(id);
    breakpts_mask = bit_clr(breakpts_mask, id);
}

void watchpt_disable(uint32_t *addr) {
    int id = watchpt_get_id(addr);
    if (id == -1) {
        return;
    }
    cp14_wcr_disable(id);
    watchpts_mask = bit_clr(watchpts_mask, id);
}