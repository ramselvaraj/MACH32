#include "mach32_flash.h"

#include "stm32f4xx.h"

#define MACH32_FLASH_KEY1       0x45670123UL
#define MACH32_FLASH_KEY2       0xCDEF89ABUL
#define MACH32_FLASH_START      0x08000000UL
#define MACH32_FLASH_END        0x08080000UL

#define MACH32_FLASH_ERROR_FLAGS (FLASH_SR_OPERR  | FLASH_SR_WRPERR \
                                | FLASH_SR_PGAERR | FLASH_SR_PGPERR \
                                | FLASH_SR_PGSERR | FLASH_SR_RDERR)

static void flash_wait_until_ready(void)
{
    while ((FLASH->SR & FLASH_SR_BSY) != 0U) {
        __NOP();
    }
}

static void flash_clear_status_flags(void)
{
    FLASH->SR = MACH32_FLASH_ERROR_FLAGS | FLASH_SR_EOP;
}

mach32_flash_status_t mach32_flash_unlock(void)
{
    if ((FLASH->CR & FLASH_CR_LOCK) != 0U) {
        FLASH->KEYR = MACH32_FLASH_KEY1;
        FLASH->KEYR = MACH32_FLASH_KEY2;
    }

    return ((FLASH->CR & FLASH_CR_LOCK) == 0U)
        ? MACH32_FLASH_OK
        : MACH32_FLASH_ERROR;
}

void mach32_flash_lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

mach32_flash_status_t mach32_flash_erase_sector(uint8_t sector)
{
    if (sector > 7U) {
        return MACH32_FLASH_INVALID_ARGUMENT;
    }

    if (mach32_flash_unlock() != MACH32_FLASH_OK) {
        return MACH32_FLASH_ERROR;
    }

    flash_wait_until_ready();
    flash_clear_status_flags();

    FLASH->CR &= ~(FLASH_CR_PG | FLASH_CR_SER
                 | FLASH_CR_SNB | FLASH_CR_PSIZE);
    FLASH->CR |= FLASH_CR_SER
               | ((uint32_t)sector << FLASH_CR_SNB_Pos)
               | FLASH_CR_PSIZE_1;
    FLASH->CR |= FLASH_CR_STRT;

    flash_wait_until_ready();
    FLASH->CR &= ~(FLASH_CR_SER | FLASH_CR_SNB);

    return ((FLASH->SR & MACH32_FLASH_ERROR_FLAGS) == 0U)
        ? MACH32_FLASH_OK
        : MACH32_FLASH_ERROR;
}

mach32_flash_status_t mach32_flash_program_word(uint32_t address, uint32_t data)
{
    if ((address < MACH32_FLASH_START) || (address > (MACH32_FLASH_END - 4U))
        || ((address & 3U) != 0U)) {
        return MACH32_FLASH_INVALID_ARGUMENT;
    }

    if (mach32_flash_unlock() != MACH32_FLASH_OK) {
        return MACH32_FLASH_ERROR;
    }

    flash_wait_until_ready();
    flash_clear_status_flags();

    FLASH->CR &= ~(FLASH_CR_SER | FLASH_CR_PSIZE);
    FLASH->CR |= FLASH_CR_PG | FLASH_CR_PSIZE_1;

    *(volatile uint32_t *)address = data;

    flash_wait_until_ready();
    FLASH->CR &= ~FLASH_CR_PG;

    if ((FLASH->SR & MACH32_FLASH_ERROR_FLAGS) != 0U) {
        return MACH32_FLASH_ERROR;
    }

    return (*(volatile uint32_t *)address == data)
        ? MACH32_FLASH_OK
        : MACH32_FLASH_ERROR;
}
