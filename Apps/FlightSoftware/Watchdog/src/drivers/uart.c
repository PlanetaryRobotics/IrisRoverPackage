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
#include <assert.h>
#include "comms/debug_comms.h"
#include "drivers/uart.h"
#include "drivers/bsp.h"
#include "event/event.h"
#include "event/event_queue.h"
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

    Event__Type gotRxEventType;
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
    .gotRxEventType = EVENT__TYPE__HERCULES_DATA
};

static UART__State uart1State = {
    .initialized = FALSE,
    .registers = &uart1Registers,
    .txRingBuff = NULL,
    .rxRingBuff = NULL,
    .gotRxEventType = EVENT__TYPE__LANDER_DATA
};

//###########################################################
// Private function declarations
//###########################################################

static UART__Status UART__initState(UART__State* state, UART__Buffers* buffers);
static void UART__uart0Init(void);
static void UART__uart1Init(void);

//###########################################################
// Public function definitions
//###########################################################

UART__Status UART__init0(UART__Config* config,
                         UART__State** uart0StateOut)
{
    if (NULL == config || NULL == uart0StateOut) {
        return UART__STATUS__ERROR_NULL;
    }

    if (uart0State.initialized) {
        return UART__STATUS__ERROR_ALREADY_INITIALIZED;
    }

    // Initialize the state structure
    UART__Status uartStatus = UART__initState(&uart0State, &(config->uart0Buffers));

    if (UART__STATUS__SUCCESS != uartStatus) {
        return uartStatus;
    }

    // Initialize the uart itself
    UART__uart0Init();

    uart0State.initialized = TRUE;

    *uart0StateOut = &uart0State;
    return UART__STATUS__SUCCESS;
 }

UART__Status UART__init1(UART__Config* config,
                         UART__State** uart1StateOut)
{
    if (NULL == config || NULL == uart1StateOut) {
        return UART__STATUS__ERROR_NULL;
    }

    if (uart1State.initialized) {
        return UART__STATUS__ERROR_ALREADY_INITIALIZED;
    }

    // Initialize the state structure
    UART__Status uartStatus = UART__initState(&uart1State, &(config->uart1Buffers));

    if (UART__STATUS__SUCCESS != uartStatus) {
        return uartStatus;
    }

    // Initialize the uart itself
    UART__uart1Init();

    uart1State.initialized = TRUE;

    *uart1StateOut = &uart1State;
    return UART__STATUS__SUCCESS;
}

UART__Status UART__uninit0(UART__State** uart0StateOut)
{
    // Put eUSCI_A0 in reset. We'll leave it in reset until it is re-initialized.
    UCA0CTLW0 = UCSWRST;

    // Also disable the uart0 tx/rx pins
    disableUart0Pins();

    if (NULL == uart0StateOut) {
        return UART__STATUS__ERROR_NULL;
    }

    if (!(uart0State.initialized)) {
        return UART__STATUS__ERROR_NOT_INITIALIZED;
    }

    uart0State.initialized = FALSE;

    *uart0StateOut = NULL;
    return UART__STATUS__SUCCESS;
 }

UART__Status UART__uninit1(UART__State** uart1StateOut)
{
    // Put eUSCI_A1 in reset. We'll leave it in reset until it is re-initialized.
    UCA1CTLW0 = UCSWRST;

    // Also disable the uart1 tx/rx pins
    disableUart1Pins();

    if (NULL == uart1StateOut) {
        return UART__STATUS__ERROR_NULL;
    }

    if (!(uart1State.initialized)) {
        return UART__STATUS__ERROR_NOT_INITIALIZED;
    }

    uart1State.initialized = FALSE;

    *uart1StateOut = NULL;
    return UART__STATUS__SUCCESS;
 }

BOOL UART__checkIfSendable(UART__State* uartState, size_t dataLen, size_t* free)
{
    if (NULL == uartState) {
        return FALSE;
    }

    if (!(uartState->initialized)) {
        return FALSE;
    }

    if (0 == dataLen) {
        return TRUE;
    }

    size_t numFree = 0;

    // Disable the tx interrupt for this uart while we get the number of free bytes in the ring buffer
    uint16_t existingTxInterruptBitState = *(uartState->registers->UCAxIE) & UCTXIE;
    *(uartState->registers->UCAxIE) &= ~UCTXIE;

    numFree = RingBuffer__freeCount(uartState->txRingBuff);

    // I'm not exactly sure what conditions are causing it, but I've seen the TX interrupts stall
    // (i.e. TX interrupts stop occurring even though there is data in the TX ring buffer)
    // To work around this, we re-enable the TXIFG bit here under certain conditions
    /** @todo Replace hard-coded 1024 with a variable. This is cheating and will break if we change the buffer size */
    //if ((existingTxInterruptBitState == 0) && numFree < 1024) {
    //    *(uartState->registers->UCAxIFG) |= UCTXIFG;
    //}

    // Re-enable the tx interrupt only if it was previously enabled or if there are bytes remaining in the ring
    // buffer
    /** @todo Replace hard-coded 1024 with a variable. This is cheating and will break if we change the buffer size */
    *(uartState->registers->UCAxIE) |= (existingTxInterruptBitState); //| ((numFree == 1024) ? UCTXIE : 0));

    if (NULL != free) {
        *free = numFree;
    }

    return (dataLen <= numFree);
}

