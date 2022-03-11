arm-none-eabi-gcc -c  start.S -o start.o
arm-none-eabi-gcc -O -Wall -nostdlib -nostartfiles -ffreestanding  -march=armv6 -std=gnu99  -Wno-unused-variable -c debugger.c -o debugger.o
arm-none-eabi-gcc -O -Wall -nostdlib -nostartfiles -ffreestanding  -march=armv6 -std=gnu99  -Wno-unused-variable ./start.o debugger.o -T ./memmap -o debugger.elf 
arm-none-eabi-objdump -D  debugger.elf > debugger.list
arm-none-eabi-objcopy debugger.elf -O binary debugger.bin
