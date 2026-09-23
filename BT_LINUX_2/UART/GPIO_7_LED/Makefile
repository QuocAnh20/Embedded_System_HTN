TARGET = main

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
STFLASH = st-flash

# 1. Thư mục mã nguồn & header
SRC_DIRS = User \
           Startup \
           Driver/GPIO/src \
           Driver/RCC/src

INC_DIRS = User \
           Startup \
           Driver/GPIO/inc \
           Driver/RCC/inc

# 2. Tìm tất cả file .c và .s
C_SOURCES  = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
ASM_SOURCES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.s))

# Tạo danh sách file .o nằm trong thư mục Output/
OBJ_FILES  = $(addprefix Output/, $(notdir $(C_SOURCES:.c=.o)))
OBJ_FILES += $(addprefix Output/, $(notdir $(ASM_SOURCES:.s=.o)))

# Đường dẫn tìm kiếm file nguồn cho Make
vpath %.c $(SRC_DIRS)
vpath %.s $(SRC_DIRS)

INC_DIR_OPT = $(foreach dir, $(INC_DIRS), -I$(dir))

PATH_OUTPUT = Output
LD_FILE     = Linker/linker.ld
CHIP        = cortex-m3

# Cờ biên dịch
CFLAGS  = -mcpu=$(CHIP) -mthumb -std=gnu11 -O0 -Wall $(INC_DIR_OPT)
LDFLAGS = -mcpu=$(CHIP) -mthumb -nostdlib -T $(LD_FILE) -Wl,-Map=$(PATH_OUTPUT)/$(TARGET).map

# Rule mặc định
all: $(PATH_OUTPUT)/$(TARGET).bin

# Rule biên dịch file .c -> Output/%.o
$(PATH_OUTPUT)/%.o: %.c | $(PATH_OUTPUT)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule biên dịch file .s -> Output/%.o
$(PATH_OUTPUT)/%.o: %.s | $(PATH_OUTPUT)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule link các file .o -> .elf
$(PATH_OUTPUT)/$(TARGET).elf: $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $^

# Rule chuyển .elf -> .bin
$(PATH_OUTPUT)/$(TARGET).bin: $(PATH_OUTPUT)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Tạo thư mục Output nếu chưa có
$(PATH_OUTPUT):
	mkdir -p $(PATH_OUTPUT)

flash: $(PATH_OUTPUT)/$(TARGET).bin
	$(STFLASH) write $(PATH_OUTPUT)/$(TARGET).bin 0x08000000

erase:
	$(STFLASH) erase

clean:
	rm -rf $(PATH_OUTPUT)/*

.PHONY: all clean flash erase
