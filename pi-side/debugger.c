#include "mini-rpi.h"
#include "debugger.h"

#define USER_MODE 0b10000
const char *HEX = "0123456789abcdef";

#define SPECIAL_REG_OFFSET 11
const char *SPECIAL_REG_NAMES[] = {"fp", "ip", "sp", "lr", "pc", "spsr", NULL};

// inline so we can use mod
inline void uart_print_int(uint32_t val, int base) {
    char buf[32];
    int msd = 0;
    for (int i = 0; i < sizeof(buf); i++) {
        int next = val % base;
        if (next != 0) {
            msd = i;
        }
        buf[i] = HEX[next];
        val /= base;
    }
    for (int i = msd; i >= 0; i--) {
        uart_putc(buf[i]);
    }
}

void uart_printf(char fmt, uint32_t val) {
    int base = 10;
    switch (fmt) {
    case 's':
        uart_puts((char *) val);
        break;
    case 'c':
        uart_putc((char) val);
        break;
    case 'x':
        uart_puts("0x");
        uart_print_int(val, 16);
        break;
    case 'b':
        uart_puts("0b");
        uart_print_int(val, 2);
        break;
    case 'd':
    case 'i':
        if ((int) val < 0) {
            uart_putc('-');
            val *= -1;
        }
    case 'u':
        uart_print_int(val, 10);
        break;
    default:
        uart_puts("<Invalid format spec>");
        break;
    }
}

char *parse_token(char **s) {
    char *start = *s;
    char *curr = start;
    while (*curr != 0 && *curr != ' ') { // while in token
        curr++;
    }
    while (*curr == ' ') {
        *(curr++) = 0;
    }
    *s = curr;
    return start;
}

int find_in_arr(char *c, const char **arr) {
    int i = 0;
    while (arr[i] != NULL) {
        for (int j = 0; c[j] == arr[i][j]; j++) {
            if (c[j] == 0) {
                return i;
            }
        }
        i++;
    }
    return -1;
}

int find_in_s(char c, const char *s) {
    int i = 0;
    while (s[i] != c && s[i] != 0) {
        i++;
    }
    if (s[i] == 0) {
        return -1;
    }
    return i;
}

int parse_int(char *expr) {
    int sign = 1;
    int base = 10;
    if (expr[0] == '-') {
        sign = -1;
        expr++;
    }
    if (expr[0] == '0') {
        if (expr[1] == 'x') {
            base = 16;
            expr += 2;
        } else if (expr[1] == 'b') {
            base = 2;
            expr += 2;
        }
    }
    uint32_t num = 0;
    int next;
    while ((next = find_in_s(expr[0], HEX)) != -1) {
        num = num * base + next;
        expr++;
    }
    return num * sign;
}

uint32_t parse_expr(char *expr, int get_addr, uint32_t *regs) {
    int n_indirects = 0; // if not addr, deref at least once
    for (; *expr == '*'; expr++) {
        n_indirects++;
    }
    uint32_t res;
    int reg = 1;
    if (expr[0] == 'r') {
        res = (uint32_t)&regs[parse_int(expr + 1)];
    } else {
        int i = find_in_arr(expr, SPECIAL_REG_NAMES);
        if (i != -1) {
            res = (uint32_t)&regs[i + SPECIAL_REG_OFFSET];
        } else {
            res = parse_int(expr);
            reg = 0;
        }
    }
    if (reg && !get_addr) { // if we want to value of a reg, dereference
        n_indirects++;
    }
    while (n_indirects-- > 0) {
        res = *(uint32_t *)res;
    }
    return res;
}

int process_input(char *line, uint32_t *regs) {
    char *cmd = parse_token(&line);
    switch (cmd[0]) {
        case 'p':;
            char format = line[1];
            char *expr = parse_token(&line);
            uint32_t val = parse_expr(expr, 0, regs);
            debugger_print(expr);
            uart_puts(" = ");
            uart_printf(cmd[1], val);
            uart_putc('\n');
            break;
        case 'k':;
            char *expr1 = parse_token(&line);
            uint32_t *dst = (uint32_t *)parse_expr(expr1, 1, regs);
            char *expr2 = parse_token(&line);
            uint32_t src = parse_expr(expr2, 0, regs);
            *dst = src;
            debugger_print("Wrote ");
            uart_printf('x', src);
            uart_puts(" to *");
            uart_printf('x', (uint32_t)dst);
            uart_putc('\n');
            break;
        default:
            debugger_println("Invalid command. Use 'h' for a list of commands");
    }
    return 0;
}

void debugger_shell(uint32_t *regs) {
    while (1) {
        char line[512];
        uart_putc('>');
        uart_gets(line, sizeof(line));
        if (process_input(line, regs)) {
            break;
        }
    }
    uart_flush_tx();
}


void move_user_program(uint32_t *dst, uint32_t *src) {
    // src is pointer to int containing length of user program
    unsigned user_len = *src / sizeof(uint32_t);
    // next word is start of user code
    uint32_t *user_code = src + 1;
    for (unsigned i = 0; i < user_len; i++) {
        dst[i] = user_code[i];
    }
}
    
uint32_t regs[17] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

void notmain(uint32_t *target_dst, uint32_t *target_src) {
    move_user_program(target_dst, target_src);

    uart_init();

    debugger_println("Hello from debugger");

    // user program is wherever the bootloader would have put it
    // if we did not inject the debugger code
    debugger_shell(regs);
    init_interrupts();

    breakpt_watchpt_init();

    // setup breakpoint at target_dst
    // breakpt_set0(target_dst);

    prefetch_flush();

    // user program is wherever the bootloader would have put it
    // if we did not inject the debugger code

    debugger_print("About to enter user code");

    // flush regs, enter user mode
    enter_user_mode(target_dst); // setup user mode and branch to target
}