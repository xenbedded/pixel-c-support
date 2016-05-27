#include "hardware.h"
#include "pin_io.h"
#include <avr/interrupt.h>
#include <util/atomic.h>

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


void set_usb_mux_debug(void)
{
    PHIGH(USBMUX);
}


void set_usb_mux_normal(void)
{
    PLOW(USBMUX);
}


static volatile bool        have_samples = false;
static volatile uint16_t    adc_value_pixc = 0;
static volatile uint16_t    adc_value_dbg = 0;


void init_adc(void)
{
    ADMUX = MUX_VBUS_PIXC_SENSE;    // Start on first input, ref = vcc

    // Clock is 8 MHz. Divide by 64 to get within the 50..200kHz range
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1);
    ADCSRB = 0;

    // Disable digital input buffers on analog pins
    DIDR0 = (1 << PIN_VBUS_DBG_SENSE) | (1 << PIN_VBUS_PIXC_SENSE);

    // Start conversion
    have_samples = false;
    ADCSRA |= (1 << ADSC);
}


ISR(ADC_vect)
{
    static uint8_t channel_id = 0;

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
        have_samples = true;
        break;
    }
    ADCSRA |= (1 << ADSC);
}


bool samples_ready(void)
{
    return have_samples;
}


void get_adc_samples(uint16_t *vbus_pixc, uint16_t *vbus_dbg)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        *vbus_pixc = adc_value_pixc;
        *vbus_dbg = adc_value_dbg;
    }
}
