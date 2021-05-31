#ifndef __WATCHDOG_WATCHDOG_CMD_MSGS_H___
#define __WATCHDOG_WATCHDOG_CMD_MSGS_H___

#include "include/comms/cmd_msgs"
#include "include/common.h"

typedef enum WdCmdMsgs__Status
{
    WD_CMD_MSGS__STATUS__SUCCESS = CMD_MSGS__STATUS__SUCCESS, /* Operation suceeded. */
    WD_CMD_MSGS__STATUS__ERROR_NULL = CMD_MSGS__STATUS__ERROR_NULL, /* A required argument or a member of an argument was NULL */
    WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL = CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL, 
    WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR = CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR, 
    WD_CMD_MSGS__STATUS__ERROR_UNKNOWN_MESSAGE_ID = CMD_MSGS__STATUS__ERROR_UNKNOWN_MESSAGE_ID,

    WD_CMD_MSGS__STATUS__ERROR__INTERNAL = CMD_MSGS__STATUS__ERROR__INTERNAL /* An unexpected internal error occurred. */
} WdCmdMsgs__Status;

//######################################################################################################################
// Message Field Types
//######################################################################################################################

typedef enum WdCmdMsgs__ResponseStatusCode
{
    WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS = 0,
    WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_PACKET_LENGTH = 1,
    WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_CHECKSUM = 2,
    WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_MODULE_ID = 3,
    WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_ID = 4,
    WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER = 5,
    WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE = 6,
    
    WD_CMD_MSGS__RESPONSE_STATUS__DEPLOY = 96,

} WdCmdMsgs__ResponseStatusCode;

typedef enum WdCmdMsgs__CommandId
{
    WD_CMD_MSGS__CMD_ID__RESET_SPECIFIC = 0x00,
    WD_CMD_MSGS__CMD_ID__PREP_FOR_DEPLOY = 0x01,
    WD_CMD_MSGS__CMD_ID__DEPLOY = 0x02,
    WD_CMD_MSGS__CMD_ID__SWITCH_CONN_MODE = 0x04,
    WD_CMD_MSGS__CMD_ID__SET_HEATER_KP = 0xAA,
    WD_CMD_MSGS__CMD_ID__SET_AUTO_HEATER_ON_VALUE = 0xAB,
    WD_CMD_MSGS__CMD_ID__SET_AUTO_HEATER_OFF_VALUE = 0xAC,
    WD_CMD_MSGS__CMD_ID__SET_HEATER_DUTY_CYCLE_MAX = 0xAD,
    WD_CMD_MSGS__CMD_ID__SET_HEATER_DUTY_CYCLE_PERIOD = 0xAE,
    WD_CMD_MSGS__CMD_ID__SET_THERMISTER_V_SETPOINT = 0xDA,
    WD_CMD_MSGS__CMD_ID__ENTER_SLEEP_MODE = 0xEA,
    WD_CMD_MSGS__CMD_ID__ENTER_KEEPALIVE_MODE = 0xEB,
    WD_CMD_MSGS__CMD_ID__ENTER_SERVICE_MODE = 0xEC
} WdCmdMsgs__CommandId;