void UART__flushTx(UART__State* uartState)
{
    if (NULL == uartState) {
        return;
    }

    if (!(uartState->initialized)) {
        return;
    }

    size_t numUsed = 0;

    do {
        // Disable the tx interrupt for this uart while we get the number of free bytes in the ring buffer
        uint16_t existingTxInterruptBitState = *(uartState->registers->UCAxIE) & UCTXIE;
        *(uartState->registers->UCAxIE) &= ~UCTXIE;

        numUsed = RingBuffer__usedCount(uartState->txRingBuff);

        // I'm not exactly sure what conditions are causing it, but I've seen the TX interrupts stall
        // (i.e. TX interrupts stop occurring even though there is data in the TX ring buffer)
        // To work around this, we re-enable the TXIFG bit here under certain conditions
        if ((existingTxInterruptBitState == 0) && numUsed > 0) {
            *(uartState->registers->UCAxIFG) |= UCTXIFG;
        }

        // Re-enable the tx interrupt only if it was previously enabled or if there are bytes remaining in the ring
        // buffer
        *(uartState->registers->UCAxIE) |= (existingTxInterruptBitState);// | ((numUsed > 0) ? UCTXIE : 0));

        if (numUsed != 0) {
            __delay_cycles(10000);
            WDTCTL = WDTPW + WDTCNTCL + WDTSSEL__ACLK + WDTIS2;//+ WDTIS0;
        }
    } while (numUsed != 0);
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
    //__disable_interrupt();
    for (size_t i = 0U; i < dataLen; ++i) {
        RingBuffer__Status rbStatus = RingBuffer__put(uartState->txRingBuff, data[i]);

        if (RB__STATUS__SUCCESS != rbStatus) {
  //          __enable_interrupt();
            return UART__STATUS__ERROR_RB_PUT_FAILURE;
        }
    }
    //__enable_interrupt();

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
        } else if (RB__STATUS__ERROR_EMPTY == rbStatus) {
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

static UART__Status UART__initState(UART__State* state, UART__Buffers* buffers)
{
    if (NULL == state
        || NULL == buffers
        || NULL == buffers->txBuffer
        || NULL == buffers->rxBuffer) {
        return UART__STATUS__ERROR_NULL;
    }

    // Make sure we don't don't re-initialize a buffer, and make sure we clear the buffer it is has previously been
    // initialized
    if (NULL == state->txRingBuff) {
        RingBuffer__Status rbStatus = RingBuffer__init(&(state->txRingBuff),
                                                       buffers->txBuffer,
                                                       buffers->txBufferSize);

        DEBUG_LOG_CHECK_STATUS_RETURN(RB__STATUS__SUCCESS, rbStatus,
                                      "Failed to init TX RB", UART__STATUS__ERROR_RB_INIT_FAILURE);
    } else {
        RingBuffer__Status rbStatus = RingBuffer__clear(state->txRingBuff);

        DEBUG_LOG_CHECK_STATUS_RETURN(RB__STATUS__SUCCESS, rbStatus,
                                      "Failed to clear TX RB", UART__STATUS__ERROR_RB_CLEAR_FAILURE);
    }


    if (NULL == state->rxRingBuff) {
        RingBuffer__Status rbStatus = RingBuffer__init(&(state->rxRingBuff),
                                                       buffers->rxBuffer,
                                                       buffers->rxBufferSize);

        DEBUG_LOG_CHECK_STATUS_RETURN(RB__STATUS__SUCCESS, rbStatus,
                                      "Failed to init RX RB", UART__STATUS__ERROR_RB_INIT_FAILURE);
    } else {
        RingBuffer__Status rbStatus = RingBuffer__clear(state->rxRingBuff);

        DEBUG_LOG_CHECK_STATUS_RETURN(RB__STATUS__SUCCESS, rbStatus,
                                      "Failed to clear RX RB", UART__STATUS__ERROR_RB_CLEAR_FAILURE);
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
/*
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
*/
    // Baud Rate calculation (Section 30.3.10, SLAU367P)
    // N = (Baud rate clock frequency) / baud rate = 8000000 / 57600 = 833.3333
    // N > 16, so we will use oversampling baud-rate generation mode (as TI recommends)
    UCA0MCTLW = UCOS16; // Enables oversampling baud-rate generation mode
    // UCBRx = int(N / 16) = int(52.08333)
    // UCBRx = 52
    UCA0BRW = 8U; // Note: UCBRSx takes up the full 16 bits of UCAxBRW
    // UCBRFx = int([(N/16) - int(N / 16)] * 16) = int([52.08333 - 52] * 16) = int(1.3333)
    // UCBRFx = 1
    UCA0MCTLW |= UCBRF_10;
    // UCBRSx = 0x04 (per Table 30-4, SLAU367P)
    // However, per table 30-5 (SLAU367P), for a BRCLK frequency of 8000000 and a baud rate of 9600,
    //  the USBRSx value of 0x49 results in the lowest error (as determined by a search algorithm).
    //  Therefore, we use 0x49 instead of 0x04.
    UCA0MCTLW |= 0xF700U;  // Note: UCBRSx is the top 8 bits of UCAxMCTLW

    enableUart0Pins();

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
/*
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
*/
    // Baud Rate calculation (Section 30.3.10, SLAU367P)
    // N = (Baud rate clock frequency) / baud rate = 8000000 / 57600 = 833.3333
    // N > 16, so we will use oversampling baud-rate generation mode (as TI recommends)
    UCA1MCTLW = UCOS16; // Enables oversampling baud-rate generation mode
    // UCBRx = int(N / 16) = int(52.08333)
    // UCBRx = 52
    UCA1BRW = 8U; // Note: UCBRSx takes up the full 16 bits of UCAxBRW
    // UCBRFx = int([(N/16) - int(N / 16)] * 16) = int([52.08333 - 52] * 16) = int(1.3333)
    // UCBRFx = 1
    UCA1MCTLW |= UCBRF_10;
    // UCBRSx = 0x04 (per Table 30-4, SLAU367P)
    // However, per table 30-5 (SLAU367P), for a BRCLK frequency of 8000000 and a baud rate of 9600,
    //  the USBRSx value of 0x49 results in the lowest error (as determined by a search algorithm).
    //  Therefore, we use 0x49 instead of 0x04.
    UCA1MCTLW |= 0xF700U;  // Note: UCBRSx is the top 8 bits of UCAxMCTLW

    enableUart1Pins();

    // Release eUSCI_A1 reset
    UCA1CTLW0 &= ~UCSWRST;
    // Enable USCI_A1 RX interrupt            
    UCA1IE |= UCRXIE;                       
}


#define UART__interruptHandler(uartState)                                                        \
{                                                                                                \
    uint8_t data = 0U;                                                                           \
    RingBuffer__Status rbStatus = RB__STATUS__SUCCESS;                                           \
                                                                                                 \
    /* two possibilities; rx or tx */                                                            \
    switch(__even_in_range(*(uartState.registers->UCAxIV), USCI_UART_UCTXCPTIFG)) {              \
        case USCI_UART_UCTXIFG: /* transmitted byte successfully */                              \
            rbStatus = RingBuffer__get(uartState.txRingBuff, &data);                             \
                                                                                                 \
            if (RB__STATUS__SUCCESS == rbStatus) {                                               \
                /* We got another byte to send, so put it in the tx buffer */                    \
                *(uartState.registers->UCAxTXBUF) = data;                                        \
            } else if (RB__STATUS__ERROR_EMPTY == rbStatus) {                                    \
                /* There are no more bytes to send, so disable TX interrupt for this uart */     \
                *(uartState.registers->UCAxIE) &= ~UCTXIE;                                       \
            } else {                                                                             \
                /* An error occurred. */                                                         \
                /** @todo handling? logging? */                                                  \
            }                                                                                    \
            break;                                                                               \
                                                                                                 \
        case USCI_UART_UCRXIFG: /* received new byte */                                          \
            /* get the received character */                                                     \
            data = *(uartState.registers->UCAxRXBUF);                                            \
                                                                                                 \
            /* Note: calling this means that if the buffer is full and we get new data, */       \
            /* we'll drop the new data instead of overwriting the oldest data in the */          \
            /* buffer. If we'd rather overwrite the old data, this will need to be updated. */   \
            rbStatus = RingBuffer__put(uartState.rxRingBuff, data);                              \
            assert(rbStatus == RB__STATUS__SUCCESS); \
                                                                                                 \
            if (RB__STATUS__ERROR_FULL == rbStatus) {                                            \
                /* The ring buffer is full, so data will be dropped. */                          \
                /** @todo handling? logging? */                                                  \
            } else if (RB__STATUS__SUCCESS != rbStatus) {                                        \
                /* An error occurred. */                                                         \
                /** @todo handling? logging? */                                                  \
            }                                                                                    \
                                                                                                 \
            EventQueue__put(uartState.gotRxEventType);                                           \
            EXIT_DEFAULT_LPM;                                                                    \
                                                                                                 \
            break;                                                                               \
                                                                                                 \
        default: /* some other possibilities */                                                  \
            break;                                                                               \
    }                                                                                            \
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
    UART__interruptHandler(uart0State);
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
    UART__interruptHandler(uart1State);
}
