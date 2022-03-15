#ifndef MINI_RPI_H
#define MINI_RPI_H

typedef unsigned int uint32_t;

#define NULL 0


void put32(volatile void *addr, uint32_t v);
void PUT32(uint32_t addr, uint32_t v);

uint32_t get32(const volatile void *addr);
uint32_t GET32(uint32_t addr);

void dev_barrier(void);
void prefetch_flush(void);

void panic(const char* msg);

#define assert(should_be_true)              \
            do {                            \
                if (!should_be_true) {      \
                    panic(#should_be_true); \
                }                           \
            } while (0);

#endif // MINI_RPI_H