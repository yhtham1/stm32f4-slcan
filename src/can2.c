/* ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <info@gerhard-bertelsmann.de> wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return
 * Gerhard Bertelsmann
 * ----------------------------------------------------------------------------
 */


#include "stm32f4-slcan.h"
#define lprintf l2printf
#define TX_BUFF_SIZE_CONST	(256)
#define RX_BUFF_SIZE_CONST	(256)
#define CANDEV	CAN2
#define IRQ_DEV	NVIC_CAN2_RX0_IRQ
#define USE_ALLOC			(0)		//alloc使用する

#if USE_ALLOC
#else
static CANMSG TX_BUFF_AREA[TX_BUFF_SIZE_CONST];
static CANMSG RX_BUFF_AREA[RX_BUFF_SIZE_CONST];
#endif

//****************************************** CAN
static CANMSG *_TxBuf;
static CANMSG *_RxBuf;
static int TX_BUFF_SIZE;
static int RX_BUFF_SIZE;

static volatile int _TxWp = 0x00;
static volatile int _TxRp = 0x00;
static volatile int _RxWp = 0x00;
static volatile int _RxRp = 0x00;

//*********************************** 受信バッファ処理
static
int _incp_rx( int p )
{
	p++;
	if( p >= RX_BUFF_SIZE ) p = 0;
	return p;
}

static int _push_rx( CANMSG dt )
{
	if( _incp_rx( _RxWp ) == _RxRp ) return -1;	// over flow
	_RxBuf[ _RxWp ] = dt;
	_RxWp = _incp_rx( _RxWp );
	return 0;
}

static CANMSG _pop_rx( void )
{
	CANMSG ans;
	ans.dlc = 0xff; // 無効なデータを知らせる。
	if( _RxRp == _RxWp ) return ans;				// empty data
	ans = _RxBuf[ _RxRp ];
	_RxRp = _incp_rx( _RxRp );
	return ans;
}
//*********************************** 送信バッファ処理
static int _incp_tx( int p )
{
	p++;
	if( p >= TX_BUFF_SIZE ) p = 0;
	return p;
}

static int _push_tx( CANMSG dt )
{
	if( _incp_tx( _TxWp ) == _TxRp ) return -1;	// over flow
	while( _incp_tx( _TxWp ) == _TxRp );
	_TxBuf[ _TxWp ] = dt;
	_TxWp = _incp_tx( _TxWp );
	return 0;
}

static CANMSG _pop_tx( void )
{
	CANMSG ans;
	ans.dlc = 0xff;
	if( _TxRp == _TxWp ) return ans;				// empty data
	ans = _TxBuf[ _TxRp ];
	_TxRp = _incp_tx( _TxRp );
	return ans;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
//*********************************** 受信ハードウェア部分
// CAN2受信割り込み

void can2_rx0_isr(void)
{
	CANMSG rx;
	gpio_set(GPIOA, GPIO0);	//PA0 RX-LED
	can_receive(CAN2, 0, false, &rx.id, &rx.ext, &rx.rtr, &rx.fmi, &rx.dlc, rx.data, &rx.timestamp);
	_push_rx( rx );
	gpio_clear(GPIOA, GPIO0);//PA0 RX-LED
	can_fifo_release(CANDEV, 0);
	return;
}

void debug_error()
{
	for(;;);
}

// CAN2送信割り込み
void can2_tx_isr( void )
{
	CANMSG a;
	// if( CAN_GetITStatus( CANDEV, CAN_IT_TME) == SET ){
	if( CAN_TSR(CANDEV) & (CAN_TSR_RQCP0 | CAN_TSR_RQCP1 | CAN_TSR_RQCP2 )){
		a = _pop_tx();
		if( 0xff == a.dlc ){
			can_disable_irq(CANDEV, CAN_IER_TMEIE);
			return;
		}
		can_transmit(CANDEV, a.id, a.ext, a.rtr, a.dlc, a.data );
	} else {
		debug_error();
	}
}

CANMSG getcCAN2( void )
{
	return _pop_rx();
}


void putcCAN2( CANMSG c )
{
#if 0
	int ans;
	ans = _push_tx( c );
	if( ans < 0 ){lprintf("putcCAN2() overflow\r\n"); for(;;); }// putcCAN2 error
#endif
	_push_tx( c );
	can_enable_irq(CAN2, CAN_IER_TMEIE);
	// CAN_ITConfig( CANDEV, CAN_IT_TME, ENABLE );	// 送信割込み許可
}

void init_can2_kick(void)
{

}

static void init_can2_sub(void)
{
	_TxWp = 0x00;
	_TxRp = 0x00;
	_RxWp = 0x00;
	_RxRp = 0x00;

#if USE_ALLOC
	void* my_calloc( size_t nums, size_t size );
	void *my_malloc( size_t size );
	_TxBuf = (CanTxMsg *)my_calloc(sizeof(CanTxMsg),TX_BUFF_SIZE_CONST);
	_RxBuf = (CanRxMsg *)my_calloc(sizeof(CanRxMsg),RX_BUFF_SIZE_CONST);
#else
	_TxBuf = &TX_BUFF_AREA[0];
	_RxBuf = &RX_BUFF_AREA[0];
#endif

	TX_BUFF_SIZE = TX_BUFF_SIZE_CONST;
	RX_BUFF_SIZE = RX_BUFF_SIZE_CONST;

	if( NULL == _TxBuf ){l2printf("CAN2-TX memory alloc error"); for(;;);}
	if( NULL == _RxBuf ){l2printf("CAN2-RX memory alloc error"); for(;;);}
	// lprintf("\r\nCAN2-TX buffer address :%08x:%d bytes\r\n", _TxBuf, sizeof(CANMSG)*TX_BUFF_SIZE_CONST );
	// lprintf("CAN2-RX buffer address :%08x:%d bytes\r\n",     _RxBuf, sizeof(CANMSG)*RX_BUFF_SIZE_CONST );
}



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

	init_can2_sub();


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
	nvic_enable_irq(  NVIC_CAN2_RX0_IRQ  );
	nvic_set_priority(NVIC_CAN2_RX0_IRQ,5);
	nvic_enable_irq(  NVIC_CAN2_TX_IRQ   );
	nvic_set_priority(NVIC_CAN2_TX_IRQ, 6);

	uint8_t d[] = {
		0xde, 0xad, 0xbe, 0xef 
	};
	can_transmit( CAN2, 0x7ff,0,0,4, d );

}

/*** EOF ***/
