/* MACH32 polling bxCAN driver. */

#ifndef MACH32_CAN_H
#define MACH32_CAN_H

#include <stdint.h>

#include "stm32f4xx.h"

typedef struct {
    CAN_TypeDef *instance;
    uint16_t prescaler;  /* 1..1024 */
    uint8_t time_segment_1; /* 1..16 time quanta */
    uint8_t time_segment_2; /* 1..8 time quanta */
    uint8_t sync_jump_width; /* 1..4 time quanta */
    uint8_t loopback;
    uint8_t silent;
} mach32_can_init_t;

typedef struct {
    uint32_t id;
    uint8_t data[8];
    uint8_t length;
    uint8_t extended;
    uint8_t remote;
} mach32_can_frame_t;

void mach32_can_init(const mach32_can_init_t *init);
uint8_t mach32_can_transmit(CAN_TypeDef *can, const mach32_can_frame_t *frame);
uint8_t mach32_can_receive(CAN_TypeDef *can, mach32_can_frame_t *frame);

#endif /* MACH32_CAN_H */
