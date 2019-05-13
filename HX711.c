#include "HX711.h"

#define NRF_LOG_MODULE_NAME "HX711"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#define SCK  15
#define DOUT 14

static uint8_t  GAIN;
static uint32_t OFFSET;
static uint32_t SCALE;

static uint8_t shiftInSlow(uint8_t dataPin, uint8_t clockPin) {
    uint8_t value = 0;

    for(uint8_t i = 0; i < 8; ++i) {
        nrf_gpio_pin_set(clockPin);
        value |= nrf_gpio_pin_read(dataPin) << (7 - i);
        nrf_gpio_pin_clear(clockPin);
    }

    return value;
}

void HX711_init(void){
  SCALE  = 925;
  OFFSET = 152000;

	nrf_gpio_cfg_output(SCK);
	nrf_gpio_cfg_input(DOUT, NRF_GPIO_PIN_NOPULL);

  HX711_set_gain(128);
}

bool HX711_is_ready() {
	return nrf_gpio_pin_read(DOUT) == 0;
}

void HX711_set_gain(uint8_t gain){
	switch (gain) {
		case 128:		// channel A, gain factor 128
			GAIN = 1;
			break;
		case 64:		// channel A, gain factor 64
			GAIN = 3;
			break;
		case 32:		// channel B, gain factor 32
			GAIN = 2;
			break;
	}

	nrf_gpio_pin_clear(SCK);
	HX711_read();
}

static void HX711_wait_ready(void) {
	// Wait for the chip to become ready.
	// This is a blocking implementation and will
	// halt the sketch until a load cell is connected.
	while (!HX711_is_ready()) {}
}


uint32_t HX711_read(void) {
	// Wait for the chip to become ready.
	HX711_wait_ready();

	// Define structures for reading data into.
	uint32_t value = 0;
	uint8_t data[3] = { 0 };
	uint8_t filler = 0x00;

	// Pulse the clock pin 24 times to read the data.
	data[2] = shiftInSlow(DOUT, SCK);
	data[1] = shiftInSlow(DOUT, SCK);
	data[0] = shiftInSlow(DOUT, SCK);

	// Set the channel and the gain factor for the next reading using the clock pin.
	for (unsigned int i = 0; i < GAIN; i++) {
		nrf_gpio_pin_set(SCK);
		nrf_gpio_pin_clear(SCK);
	}

	// Replicate the most significant bit to pad out a 32-bit signed integer
	if (data[2] & 0x80) {
		filler = 0xFF;
	} else {
		filler = 0x00;
	}

	// Construct a 32-bit signed integer
	value = ( (uint32_t)filler << 24
			| (uint32_t)data[2] << 16
			| (uint32_t)data[1] << 8
			| (uint32_t)data[0] );

	return value;
}

uint32_t HX711_read_average(uint8_t times) {
	uint32_t sum = 0;
	for (uint8_t i = 0; i < times; i++) {
		sum += HX711_read();
	}
	return sum / times;
}

uint32_t HX711_get_value(uint8_t times) {
	return HX711_read_average(times) - OFFSET;
}

uint32_t HX711_get_units(uint8_t times) {
	return HX711_get_value(times) / SCALE;
}

void HX711_tare(uint8_t times) {
	double sum = HX711_read_average(times);
	HX711_set_offset(sum);
}

void HX711_set_scale(uint32_t scale) {
	SCALE = scale;
}

uint32_t HX711_get_scale() {
	return SCALE;
}

void HX711_set_offset(uint32_t offset) {
	OFFSET = offset;
}

uint32_t HX711_get_offset() {
	return OFFSET;
}

void HX711_power_down() {
	nrf_gpio_pin_clear(SCK);
	nrf_gpio_pin_set(SCK);
}

void HX711_power_up() {
	nrf_gpio_pin_clear(SCK);
}
