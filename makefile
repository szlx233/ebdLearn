# ===== 工具链设置 =====
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-gcc

# ===== 编译参数 =====
MCU_FLAGS = -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-d16
CFLAGS = $(MCU_FLAGS) \
		 -g -O0 \
		 -Wall -ffunction-sections -fdata-sections \
		 -DSTM32H750xx \
		 -ICore \
		 -IInc \
		 -IDrivers/CMSIS/Core/Include \
		 -IDrivers/CMSIS/Device/ST/STM32H7xx/Include

ASFLAGS = $(MCU_FLAGS)

# ===== 源文件列表 =====
SRC_C = Src/main.c \
		Core/system_stm32h7xx.c

SRC_AS = Core/startup_stm32h750xx.s

# ===== 生成的目标文件 =====
OBJ_C = $(SRC_C:.c=.o)
OBJ_AS = $(SRC_AS:.s=.o)

# ===== 链接脚本 =====
LDSCRIPT = stm32h757xx_flash_CM7.ld

# ===== 最终输出文件 =====
TARGET_ELF = main.elf
TARGET_BIN = main.bin
TARGET_HEX = main.hex

# ===== 默认目标 =====
all: $(TARGET_BIN)

# ===== elf -> bin =====
$(TARGET_BIN): $(TARGET_ELF)
	arm-none-eabi-objcopy -O binary $< $@

# ===== elf -> hex =====
$(TARGET_HEX): $(TARGET_ELF)
	arm-none-eabi-objcopy -O ihex $< $@

# ===== 编译生成 elf =====
$(TARGET_ELF): $(OBJ_C) $(OBJ_AS)
	$(LD) $(MCU_FLAGS) -o $@ $^ -Wl,--gc-sections -T $(LDSCRIPT)

# ===== C文件编译规则 =====
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ===== 汇编文件编译规则 =====
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# ===== 清理 =====
clean:
	rm -f $(OBJ_C) $(OBJ_AS) $(TARGET_ELF) $(TARGET_BIN) $(TARGET_HEX)

.PHONY: all clean
