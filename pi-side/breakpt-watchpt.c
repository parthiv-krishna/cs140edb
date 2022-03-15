#include "breakpt-watchpt.h"

int breakpt_find_first_available() {
    int id = 0;
    while (breakpt_is_active(id)) {
        id++;
    }
    return id;
}

int watchpt_find_first_available() {
    int id = 0;
    while (watchpt_is_active(id)) {
        id++;
    }
    return id;
}

void breakpt_print_active(void) {
    int printed = 0;
    for (int i = 0; i < BREAKPT_MAX; i++) {
        if (breakpt_is_active(i)) {
            uart_printf('d', i);
            uart_puts(": ");
            uart_printf('x', breakpt_addr(i));
            uart_puts("\n");
            printed = 1;
        }
    }
    if (!printed) {
        uart_puts("(none)\n");
    }
}

void watchpt_print_active(void) {
    int printed = 0;
    for (int i = 0; i <= WATCHPT_MAX; i++) {
        if (watchpt_is_active(i)) {
            uart_printf('d', i);
            uart_puts(": ");
            uart_printf('x', watchpt_addr(i));
            uart_puts("\n");
            printed = 1;
        }
    }
    if (!printed) {
        uart_puts("(none)\n");
    }
}

int breakpt_get_id(uint32_t *addr) {
    for (int i = 0; i < BREAKPT_MAX; i++) {
        if (breakpt_addr(i) == addr) {
            if (breakpt_is_active(i)) {
                return i;
            }
        }
    }
    return -1;
}

int watchpt_get_id(uint32_t *addr) {
    for (int i = 0; i <= WATCHPT_MAX; i++) {
        if (watchpt_addr(i) == addr) {
            if (watchpt_is_active(i)) {
                return i;
            }
        }
    }
    return -1;
}

// setup handlers: enable cp14
void breakpt_watchpt_init(void) {
    cp14_enable();
}

// set a breakpoint on <addr> returns id or -1 if failed
int breakpt_set(uint32_t *addr) {
    int id = breakpt_find_first_available();
    // max reserved for singlestep
    if (id >= BREAKPT_MAX) {
        return -1;
    }
    breakpt_set_helper(id, addr);
    return id;
}

// set a watchpoint on <addr> returns id or -1 if failed
int watchpt_set(uint32_t *addr) {
    int id = watchpt_find_first_available();
    if (id > WATCHPT_MAX) {
        return -1;
    }
    watchpt_set_helper(id, addr);
    return id;
}

void breakpt_disable(uint32_t *addr) {
    int id = breakpt_get_id(addr);
    if (id == -1) {
        return;
    }
    cp14_bcr_disable(id);
}

void watchpt_disable(uint32_t *addr) {
    int id = watchpt_get_id(addr);
    if (id == -1) {
        return;
    }
    cp14_wcr_disable(id);
}

void breakpt_singlestep_start(uint32_t caller_pc) {
    breakpt_mismatch_set(BREAKPT_MAX, caller_pc);
}

void breakpt_singlestep_stop(void) {
    breakpt_mismatch_stop(BREAKPT_MAX);
}
