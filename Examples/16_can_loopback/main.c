#include "mach32.h"
#include "nucleo_f446re.h"

int main(void)
{
    mach32_gpio_init_t led = {
        .port = MACH32_LD2_PORT,
        .pin = MACH32_LD2_PIN,
        .mode = MACH32_GPIO_MODE_OUTPUT,
        .otype = MACH32_GPIO_OTYPE_PUSHPULL,
        .speed = MACH32_GPIO_SPEED_LOW,
        .pupd = MACH32_GPIO_PUPD_NONE,
    };
    mach32_can_init_t can = {
        .instance = CAN1,
        .prescaler = 2U,
        .time_segment_1 = 12U,
        .time_segment_2 = 3U,
        .sync_jump_width = 1U,
        .loopback = 1U,
        .silent = 1U,
    };
    mach32_can_frame_t sent = {
        .id = 0x123U,
        .data = {0x4DU, 0x41U, 0x43U, 0x48U, 0x33U, 0x32U},
        .length = 6U,
        .extended = 0U,
        .remote = 0U,
    };
    mach32_can_frame_t received = {0};
    uint8_t passed = 0U;
    mach32_gpio_init_t can_rx = {
        .port = MACH32_GPIOA,
        .pin = 11U,
        .mode = MACH32_GPIO_MODE_ALTFN,
        .otype = MACH32_GPIO_OTYPE_PUSHPULL,
        .speed = MACH32_GPIO_SPEED_HIGH,
        .pupd = MACH32_GPIO_PUPD_UP,
        .altfn = 9U,
    };
    mach32_gpio_init_t can_tx = can_rx;

    can_tx.pin = 12U;

    mach32_board_init();
    mach32_gpio_init(&led);
    mach32_gpio_init(&can_rx);
    mach32_gpio_init(&can_tx);
    mach32_can_init(&can);

    if (mach32_can_transmit(CAN1, &sent) != 0U) {
        for (uint32_t wait = 0U; wait < 1000000U; ++wait) {
            if (mach32_can_receive(CAN1, &received) != 0U) {
                passed = 1U;
                break;
            }
        }
    }

    if ((received.id != sent.id) || (received.length != sent.length)) {
        passed = 0U;
    }

    for (uint8_t index = 0U; index < sent.length; ++index) {
        if (received.data[index] != sent.data[index]) {
            passed = 0U;
        }
    }

    mach32_gpio_write_pin(MACH32_LD2_PORT, MACH32_LD2_PIN, passed);

    while (1) {
        __NOP();
    }
}
