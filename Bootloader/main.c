#include "mach32.h"
#include "nucleo_f446re.h"

#define MACH32_SLOT_A_START 0x08010000UL
#define MACH32_SLOT_A_END   0x08040000UL
#define MACH32_SLOT_B_START 0x08040000UL
#define MACH32_SLOT_B_END   0x08070000UL
#define MACH32_RAM_START    0x20000000UL
#define MACH32_RAM_END      0x20020000UL
#define MACH32_BOOT_WAIT_MS 10000UL
#define MACH32_RX_TIMEOUT_MS 2000UL

typedef struct {
    uint32_t start;
    uint32_t end;
    uint8_t first_sector;
    uint8_t last_sector;
    char name;
} mach32_slot_t;

static const mach32_slot_t slot_a = {
    MACH32_SLOT_A_START, MACH32_SLOT_A_END, 4U, 5U, 'A'
};

static const mach32_slot_t slot_b = {
    MACH32_SLOT_B_START, MACH32_SLOT_B_END, 6U, 7U, 'B'
};

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

static const mach32_slot_t *find_slot(char name)
{
    if (name == 'A') {
        return &slot_a;
    }

    if (name == 'B') {
        return &slot_b;
    }

    return 0;
}

static uint8_t parse_decimal(const char **text, uint32_t *value)
{
    uint32_t result = 0U;
    uint8_t digit_count = 0U;

    while ((**text >= '0') && (**text <= '9')) {
        uint32_t digit = (uint32_t)(**text - '0');

        if (result > ((0xFFFFFFFFUL - digit) / 10U)) {
            return 0U;
        }

        result = (result * 10U) + digit;
        ++(*text);
        ++digit_count;
    }

    *value = result;
    return digit_count != 0U;
}

static uint8_t parse_hex32(const char *text, uint32_t *value)
{
    uint32_t result = 0U;

    for (uint8_t index = 0U; index < 8U; ++index) {
        uint8_t digit;
        char byte = text[index];

        if ((byte >= '0') && (byte <= '9')) {
            digit = (uint8_t)(byte - '0');
        } else if ((byte >= 'A') && (byte <= 'F')) {
            digit = (uint8_t)(byte - 'A' + 10);
        } else if ((byte >= 'a') && (byte <= 'f')) {
            digit = (uint8_t)(byte - 'a' + 10);
        } else {
            return 0U;
        }

        result = (result << 4U) | digit;
    }

    if (text[8] != '\0') {
        return 0U;
    }

    *value = result;
    return 1U;
}

