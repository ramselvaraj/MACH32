# MACH32 — baremetal build for the STM32F446RE
#
# Build a single example at a time:
#   make                  -> builds Examples/01_led_blink (the default)
#   make EXAMPLE=02_xxx   -> builds Examples/02_xxx
#   make flash            -> writes the .bin to flash (needs stlink + st-flash)
#   make clean
#
# Output goes to build/<example>/firmware.{elf,bin,hex,map}

# ---- Toolchain ----
PREFIX  = arm-none-eabi-
CC      = $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy
SIZE    = $(PREFIX)size

# ---- Which example to build ----
EXAMPLE ?= 01_led_blink

# ---- Paths ----
CMSIS   = Drivers/cmsis
INC     = Drivers/inc
BOARD   = Board
STARTUP = startup
LINKER  = linker
EXDIR   = Examples/$(EXAMPLE)

BUILD   = build/$(EXAMPLE)
TARGET  = $(BUILD)/firmware

# ---- CPU: Cortex-M4 with single-precision FPU ----
CPU      = -mcpu=cortex-m4
FPU      = -mfpu=fpv4-sp-d16 -mfloat-abi=hard
MCUFLAGS = $(CPU) -mthumb $(FPU)

# ---- Source files ----
C_SRC   = $(wildcard Drivers/src/*.c) \
          $(wildcard $(BOARD)/*.c) \
          $(CMSIS)/system_stm32f4xx.c \
          $(wildcard $(EXDIR)/*.c)
ASM_SRC = $(STARTUP)/startup_stm32f446xx.s

# Preserve relative paths under BUILD/ so object files can't collide.
OBJ     = $(patsubst %.c,$(BUILD)/%.o,$(C_SRC)) \
          $(patsubst %.s,$(BUILD)/%.o,$(ASM_SRC))

# ---- Flags ----
DEFS    = -DSTM32F446xx
CFLAGS  = $(MCUFLAGS) $(DEFS) -Wall -Wextra -g3 -O0 \
          -I$(CMSIS) -I$(INC) -I$(BOARD)
ASFLAGS = $(MCUFLAGS)
LDFLAGS = $(MCUFLAGS) -T$(LINKER)/stm32f446re.ld \
          --specs=nano.specs --specs=nosys.specs \
          -Wl,-Map=$(TARGET).map,--cref

# ---- Rules ----
.PHONY: all clean flash

all: $(TARGET).elf
	$(SIZE) $(TARGET).elf

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(OBJ) $(LDFLAGS) -o $@
	$(OBJCOPY) -O ihex    $(TARGET).elf $(TARGET).hex
	$(OBJCOPY) -O binary  $(TARGET).elf $(TARGET).bin

flash: $(TARGET).bin
	st-flash write $(TARGET).bin 0x08000000

clean:
	rm -rf build
