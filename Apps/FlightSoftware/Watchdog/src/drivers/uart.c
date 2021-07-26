/*
 * Code for interfacing with UART protocol hardware module.
 *
 * UART communication uses eUSCI_A0 (Hercules) and eUSCI_A1 (Lander).
 *
 * 9600 baud, 1 stop bit, no parity
 * 
 * References: SLAU367P (https://www.ti.com/lit/ug/slau367p/slau367p.pdf)
 * 
 * Author: mschnur
 */

#include <msp430.h>
#include "drivers/uart.h"
#include "utils/ring_buffer.h"
#include "common.h"
#include "flags.h"

//###########################################################
// Private types
//###########################################################

typedef struct UART__Registers
{
    volatile uint16_t * const UCAxTXBUF;
    volatile uint16_t * const UCAxRXBUF;
    volatile uint16_t * const UCAxIV;
    volatile uint16_t * const UCAxIE;
    volatile uint16_t * const UCAxIFG;

} UART__Registers;

struct UART__State
{
    BOOL initialized;
    const UART__Registers* registers;

    RingBuffer* txRingBuff;
    RingBuffer* rxRingBuff;

    uint8_t gotRxLoopFlag;
};

//###########################################################
// Private globals and constants
//###########################################################

static const UART__Registers uart0Registers = {
    .UCAxTXBUF = &UCA0TXBUF,
    .UCAxRXBUF = &UCA0RXBUF,
    .UCAxIV = &UCA0IV,
    .UCAxIE = &UCA0IE,
    .UCAxIFG = &UCA0IFG
};

static const UART__Registers uart1Registers = {
    .UCAxTXBUF = &UCA1TXBUF,
    .UCAxRXBUF = &UCA1RXBUF,
    .UCAxIV = &UCA1IV,
    .UCAxIE = &UCA1IE,
    .UCAxIFG = &UCA1IFG
};

static UART__State uart0State = {
    .initialized = FALSE,
    .registers = &uart0Registers,
    .txRingBuff = NULL,
    .rxRingBuff = NULL,
    .gotRxLoopFlag = FLAG_UART0_RX_PACKET
};

static UART__State uart1State = {
    .initialized = FALSE,
    .registers = &uart1Registers,
    .txRingBuff = NULL,
    .rxRingBuff = NULL,
    .gotRxLoopFlag = FLAG_UART1_RX_PACKET
};

//###########################################################
// Private function declarations
//###########################################################

static void UART__clockInit(void);
static UART__Status UART__initState(UART__State* state, UART__Buffers* buffers);
static void UART__uart0Init(void);
static void UART__uart1Init(void);
inline static void UART__interruptHandler(UART__State* uartState);

//###########################################################
// Public function definitions
//###########################################################

UART__Status UART__init(UART__Config* config,
                        UART__State** uart0StateOut,
                        UART__State** uart1StateOut)
{
    if (NULL == config
        || NULL == uart0StateOut
        || NULL == uart1StateOut) {
        return UART__STATUS__ERROR_NULL;
    }

    if (uart0State.initialized && uart0State.initialized) {
        return UART__STATUS__ERROR_ALREADY_INITIALIZED;
    }

    // First, initialize the clocks. This could be before or after the state structures,
    // but should be before initializing the uarts themselves
    UART__clockInit();


    // Second, initialize the state structures for both uarts
    UART__Status uartStatus = UART__initState(&uart0State, &(config->uart0Buffers));

    if (UART__STATUS__SUCCESS != uartStatus) {
        return uartStatus;
    }

    uartStatus = UART__initState(&uart1State, &(config->uart1Buffers));

    if (UART__STATUS__SUCCESS != uartStatus) {
        return uartStatus;
    }

    // Third, initialize the uarts themselves
    UART__uart0Init();
    UART__uart1Init();

    *uart0StateOut = &uart0State;
    *uart1StateOut = &uart1State;

    return UART__STATUS__SUCCESS;
 }


