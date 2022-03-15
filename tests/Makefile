SRC = $(wildcard *.c)

TARGETS = $(SRC:.c=.bin)

OBJS = $(SRC:.c=.o)

ARM = arm-none-eabi
CC = $(ARM)-gcc
LD  = $(ARM)-ld
AS  = $(ARM)-as
OD  = $(ARM)-objdump
OCP = $(ARM)-objcopy

# from cs140e repo
CFLAGS = -O -Wall -nostdlib -nostartfiles -ffreestanding -march=armv6 -std=gnu99  -Wno-unused-variable -Wno-unused-function
ASFLAGS = --all-warnings -mcpu=arm1176jzf-s -march=armv6zk

MEMMAP := ./memmap
START := ./start.o

DEPS := $(START) $(MEMMAP) ./Makefile

all: $(OBJS) $(TARGETS)

# create .o from .c or .S
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S $(DEPS)
	$(CC) -c $(ASFLAGS) $< -o $@

# create .elf
%.elf: $(OBJS) $(MEMMAP) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) -T $(MEMMAP) -o $@

# convert .elf to .list
%.list: %.elf
	$(OD) -D  $< > $@

# convert .list to .bin
%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

# delete compile things
clean:
	rm -rf *.o *.bin *.elf *.list

# these are actually commands not things to make
.PHONY: all clean

# make tries to delete intermediate results if you don't say they're precious
.PRECIOUS: %.list %.elf
