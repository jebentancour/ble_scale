#ifndef HX711_h
#define HX711_h

#include <stdint.h>
#include <stdbool.h>

void HX711_init(void);

bool HX711_is_ready(void);

void HX711_set_gain(uint8_t gain);

uint32_t HX711_get_units(uint8_t times);

uint32_t HX711_read(void);

uint32_t HX711_read_average(uint8_t times);

uint32_t HX711_get_value(uint8_t times);

void HX711_tare(uint8_t times);

void HX711_set_scale(uint32_t scale);

uint32_t HX711_get_scale(void);

void HX711_set_offset(uint32_t offset);

uint32_t HX711_get_offset(void);

void HX711_power_down(void);

void HX711_power_up(void);

#endif /* HX711_h */
