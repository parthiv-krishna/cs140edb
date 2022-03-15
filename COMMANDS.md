# Debugger Commands

```
h - print the commands and usage

c - continue execution until next interrupt

s - step to next assembly instruction

p<fmt> <expr> - print the expression using the format specifier <fmt> (fmt should be {d,u,x,b,s})

k <expr> <expr2> - set the register or memory address equal to <expr2>

b <expr> - set breakpoint <num> on instruction address <expr> (num should be [0-4])

w <expr> - set watchpoint <num> on memory address <expr> (num should be [0-1])

l <type> - list breakpoints or watchpoints (type should be {b,w})

d <type><num> - delete breakpoint or watchpoint (type should be {b,w}, num depends on type)

q - quit the debugger and reboot the pi

j - skip the current instruction (increment pc by 4)

<expr> can be a register (r0, lr, spsr), or a number (128, 0x8000), or any number of dereferences (*r0, **0x8560)
```