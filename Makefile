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
EXAMPLES = $(notdir $(patsubst %/,%,$(wildcard Examples/*/)))
PROGRAM ?= example

ifeq ($(PROGRAM),bootloader)
PROGRAM_NAME   = bootloader
PROGRAM_DIR    = Bootloader
FLASH_ORIGIN   = 0x08000000
FLASH_LENGTH   = 0x00010000
PROGRAM_DEFS   =
STLINK_CONNECT = --hot-plug
else ifeq ($(PROGRAM),app_a)
PROGRAM_NAME   = app_a
PROGRAM_DIR    = Applications/slot_a
FLASH_ORIGIN   = 0x08010000
FLASH_LENGTH   = 0x00030000
PROGRAM_DEFS   = -DUSER_VECT_TAB_ADDRESS -DVECT_TAB_OFFSET=0x00010000
STLINK_CONNECT = --hot-plug
else ifeq ($(PROGRAM),app_b)
PROGRAM_NAME   = app_b
PROGRAM_DIR    = Applications/slot_b
FLASH_ORIGIN   = 0x08040000
FLASH_LENGTH   = 0x00030000
PROGRAM_DEFS   = -DUSER_VECT_TAB_ADDRESS -DVECT_TAB_OFFSET=0x00040000
STLINK_CONNECT = --hot-plug
else
PROGRAM_NAME   = $(EXAMPLE)
PROGRAM_DIR    = Examples/$(EXAMPLE)
FLASH_ORIGIN   = 0x08000000
FLASH_LENGTH   = 0x00080000
PROGRAM_DEFS   =
STLINK_CONNECT = --connect-under-reset
endif

# ---- Paths ----
CMSIS   = Drivers/cmsis
INC     = Drivers/inc
BOARD   = Board
STARTUP = startup
LINKER  = linker

BUILD   = build/$(PROGRAM_NAME)
TARGET  = $(BUILD)/firmware

# ---- CPU: Cortex-M4 with single-precision FPU ----
CPU      = -mcpu=cortex-m4
FPU      = -mfpu=fpv4-sp-d16 -mfloat-abi=hard
MCUFLAGS = $(CPU) -mthumb $(FPU)

# ---- Source files ----
C_SRC   = $(wildcard Drivers/src/*.c) \
          $(wildcard $(BOARD)/*.c) \
          $(CMSIS)/system_stm32f4xx.c \
          $(wildcard $(PROGRAM_DIR)/*.c)
ASM_SRC = $(STARTUP)/startup_stm32f446xx.s

# Preserve relative paths under BUILD/ so object files can't collide.
OBJ     = $(patsubst %.c,$(BUILD)/%.o,$(C_SRC)) \
          $(patsubst %.s,$(BUILD)/%.o,$(ASM_SRC))

# ---- Flags ----
DEFS    = -DSTM32F446xx $(PROGRAM_DEFS)
CFLAGS  = $(MCUFLAGS) $(DEFS) -Wall -Wextra -g3 -O0 \
          -I$(CMSIS) -I$(INC) -I$(BOARD)
ASFLAGS = $(MCUFLAGS)
LDFLAGS = $(MCUFLAGS) -T$(LINKER)/stm32f446re.ld \
          -nostartfiles \
          --specs=nano.specs --specs=nosys.specs \
          -Wl,--defsym=MACH32_FLASH_ORIGIN=$(FLASH_ORIGIN) \
          -Wl,--defsym=MACH32_FLASH_LENGTH=$(FLASH_LENGTH) \
          -Wl,-Map=$(TARGET).map,--cref

# ---- Rules ----
.PHONY: all clean flash examples bootloader app-a app-b boot-demo flash-demo

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

flash: $(TARGET).elf
	st-flash $(STLINK_CONNECT) --reset write $(TARGET).bin $(FLASH_ORIGIN)

examples:
	@for example in $(EXAMPLES); do \
		$(MAKE) --no-print-directory EXAMPLE=$$example || exit $$?; \
	done

bootloader:
	$(MAKE) --no-print-directory PROGRAM=bootloader all

app-a:
	$(MAKE) --no-print-directory PROGRAM=app_a all

app-b:
	$(MAKE) --no-print-directory PROGRAM=app_b all

boot-demo: bootloader app-a app-b

flash-demo: boot-demo
	$(MAKE) --no-print-directory PROGRAM=app_a flash
	$(MAKE) --no-print-directory PROGRAM=app_b flash
	$(MAKE) --no-print-directory PROGRAM=bootloader flash

clean:
	rm -rf build
