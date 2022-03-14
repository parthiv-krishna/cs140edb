#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include "debugger.h"

void init_interupts();

void undefined_intstruction_vector(uint32_t *regs);
void prefetch_abort_vector(uint32_t *regs);
void data_abort_vector(uint32_t *regs);
void interrupt_vector(uint32_t *regs);

void unexpected_interrupt(uint32_t *regs);

#endif