/* ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <info@gerhard-bertelsmann.de> wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return
 * Gerhard Bertelsmann
 * ----------------------------------------------------------------------------
 */

#ifndef _STM32_SLCAN_H_
#define _STM32_SLCAN_H_

#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/can.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include "usart_a.h"

#define CAN_MAX_RETRY	10000

/******************************************************************************
 * Simple ringbuffer implementation from open-bldc's libgovernor that
 * you can find at:
 * https://github.com/open-bldc/open-bldc/tree/master/source/libgovernor
 *****************************************************************************/

typedef int32_t ring_size_t;

#define XON 0x11
#define XOFF 0x13

struct ring {
    uint8_t *data;
    ring_size_t size;
    uint32_t begin;
    uint32_t end;
};

typedef struct {
	uint32_t	id;
	bool 		ext;
	bool 		rtr;
	uint8_t		fmi;
	uint8_t		dlc;
	uint8_t		data[8];
	uint16_t	timestamp;
} CANMSG;


/* can2 */
void can_setup(void);
int can_speed(int index);


/* usart */
void usart_setup(void);
int32_t ring_write_ch(struct ring *ring, uint8_t ch);
int32_t ring_write(struct ring *ring, uint8_t * data, ring_size_t size);
int32_t ring_read_ch(struct ring *ring, uint8_t * ch);
int32_t ring_bytes_free(const struct ring *ring);
uint8_t uart_read_blocking(void);

/* utils */
uint8_t nibble2bin(uint8_t s);
void bin2hex(uint8_t *dst, unsigned char c);
uint8_t hex2bin(char *s);


void init_buzzer(void);
void pipo(void);
void beep(int freq, int ms);
CANMSG getcCAN2( void );
void can2_poll(void);
void putcCAN2( CANMSG c );


#endif  /* _STM32_SLCAN_H_ */
