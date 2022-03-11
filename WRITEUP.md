# Writeup

## Things we wish we knew before starting

Linker script (memmap) -> code EXPECTS to be placed at 0x8000 so that absolute loads can work
(e.g. read only data .rodata is stored in an absolute sense)

Contract: bootloader agrees to copy code to 0x8000 (so it is stored after some SPACE)

If it was not copied there, then any absolute ptrs would be broken

KEEP(*(.text.boot))  

uart_init nukes FIFO --> need need to flush tx before starting user code 