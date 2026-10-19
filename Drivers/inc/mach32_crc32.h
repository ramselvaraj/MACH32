/* MACH32 software CRC-32 implementation. */

#ifndef MACH32_CRC32_H
#define MACH32_CRC32_H

#include <stddef.h>
#include <stdint.h>

uint32_t mach32_crc32(const uint8_t *data, size_t length);

#endif /* MACH32_CRC32_H */
