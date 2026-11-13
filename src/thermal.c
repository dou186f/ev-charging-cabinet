#include <xc.h>
#include <stdint.h>
#include "pragmas.h"
#include "thermal.h"

volatile uint16_t adc_last      = 0u;

volatile uint8_t  adc_ready     = 0u;

volatile char     thermal_mode  = 'N';

volatile uint8_t  thermal_cap   = 24u;

volatile uint8_t  requested_limit = 0u;

volatile uint8_t  connected_mask  = 0u;

static uint8_t adc_tick_count = 0u;

void adc_init(void)
{
    TRISHbits.TRISH4 = 1;

    ADCON0 = 0b00110001;

    ADCON1 = 0x00u;

    ADCON2 = 0b10111110;

    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    IPR1bits.ADIP = 0;
}

void adc_start_conversion(void)
{
    if (!ADCON0bits.GO) {
        ADCON0bits.GO = 1;
    }
}

void adc_tick(void)
{
    adc_tick_count++;
    if (adc_tick_count >= 5u) {
        adc_tick_count = 0u;
        adc_start_conversion();
    }
}

void thermal_update(void)
{
    uint16_t raw;

    if (!adc_ready) {
        return;
    }

    PIE1bits.ADIE = 0;
    raw       = adc_last;
    adc_ready = 0u;
    PIE1bits.ADIE = 1;

    if (raw < 700u) {
        thermal_mode = 'N';
        thermal_cap  = 24u;
    } else if (raw < 900u) {
        thermal_mode = 'D';
        thermal_cap  = 8u;
    } else {
        thermal_mode = 'H';
        thermal_cap  = 0u;
    }
}

uint8_t limit_effective(void)
{
    return (requested_limit < thermal_cap) ? requested_limit : thermal_cap;
}

void thermal_reset(void)
{
    requested_limit = 0u;
    connected_mask  = 0u;
    thermal_mode    = 'N';
    thermal_cap     = 24u;
    adc_ready       = 0u;
    adc_tick_count  = 0u;
}
