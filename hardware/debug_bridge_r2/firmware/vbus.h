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

/// @file vbus.h
/// Vbus sensing and analysis (high level, does not interact directly with ADC)

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
