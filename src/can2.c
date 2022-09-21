/* ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <info@gerhard-bertelsmann.de> wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return
 * Gerhard Bertelsmann
 * ----------------------------------------------------------------------------
 */


#include "stm32f4-slcan.h"

int can_speed(int index)
{
	int ret;
	/*
	 S0 = 10 kBaud
	 S1 = 20 kBaud
	 S2 = 50 kBaud
	 S3 = 100 kBaud
	 S4 = 125 kBaud
	 S5 = 250 kBaud
	 S6 = 500 kBaud
	 S7 = 800 kBaud
	 S8 = 1 MBaud

	 TTCM: Time triggered comm mode
	 ABOM: Automatic bus-off management
	 AWUM: Automatic wakeup mode
	 NART: No automatic retransmission
	 RFLM: Receive FIFO locked mode
	 TXFP: Transmit FIFO priority
	 */
	switch (index) {
	case 0:
		ret = can_init(CAN2, false, true, false, false, false, false,
			CAN_BTR_SJW_1TQ, CAN_BTR_TS1_15TQ, CAN_BTR_TS2_2TQ, 250, false,
			false);
		break;
	case 1:
		ret = can_init(CAN2, false, true, false, false, false, false,
			CAN_BTR_SJW_1TQ, CAN_BTR_TS1_15TQ, CAN_BTR_TS2_2TQ, 125, false,
			false);
		break;
	case 2:
		ret = can_init(CAN2, false, true, false, false, false, false,
			CAN_BTR_SJW_1TQ, CAN_BTR_TS1_15TQ, CAN_BTR_TS2_2TQ, 50, false,
			false);
		break;
	case 3:
		ret = can_init(CAN2, false, true, false, false, false, false,
			CAN_BTR_SJW_1TQ, CAN_BTR_TS1_15TQ, CAN_BTR_TS2_2TQ, 25, false,
			false);
		break;
	case 4:
		ret = can_init(CAN2, false, true, false, false, false, false,
			CAN_BTR_SJW_1TQ, CAN_BTR_TS1_15TQ, CAN_BTR_TS2_2TQ, 20, false,
			false);
		break;
	case 5:
		ret = can_init(CAN2, false, true, false, false, false, false,
			CAN_BTR_SJW_1TQ, CAN_BTR_TS1_15TQ, CAN_BTR_TS2_2TQ, 10, false,
			false);
		break;
	case 6:
		ret = can_init(CAN2, false, true, false, false, false, false,
			CAN_BTR_SJW_1TQ, CAN_BTR_TS1_15TQ, CAN_BTR_TS2_2TQ, 5, false,
			false);
		break;
	case 7:
		ret = can_init(CAN2, false, true, false, false, false, false,
			CAN_BTR_SJW_1TQ, CAN_BTR_TS1_15TQ, CAN_BTR_TS2_2TQ, 3, false,
			false);
		break;
	case 8:
		ret = can_init(CAN2, false, true, false, false, false, false,
			CAN_BTR_SJW_1TQ, CAN_BTR_TS1_12TQ, CAN_BTR_TS2_2TQ, 3, false,
			false);
		break;
	default:
		ret = -1;
		break;
	}
	return ret;
}

void can_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_CAN1);
	can_reset(CAN1);

	// enable can2 peripheral
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_CAN2);
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO5 | GPIO6);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO5 | GPIO6);
	gpio_set_af(GPIOB, GPIO_AF9, GPIO5 | GPIO6);
	can_reset(CAN2);

	can_init( CAN2, false, false, false, false, false, false, 
		CAN_BTR_SJW_2TQ,
		CAN_BTR_TS1_7TQ,
		CAN_BTR_TS2_2TQ, 9, false, false );

	can_filter_id_mask_32bit_init(0 , 0, 0, 0,  0); // set a catch-all filter for CAN1 fifo 0
	can_filter_id_mask_32bit_init(14, 0, 0, 0,  1); // set a catch-all filter for CAN2 fifo 0

	/* Enable CAN RX interrupt. */
	can_enable_irq(CAN2, CAN_IER_FMPIE0);

	/* NVIC setup. */
	nvic_enable_irq(NVIC_CAN2_RX0_IRQ);
	nvic_set_priority(NVIC_CAN2_RX0_IRQ, 5);
	nvic_enable_irq(NVIC_USART2_IRQ);

	uint8_t d[] = {
		0xde, 0xad, 0xbe, 0xef 
	};
	can_transmit( CAN2, 0x7ff,0,0,4, d );

}

/*** EOF ***/
