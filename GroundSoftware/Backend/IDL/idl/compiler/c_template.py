SERIALIZE_BODY_FUNCTION_DECLARATION_TEMPLATE = """/**
 * @brief Serializes the {message_name_pretty} message body in |src| with system endianness into the buffer |dst| with
 *        network endianness.
 *
 * @param src [IN] The message body to be serialized.
 * @param dst [OUT] The buffer into which the message body will be serialized. Must have a size, as determined by the
 *            value of |dstLen|, of at least TELEOP_MESSAGES__PACKED_SIZE__{message_name_screaming_snake}_BODY bytes.
 * @param dstLen [IN] The length of the buffer pointed to by |dst|. Must be at least
 *               TELEOP_MESSAGES__PACKED_SIZE__{message_name_screaming_snake}_BODY.
 *
 * @return TeleopMessages__Status One of the following:
 *   TELEOP_MESSAGES__STATUS__SUCCESS: The function was successful.
 *   TELEOP_MESSAGES__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   TELEOP_MESSAGES__STATUS__ERROR_BUFFER_TOO_SMALL: |dstLen| was less than
 *                                                    TELEOP_MESSAGES__PACKED_SIZE__{message_name_screaming_snake}_BODY.
 *   TELEOP_MESSAGES__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
TeleopMessages__Status
TeleopMessages__serialize{message_name_pascal_case}Body(const TeleopMessages__{message_name_pascal_case}Body *src,
                                   void *dst,
                                   size_t dstLen);"""

DESERIALIZE_BODY_FUNCTION_DECLARATION_TEMPLATE = """/**
 * @brief Deserializes the {message_name_pretty} message body packed in |src| with network endianness into the struct
 *        |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least TELEOP_MESSAGES__PACKED_SIZE__{message_name_screaming_snake}_BODY
 *            bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               TELEOP_MESSAGES__PACKED_SIZE__{message_name_screaming_snake}_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return TeleopMessages__Status One of the following:
 *   TELEOP_MESSAGES__STATUS__SUCCESS: The function was successful.
 *   TELEOP_MESSAGES__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   TELEOP_MESSAGES__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                    TELEOP_MESSAGES__PACKED_SIZE__{message_name_screaming_snake}_BODY.
 *   TELEOP_MESSAGES__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
TeleopMessages__Status
TeleopMessages__deserialize{message_name_pascal_case}Body(const void *src,
                                     size_t srcLen,
                                     TeleopMessages__{message_name_pascal_case}Body *dst);"""

VALID_MSG_ID_CASE_TEMPLATE = "        case TELEOP_MESSAGES__MESSAGE_ID__{message_name_screaming_snake}:"

GET_PACKED_MSG_SIZE_CASE_TEMPLATE = """        case TELEOP_MESSAGES__MESSAGE_ID__{message_name_screaming_snake}:
            *msgSize = TELEOP_MESSAGES__PACKED_SIZE__{message_name_screaming_snake}_MSG;
            break;"""

SERIALIZE_BODY_CASE_TEMPLATE = """        case TELEOP_MESSAGES__MESSAGE_ID__{message_name_screaming_snake}:
            return TeleopMessages__serialize{message_name_pascal_case}Body(&(src->{message_name_camel_case}),
                                                                           dst,
                                                                           dstLen);"""

DESERIALIZE_BODY_CASE_TEMPLATE = """        case TELEOP_MESSAGES__MESSAGE_ID__{message_name_screaming_snake}:
            return TeleopMessages__deserialize{message_name_pascal_case}Body(src,
                                                                             srcLen,
                                                                             &(dst->{message_name_camel_case}));"""

SERIALIZE_UINT8_OR_INT8_FIELD_TEMPLATE = \
    """    serializationResult = Serialization__serializeAs8Bit(&(src->{field_name_camel_case}),
                                                         dstIntPtr,
                                                         SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(src->{field_name_camel_case});"""

SERIALIZE_UINT16_OR_INT16_FIELD_TEMPLATE = \
    """    serializationResult = Serialization__serializeAs16Bit(&(src->{field_name_camel_case}),
                                                         dstIntPtr,
                                                         SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(src->{field_name_camel_case});"""

