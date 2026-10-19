/* MACH32 internal Flash erase and programming driver. */

#ifndef MACH32_FLASH_H
#define MACH32_FLASH_H

#include <stdint.h>

typedef enum {
    MACH32_FLASH_OK = 0,
    MACH32_FLASH_ERROR,
    MACH32_FLASH_INVALID_ARGUMENT,
} mach32_flash_status_t;

mach32_flash_status_t mach32_flash_unlock(void);
void mach32_flash_lock(void);
mach32_flash_status_t mach32_flash_erase_sector(uint8_t sector);
mach32_flash_status_t mach32_flash_program_word(uint32_t address, uint32_t data);

#endif /* MACH32_FLASH_H */
