#include "mach32.h"
#include "nucleo_f446re.h"

static void write_text(const char *text)
{
    const char *end = text;

    while (*end != '\0') {
        ++end;
    }

    mach32_usart_write(USART2, (const uint8_t *)text, (size_t)(end - text));
}

static void write_hex_byte(uint8_t value)
{
    static const uint8_t digits[] = "0123456789ABCDEF";
    uint8_t output[2] = {digits[value >> 4U], digits[value & 0x0FU]};

    mach32_usart_write(USART2, output, sizeof(output));
}

static void pins_init(void)
{
    mach32_gpio_init_t tx = {
        .port = MACH32_GPIOA,
        .pin = 2U,
        .mode = MACH32_GPIO_MODE_ALTFN,
        .otype = MACH32_GPIO_OTYPE_PUSHPULL,
        .speed = MACH32_GPIO_SPEED_HIGH,
        .pupd = MACH32_GPIO_PUPD_UP,
        .altfn = 7U,
    };
    mach32_gpio_init_t rx = tx;
    mach32_gpio_init_t i2c_pin = {
        .port = MACH32_GPIOB,
        .pin = 8U,
        .mode = MACH32_GPIO_MODE_ALTFN,
        .otype = MACH32_GPIO_OTYPE_OPENDRAIN,
        .speed = MACH32_GPIO_SPEED_HIGH,
        .pupd = MACH32_GPIO_PUPD_UP,
        .altfn = 4U,
    };

    rx.pin = 3U;
    mach32_gpio_init(&tx);
    mach32_gpio_init(&rx);
    mach32_gpio_init(&i2c_pin);
    i2c_pin.pin = 9U;
    mach32_gpio_init(&i2c_pin);
}

int main(void)
{
    mach32_usart_init_t usart = {
        .instance = USART2,
        .peripheral_clock_hz = 16000000U,
        .baud_rate = 115200U,
        .mode = MACH32_USART_MODE_TX,
        .parity = MACH32_USART_PARITY_NONE,
        .stop_bits = MACH32_USART_STOP_BITS_1,
    };
    mach32_i2c_init_t i2c = {
        .instance = I2C1,
        .peripheral_clock_hz = 16000000U,
        .bus_clock_hz = 100000U,
        .own_address = 0U,
    };

    mach32_board_init();
    pins_init();
    mach32_usart_init(&usart);
    mach32_i2c_init(&i2c);
    write_text("MACH32 I2C scan on PB8/PB9\r\n");

    for (uint8_t address = 0x08U; address <= 0x77U; ++address) {
        if (mach32_i2c_probe(I2C1, address) != 0U) {
            write_text("Found device at 0x");
            write_hex_byte(address);
            write_text("\r\n");
        }
    }

    write_text("Scan complete.\r\n");

    while (1) {
        __NOP();
    }
}
