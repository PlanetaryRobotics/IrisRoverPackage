#include "include/comms/hercules_msgs.h"
#include "include/utils/serialization.h"
/**
 * @brief Check that the status value |actual| returned from a serialization function is greater than or equal to zero.
 *        If |actual| is less than zero, logs a message to the error function and returns
 *        HERC_MSGS__STATUS__ERROR_SERIALIZATION_ERROR.
 *
 * @note Parameter |RESULT_VAR| must be implicitly castable to int.
 */
#define CHECK_SERIALIZATION_RESULT(RESULT_VAR) \
    if (0 > RESULT_VAR) { \
        return HERC_MSGS__STATUS__ERROR_SERIALIZATION_ERROR; \
    }


HercMsgs__Status HercMsgs__serializeHeader(const HercMsgs__Header* src,
                                           void* dst,
                                           size_t dstLen)
{
    if (NULL == src || NULL == dst) {
        return HERC_MSGS__STATUS__ERROR_NULL;
    }

    if (dstLen < HERC_MSGS__PACKED_SIZE__HEADER) {
        return HERC_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* dstIntPtr = (uint8_t*) dst;

    uint8_t magicAsUint = (uint8_t) HERC_MSGS__CONSTANTS__HEADER_MAGIC_ONE;
    short serializationResult = Serialization__serializeAs8Bit(&(magicAsUint), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(magicAsUint);

    magicAsUint = (uint8_t) HERC_MSGS__CONSTANTS__HEADER_MAGIC_TWO;
    serializationResult = Serialization__serializeAs8Bit(&(magicAsUint), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(magicAsUint);

    magicAsUint = (uint8_t) HERC_MSGS__CONSTANTS__HEADER_MAGIC_THREE;
    serializationResult = Serialization__serializeAs8Bit(&(magicAsUint), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(magicAsUint);

    // Calculate parity 
    uint8_t parity = (uint8_t) HERC_MSGS__CONSTANTS__HEADER_MAGIC_SUM;
    parity += (uint8_t) (src->payloadLength & 0x00FFu);
    parity += (uint8_t) ((src->payloadLength >> 8) & 0x00FFu);
    parity += (uint8_t) (src->resetValue & 0x00FFu);
    parity += (uint8_t) ((src->resetValue >> 8) & 0x00FFu);
    parity += (uint8_t) (src->lowerSeqNum & 0x00FFu);
    parity += (uint8_t) ((src->lowerSeqNum >> 8) & 0x00FFu);
    parity += (uint8_t) (src->lowerOpCode & 0x00FFu);
    parity += (uint8_t) ((src->lowerOpCode >> 8) & 0x00FFu);
    parity = ~parity;

    serializationResult = Serialization__serializeAs8Bit(&(parity), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(parity);

    serializationResult = Serialization__serializeAs16Bit(&(src->payloadLength), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(src->payloadLength);

    serializationResult = Serialization__serializeAs16Bit(&(src->resetValue), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(src->resetValue);

    serializationResult = Serialization__serializeAs16Bit(&(src->lowerSeqNum), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(src->lowerSeqNum);

    serializationResult = Serialization__serializeAs16Bit(&(src->lowerOpCode), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);

    return HERC_MSGS__STATUS__SUCCESS;
}