typedef enum WdCmdMsgs__ResetSpecificId
{
    WD_CMD_MSGS__RESET_ID__NO_RESET = 0x00, //!< TODO: What is the point of this?

    WD_CMD_MSGS__RESET_ID__HERCULES_RESET = 0x01,
    WD_CMD_MSGS__RESET_ID__HERCULES_POWER_ON = 0x02,
    WD_CMD_MSGS__RESET_ID__HERCULES_POWER_OFF = 0x03,

    WD_CMD_MSGS__RESET_ID__RADIO_RESET = 0x04,
    WD_CMD_MSGS__RESET_ID__RADIO_POWER_ON = 0x05,
    WD_CMD_MSGS__RESET_ID__RADIO_POWER_OFF = 0x06,

    WD_CMD_MSGS__RESET_ID__CAM_FPGA_RESET = 0x07,
    WD_CMD_MSGS__RESET_ID__CAM_FPGA_POWER_ON = 0x08,
    WD_CMD_MSGS__RESET_ID__CAM_FPGA_POWER_OFF = 0x09,

    WD_CMD_MSGS__RESET_ID__MOTOR_1_RESET = 0x0A,
    WD_CMD_MSGS__RESET_ID__MOTOR_2_RESET = 0x0B,
    WD_CMD_MSGS__RESET_ID__MOTOR_3_RESET = 0x0C,
    WD_CMD_MSGS__RESET_ID__MOTOR_4_RESET = 0x0D,

    WD_CMD_MSGS__RESET_ID__ALL_MOTORS_RESET = 0x0E,
    WD_CMD_MSGS__RESET_ID__ALL_MOTORS_POWER_ON = 0x0F,
    WD_CMD_MSGS__RESET_ID__ALL_MOTORS_POWER_OFF = 0x10,

    WD_CMD_MSGS__RESET_ID__3_3V_EN_RESET = 0x11,
    WD_CMD_MSGS__RESET_ID__3_3V_EN_POWER_ON = 0x12,
    WD_CMD_MSGS__RESET_ID__3_3V_EN_POWER_OFF = 0x13,

    WD_CMD_MSGS__RESET_ID__24V_EN_RESET = 0x14,
    WD_CMD_MSGS__RESET_ID__24V_EN_POWER_ON = 0x15,
    WD_CMD_MSGS__RESET_ID__24V_EN_POWER_OFF = 0x16,

    WD_CMD_MSGS__RESET_ID__HDRM_DEPLOY_SIGNAL_POWER_OFF = 0x18,

    WD_CMD_MSGS__RESET_ID__FPGA_CAM_0_SELECT = 0x19,
    WD_CMD_MSGS__RESET_ID__FPGA_CAM_1_SELECT = 0x1A,

    WD_CMD_MSGS__RESET_ID__BATTERY_CHARGE_START = 0x1B,
    WD_CMD_MSGS__RESET_ID__BATTERY_CHARGE_STOP = 0x1C,

    WD_CMD_MSGS__RESET_ID__RS422_UART_ENABLE = 0x1D,
    WD_CMD_MSGS__RESET_ID__RS422_UART_DISABLE = 0x1E,

    WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_ENABLE = 0x1F,
    WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_DISABLE = 0x20,

    WD_CMD_MSGS__RESET_ID__HERCULES_WATCHDOG_ENABLE = 0x21,
    WD_CMD_MSGS__RESET_ID__HERCULES_WATCHDOG_DISABLE = 0x22,

    WD_CMD_MSGS__RESET_ID__BATTERIES_ENABLE = 0x23,
    WD_CMD_MSGS__RESET_ID__BATTERIES_DISABLE = 0x24,

    WD_CMD_MSGS__RESET_ID__HDRM_DEPLOY_SIGNAL_POWER_ON = 0xEE
} WdCmdMsgs__ResetSpecificId;

typedef enum WdCmdMsgs__SwitchConnModeSelection
{
    WD_CMD_MSGS__CONN_MODE__WIRED = 0x01, // Wired (RS-422): Watchdog gets data, forwards to Hercules
    WD_CMD_MSGS__CONN_MODE__WIRED = 0x02 // Wireless: Hercules gets data, forwards to watchdog
} WdCmdMsgs__SwitchConnModeSelection;

static const uint8_t WD_CMD_MSGS__CONFIRM_MODE_CHANGE_MAGIC_NUMBER = 0x77u;
static const uint8_t WD_CMD_MSGS__CONFIRM_DEPLOYMENT_MAGIC_NUMBER = 0x60u;
static const uint8_t WD_CMD_MSGS__RESPONSE_MAGIC_NUMBER = 0x0Au;

//######################################################################################################################
// Message Header
//######################################################################################################################

//######################################################################################################################
// Message Bodies
//######################################################################################################################

typedef struct WdCmdMsgs__MsgBody__ResetSpecific
{
    WdCmdMsgs__ResetSpecificId resetId;
} WdCmdMsgs__MsgBody__ResetSpecific;

typedef struct WdCmdMsgs__MsgBody__PrepForDeploy
{
    uint8_t confirmationMagicNumber;
} WdCmdMsgs__MsgBody__PrepForDeploy;

typedef struct WdCmdMsgs__MsgBody__Deploy
{
    uint8_t confirmationMagicNumber;
} WdCmdMsgs__MsgBody__Deploy;

typedef struct WdCmdMsgs__MsgBody__SwitchConnMode
{
    WdCmdMsgs__SwitchConnModeSelection connMode;
} WdCmdMsgs__MsgBody__SwitchConnMode;

typedef struct WdCmdMsgs__MsgBody__SetHeaterKp
{
    uint16_t kp;
} WdCmdMsgs__MsgBody__SetHeaterKp;

typedef struct WdCmdMsgs__MsgBody__SetAutoHeaterOnValue
{
    uint16_t heaterOnValue;
} WdCmdMsgs__MsgBody__SetAutoHeaterOnValue;

typedef struct WdCmdMsgs__MsgBody__SetAutoHeaterOffValue
{
    uint16_t heaterOffValue;
} WdCmdMsgs__MsgBody__SetAutoHeaterOffValue;