SERIALIZE_UINT32_OR_INT32_FIELD_TEMPLATE = \
    """    serializationResult = Serialization__serializeAs32Bit(&(src->{field_name_camel_case}),
                                                         dstIntPtr,
                                                         SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(src->{field_name_camel_case});"""

SERIALIZE_UINT64_OR_INT64_FIELD_TEMPLATE = \
    """    serializationResult = Serialization__serializeAs64Bit(&(src->{field_name_camel_case}),
                                                         dstIntPtr,
                                                         SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(src->{field_name_camel_case});"""

SERIALIZE_FLOAT_FIELD_TEMPLATE = \
    """    union Convert{field_name_camel_case} {{
        float floatVal;
        uint32_t intVal;
    }} convert{field_name_camel_case} = { 0 };
    convert{field_name_camel_case}.floatVal = src->{field_name_camel_case};
    serializationResult = Serialization__serializeAs32Bit(&(convert{field_name_camel_case}.intVal), dstIntPtr,
                                                          SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(convert{field_name_camel_case}.intVal);"""

SERIALIZE_DOUBLE_FIELD_TEMPLATE = \
    """    union Convert{field_name_camel_case} {{
        double doubleVal;
        uint64_t intVal;
    }} convert{field_name_camel_case} = { 0 };
    convert{field_name_camel_case}.doubleVal = src->{field_name_camel_case};
    serializationResult = Serialization__serializeAs64Bit(&(convert{field_name_camel_case}.intVal), dstIntPtr,
                                                          SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(convert{field_name_camel_case}.intVal);"""

SERIALIZE_EMPTY_BODY_FUNCTION_DEFINITION_TEMPLATE = """TeleopMessages__Status
TeleopMessages__serialize{message_name_pascal_case}Body(const TeleopMessages__{message_name_pascal_case}Body *src,
                                   void *dst,
                                   size_t dstLen)
{{
    NULL_CHECK_LOG_ERROR(src, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");
    NULL_CHECK_LOG_ERROR(dst, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");

    UNUSED(dstLen);

    return TELEOP_MESSAGES__STATUS__SUCCESS;
}}"""

SERIALIZE_BODY_FUNCTION_DEFINITION_TEMPLATE = """TeleopMessages__Status
TeleopMessages__serialize{message_name_pascal_case}Body(const TeleopMessages__{message_name_pascal_case}Body *src,
                                          void *dst,
                                          size_t dstLen)
{{
    NULL_CHECK_LOG_ERROR(src, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");
    NULL_CHECK_LOG_ERROR(dst, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");

    if (dstLen < TELEOP_MESSAGES__PACKED_SIZE__{message_name_screaming_snake}_BODY)
    {{
        return TELEOP_MESSAGES__STATUS__ERROR_BUFFER_TOO_SMALL;
    }}

    uint8_t *dstIntPtr = (uint8_t *) dst;
    int serializationResult = 0;

{code_to_serialize_fields}

    return TELEOP_MESSAGES__STATUS__SUCCESS;
}}"""

DESERIALIZE_UINT8_OR_INT8_FIELD_TEMPLATE = \
    """    deserializationResult = Serialization__deserializeAs8Bit(srcIntPtr,
                                                             &(dst->{field_name_camel_case}),
                                                             SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    srcIntPtr += sizeof(dst->{field_name_camel_case});"""

DESERIALIZE_UINT16_OR_INT16_FIELD_TEMPLATE = \
    """    deserializationResult = Serialization__deserializeAs16Bit(srcIntPtr,
                                                             &(dst->{field_name_camel_case}),
                                                             SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    srcIntPtr += sizeof(dst->{field_name_camel_case});"""

DESERIALIZE_UINT32_OR_INT32_FIELD_TEMPLATE = \
    """    deserializationResult = Serialization__deserializeAs32Bit(srcIntPtr,
                                                             &(dst->{field_name_camel_case}),
                                                             SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    srcIntPtr += sizeof(dst->{field_name_camel_case});"""

DESERIALIZE_UINT64_OR_INT64_FIELD_TEMPLATE = \
    """    deserializationResult = Serialization__deserializeAs64Bit(srcIntPtr,
                                                             &(dst->{field_name_camel_case}),
                                                             SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    srcIntPtr += sizeof(dst->{field_name_camel_case});"""

