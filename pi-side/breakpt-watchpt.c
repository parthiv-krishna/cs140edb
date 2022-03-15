#include "breakpt-watchpt.h"

// setup handlers: enable cp14
void breakpt_watchpt_init(void) {
    cp14_enable();
}