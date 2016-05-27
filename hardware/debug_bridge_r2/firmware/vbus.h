#ifndef _VBUS_H
#define _VBUS_H 1

#include <inttypes.h>

/// Vbus modes represent various possible states of the vbus lines
enum vbus_mode {
    VBUS_WAIT,          // First debounced mode not delivered yet.
    VBUS_NONE,          // No valid vbus. This shouldn't happen...where are we getting power from? :)
    VBUS_PIXC_ONLY,     // Only PixC has valid vbus level
    VBUS_DEBUG_ONLY,    // Only debug port has valid vbus level
    VBUS_BOTH,          // Both ports have valid levels
    VBUS_BOTH_DIODE,    // Both valid, but debug is a diode drop under PixC
                        //  This means the debug port is being fed by the load switch's
                        //  body diode, and is not actually powered.
};


/// ADC callback to process samples. This must be registered with adc_init.
void vbus_adc_callback(uint16_t vbus_pixc, uint16_t vbus_dbg);

/// Return the current debounced vbus mode.
enum vbus_mode get_current_vbus_mode();

#endif // _VBUS_H
