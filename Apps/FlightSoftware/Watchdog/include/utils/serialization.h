#ifndef __WATCHDOG_SERIALIZATION_H___
#define __WATCHDOG_SERIALIZATION_H___

#include "include/common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup serialization Serialization
 * @addtogroup serialization
 * @{
 */

/**
 * Possible serialization endianess
 */
typedef enum Serialization__Endianness
{
    SERIALIZATION__UNSUPPORTED_ENDIAN = 0, /*!< An endianness is unsupported */
    SERIALIZATION__BIG_ENDIAN, /*!< Big Endian */
    SERIALIZATION__LITTLE_ENDIAN, /*!< Little Endian */
    SERIALIZATION__NETWORK_ENDIAN = SERIALIZATION__BIG_ENDIAN /*!< Network Endian */
} Serialization__Endianness;

/**
 * Error codes for serialization operations.
 */
typedef enum Serialization__StatusCode
{
    SERIALIZATION__SUCCESS = 0, /*!< Operation succeeded. */
    SERIALIZATION__ERROR__NULL = -1, /*!< A required argument was NULL. */
    SERIALIZATION__ERROR__ENDIANNESS = -2, /*!< Encountered an unsupported endianess. */
} Serialization__StatusCode;

/**
 * @brief Serialize an 8 bit value at address |src| into |dst|, making sure the value put into |dst| has network byte
 *        order.
 *
 * @note Data in |src| is assumed to be have the endianess of the host system.
 *
 * @param src
 * @param dst
 * @param endianness endianness of the destination
 *
 * @return If non-negative, represents the number of bytes serialized into |dst|, which should always be equal 1.
 *         If negative, the value is an error code with the following meaning:
 *               SERIALIZATION__ERROR__NULL -> |src| or |dst| was a NULL pointer
 */
short Serialization__serializeAs8Bit(const void *src, void *dst, Serialization__Endianness endianness);

/**
 * @brief Serialize a 16 bit value at address |src| into |dst|, making sure the value put into |dst| has network byte
 *        order.
 *
 * @note Data in |src| is assumed to be have the endianess of the host system.
 *
 * @param src
 * @param dst
 * @param endianness endianness of the destination
 *
 * @return If non-negative, represents the number of bytes serialized into |dst|, which should always be equal 2.
 *         If negative, the value is an error code with the following meaning:
 *               SERIALIZATION__ERROR__NULL -> |src| or |dst| was a NULL pointer
 */
short Serialization__serializeAs16Bit(const void *src, void *dst, Serialization__Endianness endianness);

/**
 * @brief Serialize a 32 bit value at address |src| into |dst|, making sure the value put into |dst| has network byte
 *        order.
 *
 * @note Data in |src| is assumed to be have the endianess of the host system.
 *
 * @param src
 * @param dst
 * @param endianness endianness of the destination
 *
 * @return If non-negative, represents the number of bytes serialized into |dst|, which should always be equal 4.
 *         If negative, the value is an error code with the following meaning:
 *               SERIALIZATION__ERROR__NULL -> |src| or |dst| was a NULL pointer
 */
short Serialization__serializeAs32Bit(const void *src, void *dst, Serialization__Endianness endianness);

/**
 * @brief Serialize a 64 bit value at address |src| into |dst|, making sure the value put into |dst| has network byte
 *        order.
 *
 * @note Data in |src| is assumed to be have the endianess of the host system.
 *
 * @param src
 * @param dst
 * @param endianness endianness of the destination
 *
 * @return If non-negative, represents the number of bytes serialized into |dst|, which should always be equal 8.
 *         If negative, the value is an error code with the following meaning:
 *               SERIALIZATION__ERROR__NULL -> |src| or |dst| was a NULL pointer
 */
short Serialization__serializeAs64Bit(const void *src, void *dst, Serialization__Endianness endianness);

/**
 * @brief Deserialize an 8 bit value at address |src| into |dst|, making sure to convert the source value from network
 *        byte order to host byte order.
 *
 * @note Data in |src| is assumed to be have network byte order.
 *
 * @param src
 * @param dst
 * @param endianness endianness of the src
 *
 * @return If non-negative, represents the number of bytes serialized into |dst|, which should always be equal 1.
 *         If negative, the value is an error code with the following meaning:
 *               SERIALIZATION__ERROR__NULL -> |src| or |dst| was a NULL pointer
 */
short Serialization__deserializeAs8Bit(const void *src, void *dst, Serialization__Endianness endianness);

/**
 * @brief Deserialize a 16 bit value at address |src| into |dst|, making sure to convert the source value from network
 *        byte order to host byte order.
 *
 * @note Data in |src| is assumed to be have network byte order.
 *
 * @param src
 * @param dst
 * @param endianness endianness of the src
 *
 * @return If non-negative, represents the number of bytes serialized into |dst|, which should always be equal 2.
 *         If negative, the value is an error code with the following meaning:
 *               SERIALIZATION__ERROR__NULL -> |src| or |dst| was a NULL pointer
 */
short Serialization__deserializeAs16Bit(const void *src, void *dst, Serialization__Endianness endianness);


/**
 * @brief Deserialize a 32 bit value at address |src| into |dst|, making sure to convert the source value from network
 *        byte order to host byte order.
 *
 * @note Data in |src| is assumed to be have network byte order.
 *
 * @param src
 * @param dst
 * @param endianness endianness of the src
 *
 * @return If non-negative, represents the number of bytes serialized into |dst|, which should always be equal 4.
 *         If negative, the value is an error code with the following meaning:
 *               SERIALIZATION__ERROR__NULL -> |src| or |dst| was a NULL pointer
 */
short Serialization__deserializeAs32Bit(const void *src, void *dst, Serialization__Endianness endianness);

/**
 * @brief Deserialize a 64 bit value at address |src| into |dst|, making sure to convert the source value from network
 *        byte order to host byte order.
 *
 * @note Data in |src| is assumed to be have network byte order.
 *
 * @param src
 * @param dst
 * @param endianness endianness of the src
 *
 * @return If non-negative, represents the number of bytes serialized into |dst|, which should always be equal 8.
 *         If negative, the value is an error code with the following meaning:
 *               SERIALIZATION__ERROR__NULL -> |src| or |dst| was a NULL pointer
 */
short Serialization__deserializeAs64Bit(const void *src, void *dst, Serialization__Endianness endianness);

/**
 * @}
 */

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __WATCHDOG_SERIALIZATION_H___ */