#define INTERRUPT_JUMPER_PIN 17

void init_debug_jumper(void);

// checks if the interrupt was caused by the debug jumper
int was_debug_jumper_interrupt(void);

// clears the interrupt event
void clear_debug_jumper_event(void);