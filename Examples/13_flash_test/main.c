#include "mach32.h"
#include "nucleo_f446re.h"

#define TEST_ADDRESS 0x08060000UL

int main(void)
{
    static const uint32_t expected[4] = {
        0x4D414348U, 0x33322046U, 0x4C415348U, 0x54455354U,
    };
    mach32_gpio_init_t led = {
        .port = MACH32_LD2_PORT,
        .pin = MACH32_LD2_PIN,
        .mode = MACH32_GPIO_MODE_OUTPUT,
        .otype = MACH32_GPIO_OTYPE_PUSHPULL,
        .speed = MACH32_GPIO_SPEED_LOW,
        .pupd = MACH32_GPIO_PUPD_NONE,
    };
    uint8_t passed = 1U;

    mach32_board_init();
    mach32_gpio_init(&led);

    if (mach32_flash_erase_sector(7U) != MACH32_FLASH_OK) {
        passed = 0U;
    }

    for (uint32_t index = 0U; (index < 4U) && (passed != 0U); ++index) {
        if (mach32_flash_program_word(TEST_ADDRESS + (index * 4U),
                                      expected[index]) != MACH32_FLASH_OK) {
            passed = 0U;
        }
    }

    mach32_flash_lock();

    for (uint32_t index = 0U; index < 4U; ++index) {
        if (*(const uint32_t *)(TEST_ADDRESS + (index * 4U)) != expected[index]) {
            passed = 0U;
        }
    }

    mach32_gpio_write_pin(MACH32_LD2_PORT, MACH32_LD2_PIN, passed);

    while (1) {
        __NOP();
    }
}
