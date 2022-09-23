/* ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <info@gerhard-bertelsmann.de> wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return
 * Gerhard Bertelsmann
 * ----------------------------------------------------------------------------
 */
/*
 * This file is derived from the libopencm3 project examples 
 */

#include "stm32f4-slcan.h"


static void gpio_setup(void)
{
	/* Enable GPIOA & GPIOB & GPIOC clock */

	/* PA2 & PA3 USART */
	rcc_periph_clock_enable(RCC_GPIOA);
	/* PB8 & PB9 CAN */
	rcc_periph_clock_enable(RCC_GPIOB);

	gpio_clear(GPIOA, GPIO5); /* board  LED green off */

	/* Configure LED&Osci GPIO */
	gpio_mode_setup(GPIOA,GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);//PA0 CANRX-LED
	gpio_mode_setup(GPIOA,GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1);//PA1 CANTX-LED
	gpio_mode_setup(GPIOA,GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);//PA5 nucleo LED
	gpio_mode_setup(GPIOA,GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6);//PA6 D12-MISO
	gpio_set( GPIOA, GPIO0 );//PA0 CANRX-LED
	gpio_set( GPIOA, GPIO1 );//PA1 CANTX-LED

}


/* monotonically increasing number of milliseconds from reset
 * overflows every 49 days if you're wondering
 */
volatile uint32_t system_millis;

int ans;
/* Called when systick fires */
void sys_tick_handler(void)
{
	system_millis++;
	volatile int i;
	i = timer_get_counter(TIM3);
	i++;
	ans = i;
}
/* sleep for delay milliseconds */
void wait1ms(uint32_t delay)
{
	uint32_t wake = system_millis + delay;
	while (wake > system_millis);
}

static void systick_setup(void)
{
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_set_reload(180000);
	systick_interrupt_enable();
	/* Start counting */
	systick_counter_enable();
}

static void put_hex(uint8_t c)
{
	uint8_t s[2];

	bin2hex(s, c);
	putcSIO2(s[0]);
	putcSIO2(s[1]);
}

void can2_rx1_isr(void)
{
	uint32_t id;
	bool ext, rtr;
	uint8_t  dlc, data[8], fmi;
	can_receive(CAN2, 1, false, &id, &ext, &rtr, &fmi, &dlc, data, NULL);
	return;
}

// void can2_rx0_isr(void);
void can2_poll(void)
{
	CANMSG rx;
	uint32_t id;
	bool ext, rtr;
	uint8_t i, dlc, data[8], fmi;
	char c;

	rx = getcCAN2();
	if( rx.dlc == 0xff ) return;
	id = rx.id;
	ext = rx.ext;
	rtr = rx.rtr;
	fmi = rx.fmi;
	dlc = rx.dlc;
	memcpy( data, rx.data, 8 );

	if (rtr) {
		if (ext)
			c = 'R';
		else
			c = 'r';
	} else {
		if (ext)
			c = 'T';
		else
			c = 't';
	}
	putcSIO2(c);
	if (ext) {
		c = (id >> 24) & 0xff;
		put_hex(c);
		c = (id >> 16) & 0xff;
		put_hex(c);
		c = (id >> 8) & 0xff;
		put_hex(c);
		c = id & 0xff;
		put_hex(c);
	} else {
		/* bits 11-9 */
		c = (id >> 8) & 0x07;
		c += 0x30;
		putcSIO2(c);
		/* bits 8-1 */
		c = id & 0xff;
		put_hex(c);
	}
	c = (dlc & 0x0f) | 0x30;
	putcSIO2(c);
	for (i = 0; i < dlc; i++)
		put_hex(data[i]);

	putcSIO2('\r');
}

static uint32_t get_nibbles(int nibbles)
{
	int i;
	uint32_t id;
	char c;

	id = 0;
	for (i = 0; i < nibbles; i++) {
		c = getcSIO2b();
		id <<= 4;
		id |= nibble2bin(c);
	}
	return id;
}

static int slcan_command(void)
{
	bool ext, rtr;
	uint8_t i, dlc, data[8];
	uint32_t id;
	int32_t ret;
	char c;
	int d;
	bool send;

	id = 0;
	dlc = 0;
	ext = true;
	send = true;
	rtr = false;
	ret = 0;

	if (!can_available_mailbox(CAN2)) {
		__asm__("nop");
		return -1;
	}

	d = getcSIO2();
	if( -1 == d ) return 0;// empty SIO buffer
	c = 0xff & d;
	switch (c){
	case 'T':
		id = get_nibbles(8);
		dlc = get_nibbles(1);
		break;
	case 't':
		ext = false;
		id = get_nibbles(3);
		dlc = get_nibbles(1);
		break;
	case 'R':
		rtr = true;
		ext = true;
		id = get_nibbles(8);
		dlc = get_nibbles(1);
		break;
	case 'r':
		rtr = true;
		ext = false;
		id = get_nibbles(3);
		dlc = get_nibbles(1);
		break;
	case 'S':
		c = get_nibbles(1);
		can_speed(c);
		send = false;
		break;
	case 'v':
		send = false;
		break;
	case 'V':
		send = false;
		break;
	case 'C':
		send = false;
		break;
	default:
		send = false;
		break;
	}
	if (dlc > 8) {
		/* consume chars until eol reached */
		do {
			ret = getcSIO2b();
		} while (ret != '\r');
		return -1;
	}

	for (i = 0; i < dlc; i++) {
		data[i] = (uint8_t)get_nibbles(2);
	}

	/* consume chars until eol reached */
	do {
		ret = getcSIO2b();
	} while (ret != '\r');

#if 1
	if (send) {
		CANMSG tx;
		tx.id = id;
		tx.ext = ext;
		tx.rtr = rtr;
		tx.dlc = dlc;
		memcpy(tx.data, data, 8);
		putcCAN2(tx);
		ret = 1; // OK


		// ret = can_transmit(CAN2, id, ext, rtr, dlc, data);
		/* gpio_debug(ret); */
	}
#else
	if (send) {
		int loop = CAN_MAX_RETRY;
		/* try to send data - omit if not possible */
		while (loop-- > 0) {
			if (can_available_mailbox(CAN1))
				break;
			/* TODO: LED overflow */
		}
		ret = can_transmit(CAN1, id, ext, rtr, dlc, data);
		gpio_debug(ret);
	}
#endif

	if(ret){
		putcSIO2('\r');//OK
	} else {
		putcSIO2('\a');//NG
	}
	return ret;
}


int main(void)
{
	// SCB_VTOR = 0x20000000;//Run from RAM for DEBUG

	rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_180MHZ]);
	gpio_setup();
	systick_setup();
	init_buzzer();

	int i;
	for( i = 0 ; i < 10 ; i++){
		gpio_toggle( GPIOA, GPIO0 );
		gpio_toggle( GPIOA, GPIO1 );
		wait1ms(200);
	}
	init_usart2(921600);
	can_setup();
	while( 0 <= getcSIO2()); // clean up buffer
	/* endless loop */
	int ct = 0;
	while (1) {
		can2_poll(); // can fifo -> uart
		slcan_command(); // uart -> can
		ct++;
	}
	return 0;
}
/*** EOF ***/
