TARGET = Output/blink

CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=cortex-m3 -mthumb -std=c99 -Wall -O0 -ffreestanding
LDFLAGS = -T Linker/linker.ld -nostdlib

SRCS = User/main.c Startup/startup_stm32f10x_md.s

all: $(TARGET).elf $(TARGET).hex $(TARGET).bin

$(TARGET).elf: $(SRCS)
	@mkdir -p Output
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCS) -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

flash: $(TARGET).bin
	st-flash write $(TARGET).bin 0x8000000

clean:
	rm -rf Output/*

.PHONY: all flash clean
