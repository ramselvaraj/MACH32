#include "mach32.h"
#include "nucleo_f446re.h"

static void usart2_pins_init(void)
{
    mach32_gpio_init_t tx = {
        .port = MACH32_USART2_TX_PORT,
        .pin = MACH32_USART2_TX_PIN,
        .mode = MACH32_GPIO_MODE_ALTFN,
        .otype = MACH32_GPIO_OTYPE_PUSHPULL,
        .speed = MACH32_GPIO_SPEED_HIGH,
        .pupd = MACH32_GPIO_PUPD_UP,
        .altfn = 7U,
    };
    mach32_gpio_init_t rx = tx;

    rx.port = MACH32_USART2_RX_PORT;
    rx.pin = MACH32_USART2_RX_PIN;
    mach32_gpio_init(&tx);
    mach32_gpio_init(&rx);
}

int main(void)
{
    static const uint8_t greeting[] =
        "MACH32 USART2 echo ready. Type a character.\r\n";
    mach32_usart_init_t usart = {
        .instance = USART2,
        .peripheral_clock_hz = 16000000U,
        .baud_rate = 115200U,
        .mode = MACH32_USART_MODE_TXRX,
        .parity = MACH32_USART_PARITY_NONE,
        .stop_bits = MACH32_USART_STOP_BITS_1,
    };

    mach32_board_init();
    usart2_pins_init();
    mach32_usart_init(&usart);
    mach32_usart_write(USART2, greeting, sizeof(greeting) - 1U);

    while (1) {
        uint8_t byte = mach32_usart_read_byte(USART2);
        mach32_usart_write_byte(USART2, byte);
    }
}
