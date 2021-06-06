#ifndef __WATCHDOG_CMD_MSGS_H___
#define __WATCHDOG_CMD_MSGS_H___

#include <stdint.h>
#include <stddef.h>

typedef enum CmdMsgs__Status
{
    CMD_MSGS__STATUS__SUCCESS = 0, /* Operation succeeded. */
    CMD_MSGS__STATUS__ERROR_NULL = -1, /* A required argument or a member of an argument was NULL */
    CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL = -2, 
    CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR = -3,

    CMD_MSGS__STATUS__ERROR__INTERNAL = -255 /* An unexpected internal error occurred. */
} CmdMsgs__Status;


typedef enum CmdMsgs__CommonHeaderTypeMagicNumber
{
    HEADER__TYPE_MAGIC_NUM__COMMAND = 0x00BADA55u,
    HEADER__TYPE_MAGIC_NUM__WATCHDOG_COMMAND = 0xC000FFEEu,
    HEADER__TYPE_MAGIC_NUM__TELEMETRY = 0xC00010FFu,
    HEADER__TYPE_MAGIC_NUM__LOG = 0x0DEADBADu,
    HEADER__TYPE_MAGIC_NUM__FILE = 0xDABAD000u
} CmdMsgs__CommonHeaderTypeMagicNumber;

//######################################################################################################################
// Message Header
//######################################################################################################################

typedef struct CmdMsgs__CommonHeader
{
    uint8_t sequenceNumber;
    uint16_t dataLength;
    uint8_t checksum;
    CmdMsgs__CommonHeaderTypeMagicNumber typeMagicNumber;
} CmdMsgs__CommonHeader;

//######################################################################################################################
// Message Bodies
//######################################################################################################################

//######################################################################################################################
// Overall Message Structure
//######################################################################################################################

//######################################################################################################################
// Packed Sizes
//######################################################################################################################

typedef enum CmdMsgs__PackedSize
{
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
 *   CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                             CMD_MSGS__PACKED_SIZE__COMMON_HEADER.
 *   CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
CmdMsgs__Status
CmdMsgs__deserializeHeader(const void* src,
                           size_t srcLen,
                           CmdMsgs__CommonHeader* dst);

#endif /* __WATCHDOG_CMD_MSGS_H___ */
