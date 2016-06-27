// Copyright (c) 2016 Assured Information Security, Inc.
// Author: Chris Pavlina <pavlina.chris@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "hardware.h"
#include "pin_io.h"
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdlib.h>

void init_ports(void)
{
    DDRB = 0;
    DDRC = 0;
    DDRD = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;

    // No LEDs by default
    PINPUT(LED_A);
    PINPUT(LED_B);

    // Mux in non-debug by default
    PLOW(USBMUX);
    POUTPUT(USBMUX);

    // Charging power off by default
    PLOW(DBG_PWR);
    POUTPUT(DBG_PWR);

    // Hubs held in reset
    PLOW(HUBnRST);
    PLOW(VBUSDET1);
    PLOW(VBUSDET2);
    POUTPUT(HUBnRST);
    POUTPUT(VBUSDET1);
    POUTPUT(VBUSDET2);

    // Analog inputs
    PINPUT(VBUS_PIXC_SENSE);
    PINPUT(VBUS_DBG_SENSE);

    // USB-C CC pins
    PINPUT(CC1PD);
    PINPUT(CC2PD);
    PINPUT(CC1PU);
    PINPUT(CC2PU);
}


void init_tick_timer(void)
{
    // Waveform mode = CTC (clear timer on compare match)
    // Prescaler = 64, fcpu = 8 MHz gives ftimer = 125 kHz
    TCCR0A = (1 << CTC0) | (1 << CS01) | (1 << CS00);
    // Count to 124 gives 125 kHz / 125 = 1 kHz overflows
    OCR0A = 124;
    // Interrupt on compare match
    TIMSK0 = (1 << OCIE0A);
}


static volatile uint16_t ticks = 0;


ISR(TIMER0_COMPA_vect)
{
    ++ticks;
}


uint16_t get_ticks(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        return ticks;
    }
    return 0; // shut up warning
}


void set_leds_host(void)
{
    PHIGH(LED_A);
    PLOW(LED_B);
    POUTPUT(LED_A);
    POUTPUT(LED_B);
}


void set_leds_dev(void)
{
    PLOW(LED_A);
    PHIGH(LED_B);
    POUTPUT(LED_A);
    POUTPUT(LED_B);
}


void set_leds_off(void)
{
    PINPUT(LED_A);
    PINPUT(LED_B);
}


void set_charge_enabled(void)
{
    PHIGH(DBG_PWR);
}


void set_charge_disabled(void)
{
    PLOW(DBG_PWR);
}


bool is_charge_enabled(void)
{
    return PGETOUT(DBG_PWR);
}


void pull_cc1(enum CC_PULL_TYPE val)
{
    switch(val) {
    case CC_OPEN:
        PLOW(CC1PD);
        PLOW(CC1PU);
        PINPUT(CC1PD);
        PINPUT(CC1PU);
        break;
    case CC_DOWN:
        PLOW(CC1PD);
        PLOW(CC1PU);
        POUTPUT(CC1PD);
        PINPUT(CC1PU);
        break;
    case CC_UP:
        PLOW(CC1PD);
        PHIGH(CC1PU);
        POUTPUT(CC1PU);
        PINPUT(CC1PD);
        break;
    case CC_MID:
        PLOW(CC1PD);
        POUTPUT(CC1PD);
        PHIGH(CC1PU);
        POUTPUT(CC1PU);
        break;
    }
}


void pull_cc2(enum CC_PULL_TYPE val)
{
    switch(val) {
    case CC_OPEN:
        PLOW(CC2PD);
        PLOW(CC2PU);
        PINPUT(CC2PD);
        PINPUT(CC2PU);
        break;
    case CC_DOWN:
        PLOW(CC2PD);
        PLOW(CC2PU);
        POUTPUT(CC2PD);
        PINPUT(CC2PU);
        break;
    case CC_UP:
        PLOW(CC2PD);
        PHIGH(CC2PU);
        POUTPUT(CC2PU);
        PINPUT(CC2PD);
        break;
    case CC_MID:
        PLOW(CC2PD);
        POUTPUT(CC2PD);
        PHIGH(CC2PU);
        POUTPUT(CC2PU);
        break;
    }
}


void set_usb_mux_debug(void)
{
    PHIGH(USBMUX);
}


void set_usb_mux_normal(void)
{
    PLOW(USBMUX);
}


void set_hub_reset(bool val)
{
    PVAL(HUBnRST, !val);
}


void set_hub1_vbus(bool val)
{
    PVAL(VBUSDET1, val);
}


void set_hub2_vbus(bool val)
{
    PVAL(VBUSDET2, val);
}


static void (* volatile adc_callback)(uint16_t pixc, uint16_t dbg) = NULL;


static void adc_muxsel(uint8_t mux)
{
    ADMUX = (1 << REFS0) | (mux & 0x0f);
}


void init_adc(void (*callback)(uint16_t pixc, uint16_t dbg))
{
    adc_muxsel(MUX_VBUS_PIXC_SENSE);    // Start on first input, ref = vcc

    // Clock is 8 MHz. Divide by 64 to get within the 50..200kHz range
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1);
    ADCSRB = 0;

    // Disable digital input buffers on analog pins
    DIDR0 = (1 << PIN_VBUS_DBG_SENSE) | (1 << PIN_VBUS_PIXC_SENSE);

    // Start conversion
    adc_callback = callback;
    ADCSRA |= (1 << ADSC);
}


ISR(ADC_vect)
{
    static uint8_t channel_id = 0;
    static uint16_t adc_value_pixc = 0;
    static uint16_t adc_value_dbg = 0;

    switch(channel_id)
    {
    case 0:
        adc_value_pixc = ADC;
        adc_muxsel(MUX_VBUS_DBG_SENSE);
        channel_id = 1;
        break;
    case 1:
        adc_value_dbg = ADC;
        adc_muxsel(MUX_VBUS_PIXC_SENSE);
        channel_id = 0;
        if (adc_callback)
            adc_callback(adc_value_pixc, adc_value_dbg);
        break;
    }
    ADCSRA |= (1 << ADSC);
}
