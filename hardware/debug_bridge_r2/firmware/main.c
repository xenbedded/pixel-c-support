#include "hardware.h"
#include <avr/interrupt.h>

int main(void)
{
    init_ports();
    init_adc();
    sei();

    // Temporary
    volatile uint16_t a = 0, b = 0;
    while (!samples_ready());
    get_adc_samples(&a, &b);
    for(;;);
}
