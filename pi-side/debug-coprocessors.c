#include "debug-coprocessors.h"

// 13-5
struct debug_id {
    uint32_t    revision:4,     // 0:3  revision number
                variant:4,      // 4:7  major revision number
                :4,             // 8:11
                debug_rev:4,    // 12:15
                debug_ver:4,    // 16:19
                context:4,      // 20:23
                brp:4,          // 24:27 --- number of breakpoint register pairs + 1
                wrp:4           // 28:31 --- number of watchpoint pairs.
        ;
};

// 13-8
enum debug_status {
    DSCR_REASON_LO = 2,    // bits 2-5 are reason for jumping to prefetch/data abort
    DSCR_REASON_HI = 5,
    DSCR_MODE_SELECT = 14,  // 14 mode select bit
    DSCR_ENABLE = 15, // 15 monitor debug mode enable

    // 13-11: methods of entering prefetch/data abort vector
    DSCR_BREAKPOINT = 0b0001,
    DSCR_WATCHPOINT = 0b0010,
};

// 3-67
enum inst_fault_status {
    IFSR_STATUS_LO = 0, // low bit of status
    IFSR_STATUS_HI = 3, // hi bit of status
    IFSR_S = 10, // 10 S bit
    
    IFSR_INSTR_DEBUG_EVENT_FAULT = 0b0010 // bottom 4 bits for debug event
};

// 13-18 and 13-19
enum breakpt_ctrl {
    BREAKPT_CTRL_EN = 0,   // 0 enable bit
};

// unified breakpoint and watchpoint ctrl
// 13-18 and 13-19 for breakpoint
// 13-21 and 13-22 for watchpoint
typedef struct {
    uint32_t enable:1,            // 0 enable
             sv_access:2,         // 1:2 supervisor access
             load_store_access:2, // 3:4 reserved
             byte_addr_sel:4,     // 5:8 byte address select
             :5,                  // 9:13 reserved
             secure:2,            // 14:15 matching in secure/not secure worlds
             brp:4,               // 16:19 linked breakpoint
             enable_linking:1,    // 20 enable linking
             meaning:2,           // meaining of BVR 
             :9                   // 23:31 reserved
             ;
} bpt_wpt_ctrl_t;

// 3-65
enum data_fault_status {
    DFSR_STATUS_LO = 0, // low bit of status
    DFSR_STATUS_HI = 3, // hi bit of status
    DFSR_S = 10, // 10 S bit
    
    DFSR_INSTR_DEBUG_EVENT_FAULT = 0b0010 // bottom 4 bits for debug event
};

// 13-21 and 13-22
enum watchpt_ctrl {
    WATCHPT_CTRL_EN = 0,   // 0 enable bit
};

// example of how to define get and set for status registers
coproc_mk(status, p14, 0, c0, c1, 0)

// Debug ID Reg page 13-5
coproc_mk(didr, p14, 0, c0, c0, 0)
// Debug Status and Control Reg page 13-5
coproc_mk(dscr, p14, 0, c0, c1, 0)

// Instruction Fault Status Reg page 3-68
coproc_mk(ifsr, p15, 0, c5, c0, 1) 
// Breakpoint Control Reg 0 page 13-6
coproc_mk(bcr0, p14, 0, c0, c0, 5)
// Instruction Fault Addr Reg page 3-69
coproc_mk(ifar, p15, 0, c6, c0, 2)
// Breakpoint Value Reg 0 page 13-6
coproc_mk(bvr0, p14, 0, c0, c0, 4)

// Data Fault Status Reg page 3-66
coproc_mk(dfsr, p15, 0, c5, c0, 0)
// Watchpoint Control Reg 0 page 13-6
coproc_mk(wcr0, p14, 0, c0, c0, 7)
// Watchpoint Fault Addr Reg page 13-12
coproc_mk(wfar, p14, 0, c0, c6, 0)
// Watchpoint Value Reg 0 page 13-20
coproc_mk(wvr0, p14, 0, c0, c0, 6)
// Fault Addr Reg page 3-68
coproc_mk(far, p15, 0, c6, c0, 0)

// return 1 if enabled, 0 otherwise.  
//    - we wind up reading the status register a bunch:
//      could return its value instead of 1 (since is 
//      non-zero).
int cp14_is_enabled() {
    uint32_t dscr = cp14_dscr_get();
    return bit_get(dscr, DSCR_ENABLE); // 13-9
}

// enable debug coprocessor 
void cp14_enable() {
    // if it's already enabled, just return?
    if(cp14_is_enabled())
        return;

    // // setup exception handlers
    // extern unsigned _interrupt_table;
    // int_init_reg(&_interrupt_table);

    // for the core to take a debug exception, monitor debug mode has to be both 
    // selected and enabled --- bit 14 clear and bit 15 set.
    uint32_t dscr = cp14_dscr_get();
    dscr = bit_clr(dscr, DSCR_MODE_SELECT); // 13-9
    dscr = bit_set(dscr, DSCR_ENABLE);  // 13-9
    cp14_dscr_set(dscr);
    prefetch_flush();

    assert(cp14_is_enabled());
}

// disable debug coprocessor
void cp14_disable() {
    if(!cp14_is_enabled())
        return;

    uint32_t dscr = cp14_dscr_get();
    dscr = bit_clr(dscr, DSCR_ENABLE);  // 13-9
    cp14_dscr_set(dscr);
    prefetch_flush();

    assert(!cp14_is_enabled());
}


int cp14_bcr0_is_enabled() {
    uint32_t bcr0 = cp14_bcr0_get();
    return bit_get(bcr0, BREAKPT_CTRL_EN);
}