DESERIALIZE_FLOAT_FIELD_TEMPLATE = \
    """    union Convert{field_name_camel_case} {{
        float floatVal;
        uint32_t intVal;
    }} convert{field_name_camel_case} = { 0 };
    int deserializationResult = Serialization__deserializeAs32Bit(srcIntPtr, &(convert{field_name_camel_case}.intVal),
                                                                  SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    srcIntPtr += sizeof(convert{field_name_camel_case}.intVal);
    dst->{field_name_camel_case} = convert{field_name_camel_case}.floatVal;"""

DESERIALIZE_DOUBLE_FIELD_TEMPLATE = \
    """    union Convert{field_name_camel_case} {{
        double doubleVal;
        uint64_t intVal;
    }} convert{field_name_camel_case} = { 0 };
    int deserializationResult = Serialization__deserializeAs64Bit(srcIntPtr, &(convert{field_name_camel_case}.intVal),
                                                                  SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    srcIntPtr += sizeof(convert{field_name_camel_case}.intVal);
    dst->{field_name_camel_case} = convert{field_name_camel_case}.doubleVal;"""

DESERIALIZE_EMPTY_BODY_FUNCTION_DEFINITION_TEMPLATE = """TeleopMessages__Status
TeleopMessages__deserialize{message_name_pascal_case}Body(const void *src,
                                     size_t srcLen,
                                     TeleopMessages__{message_name_pascal_case}Body *dst)
{{
    NULL_CHECK_LOG_ERROR(src, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");
    NULL_CHECK_LOG_ERROR(dst, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");

    UNUSED(srcLen);

    return TELEOP_MESSAGES__STATUS__SUCCESS;
}}"""

DESERIALIZE_BODY_FUNCTION_DEFINITION_TEMPLATE = """TeleopMessages__Status
TeleopMessages__deserialize{message_name_pascal_case}Body(const void *src,
                                            size_t srcLen,
                                            TeleopMessages__{message_name_pascal_case}Body *dst)
{{
    NULL_CHECK_LOG_ERROR(src, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");
    NULL_CHECK_LOG_ERROR(dst, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");

    if (srcLen < TELEOP_MESSAGES__PACKED_SIZE__{message_name_screaming_snake}_BODY)
    {{
        return TELEOP_MESSAGES__STATUS__ERROR_BUFFER_TOO_SMALL;
    }}

    uint8_t *srcIntPtr = (uint8_t *) src;
    int deserializationResult = 0;

{code_to_deserialize_fields}

    return TELEOP_MESSAGES__STATUS__SUCCESS;
}}"""

