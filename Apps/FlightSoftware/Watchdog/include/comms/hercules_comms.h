#ifndef __WATCHDOG_HERCULES_COMMS_H__
#define __WATCHDOG_HERCULES_COMMS_H__

#include "drivers/uart.h"
#include "comms/hercules_msgs.h"
#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup watchdogHerculesComms Hercules Comms
 * @addtogroup watchdogHerculesComms
 * @{
 */

/**
 * @brief An opaque structure that contains the internal state of the HerculesComms module.
 */
typedef struct HerculesComms__State HerculesComms__State;

/**
 * @brief Possible return statuses from HerculesComms functions.
 */
typedef enum HerculesComms__Status
{
    HERCULES_COMMS__STATUS__SUCCESS = 0, //!< The function completed successfully.
    HERCULES_COMMS__STATUS__ERROR_NULL = -1, //!< A required argument was NULL.
    HERCULES_COMMS__STATUS__ERROR_ALREADY_INITIALIZED = -2, //!< This module was already initialized.
    HERCULES_COMMS__STATUS__ERROR_NOT_INITIALIZED = -3, //!< This module has not been initialized.
    HERCULES_COMMS__STATUS__ERROR_BUFFER_TOO_SMALL = -4, //!< A given buffer was too small.

    /**
     * @brief Could not store all of the data to be transmitted into the UART transmit buffer.
     */
    HERCULES_COMMS__STATUS__ERROR_TX_OVERFLOW = -5,

    HERCULES_COMMS__STATUS__ERROR_MPSM_INIT_FAILURE = -10, //!< Initializing the internal MPSM failed.
    HERCULES_COMMS__STATUS__ERROR_MPSM_PROCESS_FAILURE = -11, //!< Got an error from an MPSM `process()` call.
    HERCULES_COMMS__STATUS__ERROR_MPSM_RESET_FAILURE = -12, //!< Got an error from an MPSM `reset()` call.

    HERCULES_COMMS__STATUS__ERROR_UART_RX_FAILURE = -20, //!< Got an error when calling the `UART__receive()`.
    HERCULES_COMMS__STATUS__ERROR_UART_TX_FAILURE = -21 //!< Got an error when calling `UART__transmit()`.
} HerculesComms__Status;

/**
 * @brief The opcodes used for commands that don't technically have real opcodes.
 */
typedef enum HerculesComms__MsgOpcode
{
    HERCULES_COMMS__MSG_OPCODE__STROKE = 0x100,  //!< The opcode used for stroke messages.
    HERCULES_COMMS__MSG_OPCODE__DOWNLINK = 0x101,//!< The opcode used for downlink messages.
    HERCULES_COMMS__MSG_OPCODE__UPLINK = 0x102   //!< The opcode used for uplink messages.
} HerculesComms__MsgOpcode;

/**
 * @brief The signature of callback that is called once by `HerculesComms__tryGetMessage()` for each message that
 *        function successfully gets.
 *
 * @param header The header of the retrieved message.
 * @param payloadBuffer The buffer containing the payload, if there is any payload.
 * @param payloadSize The size of the payload; will be zero if there is no payload.
 * @param userArg The user argument, which is passed to `HerculesComms__tryGetMessage()` and passed without
 *                modification to this callback.
 */
typedef void(*HerculesMsgCallback)(HercMsgs__Header* header,
                                   uint8_t* payloadBuffer,
                                   size_t payloadSize,
                                   void* userArg);

/**
 * @brief Initializes the given `HerculesComms_State` instance.
 *
 * @param hState An output parameter, set to an internally allocated instance of `HerculesComms_State` that
 *               is initialized by this call.
 * @param uartState The UART instance connected to the Hercules.
 *
 * @return One of the following:
 *   - HERCULES_COMMS__STATUS__SUCCESS: The function was successful.
 *   - HERCULES_COMMS__STATUS__ERROR_NULL: `hState` or `uartState` was NULL.
 *   - HERCULES_COMMS__STATUS__ERROR_ALREADY_INITIALIZED: `hState` was already initialized.
 *   - HERCULES_COMMS__STATUS__ERROR_MPSM_INIT_FAILURE: Initializing the internal MPSM failed.
 */
HerculesComms__Status HerculesComms__init(HerculesComms__State** hState, UART__State* uartState);

/**
 * @brief If we have received a message, parse that message from the data and call the given callback with that
 *        message.
 *
 * The message is expected to have been received with a 12 byte header. This function parses the header, so
 * it is "returned" (really passed as a parameter to the callback invocation) as an unpacked structure, while
 * the payload data is "returned" simply as a blob of data. This function takes a callback so that it can handle parsing
 * multiple messages from the data currently available in the UART receive buffer.
 *
 * @param hState The comms instance.
 * @param callback The callback to invoke when a message is parsed.
 * @param userArg A user argument to forward to the callback.
 *
 * @return One of the following:
 *   - HERCULES_COMMS__STATUS__SUCCESS: The function was successful.
 *   - HERCULES_COMMS__STATUS__ERROR_NULL: `hState` or `userArg` was NULL.
 *   - HERCULES_COMMS__STATUS__ERROR_NOT_INITIALIZED: `hState` was not initialized.
 *   - HERCULES_COMMS__STATUS__ERROR_MPSM_INIT_FAILURE: Re-initializing the internal MPSM failed.
 *   - HERCULES_COMMS__STATUS__ERROR_MPSM_PROCESS_FAILURE: Processing data with the internal MPSM failed.
 *   - HERCULES_COMMS__STATUS__ERROR_UART_RX_FAILURE: Got an error when `UART__receive` was invoked.
 */
HerculesComms__Status HerculesComms__tryGetMessage(HerculesComms__State* hState,
                                                   HerculesMsgCallback callback,
                                                   void* userArg);

/**
 * @brief Sends an uplink message containing data from the lander to Hercules.
 *
 * A header will be sent first with the length of the payload, a reset value of zero, an internally maintained
 * sequence number, and the opcode `HERCULES_COMMS__MSG_OPCODE__UPLINK`. Following the header, the given data
 * is sent without modification.
 *
 * @param hState The comms instance.
 * @param data The payload data to send.
 * @param dataLen The length of `data`, which must be non-zero.
 *
 * @return One of the following:
 *   - HERCULES_COMMS__STATUS__SUCCESS: The function was successful.
 *   - HERCULES_COMMS__STATUS__ERROR_NULL: `hState` or `data` was NULL.
 *   - HERCULES_COMMS__STATUS__ERROR_NOT_INITIALIZED: `hState` was not initialized.
 *   - HERCULES_COMMS__STATUS__ERROR_BUFFER_TOO_SMALL: `dataLen` was zero.
 *   - HERCULES_COMMS__STATUS__ERROR_TX_OVERFLOW: Could not fit all of the message data into the UART's internal
 *                                                transmit buffer.
 *   - HERCULES_COMMS__STATUS__ERROR_UART_TX_FAILURE: Got an error when `UART__transmit` was invoked.
 */
HerculesComms__Status HerculesComms__txUplinkMsg(HerculesComms__State* hState, const uint8_t* data, size_t dataLen);

/**
 * @brief Sends a response message (responding to a message from Hercules) to Hercules.
 *
 * A header containing the length of the given data (if there is any) as well as the reset value, sequence number,
 * and opcode from the given header will be sent first. Then, the data will be sent without modification (if there is
 * any data to send).
 *
 * @param hState The comms instance.
 * @param sourceCommandHeader The header from the message to which we are responding.
 * @param data The payload data. May be NULL if `dataLen` is zero.
 * @param dataLen The length of the payload in `data`, or zero if there is no payload.
 *
 * @return One of the following:
 *   - HERCULES_COMMS__STATUS__SUCCESS: The function was successful.
 *   - HERCULES_COMMS__STATUS__ERROR_NULL: `hState` or `sourceCommandHeader` was NULL, or `data` was NULL and `dataLen`
 *                                          was non-zero.
 *   - HERCULES_COMMS__STATUS__ERROR_NOT_INITIALIZED: `hState` was not initialized.
 *   - HERCULES_COMMS__STATUS__ERROR_TX_OVERFLOW: Could not fit all of the message data into the UART's internal
 *                                                transmit buffer.
 *   - HERCULES_COMMS__STATUS__ERROR_UART_TX_FAILURE: Got an error when `UART__transmit` was invoked.
 */
HerculesComms__Status HerculesComms__txResponseMsg(HerculesComms__State* hState,
                                                   const HercMsgs__Header* sourceCommandHeader,
                                                   const uint8_t* data,
                                                   size_t dataLen);

/**
 * @brief Clears the underlying UART receive buffer and resets the internal message parsing state machine.
 *
 * In short, if any data has been received or message partially parsed before this message is called, all of that
 * data will be dropped and the parsing progress forgotten.
 *
 * @param hState The comms instance.
 *
 * @return One of the following:
 *   - HERCULES_COMMS__STATUS__SUCCESS: The function was successful.
 *   - HERCULES_COMMS__STATUS__ERROR_NULL: `hState` was NULL.
 *   - HERCULES_COMMS__STATUS__ERROR_NOT_INITIALIZED: `hState` was not initialized.
 *   - HERCULES_COMMS__STATUS__ERROR_MPSM_RESET_FAILURE: Resetting the internal MPSM failed.
 *   - HERCULES_COMMS__STATUS__ERROR_UART_RX_FAILURE: Got an error when `UART__receive` was invoked.
 */
HerculesComms__Status HerculesComms__resetState(HerculesComms__State* hState);

/**
 * @}
 */

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __WATCHDOG_HERCULES_COMMS_H__ */
