NAME = debugger
OBJS = $(NAME).o
TARGET = $(NAME).bin

ARM = arm-none-eabi
CC = $(ARM)-gcc
LD  = $(ARM)-ld
AS  = $(ARM)-as
OD  = $(ARM)-objdump
OCP = $(ARM)-objcopy

CFLAGS = -O -Wall -nostdlib -nostartfiles -ffreestanding -march=armv6 -std=gnu99  -Wno-unused-variable -Wno-unused-function -I.
ASFLAGS = --all-warnings -mcpu=arm1176jzf-s -march=armv6zk -I.

MEMMAP := ./memmap
START := ./start.o

DEPS := $(MEMMAP) $(wildcard ./*.h) ./Makefile 

all: $(START) $(OBJS) $(TARGET)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S $(DEPS)
	$(CC) -c $(ASFLAGS) $< -o $@

# rules to generate .elf->.list->.bin
%.elf: $(OBJS) $(MEMMAP) $(DEPS)
	$(CC) $(CFLAGS) $(START) $(OBJS) -T $(MEMMAP) -o $@

%.list: %.elf
	$(OD) -D  $< > $@

%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

clean:
	rm -rf *.o *.bin *~ tags *.bin *.elf *.list *.img Makefile.bak 
	@make -s -C ./libpi.small clean

.PHONY: all clean  lib

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf
