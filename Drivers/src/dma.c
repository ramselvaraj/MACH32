#include "mach32_dma.h"

void mach32_dma_init(const mach32_dma_init_t *init)
{
    (void)init;
    /* TODO(core): Disable the stream, clear its flags, then configure channel,
     * direction, data sizes, increments, circular mode and priority in SxCR. */
}

void mach32_dma_start(DMA_Stream_TypeDef *stream, uintptr_t peripheral_address,
                      uintptr_t memory_address, uint16_t count)
{
    (void)stream;
    (void)peripheral_address;
    (void)memory_address;
    (void)count;
    /* TODO(core): Write PAR, M0AR and NDTR, then enable the stream. */
}

void mach32_dma_stop(DMA_Stream_TypeDef *stream)
{
    (void)stream;
    /* TODO(core): Disable the stream and wait until hardware confirms it. */
}

uint8_t mach32_dma_is_transfer_complete(DMA_TypeDef *controller, uint8_t stream_index)
{
    (void)controller;
    (void)stream_index;
    /* TODO(core): Select LISR/HISR and test the stream's TCIF bit. */
    return 0U;
}

void mach32_dma_clear_transfer_complete(DMA_TypeDef *controller, uint8_t stream_index)
{
    (void)controller;
    (void)stream_index;
    /* TODO(core): Select LIFCR/HIFCR and write the stream's CTCIF bit. */
}
