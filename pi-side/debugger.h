typedef unsigned int uint32_t;

/**
 * @brief Called by debugger-start.S
 * 
 * @param target_dst destination of user code (i.e. original_code_start)
 * @param target_src start of current user code location (i.e. original_code_end)
 */
void notmain(uint32_t *target_dst, uint32_t *target_src);

/**
 * @brief Copies user program to the location it expects to be linked to
 * 
 * @param dst location user code should be copied
 * @param src location containing size of user code and then user code
 */
void move_user_program(uint32_t *dst, uint32_t *src);

/**
 * @brief Branches to an address; does not return
 * 
 * @param addr address to branch to
 */
void BRANCHTO(uint32_t *addr);