UART__Status UART__transmit(UART__State* uartState, const uint8_t* data, size_t dataLen)
{
    if (NULL == uartState || NULL == data) {
        return UART__STATUS__ERROR_NULL;
    }

    if (!(uartState->initialized)) {
        return UART__STATUS__ERROR_NOT_INITIALIZED;
    }

    if (0 == dataLen) {
        return UART__STATUS__ERROR_ZERO_LENGTH_DATA;
    }

    size_t numFree = 0;

    // Disable the tx interrupt for this uart while we get the number of free bytes in the ring buffer
    uint16_t existingTxInterruptBitState = *(uartState->registers->UCAxIE) & UCTXIE;
    *(uartState->registers->UCAxIE) &= ~UCTXIE;

    numFree = RingBuffer__freeCount(uartState->txRingBuff);

    // Re-enable the tx interrupt only if it was previously enabled
    *(uartState->registers->UCAxIE) |= existingTxInterruptBitState;

    if (dataLen > numFree) {
        return UART__STATUS__ERROR_NOT_ENOUGH_SPACE;
    }

    // We have enough space in the ring buffer for all of our buffer data, so
    // push it all into the buffer
    for (size_t i = 0U; i < dataLen; ++i) {
        RingBuffer__Status rbStatus = RingBuffer__put(uartState->txRingBuff, data[i]);

        if (RB__STATUS__SUCCESS != rbStatus) {
            return UART__STATUS__ERROR_RB_PUT_FAILURE;
        }
    }

    // If the previous state of the TX interrupt enable pin was disabled, then we manually
    // trigger the TX interrupt so that the first byte available in the ring buffer will
    // be written to UCAxTXBUF, which will resume the asynchronous TX loop
    if (!existingTxInterruptBitState) {
        *(uartState->registers->UCAxIFG) |= UCTXIFG;
    }

    // We've written data to the tx ring buffer, so make sure that the tx interrupt is enabled
    *(uartState->registers->UCAxIE) |= UCTXIE;

    return UART__STATUS__SUCCESS;
}

UART__Status UART__receive(UART__State* uartState,
                           uint8_t* data,
                           size_t dataLen,
                           size_t* numReceived)
{
    if (NULL == uartState || NULL == data || NULL == numReceived) {
        return UART__STATUS__ERROR_NULL;
    }

    if (!(uartState->initialized)) {
        return UART__STATUS__ERROR_NOT_INITIALIZED;
    }

    // Make sure numReceived starts at zero
    *numReceived = 0U;

    // We have enough space in the ring buffer for all of our buffer data, so
    // push it all into the buffer
    for (size_t i = 0U; i < dataLen; ++i) {
        RingBuffer__Status rbStatus = RingBuffer__get(uartState->rxRingBuff, data + i);

        if (RB__STATUS__SUCCESS == rbStatus) {
            // we got another byte
            (*numReceived)++;
        } else if (RB__STATUS__ERROR__EMPTY == rbStatus) {
            // Have gotten all bytes that have been received so far
            break;
        } else {
            return UART__STATUS__ERROR_RB_GET_FAILURE;
        }
    }

    return UART__STATUS__SUCCESS;
}

//###########################################################
// Private function definitions
//###########################################################

/**
 * Initialize clocks for UART. necessary and should only be called once, at boot.
 */
