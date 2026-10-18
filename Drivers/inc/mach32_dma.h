/* MACH32 direct memory access driver. */

#ifndef MACH32_DMA_H
#define MACH32_DMA_H

#include <stdint.h>

#include "stm32f4xx.h"

typedef enum {
    MACH32_DMA_PERIPHERAL_TO_MEMORY,
    MACH32_DMA_MEMORY_TO_PERIPHERAL,
    MACH32_DMA_MEMORY_TO_MEMORY,
} mach32_dma_direction_t;

typedef enum {
    MACH32_DMA_SIZE_BYTE,
    MACH32_DMA_SIZE_HALF_WORD,
    MACH32_DMA_SIZE_WORD,
} mach32_dma_data_size_t;

typedef enum {
    MACH32_DMA_PRIORITY_LOW,
    MACH32_DMA_PRIORITY_MEDIUM,
    MACH32_DMA_PRIORITY_HIGH,
    MACH32_DMA_PRIORITY_VERY_HIGH,
} mach32_dma_priority_t;

typedef struct {
    DMA_TypeDef *controller;
    DMA_Stream_TypeDef *stream;
    uint8_t stream_index;
    uint8_t channel;
    mach32_dma_direction_t direction;
    mach32_dma_data_size_t peripheral_size;
    mach32_dma_data_size_t memory_size;
    mach32_dma_priority_t priority;
    uint8_t peripheral_increment;
    uint8_t memory_increment;
    uint8_t circular;
} mach32_dma_init_t;

void mach32_dma_init(const mach32_dma_init_t *init);
void mach32_dma_start(DMA_Stream_TypeDef *stream, uintptr_t peripheral_address,
                      uintptr_t memory_address, uint16_t count);
void mach32_dma_stop(DMA_Stream_TypeDef *stream);
uint8_t mach32_dma_is_transfer_complete(DMA_TypeDef *controller, uint8_t stream_index);
void mach32_dma_clear_transfer_complete(DMA_TypeDef *controller, uint8_t stream_index);

#endif /* MACH32_DMA_H */
