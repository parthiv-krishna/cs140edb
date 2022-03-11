#include "debugger.h"

/**
 * @brief Copies user program to the location it expects to be linked to
 * 
 * @param dst location user code should be copied
 * @param src location containing size of user code and then user code
 */
void move_user_program(uint32_t *dst, uint32_t *src) {
    // src is pointer to int containing length of user program
    unsigned user_len = *src / sizeof(uint32_t);
    // next word is start of user code
    uint32_t *user_code = src + 1;
    for (unsigned i = 0; i < user_len; i++) {
        dst[i] = user_code[i];
    }
}

/**
 * @brief Called by start.S
 * 
 * @param target_dst 
 * @param target_src 
 */
void notmain(uint32_t *target_dst, uint32_t *target_src) {
    move_user_program(target_dst, target_src);
}