static void UART__clockInit()
{
    // Unlock all CS registers
    CSCTL0_H = CSKEY_H;

    // Sets DCO Range select to high speed
    CSCTL1 = DCORSEL;

    // Sets DCO frequency to 8 MHz.
    // It is 8 MHz because DCORSEL was set; if DCORSEL was not set, this (a DCOFSEL of 3) would set DCO to 4 MHz.
    // Note that DCOFSEL_3 means setting the DCOFSEL bits to the decimal value 3, i.e. DCOFSEL is 011b
    CSCTL1 |= DCOFSEL_3;
    
    // Sets the source of ACLK to VLOCLK.
    // Per Section 8.12.3.4 (pg 42) of the datasheet, this has a typical frequency of 9.4 kHz (so nominally 10kHz)
    //    (source: https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf)
    CSCTL2 = SELA__VLOCLK;

    // Sets the source of SMCLK to DCOCLK.
    CSCTL2 |= SELS__DCOCLK;

    // Sets the source of MCLK to DCOCLK.
    CSCTL2 |= SELM__DCOCLK;
    
    // Sets an input divider of 1 (i.e. no division) for ACLK
    // Note that unlike DCOFSEL_3, DIVA_1 does NOT mean setting the DIVA bits to the decimal value of 1,
    //    but instead means that the input divider is 1 (which means setting the DIVA bits to the decimal
    //    value of 0).
    CSCTL3 = DIVA__1;

    // Sets an input divider of 1 (i.e. no division) for SMCLK
    // Note that unlike DCOFSEL_3, DIVS_1 does NOT mean setting the DIVS bits to the decimal value of 1,
    //    but instead means that the input divider is 1 (which means setting the DIVS bits to the decimal
    //    value of 0).
    CSCTL3 |= DIVS__1;

    // Sets an input divider of 1 (i.e. no division) for MCLK
    // Note that unlike DCOFSEL_3, DIVM_1 does NOT mean setting the DIVM bits to the decimal value of 1,
    //    but instead means that the input divider is 1 (which means setting the DIVM bits to the decimal
    //    value of 0).
    CSCTL3 |= DIVM__1;

    // Lock all CS registers
    CSCTL0_H = 0;
}

static UART__Status UART__initState(UART__State* state, UART__Buffers* buffers)
{
    if (NULL == state
        || NULL == buffers
        || NULL == buffers->txBuffer
        || NULL == buffers->rxBuffer) {
        return UART__STATUS__ERROR_NULL;
    }

    // Shouldn't be possible to only have one uart initialized but not the other,
    // but make sure we don't don't re-iniitalize one anyway
    if (!(state->initialized)) {
        RingBuffer__Status rbStatus = RingBuffer__init(&(state->txRingBuff),
                                                       buffers->txBuffer,
                                                       buffers->txBufferSize);

        if (RB__STATUS__SUCCESS != rbStatus) {
            return UART__STATUS__ERROR_RB_INIT_FAILURE;
        }

        rbStatus = RingBuffer__init(&(state->rxRingBuff),
                                    buffers->rxBuffer,
                                    buffers->rxBufferSize);

        if (RB__STATUS__SUCCESS != rbStatus) {
            return UART__STATUS__ERROR_RB_INIT_FAILURE;
        }

        state->initialized = TRUE;
    }

    return UART__STATUS__SUCCESS;
}

/**
 * Initialize UART0 (Hercules <-> watchdog)
 */