static uint8_t parse_upload_command(const char *command,
                                    const mach32_slot_t **slot,
                                    uint32_t *size,
                                    uint32_t *crc)
{
    const char *text = command;
    const char prefix[] = "upload ";

    for (size_t index = 0U; index < (sizeof(prefix) - 1U); ++index) {
        if (text[index] != prefix[index]) {
            return 0U;
        }
    }

    text += sizeof(prefix) - 1U;
    *slot = find_slot(*text++);

    if ((*slot == 0) || (*text++ != ' ')
        || (parse_decimal(&text, size) == 0U) || (*text++ != ' ')
        || (parse_hex32(text, crc) == 0U)) {
        return 0U;
    }

    return 1U;
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

static uint8_t read_byte_with_timeout(uint8_t *data)
{
    uint32_t start = mach32_systick_millis();

    while ((USART2->SR & USART_SR_RXNE) == 0U) {
        if ((uint32_t)(mach32_systick_millis() - start)
            >= MACH32_RX_TIMEOUT_MS) {
            return 0U;
        }

        __NOP();
    }

    *data = (uint8_t)(USART2->DR & 0xFFU);
    return 1U;
}

static uint8_t erase_slot(const mach32_slot_t *slot)
{
    for (uint8_t sector = slot->first_sector;
         sector <= slot->last_sector;
         ++sector) {
        if (mach32_flash_erase_sector(sector) != MACH32_FLASH_OK) {
            return 0U;
        }
    }

    return 1U;
}

static uint8_t receive_image(const mach32_slot_t *slot, uint32_t size)
{
    uint32_t word = 0xFFFFFFFFUL;
    uint32_t vector_words[2] = {0xFFFFFFFFUL, 0xFFFFFFFFUL};

    for (uint32_t offset = 0U; offset < size; ++offset) {
        uint8_t byte;
        uint32_t shift = (offset & 3U) * 8U;

        if (read_byte_with_timeout(&byte) == 0U) {
            return 0U;
        }

        word &= ~(0xFFUL << shift);
        word |= (uint32_t)byte << shift;

        if (((offset & 3U) == 3U) || (offset == (size - 1U))) {
            uint32_t address = slot->start + (offset & ~3UL);

            if (address < (slot->start + 8U)) {
                vector_words[(address - slot->start) / 4U] = word;
            } else if (mach32_flash_program_word(address, word)
                       != MACH32_FLASH_OK) {
                return 0U;
            }

            word = 0xFFFFFFFFUL;
        }
    }

    /* Program the vector table last. An interrupted upload then remains
     * invalid because its initial stack word is still erased. */
    if (mach32_flash_program_word(slot->start + 4U, vector_words[1])
        != MACH32_FLASH_OK) {
        return 0U;
    }

    return mach32_flash_program_word(slot->start, vector_words[0])
        == MACH32_FLASH_OK;
}

static void invalidate_slot(const mach32_slot_t *slot)
{
    (void)mach32_flash_program_word(slot->start, 0U);
    mach32_flash_lock();
}

static void upload_image(const mach32_slot_t *slot,
                         uint32_t size,
                         uint32_t expected_crc)
{
    uint32_t capacity = slot->end - slot->start;

    if ((size < 8U) || (size > capacity)) {
        write_text("ERROR size\r\n> ");
        return;
    }

    write_text((slot->name == 'A') ? "Erasing slot A...\r\n"
                                   : "Erasing slot B...\r\n");

    if (erase_slot(slot) == 0U) {
        mach32_flash_lock();
        write_text("ERROR erase\r\n> ");
        return;
    }

    write_text("READY\r\n");

    if (receive_image(slot, size) == 0U) {
        mach32_flash_lock();
        write_text("ERROR receive or program\r\n> ");
        return;
    }

    mach32_flash_lock();

    if (mach32_crc32((const uint8_t *)slot->start, size) != expected_crc) {
        invalidate_slot(slot);
        write_text("ERROR CRC32\r\n> ");
        return;
    }

    if (application_is_valid(slot->start, slot->end) == 0U) {
        invalidate_slot(slot);
        write_text("ERROR vector table\r\n> ");
        return;
    }

    write_text("OK\r\n> ");
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
    const mach32_slot_t *slot;
    uint32_t size;
    uint32_t crc;

    if (text_equals(command, "status") != 0U) {
        show_status();
        write_text("> ");
    } else if (text_equals(command, "boot A") != 0U) {
        try_boot('A');
    } else if (text_equals(command, "boot B") != 0U) {
        try_boot('B');
    } else if (parse_upload_command(command, &slot, &size, &crc) != 0U) {
        upload_image(slot, size, crc);
    } else {
        write_text("Commands: status, boot A, boot B, upload A/B\r\n> ");
    }
}

int main(void)
{
    char command[40] = {0};
    size_t command_length = 0U;
    uint32_t wait_start;
    uint8_t timeout_enabled = 1U;

    mach32_board_init();
    usart2_init();
    mach32_systick_init(16000000U);

    write_text("\r\nMACH32 bootloader\r\n");
    show_status();
    write_text("Type a command within ten seconds.\r\n> ");
    wait_start = mach32_systick_millis();

    while (1) {
        if ((USART2->SR & USART_SR_RXNE) != 0U) {
            char byte = (char)(USART2->DR & 0xFFU);

            /* Give the learner a full timeout period after each key press. */
            wait_start = mach32_systick_millis();

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
