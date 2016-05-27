#include "hardware.h"
#include <avr/interrupt.h>


/// Vbus modes represent various possible states of the vbus lines
enum vbus_mode {
    VBUS_WAIT,          // ADC is not ready yet
    VBUS_NONE,          // No valid vbus. This shouldn't happen...where are we getting power from? :)
    VBUS_PIXC_ONLY,     // Only PixC has valid vbus level
    VBUS_DEBUG_ONLY,    // Only debug port has valid vbus level
    VBUS_BOTH,          // Both ports have valid levels
    VBUS_BOTH_DIODE,    // Both valid, but debug is a diode drop under PixC
                        //  This means the debug port is being fed by the load switch's
                        //  body diode, and is not actually powered.
};


static enum vbus_mode get_vbus_mode(void);


int main(void)
{
    init_ports();
    init_adc();
    sei();

    for(;;) {
        switch(get_vbus_mode()) {
        case VBUS_WAIT:
        case VBUS_NONE:
            set_leds_off();
            set_charge_disabled();
            set_usb_mux_normal();
            break;

        case VBUS_PIXC_ONLY:
        case VBUS_BOTH_DIODE:
            set_leds_host();
            set_charge_disabled();
            set_usb_mux_normal();
            break;

        case VBUS_DEBUG_ONLY:
            // TODO: blink LED?
            set_leds_dev();
            set_charge_disabled();
            set_usb_mux_debug();
            break;

        case VBUS_BOTH:
            set_leds_dev();
            set_charge_enabled();
            set_usb_mux_debug();
            break;
        }
    }
}


static enum vbus_mode get_vbus_mode(void)
{
    uint16_t vbus_pixc, vbus_dbg;

    if (!samples_ready())
        return VBUS_WAIT;

    get_adc_samples(&vbus_pixc, &vbus_dbg);

    bool pixc_valid = (vbus_pixc >= ADC_VAL(4.0));
    bool dbg_valid  = (vbus_dbg  >= ADC_VAL(4.0));

    static const uint16_t adc_diode_min = ADC_VAL(0.45);
    static const uint16_t adc_diode_max = ADC_VAL(0.85);

    bool diode = (vbus_dbg > (vbus_pixc - adc_diode_max)) &&
                 (vbus_dbg < (vbus_pixc - adc_diode_min)) &&
                 is_charge_enabled();

    //  PIXC    DBG     DIODE   OUT
    //  0       0       0       VBUS_NONE
    //  0       0       1       VBUS_NONE (!)
    //  0       1       0       VBUS_DEBUG_ONLY
    //  0       1       1       VBUS_DEBUG_ONLY (!)
    //  1       0       0       VBUS_PIXC_ONLY
    //  1       0       1       VBUS_BOTH_DIODE
    //  1       1       0       VBUS_BOTH
    //  1       1       1       VBUS_BOTH_DIODE
    //
    //  (PIXC & !DBG & DIODE) could mean pixc's vbus was marginal, such that pixc
    //  vbus was valid but vbus-diode was not. This is still the diode case.

    if (!pixc_valid && !dbg_valid) {
        return VBUS_NONE;
    } else if (!pixc_valid && dbg_valid) {
        return VBUS_DEBUG_ONLY;
    } else if (pixc_valid && !dbg_valid && !diode) {
        return VBUS_PIXC_ONLY;
    } else if (pixc_valid && dbg_valid && !diode) {
        return VBUS_BOTH;
    } else if (pixc_valid && diode) {
        return VBUS_BOTH_DIODE;
    } else {
        // should not happen, all cases should be covered above.
        return VBUS_NONE;
    }
}