TELEOP_MESSAGES_SOURCE_FILE_TEMPLATE = """
#include "TeleopMessages.h"

#include "Serialization.h"
#include "Logging.h"
#include "ProjectDefines.h"

/**
 * @brief Check that the status value |actual| returned from a serialization function is greater than or equal to zero.
 *        If |actual| is less than zero, logs a message to the error function and returns
 *        TELEOP_MESSAGES__STATUS__ERROR_SERIALIZATION_ERROR.
 *
 * @note Parameter |RESULT_VAR| must be implicitly castable to int.
 */
#define CHECK_SERIALIZATION_RESULT(RESULT_VAR) \\
    do {{ \\
        if (0 > RESULT_VAR) \\
        {{ \\
            ERROR_MESSAGE_FORMATTED("Serialization or deserialization failed: return value = %i", RESULT_VAR); \\
            return TELEOP_MESSAGES__STATUS__ERROR_SERIALIZATION_ERROR; \\
        }} \\
    }} while (0)

//######################################################################################################################
// Private Function Forward Declarations
//######################################################################################################################

//###############################################
//################ SERIALIZATION ################
//###############################################

{serialize_body_function_declarations}

//#################################################
//################ DESERIALIZATION ################
//#################################################

{deserialize_body_function_declarations}

//######################################################################################################################
// Public Function Definitions
//######################################################################################################################

TeleopMessages__Status
TeleopMessages__isValidMessageId(TeleopMessages__MessageId msgId)
{{
    switch (msgId)
    {{
{valid_msg_id_cases}
            return TELEOP_MESSAGES__STATUS__SUCCESS;

        default:
            return TELEOP_MESSAGES__STATUS__ERROR_UNKNOWN_MESSAGE_ID;
    }}
}}

TeleopMessages__Status
TeleopMessages__getPackedMessageSize(TeleopMessages__MessageId msgId,
                                     size_t *msgSize)
{{
    NULL_CHECK_LOG_ERROR(msgSize, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");

    switch (msgId)
    {{
{get_packed_msg_size_cases}

        default:
            *msgSize = 0;
            ERROR_MESSAGE_FORMATTED("Invalid message ID: %hu", (uint16_t) msgId);
            return TELEOP_MESSAGES__STATUS__ERROR_UNKNOWN_MESSAGE_ID;
    }}

    return TELEOP_MESSAGES__STATUS__SUCCESS;
}}


TeleopMessages__Status
TeleopMessages__serializeHeader(const TeleopMessages__Header *src,
                                void *dst,
                                size_t dstLen)
{{
    NULL_CHECK_LOG_ERROR(src, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");
    NULL_CHECK_LOG_ERROR(dst, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");

    if (dstLen < TELEOP_MESSAGES__PACKED_SIZE__HEADER)
    {{
        return TELEOP_MESSAGES__STATUS__ERROR_BUFFER_TOO_SMALL;
    }}

    uint16_t msgIdAsUint = (uint16_t) src->messageId;
    uint8_t commandTypeId = (uint8_t) (((uint16_t) (msgIdAsUint >> 8u)) & 0x00FFu);
    uint8_t commandId = (uint8_t) (msgIdAsUint & 0x00FFu);

    uint8_t *dstIntPtr = (uint8_t *) dst;
    int serializationResult = Serialization__serializeAs8Bit(&commandTypeId, dstIntPtr, SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(commandTypeId);

    serializationResult = Serialization__serializeAs8Bit(&commandId, dstIntPtr, SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(commandId);

    serializationResult = Serialization__serializeAs16Bit(&(src->dataLen), dstIntPtr, SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(src->dataLen);

    serializationResult = Serialization__serializeAs64Bit(&(src->timestamp), dstIntPtr, SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);

    return TELEOP_MESSAGES__STATUS__SUCCESS;
}}


TeleopMessages__Status
TeleopMessages__serializeBody(TeleopMessages__MessageId srcMsgId,
                              const TeleopMessages__Body *src,
                              void *dst,
                              size_t dstLen)
{{
    NULL_CHECK_LOG_ERROR(src, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");
    NULL_CHECK_LOG_ERROR(dst, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");

    switch (srcMsgId)
    {{
{serialize_body_cases}

        default:
            ERROR_MESSAGE_FORMATTED("Invalid message ID: %hu", (uint16_t) srcMsgId);
            return TELEOP_MESSAGES__STATUS__ERROR_UNKNOWN_MESSAGE_ID;
    }}
}}


TeleopMessages__Status
TeleopMessages__serializeMessage(const TeleopMessages__Message *src,
                                 void *dst,
                                 size_t dstLen)
{{
    NULL_CHECK_LOG_ERROR(src, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");
    NULL_CHECK_LOG_ERROR(dst, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");

    uint8_t *dstIntPtr = (uint8_t *) dst;
    TeleopMessages__Status serializeStatus = TeleopMessages__serializeHeader(&(src->header), dstIntPtr, dstLen);
    STATUS_CHECK_LOG_ERROR(TELEOP_MESSAGES__STATUS__SUCCESS, serializeStatus, serializeStatus,
                           "Failed to serialize message header");

    dstIntPtr += (size_t) TELEOP_MESSAGES__PACKED_SIZE__HEADER;
    size_t bodyDstLen = dstLen - (size_t) TELEOP_MESSAGES__PACKED_SIZE__HEADER;

    serializeStatus = TeleopMessages__serializeBody(src->header.messageId,
                                                    &(src->body),
                                                    dstIntPtr,
                                                    bodyDstLen);
    STATUS_CHECK_LOG_ERROR(TELEOP_MESSAGES__STATUS__SUCCESS, serializeStatus, serializeStatus,
                           "Failed to serialize message body");

    return TELEOP_MESSAGES__STATUS__SUCCESS;
}}


TeleopMessages__Status
TeleopMessages__deserializeHeader(const void *src,
                                  size_t srcLen,
                                  TeleopMessages__Header *dst)
{{
    NULL_CHECK_LOG_ERROR(src, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");
    NULL_CHECK_LOG_ERROR(dst, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");

    if (srcLen < TELEOP_MESSAGES__PACKED_SIZE__HEADER)
    {{
        return TELEOP_MESSAGES__STATUS__ERROR_BUFFER_TOO_SMALL;
    }}

    uint8_t commandTypeId = 0;
    uint8_t commandId = 0;
    uint16_t msgIdAsUint = 0;

    uint8_t *srcIntPtr = (uint8_t *) src;
    int deserializationResult = Serialization__deserializeAs8Bit(srcIntPtr, &commandTypeId,
                                                                 SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    srcIntPtr += sizeof(commandTypeId);

    deserializationResult = Serialization__deserializeAs8Bit(srcIntPtr, &commandId, SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    srcIntPtr += sizeof(commandId);

    msgIdAsUint = ((uint16_t) (((uint16_t) commandTypeId) << 8u)) | ((uint16_t) commandId);
    dst->messageId = (TeleopMessages__MessageId) msgIdAsUint;

    deserializationResult = Serialization__deserializeAs16Bit(srcIntPtr, &(dst->dataLen),
                                                              SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    srcIntPtr += sizeof(dst->dataLen);

    deserializationResult = Serialization__deserializeAs64Bit(srcIntPtr, &(dst->timestamp),
                                                              SERIALIZATION__NETWORK_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);

    return TELEOP_MESSAGES__STATUS__SUCCESS;
}}


TeleopMessages__Status
TeleopMessages__deserializeBody(TeleopMessages__MessageId srcMsgId,
                                const void *src,
                                size_t srcLen,
                                TeleopMessages__Body *dst)
{{
    NULL_CHECK_LOG_ERROR(src, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");
    NULL_CHECK_LOG_ERROR(dst, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");

    switch (srcMsgId)
    {{
{deserialize_body_cases}

        default:
            ERROR_MESSAGE_FORMATTED("Invalid message ID: %hu", (uint16_t) srcMsgId);
            return TELEOP_MESSAGES__STATUS__ERROR_UNKNOWN_MESSAGE_ID;
    }}
}}


TeleopMessages__Status
TeleopMessages__deserializeMessage(const void *src,
                                   size_t srcLen,
                                   TeleopMessages__Message *dst)
{{
    NULL_CHECK_LOG_ERROR(src, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");
    NULL_CHECK_LOG_ERROR(dst, TELEOP_MESSAGES__STATUS__ERROR_NULL, "Argument is NULL");

    uint8_t *srcIntPtr = (uint8_t *) src;
    TeleopMessages__Status deserializeStatus = TeleopMessages__deserializeHeader(srcIntPtr, srcLen, &(dst->header));
    STATUS_CHECK_LOG_ERROR(TELEOP_MESSAGES__STATUS__SUCCESS, deserializeStatus, deserializeStatus,
                           "Failed to deserialize message header");

    srcIntPtr += (size_t) TELEOP_MESSAGES__PACKED_SIZE__HEADER;
    size_t bodySrcLen = srcLen - (size_t) TELEOP_MESSAGES__PACKED_SIZE__HEADER;

    deserializeStatus = TeleopMessages__deserializeBody(dst->header.messageId,
                                                        srcIntPtr,
                                                        bodySrcLen,
                                                        &(dst->body));
    STATUS_CHECK_LOG_ERROR(TELEOP_MESSAGES__STATUS__SUCCESS, deserializeStatus, deserializeStatus,
                           "Failed to deserialize message body");

    return TELEOP_MESSAGES__STATUS__SUCCESS;
}}

//######################################################################################################################
// Private Function Definitions
//######################################################################################################################

//###############################################
//################ SERIALIZATION ################
//###############################################

{serialize_body_function_definitions}

//#################################################
//################ DESERIALIZATION ################
//#################################################

{deserialize_body_function_definitions}
"""

