#include <msp430.h>

#include "include/buffer.h"
#include "include/uart.h"
#include "include/bsp.h"

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

    /* set up uart */
    uart_init();

    initializeGpios();

    enable24VPowerRail();
    powerOnHercules();
    powerOnFpga();
    powerOnMotors();

    releaseHerculesReset();
    releaseRadioReset();
    releaseFPGAReset();
	
	// Enable changes to port registers
    PM5CTL0 &= ~LOCKLPM5;

    // setup buttons
    P5REN |= BIT6;                 // Enable internal pull-up/down resistors
    P5OUT |= BIT6;                 //Select pull-up mode for P5.6
    P5REN |= BIT5;                 // Enable internal pull-up/down resistors
    P5OUT |= BIT5;                 //Select pull-up mode for P5.5

    P5IE |= BIT6;                    // P5.6 interrupt enabled
    P5IES |= BIT6;                  // P5.6 Hi/lo edge
    P5IFG &= ~BIT6;               // P5.6 IFG cleared

    P5IE |= BIT5;                    // P5.5 interrupt enabled
    P5IES |= BIT5;                  // P5.5 Hi/lo edge
    P5IFG &= ~BIT5;               // P5.5 IFG cleared

    // setup timer
    TA0CCTL0 = CCIE;                             // CCR0 interrupt enabled
    TA0CTL = TASSEL_2 + MC_1 + ID_3;           // SMCLK/8, upmode
    TA0CCR0 =  10000;                           // 12.5 Hz (pretty sure its not)

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode

    __bis_SR_register(LPM0_bits + GIE); // Enable all interrupts

    while (1)
    {
        //__bis_SR_register(LPM0_bits + GIE); // LPM0, ADC12_ISR will force exit
        __no_operation();}

	return 0;
}


// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    P1OUT ^= BIT0;
}


// Port 5 interrupt service routine
// maximum queue of 1 interrupt for each thing
#pragma vector=PORT5_VECTOR
__interrupt void Port_5(void)
{
    if (P5IFG & BIT5) {
        // toggle the LED
        P1OUT &= ~BIT1;
        P5IFG &= ~(BIT5);/*
        unsigned int i = 0;
        for (i = 0; i < 60000; i++) __no_operation();
        for (i = 0; i < 60000; i++) __no_operation();
        for (i = 0; i < 60000; i++) __no_operation();
        for (i = 0; i < 60000; i++) __no_operation();
        for (i = 0; i < 60000; i++) __no_operation();
        for (i = 0; i < 60000; i++) __no_operation();
        for (i = 0; i < 60000; i++) __no_operation();
        for (i = 0; i < 60000; i++) __no_operation();*/
    }
    if (P5IFG & BIT6) {
        // clear IFG for both P5.5 and P5.6
        P1OUT &= ~BIT1;
        P5IFG &= ~(BIT6);
    }
}

