/**
 * @file uart.h
 * @brief File for interfacing with UART protocol hardware module
 * File for interfacing with UART protocol hardware module. Also includes some
 * basic communications protocol parsing.
 * 
 * UART communication uses eUSCI_A0 (Hercules) and eUSCI_A1 (Lander).
 */
#ifndef __WATCHDOG_UART_H__
#define __WATCHDOG_UART_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup watchdogUart UART Driver.
 * @addtogroup watchdogUart
 * @{
 */

/**
 * @brief An opaque structure that contains the internal state of a UART driver for a particular UART interface.
 */
typedef struct UART__State UART__State;

/**
 * @brief The buffers to use internally for a particular UART interface.
 */
typedef struct UART__Buffers
{
    volatile uint8_t* txBuffer; //!< The buffer to use as the backing memory for the transmit ring buffer.
    size_t txBufferSize; //!< The size of `txBuffer`. MUST be a power of two.

    volatile uint8_t* rxBuffer; //!< The buffer to use as the backing memory for the receive ring buffer.
    size_t rxBufferSize; //!< The size of `rxBuffer`. MUST be a power of two.
} UART__Buffers;

/**
 * @brief The configuration to use for the overall UART module (i.e. for both UART interfaces).
 */
typedef struct UART__Config
{
    UART__Buffers uart0Buffers; //!< The buffers to use for the UART0 interface.
    UART__Buffers uart1Buffers; //!< The buffers to use for the UART1 interface.
} UART__Config;

/**
 * @brief Possible return statuses of UART functions.
 */
typedef enum UART__Status {
    UART__STATUS__SUCCESS = 0, //!< The function completed successfully.
    UART__STATUS__ERROR_NULL = -1, //!< A required argument or a member of an argument was NULL.
    UART__STATUS__ERROR_ALREADY_INITIALIZED = -2, //!< This module was already initialized.
    UART__STATUS__ERROR_NOT_INITIALIZED = -3, //!< This module has not been initialized.
    UART__STATUS__ERROR_ZERO_LENGTH_DATA = -4, //!< A buffer had zero length when not allowed.
    UART__STATUS__ERROR_NOT_ENOUGH_SPACE = -5, //!< A buffer was too small.

    UART__STATUS__ERROR_RB_PUT_FAILURE = -10, //!< An error occurred on a RingBuffer `put()` call.
    UART__STATUS__ERROR_RB_GET_FAILURE = -11, //!< An error occurred on a RingBuffer `get()` call.
    UART__STATUS__ERROR_RB_INIT_FAILURE = -12 //!< An error occurred on a RingBuffer `init()` call.
} UART__Status;

/**
 * @brief Initializes all of the clocks on MSP430 watchdog (used by all modules, not just by the UART) as well as both
 *        UART interfaces.
 *
 * @param config The configuration to use for the UART modules.
 * @param uart0State A return parameter that will be set to the module for UART0 if this call succeeds.
 * @param uart1State A return parameter that will be set to the module for UART1 if this call succeeds.
 *
 * @return One of the following:
 *   - UART__STATUS__SUCCESS: The function was successful.
 *   - UART__STATUS__ERROR_NULL: `config`, `uart0State`, or `uart1State` was NULL.
 *   - UART__STATUS__ERROR_ALREADY_INITIALIZED: This module was already initialized.
 *   - UART__STATUS__ERROR_RB_INIT_FAILURE: Initializing an internal ring buffer failed.
 */
UART__Status UART__init(UART__Config* config,
                        UART__State** uart0State,
                        UART__State** uart1State);

/**
 * @brief Puts the given data in the transmit ring buffer and kicks off the process that will pump the data from the
 *        transmit ring buffer and transmit it.
 *
 * @param uartState The UART module through which to transmit the data
 * @param data The buffer containing the data to transmit.
 * @param dataLen The size of `data`.
 *
 * @return One of the following:
 *   - UART__STATUS__SUCCESS: The function was successful.
 *   - UART__STATUS__ERROR_NULL: `uartState` or `data` was NULL.
 *   - UART__STATUS__ERROR_NOT_INITIALIZED: This module not initialized.
 *   - UART__STATUS__ERROR_ZERO_LENGTH_DATA: `dataLen` was zero.
 *   - UART__STATUS__ERROR_NOT_ENOUGH_SPACE: Not enough space in the transmit buffer to store all of the given data.
 *   - UART__STATUS__ERROR_RB_PUT_FAILURE: An error occurred while attempting to put the data into the transmit ring
 *                                         buffer.
 */
UART__Status UART__transmit(UART__State* uartState, const uint8_t* data, size_t dataLen);

/**
 * @brief Gets as much data as possible (up to the given buffer length) from the receive buffer of the given UART
 *        interface.
 *
 * @param uartState The UART module from which to receive the data.
 * @param data The output buffer into which received data will be placed.
 * @param dataLen The maximum amount of data to put into `data`.
 * @param numReceived An output parameter that will be set to the number of bytes of data that were placed into `data`.
 *
 * @return One of the following:
 *   - UART__STATUS__SUCCESS: The function was successful.
 *   - UART__STATUS__ERROR_NULL: `uartState`, `data`, or `numReceived` was NULL.
 *   - UART__STATUS__ERROR_NOT_INITIALIZED: This module not initialized.
 *   - UART__STATUS__ERROR_RB_GET_FAILURE: An error occurred while attempting to get the data from the receive ring
 *                                         buffer.
 */
UART__Status UART__receive(UART__State* uartState,
                           uint8_t* data,
                           size_t dataLen,
                           size_t* numReceived);

/**
 * @}
 */

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __WATCHDOG_UART_H__ */