static void UART__uart0Init()
{
    // Put eUSCI_A0 in reset
    UCA0CTLW0 = UCSWRST;                    

    /* Setup for eUSCI_A0 and eUSCI_A1 */
    /* On the MSP430FR5994, pin P2.0 is used for TX and pin P2.1 is used for RX
     * (ref: pg 92 of datasheet) */
    /* P2.0 TX: x = 0; P2SEL1.x = 1, P2SEL0.x = 0 */
    /* P2.1 RX: x = 1; P2SEL1.x = 1, P2SEL0.x = 0 */

    /* set P2SEL0.1, and P2SEL0.0 to 0 */
    P2SEL0 &= ~(BIT0 | BIT1);
    /* set P2SEL1.1, and P2SEL1.0 to 1 */
    P2SEL1 |= (BIT0 | BIT1);

    // Use SMCLK as BRCLK
    UCA0CTLW0 |= UCSSEL__SMCLK;

    /*
    By leaving all other fields as default in UCA0CTLW0, we are using the following configuration:
    - Parity disabled
    - LSB first (in the RX and TX shift registers)
    - 8-bit data
    - One stop bit
    - eUSCI_A0 in UART mode
    - Asynchronous mode
    - Regular UART mode (no multiprocessor mode or automatic baud-rate detection)
    - Erroneous characters rejected and corresponding interrupt disabled
    - Receive break character interrupt disabled
    - Not dormant
    - Next frame to be transmitted is data
    - Next frame to be transmitted is not a break
    */

    // Baud Rate calculation (Section 30.3.10, SLAU367P)
    // N = (Baud rate clock frequency) / baud rate = 8000000 / 9600 = 833.3333
    // N > 16, so we will use oversampling baud-rate generation mode (as TI recommends)
    UCA0MCTLW = UCOS16; // Enables oversampling baud-rate generation mode
    // UCBRx = int(N / 16) = int(52.08333)
    // UCBRx = 52
    UCA0BRW = 52U; // Note: UCBRSx takes up the full 16 bits of UCAxBRW
    // UCBRFx = int([(N/16) - int(N / 16)] * 16) = int([52.08333 - 52] * 16) = int(1.3333)
    // UCBRFx = 1
    UCA0MCTLW |= UCBRF_1;
    // UCBRSx = 0x04 (per Table 30-4, SLAU367P)
    // However, per table 30-5 (SLAU367P), for a BRCLK frequency of 8000000 and a baud rate of 9600,
    //  the USBRSx value of 0x49 results in the lowest error (as determined by a search algorithm).
    //  Therefore, we use 0x49 instead of 0x04. 
    UCA0MCTLW |= 0x4900U;  // Note: UCBRSx is the top 8 bits of UCAxMCTLW

    // Release eUSCI_A0 reset
    UCA0CTLW0 &= ~UCSWRST;
    // Enable USCI_A0 RX interrupt              
    UCA0IE |= UCRXIE;                       
}

/**
 * Initialize UART1 (Lander <-> watchdog)
 */
static void UART__uart1Init() {
    // Put eUSCI_A1 in reset
    UCA1CTLW0 = UCSWRST;                    

    /* Setup for eUSCI_A1 */
    /* On the MSP430FR5994, pin P2.5 is used for TX and pin P2.6 is used for RX
     * (ref: pg 95 of datasheet) */
    /* P2.5 TX: x = 5; P2SEL1.x = 1, P2SEL0.x = 0 */
    /* P2.6 RX: x = 6; P2SEL1.x = 1, P2SEL0.x = 0 */

    /* set P2SEL0.5, P2SEL0.6 to 0 */
    P2SEL0 &= ~(BIT5 | BIT6);
    /* set P2SEL1.5, P2SEL1.6 to 1 */
    P2SEL1 |= (BIT5 | BIT6);

    // Use SMCLK as BRCLK
    UCA1CTLW0 |= UCSSEL__SMCLK;

    /*
    By leaving all other fields as default in UCA1CTLW0, we are using the following configuration:
    - Parity disabled
    - LSB first (in the RX and TX shift registers)
    - 8-bit data
    - One stop bit
    - eUSCI_A1 in UART mode
    - Asynchronous mode
    - Regular UART mode (no multiprocessor mode or automatic baud-rate detection)
    - Erroneous characters rejected and corresponding interrupt disabled
    - Receive break character interrupt disabled
    - Not dormant
    - Next frame to be transmitted is data
    - Next frame to be transmitted is not a break
    */

    // Baud Rate calculation (Section 30.3.10, SLAU367P)
    // N = (Baud rate clock frequency) / (baud rate) = 8000000 / 9600 = 833.3333
    // N > 16, so we will use oversampling baud-rate generation mode (as TI recommends)
    UCA1MCTLW = UCOS16; // Enables oversampling baud-rate generation mode
    // UCBRx = int(N / 16) = int(52.08333)
    // UCBRx = 52
    UCA1BRW = 52U; // Note: UCBRSx takes up the full 16 bits of UCAxBRW
    // UCBRFx = int([(N/16) - int(N / 16)] * 16) = int([52.08333 - 52] * 16) = int(1.3333)
    // UCBRFx = 1
    UCA1MCTLW |= UCBRF_1;
    // UCBRSx = 0x04 (per Table 30-4, SLAU367P)
    // However, per table 30-5 (SLAU367P), for a BRCLK frequency of 8000000 and a baud rate of 9600,
    //  the USBRSx value of 0x49 results in the lowest error (as determined by a search algorithm).
    //  Therefore, we use 0x49 instead of 0x04. 
    UCA1MCTLW |= 0x4900U;  // Note: UCBRSx is the top 8 bits of UCAxMCTLW

    // Release eUSCI_A1 reset
    UCA1CTLW0 &= ~UCSWRST;
    // Enable USCI_A1 RX interrupt            
    UCA1IE |= UCRXIE;                       
}

