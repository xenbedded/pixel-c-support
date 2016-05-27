#include "hardware.h"
#include "vbus.h"
#include <avr/interrupt.h>
#include <util/atomic.h>


static void set_host_mode(void);
static void set_dev_mode(void);

// Reset hubs for 500ms if the state changes. Stores the previous state.
static void reset_hubs_on_change(enum vbus_mode mode);

int main(void)
{
    init_ports();
    init_tick_timer();
    init_adc(&vbus_adc_callback);
    sei();

    for(;;) {
        enum vbus_mode mode = get_current_vbus_mode();

        reset_hubs_on_change(mode);

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
    set_hub1_vbus(true);
    set_hub2_vbus(false);
    pull_cc1(true);
    pull_cc2(false);
}


static void set_dev_mode(void)
{
    set_usb_mux_debug();
    set_hub1_vbus(false);
    set_hub2_vbus(true);
    pull_cc1(true);
    pull_cc2(true);
}


static void reset_hubs_on_change(enum vbus_mode mode)
{
    static const uint16_t hub_reset_duration = 500u; // Time in ms to hold hubs in reset

    static enum vbus_mode last_mode = VBUS_WAIT;
    static uint16_t hub_reset_timestamp = 0;
    static bool hub_reset = false;
    static bool hub_reset_overflow = false;

    uint16_t ticks = get_ticks();

    // Hold hubs in reset briefly if the state has changed
    if (mode != last_mode) {
        set_hub_reset(true);
        hub_reset_timestamp = ticks;
        hub_reset_overflow = (hub_reset_timestamp + hub_reset_duration < ticks);
        hub_reset = true;
    } else if (hub_reset) {
        bool done;
        if (hub_reset_overflow) {
            done = (ticks >= hub_reset_timestamp + hub_reset_duration) && (ticks < hub_reset_timestamp);
        } else {
            done = (ticks >= hub_reset_timestamp + hub_reset_duration);
        }
        if (done) {
            set_hub_reset(false);
            hub_reset = false;
        }
    }
}