typedef struct WdCmdMsgs__MsgBody__SetHeaterDutyCycleMax
{
    uint16_t dutyCycleMax;
} WdCmdMsgs__MsgBody__SetHeaterDutyCycleMax;

typedef struct WdCmdMsgs__MsgBody__SetHeaterDutyCyclePeriod
{
    uint16_t dutyCyclePeriod;
} WdCmdMsgs__MsgBody__SetHeaterDutyCyclePeriod;

typedef struct WdCmdMsgs__MsgBody__SetThermisterVSetpoint
{
    uint16_t thermisterVSetpoint;
} WdCmdMsgs__MsgBody__SetThermisterVSetpoint;

typedef struct WdCmdMsgs__MsgBody__EnterSleepMode
{
    uint8_t confirmationMagicNumber;
} WdCmdMsgs__MsgBody__EnterSleepMode;

typedef struct WdCmdMsgs__MsgBody__EnterKeepAliveMode
{
    uint8_t confirmationMagicNumber;
} WdCmdMsgs__MsgBody__EnterKeepAliveMode;

typedef struct WdCmdMsgs__MsgBody__EnterServiceMode
{
    uint8_t confirmationMagicNumber;
} WdCmdMsgs__MsgBody__EnterServiceMode;

//######################################################################################################################
// Overall Message Structure
//######################################################################################################################

typedef union 
{
    WdCmdMsgs__MsgBody__ResetSpecific resetSpecific;
    WdCmdMsgs__MsgBody__PrepForDeploy prepForDeploy;
    WdCmdMsgs__MsgBody__Deploy deploy;
    WdCmdMsgs__MsgBody__SwitchConnMode switchConnMode;
    WdCmdMsgs__MsgBody__SetHeaterKp setHeaterKp;
    WdCmdMsgs__MsgBody__SetAutoHeaterOnValue setAutoHeaterOnValue;
    WdCmdMsgs__MsgBody__SetAutoHeaterOffValue setAutoHeaterOffValue;
    WdCmdMsgs__MsgBody__SetHeaterDutyCycleMax setHeaterDutyCycleMax;
    WdCmdMsgs__MsgBody__SetHeaterDutyCyclePeriod setHeaterDutyCyclePeriod;
    WdCmdMsgs__MsgBody__SetThermisterVSetpoint setThermisterVSetpoint;
    WdCmdMsgs__MsgBody__EnterSleepMode enterSleepMode;
    WdCmdMsgs__MsgBody__EnterKeepAliveMode enterKeepAliveMode;
    WdCmdMsgs__MsgBody__EnterServiceMode enterServiceMode;
} WdCmdMsgs__MessageBody;

typedef struct WdCmdMsgs__Message
{
    CmdMsgs__CommonHeader commonHeader;
    WdCmdMsgs__CommandId commandId;
    WdCmdMsgs__MessageBody body;
} WdCmdMsgs__Message;

typedef struct WdCmdMsgs__Response
{
    uint8_t magicNumber;
    WdCmdMsgs__CommandId commandId;
    WdCmdMsgs__ResponseStatusCode statusCode;
} WdCmdMsgs__Response;

//######################################################################################################################
// Packed Sizes
//######################################################################################################################

