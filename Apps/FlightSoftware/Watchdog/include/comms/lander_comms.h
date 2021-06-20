#ifndef __WATCHDOG_LANDER_COMMS_H__
#define __WATCHDOG_LANDER_COMMS_H__

#include "include/drivers/uart.h"
#include "include/common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup watchdogLanderComms Lander Comms
 * @addtogroup watchdogLanderComms
 * @{
 */

/**
 * @brief An opaque structure that contains the internal state of the LanderComms module.
 */
typedef struct LanderComms__State LanderComms__State;

/**
 * @brief Possible return statuses from LanderComms functions.
 */
typedef enum LanderComms__Status
{
    LANDER_COMMS__STATUS__SUCCESS = 0, //!< The function completed successfully
    LANDER_COMMS__STATUS__ERROR_NULL = -1, //!< A required argument or a member of an argument was NULL.
    LANDER_COMMS__STATUS__ERROR_ALREADY_INITIALIZED = -2, //!< This module was already initialized.
    LANDER_COMMS__STATUS__ERROR_NOT_INITIALIZED = -3, //!< This module has not been initialized.
    LANDER_COMMS__STATUS__ERROR_BUFFER_TOO_SMALL = -4, //!< A given buffer was too small.

    /**
     * @brief Could store all of the data to be transmitted into the UART transmit buffer.
     */
    LANDER_COMMS__STATUS__ERROR_TX_OVERFLOW = -5,

    LANDER_COMMS__STATUS__ERROR_MPSM_INIT_FAILURE = -10, //!< Initializing the internal MPSM failed

    LANDER_COMMS__STATUS__ERROR_UART_RX_FAILURE = -20, //!< Got an error when calling the `UART__receive()`.
    LANDER_COMMS__STATUS__ERROR_UART_TX_FAILURE = -21, //!< Got an error when calling `UART__transmit()`.

    LANDER_COMMS__STATUS__ERROR_SLIP_ENCODE_FAILURE = -30 //!< Got an error when attempting to SLIP encode data.
} LanderComms__Status;

/**
 * @brief The signature of callback that is called once by `LanderComms__tryGetMessage()` for each message that
 *        function successfully gets.
 *
 * @param rxDataBuffer The buffer containing the payload.
 * @param rxDataLen The size of the payload.
 * @param userArg The user argument, which is passed to `LanderComms__tryGetMessage()` and passed without
 *                modification to this callback.
 */
typedef void(*LanderMsgCallback)(uint8_t* rxDataBuffer, size_t rxDataLen, void* userArg);

/**
 * @brief Initializes the given `LanderComms__State` instance.
 *
 * @param lcState An output parameter, set to an internally allocated instance of `LanderComms__State` that
 *                is initialized by this call.
 * @param uartState The UART instance connected to the Lander.
 *
 * @return One of the following:
 *   - LANDER_COMMS__STATUS__SUCCESS: The function was successful.
 *   - LANDER_COMMS__STATUS__ERROR_NULL: `lcState` or `uartState` was NULL.
 *   - LANDER_COMMS__STATUS__ERROR_ALREADY_INITIALIZED: `lcState` was already initialized.
 *   - LANDER_COMMS__STATUS__ERROR_MPSM_INIT_FAILURE: Initializing the internal MPSM failed.
 */
LanderComms__Status LanderComms__init(LanderComms__State** lcState, UART__State* uartState);

/**
 * @brief If we have received a message, decode the SLIP encoding, parse that payload from the packet, and then call the
 *        given callback with that payload data.
 *
 * The message is expected to have been received as a SLIP-encoded full IP packet containing a UDP datagram that
 * contains the FSW message data as its payload. This function takes a callback so that it can handle parsing multiple
 * messages from the data currently available in the UART receive buffer.
 *
 * @param lcState The comms instance.
 * @param callback The callback to invoke when a message is parsed.
 * @param userArg A user argument to forward to the callback.
 *
 * @return One of the following:
 *   - LANDER_COMMS__STATUS__SUCCESS: The function was successful.
 *   - LANDER_COMMS__STATUS__ERROR_NULL: `lcState` or `userArg` was NULL.
 *   - LANDER_COMMS__STATUS__ERROR_NOT_INITIALIZED: `lcState` was not initialized.
 *   - LANDER_COMMS__STATUS__ERROR_MPSM_INIT_FAILURE: Re-initializing the internal MPSM failed.
 *   - LANDER_COMMS__STATUS__ERROR_UART_RX_FAILURE: Got an error when `UART__receive` was invoked.
 */
LanderComms__Status LanderComms__tryGetMessage(LanderComms__State* lcState,
                                               LanderMsgCallback callback,
                                               void* userArg);

/**
 * @brief Transmits the given data to the Lander.
 *
 * The given data as the payload of a UDP datagram within an IP packet, all of which is SLIP encoded and then placed
 * into the UART transmit buffer.
 *
 * @param lcState The comms instance.
 * @param data The data to send as the payload.
 * @param dataLen The length of `data`.
 *
  * @return One of the following:
 *   - LANDER_COMMS__STATUS__SUCCESS: The function was successful.
 *   - LANDER_COMMS__STATUS__ERROR_NULL: `lcState` or `data` was NULL.
 *   - LANDER_COMMS__STATUS__ERROR_NOT_INITIALIZED: `hState` was not initialized.
 *   - LANDER_COMMS__STATUS__ERROR_TX_OVERFLOW: Could not fit all of the message data into the UART's internal transmit
 *                                              buffer.
 *   - LANDER_COMMS__STATUS__ERROR_UART_TX_FAILURE: Got an error when `UART__transmit` was invoked.
 *   - LANDER_COMMS__STATUS__ERROR_SLIP_ENCODE_FAILURE: A failure occurred when attempting to SLIP encode the packet.
 */
LanderComms__Status LanderComms__txData(LanderComms__State* lcState, const uint8_t* data, size_t dataLen);

/**
 * @}
 */

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __WATCHDOG_LANDER_COMMS_H__ */
