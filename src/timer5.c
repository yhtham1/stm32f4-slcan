#include "misc.h"

#define TIM_1US	(TIM5)

void wait1us( int us )
{
	uint32_t a, b;
	if( us < 1 ) us = 1;
	a = timer_get_counter(TIM_1US);
	while( a == timer_get_counter(TIM_1US));
	a = timer_get_counter(TIM_1US);
	while(0 < us){
		b = timer_get_counter(TIM_1US);
		if( a <= b ){
			us -= (b-a);
		} else {
			us -= (b+(0xfffffffful-a));
		}
		a = b;
	}
}

void wait1ms( uint32_t ms )
{
	uint32_t us;
	us = ms * 1000UL;
	wait1us(us);
}

// TIM2,3,4,5,6,7,12,13,14 is connected APB1 2*rcc_apb1_frequency
//          TIM1,8,9,10,11 is connected APB2   rcc_apb2_frequency

void init_tim5(void)
{
	rcc_periph_clock_enable(RCC_TIM5);
	rcc_periph_reset_pulse(RST_TIM5);// DeInit()
	timer_disable_counter(TIM5);
	timer_set_prescaler(TIM5, (2*rcc_apb1_frequency/1000000)-1);//1uSのクロックとする。
	timer_set_mode(TIM5, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	timer_disable_preload(TIM5);
	timer_disable_counter(TIM5);
	timer_set_counter(TIM5, -1);//オーバーフローさせないとプリスケーラが反映されない？
	timer_continuous_mode(TIM5);

	timer_set_period(TIM5, -1);
	timer_enable_counter(TIM5);
}


/*** EOF ***/
