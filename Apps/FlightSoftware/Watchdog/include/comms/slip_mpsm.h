#ifndef __WATCHDOG_SLIP_MPSM_H__
#define __WATCHDOG_SLIP_MPSM_H__

#include <stdint.h>
#include <stddef.h>

/**
 * @defgroup watchdogSlipMpsm SLIP Message Parsing State Machine
 * @addtogroup watchdogSlipMpsm
 * @{
 */

/**
 * @brief Possible return statuses for SlipMpsm functions.
 */
typedef enum SlipMpsm__Status {
    SLIP_MPSM__STATUS__NEED_MORE_DATA = 2, //!< The function was successful, but needs more data to parse a message.
    SLIP_MPSM__STATUS__PARSED_MESSAGE = 1, //!< The function was successful, and successfully parsed a complete message.
    SLIP_MPSM__STATUS__SUCCESS = 0, //!< The function was successful.
    SLIP_MPSM__STATUS__ERROR_NULL = -1, //!< A required argument or a member of an argument was NULL.
    SLIP_MPSM__STATUS__ERROR_BUFFER_TOO_SMALL = -2, //!< A given buffer was too small for some purpose.
    SLIP_MPSM__STATUS__ERROR_INVALID_SEQUENCE = -3, //!< Encountered an illegal sequence of characters.
    SLIP_MPSM__STATUS__ERROR_ZERO_LENGTH_BUFFER = -4, //!< A given buffer has length zero when not allowed.
    SLIP_MPSM__STATUS__ERROR_WRONG_STATE = -5, //!< The given `SlipMpsm__Msg` was in the wrong state for the function.

    SLIP_MPSM__STATUS__ERROR_INTERNAL_ERROR = -255 /*!< An unexpected error occurred */
} SlipMpsm__Status;

/**
 * @brief Possible statuses of an instance of `SlipMpsm__Msg`.
 */
typedef enum SlipMpsm__MsgStatus
{
    /**
     * The state machine is done parsing the message, and it is valid.
     */
    SLIP_MPSM__MSG_STATUS__DONE_VALID = 2,

    /**
     * This message structure has been initialized, but the message hasn't been fully received and/or parsed yet.
     */
    SLIP_MPSM__MSG_STATUS__IN_PROGRESS = 1,

    /**
     * This message structure hasn't yet been initialized.
     */
    SLIP_MPSM__MSG_STATUS__NOT_INITIALIZED = 0,

    /**
     * The state machine is done parsing the message because the buffer was too small for the message
     */
    SLIP_MPSM__MSG_STATUS__ERROR_BUFFER_TOO_SMALL = SLIP_MPSM__STATUS__ERROR_BUFFER_TOO_SMALL,

    /**
     * The state machine is done parsing the message because an invalid byte sequence was encountered.
     * 
     * For SLIP, the only invalid sequences are an ESC byte followed by something other than ESC_END or ESC_ESC.
     */
    SLIP_MPSM__MSG_STATUS__ERROR_INVALID_SEQUENCE = SLIP_MPSM__STATUS__ERROR_INVALID_SEQUENCE
} SlipMpsm__MsgStatus;

/**
 * @brief Encapsulates the contents of a SLIP encoded message, as well as the state of parsing that message.
 */
typedef struct SlipMpsm__Msg
{
    SlipMpsm__MsgStatus msgStatus; //!< The status of parsing the message.

    uint8_t* buffer; //!< The buffer in which to store the decoded data of the message.
    size_t bufferLen; //!< The maximum length of `dataBuffer`.

    size_t msgLen; //!< The number of bytes in `buffer` that are currently holding message data.
} SlipMpsm__Msg;

/**
 * @brief Initializes a given `SlipMpsm__Msg` structure.
 *
 * Prior to this call, the `dataBuffer` and `dataBufferLen` fields of the given `SlipMpsm__Msg` instance must be
 * initialized.
 *
 * This should be called before the first time `process()` is called with the given `SlipMpsm__Msg` structure, and
 * before `process()` is called again with the same `SlipMpsm__Msg` structure after a complete message is received.
 *
 * @param msg The message structure to initialize.
 *
 * @return One of the following:
 *   - SLIP_MPSM__STATUS__SUCCESS: The function was successful.
 *   - SLIP_MPSM__STATUS__ERROR_NULL: `msg` or `msg->buffer` was NULL.
 *   - SLIP_MPSM__STATUS__ERROR_ZERO_LENGTH_BUFFER: `msg->bufferLen` was zero.
 */
SlipMpsm__Status SlipMpsm__initMsg(SlipMpsm__Msg* msg);

/**
 * @brief Processes a new byte of data towards parsing the next message.
 *
 * @param msg The message structure containing the message being parsed.
 * @param newData The new byte of data being processed.
 *
 * @return One of the following:
 *   - SLIP_MPSM__STATUS__NEED_MORE_DATA: More data is needed to complete the message (The STOP byte hasn't been
 *                                        encountered yet).
 *   - SLIP_MPSM__STATUS__PARSED_MESSAGE: A full message has been successfully parsed.
 *   - SLIP_MPSM__STATUS__ERROR_NULL: `msg` or `msg->dataBuffer` was NULL.
 *   - SLIP_MPSM__STATUS__ERROR_WRONG_STATE: `msg->msgStatus` was not `SLIP_MPSM__MSG_STATUS__IN_PROGRESS`
 *   - SLIP_MPSM__STATUS__ERROR_INVALID_SEQUENCE: Encountered a sequence of bytes that shouldn't be possible with SLIP
 *                                                encoding.
 */
SlipMpsm__Status SlipMpsm__process(SlipMpsm__Msg* msg, uint8_t newData);

/**
 * @}
 */

#endif /* __WATCHDOG_SLIP_MPSM_H__ */
