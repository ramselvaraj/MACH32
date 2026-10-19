#include "mach32.h"
#include "nucleo_f446re.h"

static void write_value(uint16_t value)
{
    uint8_t output[5];
    size_t length = 0U;

    do {
        output[length++] = (uint8_t)('0' + (value % 10U));
        value /= 10U;
    } while (value != 0U);

    while (length != 0U) {
        mach32_usart_write_byte(USART2, output[--length]);
    }

    mach32_usart_write(USART2, (const uint8_t *)"\r\n", 2U);
}

int main(void)
{
    mach32_gpio_init_t analog_pin = {
        .port = MACH32_GPIOA,
        .pin = 0U,
        .mode = MACH32_GPIO_MODE_ANALOG,
        .otype = MACH32_GPIO_OTYPE_PUSHPULL,
        .speed = MACH32_GPIO_SPEED_LOW,
        .pupd = MACH32_GPIO_PUPD_NONE,
    };
    mach32_gpio_init_t serial_pin = {
        .port = MACH32_GPIOA,
        .pin = 2U,
        .mode = MACH32_GPIO_MODE_ALTFN,
        .otype = MACH32_GPIO_OTYPE_PUSHPULL,
        .speed = MACH32_GPIO_SPEED_HIGH,
        .pupd = MACH32_GPIO_PUPD_UP,
        .altfn = 7U,
    };
    mach32_usart_init_t usart = {
        .instance = USART2,
        .peripheral_clock_hz = 16000000U,
        .baud_rate = 115200U,
        .mode = MACH32_USART_MODE_TX,
        .parity = MACH32_USART_PARITY_NONE,
        .stop_bits = MACH32_USART_STOP_BITS_1,
    };
    mach32_adc_init_t adc = {
        .instance = ADC1,
        .channel = 0U,
        .resolution = MACH32_ADC_RESOLUTION_12_BIT,
        .sample_time = MACH32_ADC_SAMPLE_84_CYCLES,
        .continuous = 0U,
    };

    mach32_board_init();
    mach32_gpio_init(&analog_pin);
    mach32_gpio_init(&serial_pin);
    mach32_usart_init(&usart);
    mach32_adc_init(&adc);
    mach32_systick_init(16000000U);

    while (1) {
        mach32_adc_start(ADC1);
        write_value(mach32_adc_read(ADC1));
        mach32_systick_delay_ms(500U);
    }
}
