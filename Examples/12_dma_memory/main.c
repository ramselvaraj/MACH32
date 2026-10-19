#include "mach32.h"
#include "nucleo_f446re.h"

int main(void)
{
    static const uint32_t source[4] = {
        0x12345678U, 0xA5A5A5A5U, 0x00000000U, 0xFFFFFFFFU,
    };
    static volatile uint32_t destination[4];
    mach32_dma_init_t dma = {
        .controller = DMA2,
        .stream = DMA2_Stream0,
        .stream_index = 0U,
        .channel = 0U,
        .direction = MACH32_DMA_MEMORY_TO_MEMORY,
        .peripheral_size = MACH32_DMA_SIZE_WORD,
        .memory_size = MACH32_DMA_SIZE_WORD,
        .priority = MACH32_DMA_PRIORITY_HIGH,
        .peripheral_increment = 1U,
        .memory_increment = 1U,
        .circular = 0U,
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
    mach32_dma_init(&dma);
    mach32_dma_start(DMA2_Stream0, (uintptr_t)source,
                     (uintptr_t)destination, 4U);

    while (mach32_dma_is_transfer_complete(DMA2, 0U) == 0U) {
        __NOP();
    }

    mach32_dma_clear_transfer_complete(DMA2, 0U);

    for (size_t index = 0U; index < 4U; ++index) {
        if (destination[index] != source[index]) {
            passed = 0U;
        }
    }

    mach32_gpio_write_pin(MACH32_LD2_PORT, MACH32_LD2_PIN, passed);

    while (1) {
        __NOP();
    }
}
