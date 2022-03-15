#ifndef PARSE_H
#define PARSE_H

#include "mini-rpi.h"
#include "uart.h"

char *parse_token(char **s);

int find_in_arr(char *c, const char **arr);
int find_in_s(char c, const char *s);

int parse_int(char *expr);

uint32_t parse_expr(char *expr, int get_addr, uint32_t *regs);

int process_input(char *line, uint32_t *regs);

#endif