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

/// @file hardware.h
/// Hardware access and control functions.

#ifndef _HARDWARE_H
#define _HARDWARE_H 1

#include <stdbool.h>
#include <inttypes.h>

/// Initialize GPIO ports to I/O setting and default value
void init_ports(void);

/// Initialize a 1ms tick timer
void init_tick_timer(void);

/// Return number of 1ms ticks elapsed.
uint16_t get_ticks(void);

/// Initialize ADC
/// @param callback - callback function to be called when a pair of samples has been acquired.
void init_adc(void (*callback)(uint16_t pixc, uint16_t dbg));

/// Return ADC value for floating-point voltage
#define ADC_VAL(voltage) ((uint16_t)(1024.0 * (voltage) / 6.6) & 0x3ffu)

void set_leds_host(void);       ///< Switch to "HOST" LED
void set_leds_dev(void);        ///< Switch to "DEV" LED
void set_leds_off(void);        ///< No LEDs


void set_charge_enabled(void);  ///< Enable charging from debug port to PixC
void set_charge_disabled(void); ///< Disable charging from debug port to PixC
bool is_charge_enabled(void);   ///< Return whether charging is enabled


enum CC_PULL_TYPE { CC_OPEN, CC_DOWN, CC_UP, CC_MID };

void pull_cc1(enum CC_PULL_TYPE val);    ///< Pull CC1 in the specified direction
void pull_cc2(enum CC_PULL_TYPE val);    ///< Pull CC2 in the specified direction


void set_usb_mux_debug(void);   ///< Set USB mux to debug mode (PixC is device)
void set_usb_mux_normal(void);  ///< Set USB mux to normal mode (PixC is host)


void set_hub_reset(bool val);   ///< Set whether the hubs are held in reset
void set_hub1_vbus(bool val);   ///< Set whether hub 1 (downstream of pixel C, for host mode) sees vbus
void set_hub2_vbus(bool val);   ///< Set whether hub 2 (upstream of pixel C, for device mode) sees vbus

#endif // _HARDWARE_H
