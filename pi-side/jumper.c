#include "gpio.h"
#include "jumper.h"

#define PIN INTERRUPT_JUMPER_PIN

#define GPIO_BASE 0x20200000
enum {
    ENABLE_IRQS_2 = 0x2000B214,
    GPPUD = (GPIO_BASE + 0x94),
    GPPUDCLK0 = (GPIO_BASE + 0x98),
    GPEDS0 = (GPIO_BASE + 0x40),
    GPFEN0 = (GPIO_BASE + 0x58)
};

int gpio_event_detected(unsigned pin) {
    if (pin > MAX_PIN) {
        return -1;
    }
    dev_barrier();
    return (GET32(GPEDS0) & 1 << pin) >> pin;
}

void gpio_event_clear(unsigned pin) {
    if (pin > MAX_PIN) {
        return;
    }
    PUT32(GPEDS0, 1 << pin);
    dev_barrier();
}


void gpio_enable_interrupts(int gpio_int) {
    uint32_t mask = (1 << (gpio_int - 32));
    PUT32(ENABLE_IRQS_2, mask);
    dev_barrier();
}

void gpio_int_falling_edge(unsigned pin) {
    if (pin > MAX_PIN) {
        return;
    } 
    gpio_enable_interrupts(GPIO_INT0);
    uint32_t old = GET32(GPFEN0);
    PUT32(GPFEN0, old | (1 << pin));
    dev_barrier();
}

typedef enum pullupdown {
    DISABLE = 0,
    PULLDOWN = 1,
    PULLUP = 2
} pullupdown_t;

void delay_cycles(unsigned cycles) {
    while (cycles-- > 0) {
    }
}

#define DELAY_CYCLES 150
void gpio_set_pud(uint32_t mask, pullupdown_t value) {
    PUT32(GPPUD, value);
    delay_cycles(DELAY_CYCLES);
    PUT32(GPPUDCLK0, mask);
    delay_cycles(DELAY_CYCLES);
    PUT32(GPPUD, 0);
    PUT32(GPPUDCLK0, 0);
    dev_barrier();
}

void gpio_set_pullup(unsigned pin) {
    if (pin > MAX_PIN) {
        return;
    }
    gpio_set_pud(1 << pin, PULLUP);
}

void gpio_set_pulldown(unsigned pin) {
    if (pin > MAX_PIN) {
        return;
    }
    gpio_set_pud(1 << pin, PULLDOWN);
}



void init_debug_jumper(void) {
    gpio_set_input(PIN);
    gpio_set_pullup(PIN);
    gpio_int_falling_edge(PIN);
    gpio_event_clear(PIN);
}

// checks if the interrupt was caused by the debug jumper
int was_debug_jumper_interrupt(void) {
    return gpio_event_detected(PIN);
}

// clears the interrupt event
void clear_debug_jumper_event(void) {
    gpio_event_clear(PIN);
}