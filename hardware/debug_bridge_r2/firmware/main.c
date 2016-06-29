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
#include "vbus.h"
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/atomic.h>
#include <util/delay.h>


static void set_host_mode(void);
static void set_dev_mode(void);

// Reset hubs and CC pins for 500ms if the state changes. Stores the previous state.
static void reset_on_change(enum vbus_mode mode);

int main(void)
{
    wdt_disable();
    init_ports();
    init_tick_timer();
    init_adc(&vbus_adc_callback);
    sei();

    set_hub_reset(false);
    set_charge_disabled();
    set_host_mode();

    for(;;) {
        enum vbus_mode mode = get_current_vbus_mode();

        reset_on_change(mode);

        switch(mode) {
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
    pull_cc1(CC_DOWN);
    pull_cc2(CC_OPEN);
}


static void set_dev_mode(void)
{
    set_usb_mux_debug();
    set_hub1_vbus(false);
    set_hub2_vbus(true);
    pull_cc1(CC_DOWN);
    pull_cc2(CC_DOWN);
}


static void reset_on_change(enum vbus_mode mode)
{
    static enum vbus_mode last_mode = VBUS_WAIT;

    // Hold hubs in reset briefly if the state has changed
    if (mode != last_mode) {
        set_hub_reset(true);
        pull_cc1(CC_MID);
        pull_cc2(CC_MID);
        _delay_ms(500);
        set_hub_reset(false);
    }

    last_mode = mode;
}
