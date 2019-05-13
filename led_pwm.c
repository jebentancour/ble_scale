#include "led_pwm.h"

#define NRF_LOG_MODULE_NAME "PWM"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "nrf.h"
#include "nrf_nvic.h"
#include "nrf_gpio.h"

#define LED_B_PIN 18
#define LED_R_PIN 19
#define LED_G_PIN 20

#define TIMER_PRESCALER 9 // f = 16Mhz/(2**9) = 31,250kHz

// Dado que el timer está configurado en modo 8 bits
// la frecuencia que se le aplica a los LEDs es de
// f = 31,250kHz / 256 = 122 Hz

#define MIN_PWM 6

static uint8_t r;
static uint8_t g;
static uint8_t b;

void TIMER2_IRQHandler(void){
	if(NRF_TIMER2->EVENTS_COMPARE[0]){
		NRF_TIMER2->EVENTS_COMPARE[0] = 0;
		nrf_gpio_pin_set(LED_R_PIN);
	}

	if(NRF_TIMER2->EVENTS_COMPARE[1]){
		NRF_TIMER2->EVENTS_COMPARE[1] = 0;
		nrf_gpio_pin_set(LED_G_PIN);
	}

	if(NRF_TIMER2->EVENTS_COMPARE[2]){
		NRF_TIMER2->EVENTS_COMPARE[2] = 0;
		nrf_gpio_pin_set(LED_B_PIN);
	}

	if(NRF_TIMER2->EVENTS_COMPARE[3]){
		NRF_TIMER2->EVENTS_COMPARE[3] = 0;
		NRF_TIMER2->CC[0] = r;
		NRF_TIMER2->CC[1] = g;
		NRF_TIMER2->CC[2] = b;
		if (NRF_TIMER2->CC[0] > MIN_PWM){
			nrf_gpio_pin_clear(LED_R_PIN);
		} else {
			nrf_gpio_pin_set(LED_R_PIN);
		}
		if (NRF_TIMER2->CC[1] > MIN_PWM){
			nrf_gpio_pin_clear(LED_G_PIN);
		} else {
			nrf_gpio_pin_set(LED_G_PIN);
		}
		if (NRF_TIMER2->CC[2] > MIN_PWM){
			nrf_gpio_pin_clear(LED_B_PIN);
		} else {
			nrf_gpio_pin_set(LED_B_PIN);
		}
	}
}

void led_pwm_init(void){
	r = 0;
	g = 0;
	b = 0;

	nrf_gpio_cfg_output(LED_R_PIN);
	nrf_gpio_cfg_output(LED_G_PIN);
	nrf_gpio_cfg_output(LED_B_PIN);

	nrf_gpio_pin_set(LED_R_PIN);
	nrf_gpio_pin_set(LED_G_PIN);
	nrf_gpio_pin_set(LED_B_PIN);

	NRF_TIMER2->POWER	   = 1;
	NRF_TIMER2->MODE		= TIMER_MODE_MODE_Timer;
	NRF_TIMER2->BITMODE	 = TIMER_BITMODE_BITMODE_08Bit << TIMER_BITMODE_BITMODE_Pos;
	NRF_TIMER2->PRESCALER   = TIMER_PRESCALER;
	NRF_TIMER2->CC[3]	   = 0;

	NRF_TIMER2->INTENSET	= TIMER_INTENSET_COMPARE0_Msk | TIMER_INTENSET_COMPARE1_Msk |TIMER_INTENSET_COMPARE2_Msk | TIMER_INTENSET_COMPARE3_Msk;

	NRF_TIMER2->TASKS_START = 1;

	sd_nvic_SetPriority(TIMER2_IRQn, 3);  	/* Seteo la prioridad de la interrupcion. */
	sd_nvic_EnableIRQ(TIMER2_IRQn); 		/* Habilito la interrupcion. */
}

void led_pwm_stop(void){
	NRF_TIMER2->TASKS_STOP  = 1;
	NRF_TIMER2->POWER	   = 0;
	nrf_gpio_pin_set(LED_R_PIN);
	nrf_gpio_pin_set(LED_G_PIN);
	nrf_gpio_pin_set(LED_B_PIN);
}

void led_pwm_r_set(uint8_t r_pwm){
	CRITICAL_REGION_ENTER();
	r = r_pwm;
	CRITICAL_REGION_EXIT();
}

void led_pwm_g_set(uint8_t g_pwm){
	CRITICAL_REGION_ENTER();
	g = g_pwm;
	CRITICAL_REGION_EXIT();
}

void led_pwm_b_set(uint8_t b_pwm){
	CRITICAL_REGION_ENTER();
	b = b_pwm;
	CRITICAL_REGION_EXIT();
}
