#include "mach32.h"
#include "nucleo_f446re.h"

#define MACH32_SLOT_A_START 0x08010000UL
#define MACH32_SLOT_A_END   0x08040000UL
#define MACH32_SLOT_B_START 0x08040000UL
#define MACH32_SLOT_B_END   0x08070000UL
#define MACH32_RAM_START    0x20000000UL
#define MACH32_RAM_END      0x20020000UL
#define MACH32_BOOT_WAIT_MS 3000UL

static void write_text(const char *text)
{
    const char *end = text;

    while (*end != '\0') {
        ++end;
    }

    mach32_usart_write(USART2, (const uint8_t *)text, (size_t)(end - text));
}

static uint8_t text_equals(const char *left, const char *right)
{
    while ((*left != '\0') && (*right != '\0')) {
        if (*left != *right) {
            return 0U;
        }

        ++left;
        ++right;
    }

    return (*left == '\0') && (*right == '\0');
}

static void usart2_init(void)
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
    mach32_usart_init_t usart = {
        .instance = USART2,
        .peripheral_clock_hz = 16000000U,
        .baud_rate = 115200U,
        .mode = MACH32_USART_MODE_TXRX,
        .parity = MACH32_USART_PARITY_NONE,
        .stop_bits = MACH32_USART_STOP_BITS_1,
    };

    rx.port = MACH32_USART2_RX_PORT;
    rx.pin = MACH32_USART2_RX_PIN;

    mach32_gpio_init(&tx);
    mach32_gpio_init(&rx);
    mach32_usart_init(&usart);
}

static uint8_t application_is_valid(uint32_t start, uint32_t end)
{
    uint32_t initial_stack = *(const uint32_t *)start;
    uint32_t reset_handler = *(const uint32_t *)(start + 4U);
    uint32_t reset_address = reset_handler & ~1UL;

    if ((initial_stack < MACH32_RAM_START) || (initial_stack > MACH32_RAM_END)
        || ((initial_stack & 3U) != 0U)) {
        return 0U;
    }

    if (((reset_handler & 1U) == 0U) || (reset_address < start)
        || (reset_address >= end)) {
        return 0U;
    }

    return 1U;
}

static void show_status(void)
{
    write_text("Slot A: ");
    write_text(application_is_valid(MACH32_SLOT_A_START,
                                    MACH32_SLOT_A_END) != 0U
                   ? "valid\r\n" : "empty or invalid\r\n");

    write_text("Slot B: ");
    write_text(application_is_valid(MACH32_SLOT_B_START,
                                    MACH32_SLOT_B_END) != 0U
                   ? "valid\r\n" : "empty or invalid\r\n");
}

__attribute__((noreturn))
static void boot_application(uint32_t start)
{
    uint32_t initial_stack = *(const uint32_t *)start;
    uint32_t reset_handler = *(const uint32_t *)(start + 4U);

    __disable_irq();
    SysTick->CTRL = 0U;
    SysTick->LOAD = 0U;
    SysTick->VAL = 0U;

    for (uint32_t index = 0U; index < 8U; ++index) {
        NVIC->ICER[index] = 0xFFFFFFFFUL;
        NVIC->ICPR[index] = 0xFFFFFFFFUL;
    }

    SCB->VTOR = start;
    __DSB();
    __ISB();

    __asm volatile(
        "msr msp, %0\n"
        "dsb\n"
        "isb\n"
        "bx %1\n"
        :
        : "r"(initial_stack), "r"(reset_handler)
        : "memory");

    __builtin_unreachable();
}

static void try_boot(char slot)
{
    uint32_t start = (slot == 'A') ? MACH32_SLOT_A_START : MACH32_SLOT_B_START;
    uint32_t end = (slot == 'A') ? MACH32_SLOT_A_END : MACH32_SLOT_B_END;

    if (application_is_valid(start, end) == 0U) {
        write_text("The selected slot is empty or invalid.\r\n> ");
        return;
    }

    write_text((slot == 'A') ? "Starting slot A.\r\n"
                             : "Starting slot B.\r\n");
    boot_application(start);
}

static void process_command(char *command)
{
    if (text_equals(command, "status") != 0U) {
        show_status();
        write_text("> ");
    } else if (text_equals(command, "boot A") != 0U) {
        try_boot('A');
    } else if (text_equals(command, "boot B") != 0U) {
        try_boot('B');
    } else {
        write_text("Commands: status, boot A, boot B\r\n> ");
    }
}

int main(void)
{
    char command[16] = {0};
    size_t command_length = 0U;
    uint32_t wait_start;
    uint8_t timeout_enabled = 1U;

    mach32_board_init();
    usart2_init();
    mach32_systick_init(16000000U);

    write_text("\r\nMACH32 bootloader\r\n");
    show_status();
    write_text("Type a command within three seconds.\r\n> ");
    wait_start = mach32_systick_millis();

    while (1) {
        if ((USART2->SR & USART_SR_RXNE) != 0U) {
            char byte = (char)(USART2->DR & 0xFFU);

            if ((byte == '\r') || (byte == '\n')) {
                if (command_length != 0U) {
                    command[command_length] = '\0';
                    write_text("\r\n");
                    process_command(command);
                    command_length = 0U;
                    wait_start = mach32_systick_millis();
                }
            } else if (command_length < (sizeof(command) - 1U)) {
                command[command_length++] = byte;
                mach32_usart_write_byte(USART2, (uint8_t)byte);
            }
        }

        if ((timeout_enabled != 0U)
            && ((uint32_t)(mach32_systick_millis() - wait_start)
                >= MACH32_BOOT_WAIT_MS)) {
            if (application_is_valid(MACH32_SLOT_A_START,
                                     MACH32_SLOT_A_END) != 0U) {
                write_text("\r\nNo command. Starting slot A.\r\n");
                boot_application(MACH32_SLOT_A_START);
            }

            if (application_is_valid(MACH32_SLOT_B_START,
                                     MACH32_SLOT_B_END) != 0U) {
                write_text("\r\nSlot A is invalid. Starting slot B.\r\n");
                boot_application(MACH32_SLOT_B_START);
            }

            write_text("\r\nNo valid application. Waiting for commands.\r\n> ");
            timeout_enabled = 0U;
        }
    }
}
