//ＵＴＦ８
#ifndef _MISC_H_
#define _MISC_H_



#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/can.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>


void wait1ms(uint32_t delay);


#endif
