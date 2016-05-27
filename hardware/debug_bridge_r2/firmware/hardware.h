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
#define ADC_VAL(voltage) ((uint16_t)(1023.0 * (voltage) / 6.6) & 0x3ffu)

void set_leds_host(void);       ///< Switch to "HOST" LED
void set_leds_dev(void);        ///< Switch to "DEV" LED
void set_leds_off(void);        ///< No LEDs


void set_charge_enabled(void);  ///< Enable charging from debug port to PixC
void set_charge_disabled(void); ///< Disable charging from debug port to PixC
bool is_charge_enabled(void);   ///< Return whether charging is enabled


void pull_cc1(bool val);        ///< Pull down CC1 if true, else let it float
void pull_cc2(bool val);        ///< Pull down CC2 if true, else let it float


void set_usb_mux_debug(void);   ///< Set USB mux to debug mode (PixC is device)
void set_usb_mux_normal(void);  ///< Set USB mux to normal mode (PixC is host)

#endif // _HARDWARE_H
