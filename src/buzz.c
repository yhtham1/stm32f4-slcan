/* ----------------------------------------------------------------------------
 * ----------------------------------------------------------------------------
 */

#include "stm32f4-slcan.h"
#include "usart.h"




void pipo(void)
{

}


void init_buzzer(void)
{
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_TIM3);

	gpio_mode_setup(GPIOB,GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);//PA0 CANRX-LED
	int i;
	for( i = 0 ; i < 200 ; i++){
		gpio_toggle(GPIOB, GPIO4);
		wait1ms(1);
	}



	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO4);
	gpio_set_af(GPIOB, GPIO_AF1, GPIO4);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO4);

	/* Reset TIM3 peripheral. */
	rcc_periph_reset_pulse(RST_TIM3);

	/* Timer global mode:
	 * - No divider
	 * - Alignment edge
	 * - Direction up
	 */
	timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT,
		 TIM_CR1_CMS_CENTER_1, TIM_CR1_DIR_UP);
	/* Reset prescaler value. */
	timer_set_prescaler(TIM3, 160);
	timer_set_clock_division(TIM3, TIM_CR1_CKD_CK_INT);
	/* Reset repetition counter value. */
	// timer_set_repetition_counter(TIM3, 100);//RCR
	timer_set_period(TIM3, 128);//ARR

	/* Enable preload. */
	timer_enable_preload(TIM3);

	/* Continuous mode. */
	timer_continuous_mode(TIM3);

	/* -- OC1 configuration -- */
	/* Disable outputs. */
	timer_disable_oc_output(TIM3, TIM_OC1);
	timer_disable_oc_output(TIM3, TIM_OC1N);

	/* Configure global mode of line 3. */
	timer_disable_oc_clear(TIM3, TIM_OC1);
	timer_enable_oc_preload(TIM3, TIM_OC1);
	timer_set_oc_fast_mode(TIM3, TIM_OC1);
	timer_set_oc_mode(TIM3, TIM_OC1, TIM_OCM_TOGGLE);

	/* Configure OC1. */
	timer_set_oc_polarity_high(TIM3, TIM_OC1);

	/* Set the capture compare value for OC1. 50% duty */
	timer_set_oc_value(TIM3, TIM_OC1, 1);
	timer_enable_oc_clear(TIM3, TIM_OC1);
	/* Reenable outputs. */
	timer_enable_oc_output(TIM3, TIM_OC1);

	timer_enable_break_main_output(TIM3);
	/* ARR reload enable. */
	timer_enable_preload(TIM3);

	/* Counter enable. */
	timer_enable_counter(TIM3);
	for(;;);
}

static void gpio_setup(void)
{
	/* Set GPIOE12 and GPIO13 to 'output push-pull' and AF1 (TIM3_CH3N, TIM3_CH3). */
	gpio_mode_setup(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO12 | GPIO13);
	gpio_set_af(GPIOE, GPIO_AF1, GPIO12 | GPIO13);
	gpio_set_output_options(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO12 | GPIO13);
}







/*** EOF ***/
