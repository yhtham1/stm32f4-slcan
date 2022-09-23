/* ----------------------------------------------------------------------------
 * ----------------------------------------------------------------------------
 */

#include "stm32f4-slcan.h"

#define TIMDEV	(TIM3)
#define RCCDEV	RCC_TIM3
#define RSTDEV	RST_TIM3

void beep1(int freq)
{
	int t;
	if( freq < 120 || 15000 < freq ){
	    timer_disable_counter(TIMDEV);
		return;
	}
	t = (7500000/freq)-1;
	timer_set_period(TIMDEV, t);
    timer_enable_counter(TIMDEV);
}
void beep0(void)
{
	timer_disable_counter(TIMDEV);
}

void beep(int freq, int ms)
{
	beep1(freq);
	if( 0 == ms ){
		return;
	}
	wait1ms(ms);
	beep0();
}

void pipo(void)
{
	beep( 2000, 100 );
	beep( 1000, 100 );
}

void init_buzzer(void)
{
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCCDEV);

	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO4);
	gpio_set_af(GPIOB, GPIO_AF2, GPIO4);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO4);

	/* Reset TIMDEV peripheral. */
	rcc_periph_reset_pulse(RSTDEV);

	/* Timer global mode:
	 * - No divider
	 * - Alignment edge
	 * - Direction up
	 */
	timer_set_mode(TIMDEV, TIM_CR1_CKD_CK_INT,
		 TIM_CR1_CMS_CENTER_1, TIM_CR1_DIR_UP);
	/* Reset prescaler value. */
	timer_set_prescaler(TIMDEV, 3-1);
	timer_set_clock_division(TIMDEV, TIM_CR1_CKD_CK_INT);
	/* Reset repetition counter value. */
	// timer_set_repetition_counter(TIMDEV, 100);//RCR
	timer_set_period(TIMDEV, 128);//ARR

	/* Enable preload. */
	timer_enable_preload(TIMDEV);

	/* Continuous mode. */
	timer_continuous_mode(TIMDEV);

	/* -- OC1 configuration -- */
	/* Disable outputs. */
	timer_disable_oc_output(TIMDEV, TIM_OC1);
	timer_disable_oc_output(TIMDEV, TIM_OC1N);

	/* Configure global mode of line 3. */
	timer_disable_oc_clear(TIMDEV, TIM_OC1);
	timer_enable_oc_preload(TIMDEV, TIM_OC1);
	timer_set_oc_fast_mode(TIMDEV, TIM_OC1);
	timer_set_oc_mode(TIMDEV, TIM_OC1, TIM_OCM_TOGGLE);

	/* Configure OC1. */
	timer_set_oc_polarity_high(TIMDEV, TIM_OC1);

	/* Set the capture compare value for OC1. 50% duty */
	timer_set_oc_value(TIMDEV, TIM_OC1, 0);
	/* Reenable outputs. */
	timer_enable_oc_output(TIMDEV, TIM_OC1);

	// timer_enable_break_main_output(TIMDEV);
	/* ARR reload enable. */
	timer_enable_preload(TIMDEV);

	timer_disable_counter(TIMDEV);
	pipo();
}




/*** EOF ***/
