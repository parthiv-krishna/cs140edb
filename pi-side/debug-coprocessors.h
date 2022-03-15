#ifndef __ARMV6_DEBUG_H__
#define __ARMV6_DEBUG_H__

#include "mini-rpi.h"
#include "bits.h"

// turn <x> into a string
#define MK_STR(x) #x

// define a general co-processor inline assembly routine to set the value.
// from manual: must prefetch-flush after each set.
#define coproc_mk_set(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)        \
    static inline void c ## coproc ## _ ## fn_name ## _set(uint32_t v) {    \
        asm volatile ("mcr " MK_STR(coproc) ", "                            \
                             MK_STR(opcode_1) ", "                          \
                             "%0, "                                         \
                            MK_STR(Crn) ", "                                \
                            MK_STR(Crm) ", "                                \
                            MK_STR(opcode_2) :: "r" (v));                   \
        prefetch_flush();                                                   \
    }

#define coproc_mk_get(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)        \
    static inline uint32_t c ## coproc ## _ ## fn_name ## _get(void) {      \
        uint32_t ret=0;                                                     \
        asm volatile ("mrc " MK_STR(coproc) ", "                            \
                             MK_STR(opcode_1) ", "                          \
                             "%0, "                                         \
                            MK_STR(Crn) ", "                                \
                            MK_STR(Crm) ", "                                \
                            MK_STR(opcode_2) : "=r" (ret));                 \
        return ret;                                                         \
    }


// make both get and set methods.
#define coproc_mk(fn, coproc, opcode_1, Crn, Crm, opcode_2)     \
    coproc_mk_set(fn, coproc, opcode_1, Crn, Crm, opcode_2)     \
    coproc_mk_get(fn, coproc, opcode_1, Crn, Crm, opcode_2) 

///////////////
// Functions //
///////////////

int cp14_is_enabled(void);

// enable debug coprocessor 
void cp14_enable(void);

// disable debug coprocessor
void cp14_disable(void);

int cp14_bcr0_is_enabled(void);

void cp14_bcr0_enable(void);

void cp14_bcr0_disable(void);

int was_brkpt_fault(void);

int datafault_from_ld(void);

int datafault_from_st(void);

int was_watchpt_fault(void);

int cp14_wcr0_is_enabled(void);

void cp14_wcr0_enable(void);

void cp14_wcr0_disable(void);

void breakpt_set0(uint32_t *addr);
void watchpt_set0(uint32_t *addr);

#endif
