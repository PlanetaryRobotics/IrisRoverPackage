#ifndef __WATCHDOG_HERCULES_MSGS_H___
#define __WATCHDOG_HERCULES_MSGS_H___

#include <stdint.h>
#include <stddef.h>

/**
 * @defgroup watchdogHerculesMsgs Hercules Messages
 * @addtogroup watchdogHerculesMsgs
 * @{
 */

/**
 * @brief Possible return statuses from HercMsgs functions.
 */
typedef enum HercMsgs__Status
{
    HERC_MSGS__STATUS__SUCCESS = 0, //!< The function succeeded.
    HERC_MSGS__STATUS__ERROR_NULL = -1, //!< A required argument or a member of an argument was NULL
    HERC_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL = -2, //!< A given buffer was too small.
    HERC_MSGS__STATUS__ERROR_SERIALIZATION_ERROR = -3, //!< A serialization error occurred.

    HERC_MSGS__STATUS__ERROR__INTERNAL = -255 //!< An unexpected error occurred.
} HercMsgs__Status;

/**
 * @brief Constants related to Hercules messages.
 */
typedef enum HercMsgs__Constants
{
    HERC_MSGS__CONSTANTS__HEADER_MAGIC_ONE = 0x0BU,  //!< The value of the first magic number (at index zero)
    HERC_MSGS__CONSTANTS__HEADER_MAGIC_TWO = 0xB0U,  //!< The value of the second magic number (at index one)
    HERC_MSGS__CONSTANTS__HEADER_MAGIC_THREE = 0x21U,//!< The value of the third magic number (at index two)
    HERC_MSGS__CONSTANTS__HEADER_MAGIC_SUM = 0xDCU,  //!< The sum of the three magic numbers in the header.
    HERC_MSGS__CONSTANTS__MAX_PAYLOAD_SIZE = 650     //!< The maximum size of the payload of a Hercules message.
} HercMsgs__Constants;

//######################################################################################################################
// Message Header
//######################################################################################################################

/**
 * @brief An unpacked Hercules message header.
 *
 * Excludes the magic numbers (first three bytes) and the parity (fourth byte), as they are generated automatically by
 * the serialization function for this header and are not needed after this header is deserialized.
 */
typedef struct HercMsgs__Header
{
    uint16_t payloadLength; //!< The length of the payload.
    uint16_t resetValue; //!< The reset value.
    uint16_t lowerSeqNum; //!< The lower 16 bits of the sequence number.
    uint16_t lowerOpCode; //!< The lower 16 bits of the opcode.
} HercMsgs__Header;

//######################################################################################################################
// Packed Sizes
//######################################################################################################################

/**
 * @brief Packed size constants related to Hercules messages,
 */
typedef enum HercMsgs__PackedSize
{
    /**
     * The packed size of the header of a Hercules message.
     */
    HERC_MSGS__PACKED_SIZE__HEADER = (4 * sizeof(uint8_t)) + (4 * sizeof(uint16_t))
} HercMsgs__PackedSize;

//######################################################################################################################
// Message Serialization and Deserialization Functions
//######################################################################################################################

/**
 * @brief Serializes the message in `src` with system endianness into the buffer `dst` with little endianness.
 *
 * @param src [IN] The header to be serialized.
 * @param dst [OUT] The buffer into which the header will be serialized. Must have a large enough size, as determined
 *                  by the value of `dstLen`, of at least HERC_MSGS__PACKED_SIZE__HEADER bytes.
 * @param dstLen [IN] The length of the buffer pointed to by `dst`.
 *
 * @return One of the following:
 *   - HERC_MSGS__STATUS__SUCCESS: The function was successful.
 *   - HERC_MSGS__STATUS__ERROR_NULL: `src` or `dst` was NULL.
 *   - HERC_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: `dst` was too small to hold the serialized message.
 *   - HERC_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
HercMsgs__Status HercMsgs__serializeHeader(const HercMsgs__Header* src,
                                           void* dst,
                                           size_t dstLen);

/**
 * @}
 */

#endif /* __WATCHDOG_HERCULES_MSGS_H___ */
