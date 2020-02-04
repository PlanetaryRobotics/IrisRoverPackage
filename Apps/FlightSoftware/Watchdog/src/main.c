#include <msp430.h>
#include "driverlib.h"
#include "bsp.h"

uint8_t g_testData = 0xAA;

/**
 * main.c
 */
int main(void)
{
	WDT_A_hold(WDT_A_BASE);

	initializeGpios();

    enable24VPowerRail();
    enable3V3PowerRail();


    //Set DCO frequency to 16MHz
    CS_setDCOFreq(CS_DCORSEL_1, CS_DCOFSEL_4);

    CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);

    /* set up uart */
	EUSCI_A_UART_initParam uartParam;

	// Configure baud-rate to run at 115200
	uartParam.clockPrescalar = 8; //1667 / 16
	uartParam.firstModReg = 10;
	uartParam.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
	uartParam.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
	uartParam.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
	uartParam.parity = EUSCI_A_UART_NO_PARITY;
	uartParam.secondModReg = 0xFB;
	uartParam.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
	uartParam.uartMode = EUSCI_A_UART_MODE;

    // Initialize the UART registers
    EUSCI_A_UART_init(EUSCI_A0_BASE, &uartParam);
    EUSCI_A_UART_init(EUSCI_A1_BASE, &uartParam);

    powerOnHercules();
    powerOnFpga();
    powerOnMotors();
    powerOnRadio();

    releaseHerculesReset();
    releaseRadioReset();
    releaseFPGAReset();

    // Enable UART module
    EUSCI_A_UART_enable(EUSCI_A0_BASE); // Hercules
    EUSCI_A_UART_enable(EUSCI_A1_BASE); // Lander

    // Clear interrupt
    EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE,
                                EUSCI_A_UART_RECEIVE_INTERRUPT);

    // Enable USCI_A0 RX interrupt
    EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE,
                                 EUSCI_A_UART_RECEIVE_INTERRUPT);

    __bis_SR_register(GIE); // Enable all interrupts

    while (1){
        asm("  NOP");
    }
}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void){
  switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG)){
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
        g_testData = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
        EUSCI_A_UART_transmitData(EUSCI_A1_BASE, g_testData);
        break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
  }
}

