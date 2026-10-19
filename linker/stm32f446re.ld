/*
 * MACH32 — linker script for the STM32F446RE
 *
 * STM32F446RE memory map (RM0390 §2):
 *   - Flash: selected by MACH32_FLASH_ORIGIN and MACH32_FLASH_LENGTH
 *   - SRAM : 128 KB @ 0x20000000
 *
 * The startup code (startup_stm32f446xx.s) depends on these exported symbols:
 *   _estack, _sidata, _sdata, _edata, _sbss, _ebss
 */

ENTRY(Reset_Handler)

__flash_origin = DEFINED(MACH32_FLASH_ORIGIN)
               ? MACH32_FLASH_ORIGIN : 0x08000000;
__flash_length = DEFINED(MACH32_FLASH_LENGTH)
               ? MACH32_FLASH_LENGTH : 512K;

_estack = ORIGIN(RAM) + LENGTH(RAM);   /* top of stack: SRAM grows downward from here */

MEMORY
{
    FLASH (rx)  : ORIGIN = __flash_origin, LENGTH = __flash_length
    RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS
{
    /* 1. Interrupt vector table — must be the very first thing in flash. */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >FLASH

    /* 2. Code + read-only data. _etext marks end of code in flash. */
    .text :
    {
        . = ALIGN(4);
        *(.text)           /* function code */
        *(.text*)
        *(.rodata)         /* string literals, const globals */
        *(.rodata*)
        *(.glue_7)         /* arm/thumb interworking stubs */
        *(.glue_7t)

        /* Tables used by __libc_init_array for static constructors. */
        . = ALIGN(4);
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);

        . = ALIGN(4);
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);

        . = ALIGN(4);
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);

        . = ALIGN(4);
        _etext = .;
    } >FLASH

    /* 3. Initialised data: lives in RAM at runtime, but its *initial values*
     *    are stored in flash. The startup code copies from _sidata -> _sdata. */
    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT> FLASH

    /* 4. Zero-initialised data: only needs to exist in RAM. Startup zeroes it. */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM
}
