#include "mach32_crc32.h"

uint32_t mach32_crc32(const uint8_t *data, size_t length)
{
    if ((data == 0) && (length != 0U)) {
        return 0U;
    }

    uint32_t crc = 0xFFFFFFFFUL;

    for (size_t index = 0U; index < length; ++index) {
        crc ^= data[index];

        for (uint8_t bit = 0U; bit < 8U; ++bit) {
            if ((crc & 1U) != 0U) {
                crc = (crc >> 1U) ^ 0xEDB88320UL;
            } else {
                crc >>= 1U;
            }
        }
    }

    return crc ^ 0xFFFFFFFFUL;
}
