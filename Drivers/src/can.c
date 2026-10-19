#include "mach32_can.h"
#include "mach32_rcc.h"

static uint8_t can_enable_clock(CAN_TypeDef *can)
{
    if (can == CAN1) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_CAN1EN);
    } else if (can == CAN2) {
        /* CAN2 depends on the CAN1 clock domain. */
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_CAN1EN
                                           | RCC_APB1ENR_CAN2EN);
    } else {
        return 0U;
    }

    return 1U;
}

static void can_configure_accept_all_filter(CAN_TypeDef *can)
{
    uint8_t bank = (can == CAN1) ? 0U : 14U;
    uint32_t bank_mask = 1UL << bank;

    CAN1->FMR |= CAN_FMR_FINIT;
    CAN1->FMR &= ~CAN_FMR_CAN2SB;
    CAN1->FMR |= 14UL << CAN_FMR_CAN2SB_Pos;

    CAN1->FA1R &= ~bank_mask;
    CAN1->FM1R &= ~bank_mask;
    CAN1->FS1R |= bank_mask;
    CAN1->FFA1R &= ~bank_mask;
    CAN1->sFilterRegister[bank].FR1 = 0U;
    CAN1->sFilterRegister[bank].FR2 = 0U;
    CAN1->FA1R |= bank_mask;

    CAN1->FMR &= ~CAN_FMR_FINIT;
}

void mach32_can_init(const mach32_can_init_t *init)
{
    if ((init == 0) || (init->instance == 0)
        || (init->prescaler == 0U) || (init->prescaler > 1024U)
        || (init->time_segment_1 == 0U) || (init->time_segment_1 > 16U)
        || (init->time_segment_2 == 0U) || (init->time_segment_2 > 8U)
        || (init->sync_jump_width == 0U) || (init->sync_jump_width > 4U)
        || (can_enable_clock(init->instance) == 0U)) {
        return;
    }

    CAN_TypeDef *can = init->instance;

    can->MCR = CAN_MCR_INRQ | CAN_MCR_ABOM;

    while ((can->MSR & CAN_MSR_INAK) == 0U) {
        __NOP();
    }

    can->BTR = ((uint32_t)(init->prescaler - 1U) << CAN_BTR_BRP_Pos)
             | ((uint32_t)(init->time_segment_1 - 1U) << CAN_BTR_TS1_Pos)
             | ((uint32_t)(init->time_segment_2 - 1U) << CAN_BTR_TS2_Pos)
             | ((uint32_t)(init->sync_jump_width - 1U) << CAN_BTR_SJW_Pos);

    if (init->loopback != 0U) {
        can->BTR |= CAN_BTR_LBKM;
    }

    if (init->silent != 0U) {
        can->BTR |= CAN_BTR_SILM;
    }

    can_configure_accept_all_filter(can);

    can->MCR &= ~CAN_MCR_INRQ;

    while ((can->MSR & CAN_MSR_INAK) != 0U) {
        __NOP();
    }
}

uint8_t mach32_can_transmit(CAN_TypeDef *can, const mach32_can_frame_t *frame)
{
    if ((can == 0) || (frame == 0) || (frame->length > 8U)) {
        return 0U;
    }

    uint8_t mailbox;

    if ((can->TSR & CAN_TSR_TME0) != 0U) {
        mailbox = 0U;
    } else if ((can->TSR & CAN_TSR_TME1) != 0U) {
        mailbox = 1U;
    } else if ((can->TSR & CAN_TSR_TME2) != 0U) {
        mailbox = 2U;
    } else {
        return 0U;
    }

    CAN_TxMailBox_TypeDef *tx = &can->sTxMailBox[mailbox];

    if (frame->extended != 0U) {
        tx->TIR = ((frame->id & 0x1FFFFFFFUL) << CAN_TI0R_EXID_Pos)
                | CAN_TI0R_IDE;
    } else {
        tx->TIR = (frame->id & 0x7FFUL) << CAN_TI0R_STID_Pos;
    }

    if (frame->remote != 0U) {
        tx->TIR |= CAN_TI0R_RTR;
    }

    uint32_t data_low = 0U;
    uint32_t data_high = 0U;

    for (uint8_t index = 0U; index < frame->length; ++index) {
        if (index < 4U) {
            data_low |= (uint32_t)frame->data[index] << (index * 8U);
        } else {
            data_high |= (uint32_t)frame->data[index] << ((index - 4U) * 8U);
        }
    }

    tx->TDTR = frame->length;
    tx->TDLR = data_low;
    tx->TDHR = data_high;
    tx->TIR |= CAN_TI0R_TXRQ;
    return 1U;
}

uint8_t mach32_can_receive(CAN_TypeDef *can, mach32_can_frame_t *frame)
{
    if ((can == 0) || (frame == 0) || ((can->RF0R & CAN_RF0R_FMP0) == 0U)) {
        return 0U;
    }

    CAN_FIFOMailBox_TypeDef *rx = &can->sFIFOMailBox[0];
    uint32_t identifier = rx->RIR;
    uint32_t data_low = rx->RDLR;
    uint32_t data_high = rx->RDHR;

    frame->extended = (identifier & CAN_RI0R_IDE) != 0U;
    frame->remote = (identifier & CAN_RI0R_RTR) != 0U;
    frame->id = (frame->extended != 0U)
        ? ((identifier >> CAN_RI0R_EXID_Pos) & 0x1FFFFFFFUL)
        : ((identifier & CAN_RI0R_STID) >> CAN_RI0R_STID_Pos);
    frame->length = (uint8_t)(rx->RDTR & 0xFU);

    if (frame->length > 8U) {
        frame->length = 8U;
    }
    frame->data[0] = (uint8_t)data_low;
    frame->data[1] = (uint8_t)(data_low >> 8U);
    frame->data[2] = (uint8_t)(data_low >> 16U);
    frame->data[3] = (uint8_t)(data_low >> 24U);
    frame->data[4] = (uint8_t)data_high;
    frame->data[5] = (uint8_t)(data_high >> 8U);
    frame->data[6] = (uint8_t)(data_high >> 16U);
    frame->data[7] = (uint8_t)(data_high >> 24U);

    can->RF0R |= CAN_RF0R_RFOM0;
    return 1U;
}
