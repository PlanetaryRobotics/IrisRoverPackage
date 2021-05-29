#include "include/utils/serialization.h"

#include <stdint.h>
#include <stddef.h>

#define SERIALIZE_INTEGER_HELPER(srcPtr, dstPtr, numBytes, endianness) \
    do { \
        if (SERIALIZATION__LITTLE_ENDIAN == endianness) \
        { \
            size_t index; \
            for (index = 0; index < numBytes; ++index) \
            { \
                dstPtr[index] = (*srcPtr >> (8u * index)) & 0xFFu; \
            } \
        } \
        else if (SERIALIZATION__BIG_ENDIAN == endianness) \
        { \
            uint8_t highestIndex = numBytes - 1; \
            uint8_t dstIndex; \
            for (dstIndex = 0; dstIndex < numBytes; ++dstIndex) \
            { \
                uint8_t srcByteSelector = highestIndex - dstIndex; \
                dstPtr[dstIndex] = (*srcPtr >> (8u * srcByteSelector)) & 0xFFu; \
            } \
        }\
        else \
        { \
            return SERIALIZATION__ERROR__ENDIANNESS; \
        } \
    } while (0)


#define DESERIALIZE_INTEGER_HELPER(srcPtr, dstPtr, numBytes, endianness) \
    do { \
        *dstPtr = 0; \
        if (endianness == SERIALIZATION__LITTLE_ENDIAN) \
        { \
            int16_t srcIndex; \
            for (srcIndex = numBytes-1; srcIndex >= 0; --srcIndex) \
            { \
                *dstPtr = (*dstIntPtr << 8) | srcIntPtr[srcIndex]; \
            } \
        } \
        else if (endianness == SERIALIZATION__BIG_ENDIAN) \
        { \
            uint8_t srcIndex; \
            for (srcIndex = 0; srcIndex < numBytes; ++srcIndex) \
            { \
                *dstPtr = (*dstPtr << 8) + srcPtr[srcIndex]; \
            } \
        } \
        else \
        { \
            return SERIALIZATION__ERROR__ENDIANNESS; \
        } \
    } while (0)


int Serialization__serializeAs8Bit(const void* src, void* dst, Serialization__Endianness endianness)
{
    if (NULL == src || NULL == dst) {
        return SERIALIZATION__ERROR__NULL;
    }

    uint8_t* srcIntPtr = (uint8_t*)src;
    uint8_t* dstIntPtr = (uint8_t*)dst;

    SERIALIZE_INTEGER_HELPER(srcIntPtr, dstIntPtr, 1, endianness);

    return 1;
}

int Serialization__serializeAs16Bit(const void* src, void* dst, Serialization__Endianness endianness)
{
    if (NULL == src || NULL == dst) {
        return SERIALIZATION__ERROR__NULL;
    }

    uint16_t* srcIntPtr = (uint16_t*)src;
    uint8_t* dstIntPtr = (uint8_t*)dst;

    SERIALIZE_INTEGER_HELPER(srcIntPtr, dstIntPtr, 2, endianness);

    return 2;
}

int Serialization__serializeAs32Bit(const void* src, void* dst, Serialization__Endianness endianness)
{
    if (NULL == src || NULL == dst) {
        return SERIALIZATION__ERROR__NULL;
    }

    uint32_t* srcIntPtr = (uint32_t*)src;
    uint8_t* dstIntPtr = (uint8_t*)dst;

    SERIALIZE_INTEGER_HELPER(srcIntPtr, dstIntPtr, 4, endianness);

    return 4;
}

int Serialization__serializeAs64Bit(const void* src, void* dst, Serialization__Endianness endianness)
{
    if (NULL == src || NULL == dst) {
        return SERIALIZATION__ERROR__NULL;
    }

    uint64_t* srcIntPtr = (uint64_t*)src;
    uint8_t* dstIntPtr = (uint8_t*)dst;

    SERIALIZE_INTEGER_HELPER(srcIntPtr, dstIntPtr, 8, endianness);

    return 8;
}

int Serialization__deserializeAs8Bit(const void* src, void* dst, Serialization__Endianness endianness)
{
    if (NULL == src || NULL == dst) {
        return SERIALIZATION__ERROR__NULL;
    }

    uint8_t* srcIntPtr = (uint8_t*)src;
    uint8_t* dstIntPtr = (uint8_t*)dst;

    DESERIALIZE_INTEGER_HELPER(srcIntPtr, dstIntPtr, 1, endianness);

    return 1;
}

int Serialization__deserializeAs16Bit(const void* src, void* dst, Serialization__Endianness endianness)
{
    if (NULL == src || NULL == dst) {
        return SERIALIZATION__ERROR__NULL;
    }

    uint8_t* srcIntPtr = (uint8_t*)src;
    uint16_t* dstIntPtr = (uint16_t*)dst;

    DESERIALIZE_INTEGER_HELPER(srcIntPtr, dstIntPtr, 2, endianness);

    return 2;
}


int Serialization__deserializeAs32Bit(const void* src, void* dst, Serialization__Endianness endianness)
{
    if (NULL == src || NULL == dst) {
        return SERIALIZATION__ERROR__NULL;
    }

    uint8_t* srcIntPtr = (uint8_t*)src;
    uint32_t* dstIntPtr = (uint32_t*)dst;

    DESERIALIZE_INTEGER_HELPER(srcIntPtr, dstIntPtr, 4, endianness);

    return 4;
}

int Serialization__deserializeAs64Bit(const void* src, void* dst, Serialization__Endianness endianness)
{
    if (NULL == src || NULL == dst) {
        return SERIALIZATION__ERROR__NULL;
    }

    uint8_t* srcIntPtr = (uint8_t*)src;
    uint64_t* dstIntPtr = (uint64_t*)dst;

    DESERIALIZE_INTEGER_HELPER(srcIntPtr, dstIntPtr, 8, endianness);

    return 8;
}