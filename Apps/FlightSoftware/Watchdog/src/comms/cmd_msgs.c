#include "include/comms/cmd_msgs.h"

/**
 * @brief Check that the status value |actual| returned from a serialization function is greater than or equal to zero.
 *        If |actual| is less than zero, logs a message to the error function and returns
 *        CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR.
 *
 * @note Parameter |RESULT_VAR| must be implicitly castable to int.
 */
#define CHECK_SERIALIZATION_RESULT(RESULT_VAR) \
    if (0 > RESULT_VAR) { \
        return CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR; \
    }

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
                           CmdMsgs__CommonHeader* dst)
{
    if (NULL == src || NULL == dst) {
        return CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < CMD_MSGS__PACKED_SIZE__COMMON_HEADER) {
        return CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    short deserializationResult = Serialization__deserializeAs8Bit(srcIntPtr, &(dst->sequenceNumber), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    srcIntPtr += sizeof(dst->sequenceNumber);

    deserializationResult = Serialization__deserializeAs16Bit(srcIntPtr, &(dst->dataLength), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    srcIntPtr += sizeof(dst->dataLength);

    deserializationResult = Serialization__deserializeAs8Bit(srcIntPtr, &(dst->checksum), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    srcIntPtr += sizeof(dst->checksum);

    uint32_t magicNumberUint = 0;
    deserializationResult = Serialization__deserializeAs32Bit(srcIntPtr, &(magicNumberUint), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    dst->typeMagicNumber = (CmdMsgs__CommonHeaderTypeMagicNumber) magicNumberUint;

    return CMD_MSGS__STATUS__SUCCESS;
}