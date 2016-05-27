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


static volatile bool        have_samples = false;
static volatile uint16_t    adc_value_pixc = 0;
static volatile uint16_t    adc_value_dbg = 0;


void init_adc(void)
{
    ADMUX = MUX_VBUS_PIXC_SENSE;    // Start on first input, ref = vcc
    ADCSRA = (1 << ADEN) | (1 << ADIE) | /* ADPS */ 0;
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
