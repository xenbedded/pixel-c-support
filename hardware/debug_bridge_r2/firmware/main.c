#include "hardware.h"
#include "vbus.h"
#include <avr/interrupt.h>
#include <util/atomic.h>


static void set_host_mode(void);
static void set_dev_mode(void);


int main(void)
{
    init_ports();
    init_tick_timer();
    init_adc(&vbus_adc_callback);
    sei();

    for(;;) {
        switch(get_current_vbus_mode()) {
        case VBUS_WAIT:
            continue;

        case VBUS_NONE:
            set_leds_off();
            set_charge_disabled();
            set_host_mode();
            break;

        case VBUS_PIXC_ONLY:
        case VBUS_BOTH_DIODE:
            set_leds_host();
            set_charge_disabled();
            set_host_mode();
            break;

        case VBUS_DEBUG_ONLY:
            // Blink LED
            if (get_ticks() % 333 < 166) {
                set_leds_dev();
            } else {
                set_leds_off();
            }
            set_charge_disabled();
            set_dev_mode();
            break;

        case VBUS_BOTH:
            set_leds_dev();
            set_charge_enabled();
            set_dev_mode();
            break;
        }
    }
}


static void set_host_mode(void)
{
    set_usb_mux_normal();
    pull_cc1(true);
    pull_cc2(false);
}


static void set_dev_mode(void)
{
    set_usb_mux_debug();
    pull_cc1(true);
    pull_cc2(true);
}


