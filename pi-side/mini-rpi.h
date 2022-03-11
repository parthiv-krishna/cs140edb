#ifndef MINI_RPI_H
#define MINI_RPI_H

typedef unsigned int uint32_t;


void put32(volatile void *addr, uint32_t v) {
    asm volatile ("str %0, [%1]" : : "r" (v), "r" (addr) :);
}
void PUT32(uint32_t addr, uint32_t v) {
    put32((volatile void *)addr, v);
}

uint32_t get32(const volatile void *addr) {
    uint32_t ret;
    asm volatile ("ldr %0, [%1]" : "=r" (ret) : "r" (addr) :);
    return ret;
}
uint32_t GET32(uint32_t addr) {
    return get32((const volatile void *)addr);
}

void dev_barrier(void) {
    asm volatile ("mcr p15, 0, r0, c7, c10, 4" : : :);
}

#endif // MINI_RPI_H