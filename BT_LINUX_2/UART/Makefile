TARGET = main
BUILD = build
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CPU = -mcpu=cortex-m3 -mthumb
CFLAGS = $(CPU) -O2 -Wall
CFLAGS += -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER
CFLAGS += -IDriver/inc

LDFLAGS = $(CPU) -TLinker/linker.ld
LDFLAGS += -Wl,--gc-sections
LDFLAGS += --specs=nosys.specs

SRC = User/main.c \
      Driver/src/misc.c \
      Driver/src/stm32f10x_gpio.c \
      Driver/src/stm32f10x_rcc.c \
      Driver/src/stm32f10x_usart.c \
      Driver/src/system_stm32f10x.c

OBJ = $(SRC:%.c=$(BUILD)/%.o)

all: $(BUILD)/$(TARGET).bin

$(BUILD)/$(TARGET).elf: $(OBJ) $(BUILD)/startup_stm32f10x_md.o
	$(CC) $(LDFLAGS) $^ -o $@

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/startup_stm32f10x_md.o: Startup/startup_stm32f10x_md.s
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD)

.PHONY: all clean