MESSAGE_ID_MEMBER_TEMPLATE = "    TELEOP_MESSAGES__MESSAGE_ID__{message_name_screaming_snake} = " \
                             "0x{command_type_id_int:02X}{command_id_int:02X}"

MESSAGE_BODY_FIELD_TEMPLATE = "    {type} {name};"

MESSAGE_BODY_DECLARATION_TEMPLATE = """typedef struct TeleopMessages__{message_name_pascal_case}Body
{{
{message_body_fields}
}} TeleopMessages__{message_name_pascal_case}Body;"""

MESSAGE_BODY_MEMBER_TEMPLATE = "    TeleopMessages__{message_name_pascal_case}Body {message_name_camel_case};"

BODY_PACKED_SIZE_ENUM_MEMBER_TEMPLATE = """    /**
     * The packed size of a TeleopMessages__{message_name_pascal_case}Body.
     */
    TELEOP_MESSAGES__PACKED_SIZE__{message_name_screaming_snake}_BODY = {body_packed_size}"""

MSG_PACKED_SIZE_ENUM_MEMBER_TEMPLATE = """    /**
     * The packed size of a TeleopMessages__Message when it contains an {message_name_pretty} message.
     */
    TELEOP_MESSAGES__PACKED_SIZE__{message_name_screaming_snake}_MSG =
            TELEOP_MESSAGES__PACKED_SIZE__HEADER + TELEOP_MESSAGES__PACKED_SIZE__{message_name_screaming_snake}_BODY"""

