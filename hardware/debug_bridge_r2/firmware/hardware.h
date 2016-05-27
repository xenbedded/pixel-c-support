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

#endif // _HARDWARE_H