typedef enum WdCmdMsgs__PackedSize
{
    WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER = CMD_MSGS__PACKED_SIZE__COMMON_HEADER,

    // Bodies

    WD_CMD_MSGS__PACKED_SIZE__RESET_SPECIFIC_BODY = sizeof(uint8_t),
    WD_CMD_MSGS__PACKED_SIZE__PREP_FOR_DEPLOY_BODY = sizeof(uint8_t),
    WD_CMD_MSGS__PACKED_SIZE__DEPLOY_BODY = sizeof(uint8_t),
    WD_CMD_MSGS__PACKED_SIZE__SWITCH_CONN_MODE_BODY = sizeof(uint8_t),
    WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_KP_BODY = sizeof(uint16_t),
    WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_ON_VALUE_BODY = sizeof(uint16_t),
    WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_OFF_VALUE_BODY = sizeof(uint16_t),
    WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_MAX_BODY = sizeof(uint16_t),
    WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_PERIOD_BODY = sizeof(uint16_t),
    WD_CMD_MSGS__PACKED_SIZE__SET_THERMISTER_V_SETPOINT_BODY = sizeof(uint16_t),
    WD_CMD_MSGS__PACKED_SIZE__ENTER_SLEEP_MODE_BODY = sizeof(uint8_t),
    WD_CMD_MSGS__PACKED_SIZE__ENTER_KEEPALIVE_MODE_BODY = sizeof(uint8_t),
    WD_CMD_MSGS__PACKED_SIZE__ENTER_SERVICE_MODE_BODY = sizeof(uint8_t),

    // Full Messages

    WD_CMD_MSGS__PACKED_SIZE__RESPONSE_MSG = (3 * sizeof(uint8_t)),

    WD_CMD_MSGS__PACKED_SIZE__RESET_SPECIFIC_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint8_t) + WD_CMD_MSGS__PACKED_SIZE__RESET_SPECIFIC_BODY,
    WD_CMD_MSGS__PACKED_SIZE__PREP_FOR_DEPLOY_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint8_t) + WD_CMD_MSGS__PACKED_SIZE__PREP_FOR_DEPLOY_BODY,
    WD_CMD_MSGS__PACKED_SIZE__DEPLOY_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint8_t) + WD_CMD_MSGS__PACKED_SIZE__DEPLOY_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SWITCH_CONN_MODE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint8_t) + WD_CMD_MSGS__PACKED_SIZE__SWITCH_CONN_MODE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_KP_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint8_t) + WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_KP_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_ON_VALUE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint8_t) + WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_ON_VALUE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_OFF_VALUE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint8_t) + WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_OFF_VALUE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_MAX_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint8_t) + WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_MAX_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_PERIOD_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint8_t) + WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_PERIOD_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_THERMISTER_V_SETPOINT_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint8_t) + WD_CMD_MSGS__PACKED_SIZE__SET_THERMISTER_V_SETPOINT_BODY,
    WD_CMD_MSGS__PACKED_SIZE__ENTER_SLEEP_MODE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint8_t) + WD_CMD_MSGS__PACKED_SIZE__ENTER_SLEEP_MODE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__ENTER_KEEPALIVE_MODE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint8_t) + WD_CMD_MSGS__PACKED_SIZE__ENTER_KEEPALIVE_MODE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__ENTER_SERVICE_MODE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint8_t) + WD_CMD_MSGS__PACKED_SIZE__ENTER_SERVICE_MODE_BODY,

    WD_CMD_MSGS__PACKED_SIZE__SMALLEST_MSG = WD_CMD_MSGS__PACKED_SIZE__RESET_SPECIFIC_MSG,
    WD_CMD_MSGS__PACKED_SIZE__LARGEST_MSG = WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_ON_VALUE_MSG
} WdCmdMsgs__PackedSize;

//######################################################################################################################
// Message Serialization and Deserialization Functions
//######################################################################################################################


/**
 * @brief Serializes the message in |src| with system endianness into the buffer |dst| with little endianness.
 *
 * @param src [IN] The response to be serialized.
 * @param dst [OUT] The buffer into which the message will be serialized. Must have a large enough size, as determined
 *            by the value of |dstLen|, of at least WD_CMD_MSGS__PACKED_SIZE__RESPONSE_MSG bytes.
 * @param dstLen [IN] The length of the buffer pointed to by |dst|.
 *
 * @return One of the following:
 *   WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |dst| was too small to hold the serialized message.
 *   WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
WdCmdMsgs__Status WdCmdMsgs__serializeGroundResponse(const WdCmdMsgs__Response* src,
                                                     void* dst,
                                                     size_t dstLen);

/**
 * @brief Deserializes the message body packed in |src| with little endianness into the struct |dst| with system
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
 * @return One of the following:
 *   WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was too small to hold the message body.
 *   WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
WdCmdMsgs__Status
WdCmdMsgs__deserializeBody(WdCmdMsgs__MessageId srcMsgId,
                           const void* src,
                           size_t srcLen,
                           WdCmdMsgs__Body* dst);

/**
 * @brief Deserializes the message packed in |src| with little endianness into the struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message to be deserialized. Must have a size, as determined by the
 *            value of |srcLen|, that is large enough to fit a message header and the message body whose type is
 *            specified in the message header. The exact size required varies based on the type of the message. At
 *            a minimum it must be WD_CMD_MSGS__PACKED_SIZE__SMALLEST_MSG, but it may need to be as large as 
 *            WD_CMD_MSGS__PACKED_SIZE__LARGEST_MSG.
 * @param srcLen [IN] The length of the buffer pointed to by |src|.
 * @param dst [OUT] The struct into which the message will be deserialized.
 * @param deserializeHeader [IN] Whether or not to deserialize the header into dst->commonHeader. This can be set to
 *                           false if (for instance) the command header has already been deserialized prior to calling
 *                           this function.
 *
 * @return One of the following:
 *   WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was too small to hold the message.
 *   WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
WdCmdMsgs__Status
WdCmdMsgs__deserializeMessage(const void* src,
                              size_t srcLen,
                              WdCmdMsgs__Message* dst,
                              BOOL deserializeHeader);

#endif /* __WATCHDOG_WATCHDOG_CMD_MSGS_H___ */
