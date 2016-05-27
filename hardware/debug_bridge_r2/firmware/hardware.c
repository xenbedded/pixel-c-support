#include "hardware.h"
#include "pin_io.h"
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdlib.h>

void init_ports(void)
{
    DDRA = 0;
    DDRB = 0;
    PORTA = 0;
    PORTB = 0;

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
    PINPUT(CC1);
    PINPUT(CC2);
}


void init_tick_timer(void)
{
    // Waveform mode = CTC (clear timer on compare match)
    TCCR0A = (1 << WGM01);
    // Prescaler = 64, fcpu = 8 MHz gives ftimer = 125 kHz
    TCCR0B = (1 << CS01) | (1 << CS00);
    // Count to 124 gives 125 kHz / 125 = 1 kHz overflows
    OCR0A = 124;
    // Interrupt on compare match
    TIMSK0 = (1 << OCIE0A);
}


static volatile uint16_t ticks = 0;


ISR(TIM0_COMPA_vect)
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


void pull_cc1(bool val)
{
    if (val) {
        PLOW(CC1);
        POUTPUT(CC1);
    } else {
        PINPUT(CC1);
    }
}


void pull_cc2(bool val)
{
    if (val) {
        PLOW(CC2);
        POUTPUT(CC2);
    } else {
        PINPUT(CC2);
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


void init_adc(void (*callback)(uint16_t pixc, uint16_t dbg))
{
    ADMUX = MUX_VBUS_PIXC_SENSE;    // Start on first input, ref = vcc

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
        ADMUX = MUX_VBUS_DBG_SENSE;
        channel_id = 1;
        break;
    case 1:
        adc_value_dbg = ADC;
        ADMUX = MUX_VBUS_PIXC_SENSE;
        channel_id = 0;
        adc_callback(adc_value_pixc, adc_value_dbg);
        break;
    }
    ADCSRA |= (1 << ADSC);
}
