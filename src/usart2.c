//ＵＴＦ８

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "usart.h"
#include "misc.h"


#define TX_BUFF_SIZE_CONST	(16384)
#define RX_BUFF_SIZE_CONST	(16384)
#define USARTDEV	USART2
#define RCC_DEV		RCC_USART2
#define IRQ_DEV		NVIC_USART2_IRQ
#define USE_ALLOC			(0)		//alloc使用する

#if USE_ALLOC
#else
static char TX_BUFF_AREA[TX_BUFF_SIZE_CONST];
static char RX_BUFF_AREA[RX_BUFF_SIZE_CONST];//FIX20161104
#endif

//******************************************
static volatile int sig_break=0;
static char* _TxBuf;
static char* _RxBuf;
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

static
int _push_rx( char dt )
{
	if( _incp_rx( _RxWp ) == _RxRp ) return -1;	// over flow
	_RxBuf[ _RxWp ] = dt;
	_RxWp = _incp_rx( _RxWp );
	return dt;
}

static
int _pop_rx( void )
{
	int ans;
	if( _RxRp == _RxWp ) return -1;				// empty data
	ans = _RxBuf[ _RxRp ];
	_RxRp = _incp_rx( _RxRp );
	return ans;
}
//*********************************** 送信バッファ処理
static
int _incp_tx( int p )
{
	p++;
	if( p >= TX_BUFF_SIZE ) p = 0;
	return p;
}

static
int _push_tx( char dt )
{
//	if( _incp_tx( _TxWp ) == _TxRp ) return -1;	// over flow
	while( _incp_tx( _TxWp ) == _TxRp );
	_TxBuf[ _TxWp ] = dt;
	_TxWp = _incp_tx( _TxWp );

	usart_enable_tx_interrupt(USARTDEV);
	// USART_ITConfig( USARTDEV, USART_IT_TXE, ENABLE );				// 送信割込み許可
	return 0;
}

static int _pop_tx( void )
{
	int ans;
	if( _TxRp == _TxWp ) return -1;				// empty data
	ans = _TxBuf[ _TxRp ];
	_TxRp = _incp_tx( _TxRp );
	return ans;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//*********************************** 受信ハードウェア部分
// これを受信割り込みに入れる。
void intr_USART2_rx( void )
{
	/* Check if we were called because of RXNE. */
	if (((USART_SR(USARTDEV) & USART_SR_RXNE) != 0)) {
		/* Retrieve the data from the peripheral. */
		gpio_set(GPIOA, GPIO0);//RX LED PA0
		_push_rx(usart_recv(USARTDEV));
		gpio_clear(GPIOA, GPIO0);//RX LED PA0
	}
}
//*********************************** 送信ハードウェア部分
static
void _1byte_( char c )
{
	c &= 0xff;
	usart_send(USARTDEV, c );
}
// これを送信割り込みに入れる。
void intr_USART2_tx( void )
{
	int _c;
	/* Check if we were called because of TXE. */
	if (((USART_SR(USARTDEV) & USART_SR_TXE) != 0)) {
		_c = _pop_tx();

		if( -1 == _c ){
			usart_disable_tx_interrupt(USARTDEV);
			return;
		}
		_1byte_( _c );
	}
}


void usart2_isr( void )
{
	intr_USART2_rx();
	intr_USART2_tx();
}

void can2_poll(void);

char getcSIO2b(void)
{
	int d;
	while(-1 == (d = getcSIO2())){
		// blocking input
	};
	return d & 0xff;
}


int getcSIO2( void )
{
	if( sig_break ){
		sig_break = 0;
		return -2;//break signal
	}
	return _pop_rx();
}

void putcSIO2( char c )
{
	_push_tx( c );
}

void putsSIO2( char *s )
{
	while( *s ){
		putcSIO2( *s );
		s++;
	}
}

void init_usart2( uint baudrate )
{
	_TxWp = 0x00;
	_TxRp = 0x00;
	_RxWp = 0x00;
	_RxRp = 0x00;

	TX_BUFF_SIZE = TX_BUFF_SIZE_CONST;
	RX_BUFF_SIZE = RX_BUFF_SIZE_CONST;

#if USE_ALLOC
	_TxBuf = (char *)my_calloc(sizeof(char),TX_BUFF_SIZE_CONST);
	_RxBuf = (char *)my_calloc(sizeof(char),RX_BUFF_SIZE_CONST);
	if( NULL == _TxBuf ) for(;;);
	if( NULL == _RxBuf ) for(;;);
#else
	_TxBuf = &TX_BUFF_AREA[0];
	_RxBuf = &RX_BUFF_AREA[0];
#endif

	//USARTにクロック供給
	rcc_periph_clock_enable(RCC_DEV);

	//USARTに使用するGPIO初期化
	/* Setup GPIO pins for USARTDEV transmit and receive. */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
	/* Setup USARTDEV TX,RX pin as alternate function. */
	gpio_set_af(GPIOA, GPIO_AF7, GPIO2 | GPIO3);

    /* Setup UART parameters. */
    usart_set_baudrate(USARTDEV, baudrate);
    usart_set_databits(USARTDEV, 8);
    usart_set_stopbits(USARTDEV, USART_STOPBITS_1);
	usart_set_mode(USARTDEV, USART_MODE_TX_RX);
    usart_set_parity(USARTDEV, USART_PARITY_NONE);
	usart_set_flow_control(USARTDEV, USART_FLOWCONTROL_NONE);

	nvic_set_priority(IRQ_DEV, 4);
	nvic_enable_irq(IRQ_DEV);

	usart_enable_rx_interrupt(USARTDEV);
//	usart_enable_tx_interrupt(USARTDEV);
	usart_enable(USARTDEV);
}


int l2printf( char *format, ... )
{
	int ans;
	va_list arg;
	char buf[200];
	va_start( arg, format );
	ans = vsnprintf( buf, sizeof buf, format, arg );
	putsSIO2( buf );
	va_end( arg );
	return ans;
}

/*** EOF ***/
