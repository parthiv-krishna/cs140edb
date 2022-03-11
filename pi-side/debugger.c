
typedef unsigned int uint32_t;

#define ARMBASE 0x8000

void move_user_program(void) {
    extern unsigned *__user_len__;
    unsigned user_len = *__user_len__ / sizeof(uint32_t);
    uint32_t *user_code = __user_len__ + 1;
    uint32_t *dest = (uint32_t *)ARMBASE;
    for (unsigned i = 0; i < user_len; i++) {
        dest[i] = user_code[i];
    }
}

void notmain(void) {
    move_user_program();
}