TELEOP_MESSAGES_HEADER_FILE_TEMPLATE = """
#ifndef __TELEOP_MESSAGES_H___
#define __TELEOP_MESSAGES_H___

#include "Logging.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{{
#endif

/**
 * @defgroup teleopMessages Teleop Messages
 *
 * @brief Defines the formats of the messages sent to and from the CubeRover, and also provides functions
 *        to serialize and deserialize these messages.
 */

/**
 * @addtogroup teleopMessages
 * @{{
 */

/**
 * Possible return statuses of serialization and deserialization functions.
 */
typedef enum TeleopMessages__Status
{{
    TELEOP_MESSAGES__STATUS__SUCCESS = 0, /*!< The function was successful. */
    TELEOP_MESSAGES__STATUS__ERROR_NULL = -1, /*!< An argument to the function was NULL. */
    TELEOP_MESSAGES__STATUS__ERROR_BUFFER_TOO_SMALL = -2, /*!< The input or output buffer is too small. */
    TELEOP_MESSAGES__STATUS__ERROR_SERIALIZATION_ERROR = -3, /*!< A Serialization function call returned an error. */
    TELEOP_MESSAGES__STATUS__ERROR_UNKNOWN_MESSAGE_ID = -4 /*!< An unknown message ID was encountered. */
}} TeleopMessages__Status;

/**
 * The message IDs of the CubeRover messages.
 *
 * All message ID values should be a 16-bit value, where the first byte is the Command Type ID, and the second byte is
 * the Command ID.
 */
typedef enum TeleopMessages__MessageId
{{
{message_id_members}
}} TeleopMessages__MessageId;

//######################################################################################################################
// Message Header
//######################################################################################################################

/**
 * The generic header for a message sent or received by the controller.
 */
typedef struct TeleopMessages__Header
{{
    TeleopMessages__MessageId messageId; //!< The message ID, the integer value of which fits in a uint16_t.
    uint16_t dataLen; //!< The complete size, in bytes, of the message.
    uint64_t timestamp; //!< The time at which this message was sent, in microseconds.
}} TeleopMessages__Header;

//######################################################################################################################
// Message Bodies
//######################################################################################################################

{message_body_declarations}

//######################################################################################################################
// Overall Message Structure
//######################################################################################################################

/**
 * The body of a generic teleoperation message.
 */
typedef union
{{
{message_body_members}
}} TeleopMessages__Body;

/**
 * A generic teleoperation message.
 */
typedef struct TeleopMessages__Message
{{
    TeleopMessages__Header header; //!< The message header.
    TeleopMessages__Body body; //!< The message body, the type of which is determined by the value of |header.msgId|.
}} TeleopMessages__Message;

/**
 * The size, in bytes, of TeleopMessages messages (and message components) when they are packed (i.e., serialized).
 */
typedef enum TeleopMessages_PackedSize
{{
    /**
     * The packed size of a TeleopMessages__Header.
     */
    TELEOP_MESSAGES__PACKED_SIZE__HEADER = (2 * sizeof(uint16_t)) + sizeof(uint64_t),

{body_packed_size_members},

    //#################################################################################################################
    //#################################################################################################################

{msg_packed_size_members},

    /**
     * The largest possible packed size of a TeleopMessages__Message.
     */
    TELEOP_MESSAGES__PACKED_SIZE__LARGEST_MSG = TELEOP_MESSAGES__PACKED_SIZE__{largest_msg_screaming_snake}_MSG
}} TeleopMessages_PackedSize;

//######################################################################################################################
// Message Serialization and Deserialization Functions
//######################################################################################################################

/**
 * @brief Checks if |msgId| is the message ID of one of the messages defined in this file.
 *
 * @param msgId [IN] The message ID.
 *
 * @return TeleopMessages__Status One of the following:
 *   TELEOP_MESSAGES__STATUS__SUCCESS: |msgId| is a known message ID.
 *   TELEOP_MESSAGES__STATUS__ERROR_UNKNOWN_MESSAGE_ID: |msgId| is not a known message ID.
 */
TeleopMessages__Status
TeleopMessages__isValidMessageId(TeleopMessages__MessageId msgId);

/**
 * @brief Retrieves the packed size of the message type with the message ID given in |msgId|.
 *
 * @param msgId [IN] The message ID.
 * @param msgSize [OUT] The packed size of the message type with the message ID given by |msgId|.
 *
 * @return TeleopMessages__Status One of the following:
 *   TELEOP_MESSAGES__STATUS__SUCCESS: The function was successful.
 *   TELEOP_MESSAGES__STATUS__ERROR_NULL: |msgSize| was NULL.
 *   TELEOP_MESSAGES__STATUS__ERROR_UNKNOWN_MESSAGE_ID: |msgId| was not a known message ID.
 */
TeleopMessages__Status
TeleopMessages__getPackedMessageSize(TeleopMessages__MessageId msgId,
                                     size_t *msgSize);

/**
 * @brief Serializes the message header in |src| with system endianness into the buffer |dst| with network endianness.
 *
 * @param src [IN] The header to be serialized.
 * @param dst [OUT] The buffer into which the header will be serialized. Must have a size, as determined by the value of
 *            |dstLen|, of at least TELEOP_MESSAGES__PACKED_SIZE__HEADER bytes.
 * @param dstLen [IN] The length of the buffer pointed to by |dst|. Must be at least
 *               TELEOP_MESSAGES__PACKED_SIZE__HEADER.
 *
 * @return TeleopMessages__Status One of the following:
 *   TELEOP_MESSAGES__STATUS__SUCCESS: The function was successful.
 *   TELEOP_MESSAGES__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   TELEOP_MESSAGES__STATUS__ERROR_BUFFER_TOO_SMALL: |dstLen| was less than
 *                                                        TELEOP_MESSAGES__PACKED_SIZE__HEADER.
 *   TELEOP_MESSAGES__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
TeleopMessages__Status
TeleopMessages__serializeHeader(const TeleopMessages__Header *src,
                                void *dst,
                                size_t dstLen);

/**
 * @brief Serializes the message body in |src| with system endianness into the buffer |dst| with network endianness.
 *
 * The type of body in |src| is determined by |srcMsgId|.
 *
 * @param srcMsgId [IN] The message ID of the message body type stored in |src|.
 * @param src [IN] The message body to be serialized.
 * @param dst [OUT] The buffer into which the message body will be serialized. Must have a large enough size, as
 *            determined by the value of |dstLen|, to store the message body in |src|. The exact size required varies
 *            based on the type of the message body.
 * @param dstLen [IN] The length of the buffer pointed to by |dst|.
 *
 * @return TeleopMessages__Status One of the following:
 *   TELEOP_MESSAGES__STATUS__SUCCESS: The function was successful.
 *   TELEOP_MESSAGES__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   TELEOP_MESSAGES__STATUS__ERROR_BUFFER_TOO_SMALL: |dst| was too small to hold the serialized message body.
 *   TELEOP_MESSAGES__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 *   TELEOP_MESSAGES__STATUS__ERROR_UNKNOWN_MESSAGE_ID: |msgId| was not a known message ID.
 */
TeleopMessages__Status
TeleopMessages__serializeBody(TeleopMessages__MessageId srcMsgId,
                              const TeleopMessages__Body *src,
                              void *dst,
                              size_t dstLen);

/**
 * @brief Serializes the message in |src| with system endianness into the buffer |dst| with network endianness.
 *
 * @param src [IN] The header to be serialized.
 * @param dst [OUT] The buffer into which the message will be serialized. Must have a large enough size, as determined
 *            by the value of |dstLen|, to store the message in |src|. The exact size required varies based on the type
 *            of the message.
 * @param dstLen [IN] The length of the buffer pointed to by |dst|.
 *
 * @return TeleopMessages__Status One of the following:
 *   TELEOP_MESSAGES__STATUS__SUCCESS: The function was successful.
 *   TELEOP_MESSAGES__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   TELEOP_MESSAGES__STATUS__ERROR_BUFFER_TOO_SMALL: |dst| was too small to hold the serialized message.
 *   TELEOP_MESSAGES__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 *   TELEOP_MESSAGES__STATUS__ERROR_UNKNOWN_MESSAGE_ID: |src->header.msgId| was not a known message ID.
 */
TeleopMessages__Status
TeleopMessages__serializeMessage(const TeleopMessages__Message *src,
                                 void *dst,
                                 size_t dstLen);

/**
 * @brief Deserializes the message header packed in |src| with network endianness into the struct |dst| with system
 *        endianness.
 *
 * @param src [IN] The buffer containing the packed message header to be deserialized. Must have a size, as determined
 *            by the value of |srcLen|, of at least TELEOP_MESSAGES__PACKED_SIZE__HEADER bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               TELEOP_MESSAGES__PACKED_SIZE__HEADER.
 * @param dst [OUT] The struct into which the message header will be deserialized.
 *
 * @return TeleopMessages__Status One of the following:
 *   TELEOP_MESSAGES__STATUS__SUCCESS: The function was successful.
 *   TELEOP_MESSAGES__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   TELEOP_MESSAGES__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                        TELEOP_MESSAGES__PACKED_SIZE__HEADER.
 *   TELEOP_MESSAGES__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
TeleopMessages__Status
TeleopMessages__deserializeHeader(const void *src,
                                  size_t srcLen,
                                  TeleopMessages__Header *dst);

/**
 * @brief Deserializes the message body packed in |src| with network endianness into the struct |dst| with system
 *        endianness.
 *
 * The type of body in |src| is determined by |srcMsgId|.
 *
 * @param srcMsgId [IN] The message ID of the message body type packed in |src|.
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined
 *            by the value of |srcLen|, that is large enough to fit the message body with the type specified by
 *            |srcMsgId|. The exact size required varies based on the type of the message body.
 * @param srcLen [IN] The length of the buffer pointed to by |src|.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return TeleopMessages__Status One of the following:
 *   TELEOP_MESSAGES__STATUS__SUCCESS: The function was successful.
 *   TELEOP_MESSAGES__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   TELEOP_MESSAGES__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was too small to hold the message body.
 *   TELEOP_MESSAGES__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
TeleopMessages__Status
TeleopMessages__deserializeBody(TeleopMessages__MessageId srcMsgId,
                                const void *src,
                                size_t srcLen,
                                TeleopMessages__Body *dst);

/**
 * @brief Deserializes the message packed in |src| with network endianness into the struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message to be deserialized. Must have a size, as determined by the
 *            value of |srcLen|, that is large enough to fit a message header and the message body whose type is
 *            specified in the message header. The exact size required varies based on the type of the message.
 * @param srcLen [IN] The length of the buffer pointed to by |src|.
 * @param dst [OUT] The struct into which the message will be deserialized.
 *
 * @return TeleopMessages__Status One of the following:
 *   TELEOP_MESSAGES__STATUS__SUCCESS: The function was successful.
 *   TELEOP_MESSAGES__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   TELEOP_MESSAGES__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was too small to hold the message.
 *   TELEOP_MESSAGES__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
TeleopMessages__Status
TeleopMessages__deserializeMessage(const void *src,
                                   size_t srcLen,
                                   TeleopMessages__Message *dst);

/**
 * @}}
 */

#ifdef __cplusplus
}} /* close extern "C" */
#endif

#endif // #ifndef __TELEOP_MESSAGES_H___
"""