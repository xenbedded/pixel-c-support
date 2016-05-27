#ifndef _PIN_IO_H
#define _PIN_IO_H 1

#include <avr/io.h>

#define PRT_LED_A           A
#define PIN_LED_A           6

#define PRT_LED_B           A
#define PIN_LED_B           4

#define PRT_USBMUX          A
#define PIN_USBMUX          5

#define PRT_HUBnRST         A
#define PIN_HUBnRST         3

#define PRT_DBG_PWR         A
#define PIN_DBG_PWR         0

#define PRT_VBUSDET1        B
#define PIN_VBUSDET1        2

#define PRT_VBUSDET2        A
#define PIN_VBUSDET2        2

#define PRT_VBUS_PIXC_SENSE A
#define PIN_VBUS_PIXC_SENSE 1
#define MUX_VBUS_PIXC_SENSE (1 << MUX0)

#define PRT_VBUS_DBG_SENSE  A
#define PIN_VBUS_DBG_SENSE  7
#define MUX_VBUS_DBG_SENSE  ((1 << MUX0) | (1 << MUX1) | (1 << MUX2))

#define PRT_CC1             B
#define PIN_CC1             1

#define PRT_CC2             B
#define PIN_CC2             0

#define PRT_MCUnRST         B
#define PIN_MCUnRST         3


/// Join and evaluate two macros
#define _HELP_CONCAT(x, y) x ## y
#define _CONCAT(x, y)  _HELP_CONCAT(x, y)

/// Extract and return the DDR register for a pin by name
#define _DDR_FOR_PIN(pin) _CONCAT(DDR, _CONCAT(PRT_, pin))

/// Extract and return the PORT register for a pin by name
#define _PORT_FOR_PIN(pin) _CONCAT(PORT, _CONCAT(PRT_, pin))

/// Extract and return the PIN register for a pin by name
#define _PIN_FOR_PIN(pin) _CONCAT(PIN, _CONCAT(PRT_, pin))

/// Extract and return the pin number for a pin by name
#define _NUM_FOR_PIN(pin) _CONCAT(PIN_, pin)



/// Configure a pin as output
#define POUTPUT(pin)    do { _DDR_FOR_PIN(pin)  |=   1 << (_NUM_FOR_PIN(pin)) ; } while (0)

/// Configure a pin as input
#define PINPUT(pin)     do { _DDR_FOR_PIN(pin)  &= ~(1 << (_NUM_FOR_PIN(pin))); } while (0)

/// Write 1 to a pin
#define PHIGH(pin)      do { _PORT_FOR_PIN(pin) |=   1 << (_NUM_FOR_PIN(pin)) ; } while (0)

/// Write 0 to a pin
#define PLOW(pin)       do { _PORT_FOR_PIN(pin) &= ~(1 << (_NUM_FOR_PIN(pin))); } while (0)

/// Read a pin
#define PGET(pin)       ( _PIN_FOR_PIN(pin) & (1 << (_NUM_FOR_PIN(pin))) )

/// Read a pin's output value
#define PGETOUT(pin)    ( _PORT_FOR_PIN(pin) & (1 << (_NUM_FOR_PIN(pin))) )

#endif // _PIN_IO_H
