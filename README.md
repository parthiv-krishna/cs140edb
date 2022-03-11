# cs140edb
Debugger

UART

arm-none-eabi-gcc -c  start.S -o start.o
arm-none-eabi-gcc -O -Wall -nostdlib -nostartfiles -ffreestanding  -march=armv6 -std=gnu99  -Wno-unused-variable -c debugger.c -o debugger.o
arm-none-eabi-gcc -O -Wall -nostdlib -nostartfiles -ffreestanding  -march=armv6 -std=gnu99  -Wno-unused-variable ./start.o debugger.o gpio.o -T ./memmap -o debugger.elf 
arm-none-eabi-objdump -D  debugger.elf > debugger.list
arm-none-eabi-objcopy debugger.elf -O binary debugger.bin

your program:
blink.bin

here:
cs140edb.bin
$ debug blink.bin
    -> concatenate files with pitag linker (or replacement)
    -> my-install to 0x8000
    -> copy self to another location A
    -> jump to A
    -> copy blink.bin to 0x8000
    -> jump to 0x8000
