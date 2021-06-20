#ifndef __WATCHDOG_HERCULES_MPSM_H__
#define __WATCHDOG_HERCULES_MPSM_H__

#include "include/comms/hercules_msgs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup watchdogHerculesMpsm Hercules Message Parsing State Machine
 * @addtogroup watchdogHerculesMpsm
 * @{
 */

/**
 * @brief Possible return statuses for HerculesMpsm functions.
 */
typedef enum HerculesMpsm__Status {
    HERCULES_MPSM__STATUS__NEED_MORE_DATA = 2, /*!< The function was successful, but needs more data to parse a message. */
    HERCULES_MPSM__STATUS__PARSED_MESSAGE = 1, /*!< The function was successful, and successfully parsed a complete message. */
    HERCULES_MPSM__STATUS__SUCCESS = 0, /*!< The function was successful. */
    HERCULES_MPSM__STATUS__ERROR_NULL = -1, /*!< A required argument was NULL */
    HERCULES_MPSM__STATUS__ERROR_BUFFER_TOO_SMALL = -2, //!< A given buffer was too small for some purpose.
    HERCULES_MPSM__STATUS__ERROR_NOT_INITIALIZED = -3, //!< The module was not initialized.
    HERCULES_MPSM__STATUS__ERROR_ZERO_LENGTH_BUFFER = -4, //!< A given buffer has length zero when not allowed.
    HERCULES_MPSM__STATUS__ERROR_WRONG_STATE = -5, //!< The given `HerculesMpsm__Msg` was in the wrong state for the function.
    
    HERCULES_MPSM__STATUS__ERROR_RB_INIT_FAILURE = -10, //!< A ring buffer init call failed.
    HERCULES_MPSM__STATUS__ERROR_RB_PUT_FAILURE = -11, //!< A ring buffer put call failed.
    HERCULES_MPSM__STATUS__ERROR_RB_GET_FAILURE = -12, //!< A ring buffer get call failed.
    HERCULES_MPSM__STATUS__ERROR_RB_CLEAR_FAILURE = -13, //!< A ring buffer clear call failed.

    HERCULES_MPSM__STATUS__ERROR_INTERNAL_ERROR = -255 /*!< An unexpected error occurred */
} HerculesMpsm__Status;

/**
 * @brief Possible statuses of an instance of `HerculesMpsm__Msg`.
 */
typedef enum HerculesMpsm__MsgStatus
{
    /**
     * The state machine is done parsing the message, and it is valid.
     */
    HERCULES_MPSM__MSG_STATUS__DONE_VALID = 2,

    /**
     * This message structure has been initialized, but the message hasn't been fully received and/or parsed yet.
     */
    HERCULES_MPSM__MSG_STATUS__IN_PROGRESS = 1,

    /**
     * This message structure hasn't yet been initialized.
     */
    HERCULES_MPSM__MSG_STATUS__NOT_INITIALIZED = 0,

    /**
     * The state machine is done parsing the message because the buffer was too small for the message
     */
    HERCULES_MPSM__MSG_STATUS__ERROR_BUFFER_TOO_SMALL = HERCULES_MPSM__STATUS__ERROR_BUFFER_TOO_SMALL,

    /**
     * The state machine is done parsing the message because an unexpected failure occurred with a RingBuffer function
     */
    HERCULES_MPSM__MSG_STATUS__ERROR_RB_FAILURE = -10
} HerculesMpsm__MsgStatus;

/**
 * @brief Encapsulates the contents of a message from Hercules, as well as the state of parsing that message.
 */
typedef struct HerculesMpsm__Msg
{
    HerculesMpsm__MsgStatus msgStatus; //!< The status of parsing the message.

    HercMsgs__Header header; //!< The message header, once parsed.

    uint8_t* dataBuffer; //!< The buffer in which to store the message payload.
    size_t dataBufferLen; //!< The maximum length of `dataBuffer`.

    size_t msgLen; //!< How many bytes in `dataBuffer` are currently holding payload data.
} HerculesMpsm__Msg;

/**
 * @brief Initializes a given `HerculesMpsm__Msg` structure.
 *
 * Prior to this call, the `dataBuffer` and `dataBufferLen` fields of the given `HerculesMpsm__Msg` instance must be
 * initialized.
 *
 * This should be called before the first time `process()` is called with the given `HerculesMpsm__Msg` structure, and
 * before `process()` is called again with the same `HerculesMpsm__Msg` structure after a complete message is received.
 *
 * @param msg The message structure to initialize.
 *
 * @return One of the following:
 *   - HERCULES_MPSM__STATUS__SUCCESS: The function was successful.
 *   - HERCULES_MPSM__STATUS__ERROR_NULL: `msg` or `msg->dataBuffer` was NULL.
 *   - HERCULES_MPSM__STATUS__ERROR_ZERO_LENGTH_BUFFER: `msg->dataBufferLen` was zero.
 *   - HERCULES_MPSM__STATUS__ERROR_RB_INIT_FAILURE: Initializing the internal RingBuffer failed.
 */
HerculesMpsm__Status HerculesMpsm__initMsg(HerculesMpsm__Msg* msg);

/**
 * @brief Processes a new byte of data towards parsing the next message.
 *
 * @param msg The message structure containing the message being parsed.
 * @param newData The new byte of data being processed.
 *
 * @return One of the following:
 *   - HERCULES_MPSM__STATUS__NEED_MORE_DATA: Either (1) no valid header was found and we need more data to find a valid
 *                                            header, (2) a valid header was found with a non-zero payload size, so
 *                                            payload data is needed, or (3) we're accumulating payload data to complete
 *                                            the message, and we need more.
 *   - HERCULES_MPSM__STATUS__PARSED_MESSAGE: Either (1) found a valid header with a payload size of zero, meaning a
 *                                            complete message has been parsed, or (2) all payload data has been
 *                                            accumulated, so the message has been completely parsed.
 *   - HERCULES_MPSM__STATUS__ERROR_NULL: `msg` or `msg->dataBuffer` was NULL.
 *   - HERCULES_MPSM__STATUS__ERROR_WRONG_STATE: `msg->msgStatus` was not `HERCULES_MPSM__MSG_STATUS__IN_PROGRESS`
 *   - HERCULES_MPSM__STATUS__ERROR_RB_PUT_FAILURE: Calling `put()` on the internal RingBuffer failed.
 */
HerculesMpsm__Status HerculesMpsm__process(HerculesMpsm__Msg* msg, uint8_t newData);

/**
 * @brief Resets the state machine.
 *
 * @param msg The message structure to be reset along with the overall state machine.
 *
 * @return One of the following:
 *   - HERCULES_MPSM__STATUS__SUCCESS: The function was successful.
 *   - HERCULES_MPSM__STATUS__ERROR_NULL: `msg` or `msg->dataBuffer` was NULL.
 *   - HERCULES_MPSM__STATUS__ERROR_NOT_INITIALIZED: `msg` has never been initialized.
 *   - HERCULES_MPSM__STATUS__ERROR_ZERO_LENGTH_BUFFER: `msg->dataBufferLen` was zero.
 *   - HERCULES_MPSM__STATUS__ERROR_RB_INIT_FAILURE: Re-initializing the internal RingBuffer failed.
 *   - HERCULES_MPSM__STATUS__ERROR_RB_CLEAR_FAILURE: Clearing the internal RingBuffer failed.
 */
HerculesMpsm__Status HerculesMpsm__reset(HerculesMpsm__Msg* msg);

/**
 * @}
 */

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __WATCHDOG_HERCULES_MPSM_H__ */
