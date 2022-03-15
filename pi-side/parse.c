#include "parse.h"


#define SPECIAL_REG_OFFSET 11
const char *SPECIAL_REG_NAMES[] = {"fp", "ip", "sp", "lr", "pc", "spsr", NULL};


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


// returns 1 if we should return to the program
int process_input(char *line, uint32_t *regs) {
    char *cmd = parse_token(&line);
    switch (cmd[0]) {
        case 'c':
            return 1;
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
        case 'q':
            debugger_println("DONE!!!");
            rpi_reboot();
        default:
            debugger_println("Invalid command. Use 'h' for a list of commands");
    }
    return 0;
}