void cp14_bcr0_enable() {
    // 13-45
    
    // read the BCR
    uint32_t bcr0 = cp14_bcr0_get();

    // clear the enable breakpoint bit and write back
    bcr0 = bit_clr(bcr0, BREAKPT_CTRL_EN);
    cp14_bcr0_set(bcr0);

    bpt_wpt_ctrl_t *bcr0_ptr = (bpt_wpt_ctrl_t *)&bcr0;

    // page 13-18
    bcr0_ptr->meaning = 0b00; // imva
    bcr0_ptr->enable_linking = 0; // no linking
    bcr0_ptr->secure = 0b00; // breakpoints both in secure and non secure 
    bcr0_ptr->byte_addr_sel = 0b1111; // byte address select all addresses
    bcr0_ptr->sv_access = 0b11; // privileged and user
    bcr0_ptr->enable = 1;

    cp14_bcr0_set(bcr0);
    prefetch_flush();
}

void cp14_bcr0_disable() {
    uint32_t bcr0 = cp14_bcr0_get();
    bcr0 = bit_clr(bcr0, BREAKPT_CTRL_EN); // disable 13-22
    cp14_bcr0_set(bcr0);
    prefetch_flush();
}

// was this a brkpt fault?
int was_brkpt_fault(void) {
    // use IFSR and then DSCR
    
    // confirm a debug exception has occurred
    uint32_t ifsr = cp15_ifsr_get();
    // 3-65
    if (bit_get(ifsr, IFSR_S)) {
        return 0;
    }
    uint32_t status = bits_get(ifsr, IFSR_STATUS_LO, IFSR_STATUS_HI);
    if (status != IFSR_INSTR_DEBUG_EVENT_FAULT) {
        return 0;
    }
    // if bit 10 is off and bits 0:3 are 0b0010 then 
    // an instruction debug event fault occurred
    
    // check DSCR bits 2:5 for reason for entering 
    // see page 13-11
    uint32_t dscr = cp14_dscr_get();
    uint32_t reason = bits_get(dscr, DSCR_REASON_LO, DSCR_REASON_HI);
    if (reason == DSCR_BREAKPOINT) {
        return 1;
    }
    return 0;
}

// was watchpoint debug fault caused by a load?
int datafault_from_ld(void) {
    return bit_get(cp15_dfsr_get(), 11) == 0;
}
// ...  by a store?
int datafault_from_st(void) {
    return !datafault_from_ld();
}


// 13-33: tabl 13-23
int was_watchpt_fault(void) {
    // use DFSR then DSCR
    
    // confirm a debug exception has occurred
    uint32_t dfsr = cp15_dfsr_get();
    // 3-65
    if (bit_get(dfsr, DFSR_S)) {
        return 0;
    }
    uint32_t status = bits_get(dfsr, DFSR_STATUS_LO, DFSR_STATUS_HI);
    if (status != DFSR_INSTR_DEBUG_EVENT_FAULT) {
        return 0;
    }
    // if bit 10 is off and bits 0:3 are 0b0010 then 
    // an instruction debug event fault occurred
    
    // check DSCR bits 2:5 for reason for entering 
    // see page 13-11
    uint32_t dscr = cp14_dscr_get();
    uint32_t reason = bits_get(dscr, DSCR_REASON_LO, DSCR_REASON_HI);
    if (reason == DSCR_WATCHPOINT) {
        return 1;
    }
    return 0;
}

int cp14_wcr0_is_enabled(void) {
    uint32_t wcr0 = cp14_wcr0_get();
    return bit_get(wcr0, WATCHPT_CTRL_EN);
}

void cp14_wcr0_enable(void) {
    // 13-45
    uint32_t wcr0 = cp14_wcr0_get();

    wcr0 = bit_clr(wcr0, WATCHPT_CTRL_EN);
    cp14_wcr0_set(wcr0);

    bpt_wpt_ctrl_t *wcr0_ptr = (bpt_wpt_ctrl_t *)&wcr0;

    // 13-21 and 13-22
    wcr0_ptr->enable_linking = 0b0; // no linking
    wcr0_ptr->secure = 0b00; // watchpoints both in secure and non secure 
    wcr0_ptr->byte_addr_sel = 0b1111; // byte address select all accesses
    wcr0_ptr->load_store_access = 0b11; // loads and stores
    wcr0_ptr->sv_access = 0b11; // privileged and user
    wcr0_ptr->enable = 0b1; // enable

    cp14_wcr0_set(wcr0);
    prefetch_flush();
}

void cp14_wcr0_disable(void) {
    uint32_t wcr0 = cp14_wcr0_get();
    wcr0 = bit_clr(wcr0, WATCHPT_CTRL_EN); // disable 13-22
    cp14_wcr0_set(wcr0);
    prefetch_flush();
}

// Get watchpoint fault using WFAR
uint32_t watchpt_fault_pc(void) {
    uint32_t wfar = cp14_wfar_get();
    // see 13-12:  WFAR contains the address of the instruction causing it plus 0x8.
    return wfar - 0x8;
}

uint32_t watchpt_fault_addr(void) {
    uint32_t far = cp15_far_get();
    return far;
}

// set a breakpoint on <addr>: call <h> when triggers.
void breakpt_set0(uint32_t *addr) {
    // assert(addr % 4 == 0);  // enforce alignment
    cp14_bcr0_disable();
    cp14_bvr0_set((uint32_t) addr);
    cp14_bcr0_enable();


    assert(cp14_bcr0_is_enabled());
}

// set a watchpoint on <addr>: call handler <h> when triggers.
void watchpt_set0(uint32_t *addr) {
    // assert(addr % 4 == 0); // enforce alignment
    cp14_wcr0_disable();
    cp14_wvr0_set((uint32_t) addr);
    cp14_wcr0_enable();


    assert(cp14_wcr0_is_enabled());
}