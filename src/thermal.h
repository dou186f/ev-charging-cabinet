#ifndef THERMAL_H
#define THERMAL_H

#include <stdint.h>

extern volatile uint16_t adc_last;

extern volatile uint8_t adc_ready;

extern volatile char thermal_mode;

extern volatile uint8_t thermal_cap;

extern volatile uint8_t requested_limit;

extern volatile uint8_t connected_mask;

void adc_init(void);

void adc_start_conversion(void);

void adc_tick(void);

void thermal_update(void);

uint8_t limit_effective(void);

void thermal_reset(void);

#endif
