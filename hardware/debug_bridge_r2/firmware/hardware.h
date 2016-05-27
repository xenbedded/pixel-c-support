#ifndef _HARDWARE_H
#define _HARDWARE_H 1

#include <stdbool.h>
#include <inttypes.h>

/// Initialize GPIO ports to I/O setting and default value
void init_ports(void);

/// Initialize ADC
void init_adc(void);

/// Return true if a new set of samples is ready
bool samples_ready(void);

/// Get both ADC samples.
/// Only call after samples_ready() returns true for the first time after init.
void get_adc_samples(uint16_t *vbus_pixc, uint16_t *vbus_dbg);

/// Return ADC value for floating-point voltage
#define ADC_VAL(voltage) ((uint16_t)(1023.0 * (voltage) / 6.6) & 0x3ffu)

void set_leds_host(void);       ///< Switch to "HOST" LED
void set_leds_dev(void);        ///< Switch to "DEV" LED
void set_leds_off(void);        ///< No LEDs


void set_charge_enabled(void);  ///< Enable charging from debug port to PixC
void set_charge_disabled(void); ///< Disable charging from debug port to PixC
bool is_charge_enabled(void);   ///< Return whether charging is enabled


void set_usb_mux_debug(void);   ///< Set USB mux to debug mode (PixC is device)
void set_usb_mux_normal(void);  ///< Set USB mux to normal mode (PixC is host)

#endif // _HARDWARE_H
