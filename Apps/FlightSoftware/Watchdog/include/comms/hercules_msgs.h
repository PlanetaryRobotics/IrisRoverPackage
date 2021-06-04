#ifndef __WATCHDOG_HERCULES_MSGS_H___
#define __WATCHDOG_HERCULES_MSGS_H___

typedef enum HercMsgs__Status
{
    HERC_MSGS__STATUS__SUCCESS = 0, /* Operation suceeded. */
    HERC_MSGS__STATUS__ERROR_NULL = -1, /* A required argument or a member of an argument was NULL */
    HERC_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL = -2, 
    HERC_MSGS__STATUS__ERROR_SERIALIZATION_ERROR = -3, 

    HERC_MSGS__STATUS__ERROR__INTERNAL = -255 /* An unexpected internal error occurred. */
} HercMsgs__Status;

typedef enum HercMsgs__Constants
{
    HERC_MSGS__CONSTANTS__HEADER_MAGIC_ONE = 0x0BU,
    HERC_MSGS__CONSTANTS__HEADER_MAGIC_TWO = 0xB0U,
    HERC_MSGS__CONSTANTS__HEADER_MAGIC_THREE = 0x21U,
    HERC_MSGS__CONSTANTS__HEADER_MAGIC_SUM = 0xDCU,
    HERC_MSGS__CONSTANTS__MAX_PAYLOAD_SIZE = 650
}

//######################################################################################################################
// Message Header
//######################################################################################################################

typedef struct HercMsgs__Header
{
    // Magic numbers (first 3 bytes) and parity (4th byte) are not in this structure, as they will be generated
    // automatically. 
    uint16_t payloadLength;
    uint16_t resetValue;
    uint16_t lowerSeqNum;
    uint16_t lowerOpCode;
} HercMsgs__CommonHeader;

//######################################################################################################################
// Message Bodies
//######################################################################################################################

//######################################################################################################################
// Overall Message Structure
//######################################################################################################################

//######################################################################################################################
// Packed Sizes
//######################################################################################################################

typedef enum HercMsgs__PackedSize
{
    HERC_MSGS__PACKED_SIZE__HEADER = (4 * sizeof(uint8_t)) + (4 * sizeof(uint16_t)),
    HERC_MSGS__MAX_DATA_SIZE = 650
} HercMsgs__PackedSize;

//######################################################################################################################
// Message Serialization and Deserialization Functions
//######################################################################################################################

/**
 * @brief Serializes the message in |src| with system endianness into the buffer |dst| with little endianness.
 *
 * @param src [IN] The response to be serialized.
 * @param dst [OUT] The buffer into which the message will be serialized. Must have a large enough size, as determined
 *            by the value of |dstLen|, of at least HERC_MSGS__PACKED_SIZE__HEADER bytes.
 * @param dstLen [IN] The length of the buffer pointed to by |dst|.
 *
 * @return One of the following:
 *   HERC_MSGS__STATUS__SUCCESS: The function was successful.
 *   HERC_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   HERC_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |dst| was too small to hold the serialized message.
 *   HERC_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
HercMsgs__Status HercMsgs__serializeHeader(const HercMsgs__Header* src,
                                           void* dst,
                                           size_t dstLen);

#endif /* __WATCHDOG_HERCULES_MSGS_H___ */