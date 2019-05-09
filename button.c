#include "button.h"

#define NRF_LOG_MODULE_NAME "GPIO"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "nrf_delay.h"
#include "nrf_gpio.h"

#include "clock.h"

#define BUTTON_PIN					0

#define LONG_CLICK_MS 			1250
#define DOUBLE_CLICK_MS 		250
#define DOUBLE_CLICK_MIN_MS 50

static volatile uint8_t* m_gpio_button_flag;
static volatile uint8_t* m_gpio_long_button_flag;
static volatile uint8_t* m_gpio_double_button_flag;

static uint32_t last_click = 0;
static uint32_t pulse_start = 0;
static uint32_t pulse_stop = 0;
static int	  	pulse_len = 0;
static uint8_t  long_timeout = 0;
static uint8_t  old_in = 1;
static uint8_t  new_in = 1;

void gpio_init(){
	nrf_gpio_cfg_input(BUTTON_PIN, NRF_GPIO_PIN_PULLUP);
}

void gpio_button_set_flag(volatile uint8_t* main_button_flag){
	m_gpio_button_flag = main_button_flag;
}

void gpio_long_button_set_flag(volatile uint8_t* main_long_button_flag){
	m_gpio_long_button_flag = main_long_button_flag;
}

void gpio_double_button_set_flag(volatile uint8_t* main_double_button_flag){
	m_gpio_double_button_flag = main_double_button_flag;
}

void gpio_process(uint32_t now){
	// debouncing
	new_in = nrf_gpio_pin_read(BUTTON_PIN);
	nrf_delay_ms(1);
	if (new_in != nrf_gpio_pin_read(BUTTON_PIN)){
			return;
	}

	if (old_in != new_in){
		if (!new_in){
			pulse_start = now;
		} else {
			pulse_stop = now;
			pulse_len = pulse_stop - pulse_start;
			if (pulse_len < LONG_CLICK_MS){
				if ((now - last_click) <= DOUBLE_CLICK_MS && (now - last_click) > DOUBLE_CLICK_MIN_MS){
					*m_gpio_double_button_flag = 1;
				} else {
					*m_gpio_button_flag = 1;
				}
				last_click = now;
			}
			long_timeout = 0;
		}
	}

	if (!new_in && !long_timeout){
		pulse_len = now - pulse_start;
		if (pulse_len > LONG_CLICK_MS){
			*m_gpio_long_button_flag = 1;
			long_timeout = 1;
		}
	}

	old_in = new_in;
}
