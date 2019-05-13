#include "rgb_led.h"

#define NRF_LOG_MODULE_NAME "RGB"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "led_pwm.h"

#include <math.h>

#define M_PI 3.14159265358979323846

enum {
	FULL,
	TOGGLE,
	BREATHE,
	FADE
} mode;

// toggle vars
static	uint16_t togglePeriod;		  // ms
static	bool toggleState;

// breathe vars
static	float two_pi_f;
static	uint16_t breatheGrain;		  // ms
static	uint32_t breatheCount;

// fade vars
static	uint16_t fadeGrain;			 // ms
static  uint16_t fadeInc;

// general vars
static	uint32_t lastUpdate;
static	uint8_t bright;				 // 0 - 255
static  uint8_t r;
static  uint8_t g;
static  uint8_t b;

void rgb_led_init(void){
	mode = FULL;

	led_pwm_init();

	led_pwm_r_set(0);
	led_pwm_g_set(0);
	led_pwm_b_set(0);

	lastUpdate = 0;
	bright = 255;
	r = 0;
	g = 0;
	b = 0;
}

void rgb_led_set_color(led_color_t color){
	switch (color){
		case BLACK:
			r = 0;
			g = 0;
			b = 0;
			break;
		case RED:
			g = 0;
			b = 0;
			r = 255;
			break;
		case GREEN:
			r = 0;
			b = 0;
			g = 52;
			break;
		case BLUE:
			r = 0;
			g = 0;
			b = 255;
			break;
		case YELLOW:
			b = 0;
			r = 255;
			g = 255;
			break;
		case MAGENTA:
			g = 0;
			r = 255;
			b = 255;
			break;
		case CYAN:
			r = 0;
			g = 255;
			b = 255;
			break;
		}
}

void rgb_led_show(void){
	led_pwm_r_set((r*bright)/255);
	led_pwm_g_set((g*bright)/255);
	led_pwm_b_set((b*bright)/255);
}

void rgb_led_full(led_color_t color){
	rgb_led_set_color(color);
	mode = FULL;
	bright = 255;
	rgb_led_show();
}

void rgb_led_toggle(led_color_t color, uint16_t period){
  togglePeriod = period;
	mode = TOGGLE;
	toggleState = true;
	rgb_led_set_color(color);
}

void rgb_led_breathe(led_color_t color, uint16_t period, uint16_t grain){
	breatheGrain = grain;
	two_pi_f = (2 * M_PI * breatheGrain) / period;
	mode = BREATHE;
	breatheCount = 0;
	rgb_led_set_color(color);
}

void rgb_led_fade(uint16_t time, uint16_t grain){
	fadeGrain = grain;
	int numSlices = time / grain;
	fadeInc = 255 / numSlices;
	mode = FADE;
}

void rgb_led_bright(uint8_t br){
	bright = br;
}

void rgb_led_update(uint32_t now){
	switch (mode){
		case FULL:
			// nada
			break;
		case TOGGLE:
			if (now - lastUpdate > togglePeriod){
				lastUpdate = now;
				if (toggleState) bright = 255;
				else bright = 0;
				toggleState = !toggleState;
				rgb_led_show();
			}
			break;
		case BREATHE:
			if (now - lastUpdate > breatheGrain){
				lastUpdate = now;
				float val = (exp(cos(breatheCount * two_pi_f)) - 0.36787944) * 0.42545906;
				bright = 255 * val;
				breatheCount++;
				rgb_led_show();
			}
			break;
		case FADE:
			if (now - lastUpdate > fadeGrain){
				lastUpdate = now;
				if (bright > fadeInc){
					bright -= fadeInc;
				} else {
					rgb_led_full(BLACK);
				}
				rgb_led_show();
			}
			break;
	}
}

void rgb_led_disable(void){
	led_pwm_stop();
}
