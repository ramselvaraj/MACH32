#include "mach32_dma.h"
#include "mach32_rcc.h"

static const uint32_t dma_all_flag_masks[4] = {
    0x0000003DU,
    0x00000F40U,
    0x003D0000U,
    0x0F400000U,
};

static const uint32_t dma_transfer_complete_masks[4] = {
    0x00000020U,
    0x00000800U,
    0x00200000U,
    0x08000000U,
};

static uint8_t dma_enable_clock(DMA_TypeDef *controller)
{
    if (controller == DMA1) {
        mach32_rcc_ahb1_periph_clock_enable(RCC_AHB1ENR_DMA1EN);
    } else if (controller == DMA2) {
        mach32_rcc_ahb1_periph_clock_enable(RCC_AHB1ENR_DMA2EN);
    } else {
        return 0U;
    }

    return 1U;
}

static void dma_clear_all_flags(DMA_TypeDef *controller, uint8_t stream_index)
{
    uint32_t mask = dma_all_flag_masks[stream_index % 4U];

    if (stream_index < 4U) {
        controller->LIFCR = mask;
    } else {
        controller->HIFCR = mask;
    }
}

void mach32_dma_init(const mach32_dma_init_t *init)
{
    if ((init == 0) || (init->stream == 0) || (init->stream_index > 7U)
        || (init->channel > 7U) || (dma_enable_clock(init->controller) == 0U)) {
        return;
    }

    mach32_dma_stop(init->stream);
    dma_clear_all_flags(init->controller, init->stream_index);

    init->stream->CR = ((uint32_t)init->channel << DMA_SxCR_CHSEL_Pos)
                     | ((uint32_t)init->direction << DMA_SxCR_DIR_Pos)
                     | ((uint32_t)init->peripheral_size << DMA_SxCR_PSIZE_Pos)
                     | ((uint32_t)init->memory_size << DMA_SxCR_MSIZE_Pos)
                     | ((uint32_t)init->priority << DMA_SxCR_PL_Pos);

    if (init->peripheral_increment != 0U) {
        init->stream->CR |= DMA_SxCR_PINC;
    }

    if (init->memory_increment != 0U) {
        init->stream->CR |= DMA_SxCR_MINC;
    }

    if (init->circular != 0U) {
        init->stream->CR |= DMA_SxCR_CIRC;
    }

    init->stream->FCR = 0U;
}

void mach32_dma_start(DMA_Stream_TypeDef *stream, uintptr_t peripheral_address,
                      uintptr_t memory_address, uint16_t count)
{
    if ((stream == 0) || (count == 0U)) {
        return;
    }

    mach32_dma_stop(stream);
    stream->PAR = (uint32_t)peripheral_address;
    stream->M0AR = (uint32_t)memory_address;
    stream->NDTR = count;
    stream->CR |= DMA_SxCR_EN;
}

void mach32_dma_stop(DMA_Stream_TypeDef *stream)
{
    if (stream == 0) {
        return;
    }

    stream->CR &= ~DMA_SxCR_EN;

    while ((stream->CR & DMA_SxCR_EN) != 0U) {
        __NOP();
    }
}

uint8_t mach32_dma_is_transfer_complete(DMA_TypeDef *controller, uint8_t stream_index)
{
    if (((controller != DMA1) && (controller != DMA2)) || (stream_index > 7U)) {
        return 0U;
    }

    uint32_t mask = dma_transfer_complete_masks[stream_index % 4U];
    uint32_t status = (stream_index < 4U) ? controller->LISR : controller->HISR;

    return (status & mask) != 0U;
}

void mach32_dma_clear_transfer_complete(DMA_TypeDef *controller, uint8_t stream_index)
{
    if (((controller != DMA1) && (controller != DMA2)) || (stream_index > 7U)) {
        return;
    }

    uint32_t mask = dma_transfer_complete_masks[stream_index % 4U];

    if (stream_index < 4U) {
        controller->LIFCR = mask;
    } else {
        controller->HIFCR = mask;
    }
}
