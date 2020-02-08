#include <msp430.h>

#include "include/buffer.h"
#include "include/uart.h"
#include "include/bsp.h"
#include "include/adc.h"

/* define all of the buffers used in other files */
__volatile struct buffer uart0rx, uart0tx, uart1rx, uart1tx, i2crx, i2ctx;

/**
 * main.c
 */
int main(void)
{
    /* stop watchdog timer */
	WDTCTL = WDTPW | WDTHOLD;

	/* unlock changes to registers/ports, etc. */
	PM5CTL0 &= ~LOCKLPM5;

	/* initialize the board */
    initializeGpios();

    /* set up uart */
    uart_init();

    /* set up the ADC */
    adc_init();

    /* bootup process - enable all rails */
    enable3V3PowerRail();
    enable24VPowerRail();

    /* take some power readings */
    adc_sample();

    //powerOnHercules();
    powerOnFpga();
    //powerOnMotors();

    //releaseHerculesReset();
    //releaseRadioReset();
    releaseFPGAReset();

    __bis_SR_register(GIE); // Enable all interrupts

    while (1)
    {
        //__bis_SR_register(LPM0_bits + GIE); // LPM0, ADC12_ISR will force exit
        __no_operation();}

	return 0;
}
