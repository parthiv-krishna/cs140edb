# CS140EDB
Parthiv Krishna, Colin Schultz

## Project Description

We created a debugging shell that can debug arbitrary .bin files compiled with libpi. This consists of a desktop-side installer (written in Rust) which packages the input binary with the pi-side debugger binary and installs it on the pi. At the moment, the installer just performs the installation exactly as my-install, and then passes the uart I/O through to the terminal interface.

Motivation

Having spent the entire quarter working on the labs with very little debugging capabilities, we were frustrated by the difficulty of diagnosing issues with our code. After working with the ARM-v6 debugging hardware in lab 9, we were interested in building an interface that would allow for transparent debugging of arbitrary user programs, akin to gdb.

## Commands
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
## Problems

The most difficult issues we faced while making this project were due to our very limited understanding of the code linking process. Specifically, we had to tackle the problem of making sure that all C code is run from the same memory address that it was linked to. The Raspberry Pi firmware loads programs to 0x8000 by default, so this is where most programs we work with expect to run. Thus, our bootloader also loads programs to 0x8000 (ostensibly, we can pass a different address to the bootloader, but in reality there is not much flexibility). Our solution to this involves a multistep process where 1) The debugger code and user code are concatenated and loaded to 0x8000, 2) the debugger copies itself to 0xFF8000 (arbitrary), 3) The debugger code, from 0xFF8000, copies the user code to 0x8000 to let it run.

It took a while for us to understand why this is necessary, but the summary is that some addresses in the assembly code are relative, but some are absolute, and the latter are dependent on having a known absolute address for the entire binary. Normally virtual memory could handle these dependencies. It was also confusing why the .list file showed the data beginning at 0x8000, but the .bin file does not appear to have enough bytes in it. This is because objcopy decides how to pack the code and data into a flat binary, and that means it does not include many addresses that appear to exist in the .list. Instead, there is an implied “contract” between the linker and the bootloader, where both agree (through the linker script and the pi-side bootloader C code) that the program must be stored starting at 0x8000.

This confusion caused another difficult problem later on. In order for the debugger to copy its own code, it needs labels to the start and end of the code. This is easy to do in the linker script. However, it turns out that just because there is a label at some address does not mean that the .bin will include enough bytes for the label to be within its size. Since our linker script ended with ALIGN(8) before __prog_end__, the __prog_end__ label was actually outside of the bounds of the .bin whenever the .bin was not aligned to 8 bytes in size. This normally would not be an issue, except the installer places the start of the user program directly after the end of the .bin. This means that the __prog_end__ label actually existed inside the bounds of the user code once the combined binary was loaded. This is a problem because we expect the size of the user program to be stored at __prog_end__, but instead we read an instruction (e.g. e24f0008 – sub r0, pc, #8) and enter a practically-infinite loop. This and other opaque infinite loops also motivated our “Ctrl-C jumper” to stop the user program wherever it’s currently running, though it would not have helped us debug the kernel-mode debugger loop. We updated the linker script to guarantee that __prog_end__ would be inside the .bin.

## Future Work

There were a couple of quality-of-life features that we were unable to implement in the limited time we had for the project. Though CS140EDB is useable as-is to debug programs, some more development would make it much more powerful.

Our first priority would be to have the unix-side Rust program parse the .list file to enable more intuitive operation. Currently, both the unix-side and pi-side of the debugger are unaware of function names even with -mpoke-function-name enabled. The only way to interact with the function names is via initializing a backtrace with `t`, and setting breakpoints can only happen at user-provided addresses (e.g. `b 0x825c`). By having the unix-side Rust load and parse the .list file, we would be able to instead `b printk` or similar, which would be much easier to use. This would also require the unix-side application to “intercept” the commands and determine whether it needed to translate them to, for example, tell the pi-side to `b 0x825c` when the user types `b printk`.

Analogous to symbol translation when making commands, symbol translation for the current pc location would also make the debugger easier to use. For example, rather than saying `Breakpoint #0 triggered at pc=0x825c`, the Rust-side code would translate this to `Breakpoint #0 triggered at pc=<printk+0x0>`. This would also involve reading the .list. By compiling with additional debug symbols, we may even be able to single-step at a C-line level or print values using variable names in addition to registers.

## Code

Our code can be found here: https://github.com/parthiv-krishna/cs140edb. Our goal was to create a standalone repository that has no dependency on libpi; we therefore reimplemented and reorganized the foundational code inside pi-side. Our goal was to truly understand every line of code that went into our project, including Makefiles, assembly, and linker scripts.