static void UART__interruptHandler(UART__State* uartState)
{
    if (NULL == uartState) {
        return;
    }

    uint8_t data = 0U;
    RingBuffer__Status rbStatus = RB__STATUS__SUCCESS;

    /* two possibilities; rx or tx */
    switch(__even_in_range(*(uartState->registers->UCAxIV), USCI_UART_UCTXCPTIFG)) {
        case USCI_UART_UCTXIFG: /* transmitted byte successfully */
            rbStatus = RingBuffer__get(uartState->txRingBuff, &data);

            if (RB__STATUS__SUCCESS == rbStatus) {
                // We got another byte to send, so put it in the tx buffer
                *(uartState->registers->UCAxTXBUF) = data;
            } else if (RB__STATUS__ERROR__EMPTY == rbStatus) {
                // There are no more bytes to send, so disable TX interrupt for this uart
                *(uartState->registers->UCAxIE) &= ~UCTXIE;
            } else {
                // An error occurred.
                //!< @todo handling? logging?
            }
            break;

        case USCI_UART_UCRXIFG: /* received new byte */
            /* get the received character */
            data = *(uartState->registers->UCAxRXBUF);

            // Note: calling this means that if the buffer is full and we get new data, 
            // we'll drop the new data instead of overwriting the oldest data in the 
            // buffer. If we'd rather overwrite the old data, this will need to be updated. 
            rbStatus = RingBuffer__put(uartState->rxRingBuff, data);

            if (RB__STATUS__ERROR__FULL == rbStatus) {
                // The ring buffer is full, so data will be dropped.
                //!< @todo handling? logging?
            } else if (RB__STATUS__SUCCESS != rbStatus) {
                // An error occurred.
                //!< @todo handling? logging?
            }

            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            //!< @todo REPLACE THESE WITH EVENT DISPATCH
            loop_flags |= uartState->gotRxLoopFlag;
            __bic_SR_register(DEFAULT_LPM); //Exits LPM
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

            break;

        default: /* some other possibilities */
            break;
    }
}

//###########################################################
// Interrupt handlers
//###########################################################

/* uart0 rx interrupt handler (Hercules) */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
    #pragma vector=EUSCI_A0_VECTOR
    __interrupt void USCI_A0_ISR(void) {
#elif defined(__GNUC__)
    void __attribute__ ((interrupt(EUSCI_A0_VECTOR))) USCI_A0_ISR(void) {
#else
    #error Compiler not supported!
#endif
    
#pragma FORCEINLINE_RECURSIVE
    UART__interruptHandler(&uart0State);
}


/* UCA1 interrupt handler (watchdog SLIP) */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
    #pragma vector=EUSCI_A1_VECTOR
    __interrupt void USCI_A1_ISR(void) {
#elif defined(__GNUC__)
    void __attribute__ ((interrupt(EUSCI_A1_VECTOR))) USCI_A1_ISR (void) {
#else
    #error Compiler not supported!
#endif
    
#pragma FORCEINLINE_RECURSIVE
    UART__interruptHandler(&uart1State);
}
