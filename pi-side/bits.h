#ifndef BITS_H
#define BITS_H

#include "mini-rpi.h"

// set x[bit]=0 (leave the rest unaltered) and return the value
static inline uint32_t 
bit_clr(uint32_t x, unsigned bit) {
    return x & ~(1<<bit);
}

// set x[bit]=1 (leave the rest unaltered) and return the value
static inline uint32_t 
bit_set(uint32_t x, unsigned bit) {
    return x | (1<<bit);
}

// get x[bit]
static inline unsigned 
bit_get(uint32_t x, unsigned bit) {
    return (x >> bit) & 1;
}


// return a mask with the <n> low bits set to 1.
//  error if nbits > 32.  ok if nbits = 0.
//
// we use this routine b/c unsigned shift by a variable greater than 
// bit-width gives unexpected results.  
// eg. gcc on x86:
//  n = 32;
//  ~0 >> n == ~0
static inline uint32_t bits_mask(unsigned nbits) {
    // all bits on.
    if(nbits==32)
        return ~0;
    return (1 << nbits) - 1;
}

// extract bits [lb:ub]  (inclusive)
static inline uint32_t 
bits_get(uint32_t x, unsigned lb, unsigned ub) {
    return (x >> lb) & bits_mask(ub-lb+1);
}

#endif // BITS_H