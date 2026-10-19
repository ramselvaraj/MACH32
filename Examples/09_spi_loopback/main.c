#include "mach32.h"
#include "nucleo_f446re.h"

static void spi1_pins_init(void)
{
    mach32_gpio_init_t pin = {
        .port = MACH32_GPIOA,
        .pin = 5U,
        .mode = MACH32_GPIO_MODE_ALTFN,
        .otype = MACH32_GPIO_OTYPE_PUSHPULL,
        .speed = MACH32_GPIO_SPEED_HIGH,
        .pupd = MACH32_GPIO_PUPD_NONE,
        .altfn = 5U,
    };

    mach32_gpio_init(&pin);
    pin.pin = 6U;
    mach32_gpio_init(&pin);
    pin.pin = 7U;
    mach32_gpio_init(&pin);
}

int main(void)
{
    static const uint8_t sent[] = {0x12U, 0x34U, 0xA5U, 0x5AU};
    mach32_spi_init_t spi = {
        .instance = SPI1,
        .role = MACH32_SPI_ROLE_MASTER,
        .mode = MACH32_SPI_MODE_0,
        .baud_div = MACH32_SPI_DIV_32,
        .bit_order = MACH32_SPI_MSB_FIRST,
    };
    uint8_t passed = 1U;

    mach32_board_init();
    spi1_pins_init();
    mach32_spi_init(&spi);

    for (size_t index = 0U; index < sizeof(sent); ++index) {
        if (mach32_spi_transfer_byte(SPI1, sent[index]) != sent[index]) {
            passed = 0U;
        }
    }

    mach32_gpio_init_t led = {
        .port = MACH32_LD2_PORT,
        .pin = MACH32_LD2_PIN,
        .mode = MACH32_GPIO_MODE_OUTPUT,
        .otype = MACH32_GPIO_OTYPE_PUSHPULL,
        .speed = MACH32_GPIO_SPEED_LOW,
        .pupd = MACH32_GPIO_PUPD_NONE,
    };

    mach32_gpio_init(&led);
    mach32_gpio_write_pin(MACH32_LD2_PORT, MACH32_LD2_PIN, passed);

    while (1) {
        __NOP();
    }
}
