#ifndef __WATCHDOG_CMD_MSGS_H___
#define __WATCHDOG_CMD_MSGS_H___

#include <stdint.h>
#include <stddef.h>

/**
 * @defgroup watchdogCmdMsgs Generic Messages
 * @addtogroup watchdogCmdMsgs
 * @{
 */

/**
 * @brief Possible return statuses of CmdMsgs functions.
 */
typedef enum CmdMsgs__Status
{
    CMD_MSGS__STATUS__SUCCESS = 0, //!< Operation succeeded.
    CMD_MSGS__STATUS__ERROR_NULL = -1, //!< A required argument or a member of an argument was NULL.
    CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL = -2, //!< A given buffer was too small.
    CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR = -3, //!< A serialization error occurred.

    CMD_MSGS__STATUS__ERROR__INTERNAL = -255 //!< An unexpected error occurred.
} CmdMsgs__Status;


/**
 * @brief Possible values of the "Type Magic" field in the FSW common header.
 */
typedef enum CmdMsgs__CommonHeaderTypeMagicNumber
{
    HEADER__TYPE_MAGIC_NUM__COMMAND = 0x00BADA55u, //!< The magic number for Hercules commands.
    HEADER__TYPE_MAGIC_NUM__WATCHDOG_COMMAND = 0xC000FFEEu, //!< The magic number for Watchdog commands.
    HEADER__TYPE_MAGIC_NUM__TELEMETRY = 0xC00010FFu, //!< The magic number for telemetry.
    HEADER__TYPE_MAGIC_NUM__LOG = 0x0DEADBADu, //!< The magic number for log messages.
    HEADER__TYPE_MAGIC_NUM__FILE = 0xDABAD000u //!< The magic number for file messages.
} CmdMsgs__CommonHeaderTypeMagicNumber;

//######################################################################################################################
// Message Header
//######################################################################################################################

/**
 * @brief The contents of the FSW common header.
 */
typedef struct CmdMsgs__CommonHeader
{
    uint8_t sequenceNumber; //!< The sequence number.
    uint16_t dataLength; //!< The length of the payload.
    uint8_t checksum; //!< The checksum.
    CmdMsgs__CommonHeaderTypeMagicNumber typeMagicNumber; //!< The magic number specifying the type of the message.
} CmdMsgs__CommonHeader;

//######################################################################################################################
// Packed Sizes
//######################################################################################################################

/**
 * @brief Packed sizes of CmdMsgs structures.
 */
typedef enum CmdMsgs__PackedSize
{
    /**
     * @brief The packed size of the FSW common header.
     */
    CMD_MSGS__PACKED_SIZE__COMMON_HEADER = (2 * sizeof(uint8_t)) + sizeof(uint16_t) + sizeof(uint32_t)
} CmdMsgs__PackedSize;

//######################################################################################################################
// Message Serialization and Deserialization Functions
//######################################################################################################################

/**
 * @brief Deserializes the message header packed in |src| with little endianness into the struct |dst| with system
 *        endianness.
 *
 * @param src [IN] The buffer containing the packed message header to be deserialized. Must have a size, as determined
 *            by the value of |srcLen|, of at least CMD_MSGS__PACKED_SIZE__COMMON_HEADER bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               CMD_MSGS__PACKED_SIZE__COMMON_HEADER.
 * @param dst [OUT] The struct into which the message header will be deserialized.
 *
 * @return One of the following:
 *   - CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                               CMD_MSGS__PACKED_SIZE__COMMON_HEADER.
 *   - CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
CmdMsgs__Status
CmdMsgs__deserializeHeader(const void* src,
                           size_t srcLen,
                           CmdMsgs__CommonHeader* dst);

/**
 * @}
 */

#endif /* __WATCHDOG_CMD_MSGS_H___ */
