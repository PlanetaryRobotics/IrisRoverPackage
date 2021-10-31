#include "comms/watchdog_cmd_msgs.h"
#include "utils/serialization.h"


/**
 * @brief Check that the status value |actual| returned from a serialization function is greater than or equal to zero.
 *        If |actual| is less than zero, logs a message to the error function and returns
 *        WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR.
 *
 * @note Parameter |RESULT_VAR| must be implicitly castable to int.
 */
#define CHECK_SERIALIZATION_RESULT(RESULT_VAR) \
    if (0 > RESULT_VAR) { \
        return WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR; \
    }

//######################################################################################################################
// Private Function Forward Declarations
//######################################################################################################################

/**
 * @brief Deserializes the reset specific message body packed in |src| with little endianness into the struct |dst|
 *        with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__RESET_SPECIFIC_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__RESET_SPECIFIC_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__RESET_SPECIFIC_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeResetSpecificBody(const void* src,
                                        size_t srcLen,
                                        WdCmdMsgs__MsgBody__ResetSpecific* dst);

/**
 * @brief Deserializes the prepare for deploy message body packed in |src| with little endianness into the struct
 *        |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__PREP_FOR_DEPLOY_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__PREP_FOR_DEPLOY_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__PREP_FOR_DEPLOY_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializePrepForDeployBody(const void* src,
                                        size_t srcLen,
                                        WdCmdMsgs__MsgBody__PrepForDeploy* dst);

/**
 * @brief Deserializes the deploy message body packed in |src| with little endianness into the struct
 *        |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__DEPLOY_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__DEPLOY_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__DEPLOY_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeDeployBody(const void* src,
                                 size_t srcLen,
                                 WdCmdMsgs__MsgBody__Deploy* dst);

/**
 * @brief Deserializes the switch connection mode message body packed in |src| with little endianness into the struct
 *        |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__SWITCH_CONN_MODE_MSG bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__SWITCH_CONN_MODE_MSG.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__SWITCH_CONN_MODE_MSG.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeSwitchConnModeBody(const void* src,
                                         size_t srcLen,
                                         WdCmdMsgs__MsgBody__SwitchConnMode* dst);

/**
 * @brief Deserializes the set heater Kp message body packed in |src| with little endianness into the struct
 *        |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_KP_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_KP_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_KP_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetHeaterKpBody(const void* src,
                                      size_t srcLen,
                                      WdCmdMsgs__MsgBody__SetHeaterKp* dst);

/**
 * @brief Deserializes the set auto heater on value message body packed in |src| with little endianness into the
 *        struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_ON_VALUE_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_ON_VALUE_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_ON_VALUE_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetAutoHeaterOnValueBody(const void* src,
                                               size_t srcLen,
                                               WdCmdMsgs__MsgBody__SetAutoHeaterOnValue* dst);

/**
 * @brief Deserializes the set auto heater off value message body packed in |src| with little endianness into the
 *        struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_OFF_VALUE_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_OFF_VALUE_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_OFF_VALUE_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetAutoHeaterOffValueBody(const void* src,
                                                size_t srcLen,
                                                WdCmdMsgs__MsgBody__SetAutoHeaterOffValue* dst);

/**
 * @brief Deserializes the set heater duty cycle max message body packed in |src| with little endianness into the
 *        struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_MAX_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_MAX_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_MAX_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetHeaterDutyCycleMaxBody(const void* src,
                                                size_t srcLen,
                                                WdCmdMsgs__MsgBody__SetHeaterDutyCycleMax* dst);

/**
 * @brief Deserializes the set heater duty cycle peiod message body packed in |src| with little endianness into the
 *        struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_PERIOD_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_PERIOD_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_PERIOD_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetHeaterDutyCyclePeriodBody(const void* src,
                                                   size_t srcLen,
                                                   WdCmdMsgs__MsgBody__SetHeaterDutyCyclePeriod* dst);

/**
 * @brief Deserializes the set thermister V setpoint message body packed in |src| with little endianness into the
 *        struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__SET_THERMISTER_V_SETPOINT_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__SET_THERMISTER_V_SETPOINT_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__SET_THERMISTER_V_SETPOINT_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetThermisterVSetpointBody(const void* src,
                                                 size_t srcLen,
                                                 WdCmdMsgs__MsgBody__SetThermisterVSetpoint* dst);

/**
 * @brief Deserializes the enter sleep mode message body packed in |src| with little endianness into the
 *        struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__ENTER_SLEEP_MODE_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__ENTER_SLEEP_MODE_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__ENTER_SLEEP_MODE_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeEnterSleepModeBody(const void* src,
                                         size_t srcLen,
                                         WdCmdMsgs__MsgBody__EnterSleepMode* dst);

/**
 * @brief Deserializes the enter keepalive mode message body packed in |src| with little endianness into the
 *        struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__ENTER_KEEPALIVE_MODE_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__ENTER_KEEPALIVE_MODE_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__ENTER_KEEPALIVE_MODE_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeEnterKeepAliveModeBody(const void* src,
                                             size_t srcLen,
                                             WdCmdMsgs__MsgBody__EnterKeepAliveMode* dst);

/**
 * @brief Deserializes the enter service mode message body packed in |src| with little endianness into the
 *        struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__ENTER_SERVICE_MODE_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__ENTER_SERVICE_MODE_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__ENTER_SERVICE_MODE_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeEnterServiceModeBody(const void* src,
                                           size_t srcLen,
                                           WdCmdMsgs__MsgBody__EnterServiceMode* dst);

/**
 * @brief Deserializes the dangerous force battery state message body packed in |src| with little endianness into the
 *        struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__DANG_FORCE_BATT_STATE_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__DANG_FORCE_BATT_STATE_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__DANG_FORCE_BATT_STATE_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeDangForceBattStateBody(const void* src,
                                             size_t srcLen,
                                             WdCmdMsgs__MsgBody__DangForceBattState* dst);

/**
 * @brief Deserializes the set charge en state message body packed in |src| with little endianness into the
 *        struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__SET_CHARGE_EN_STATE_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__SET_CHARGE_EN_STATE_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__SET_CHARGE_EN_STATE_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetChargeEnStateBody(const void* src,
                                           size_t srcLen,
                                           WdCmdMsgs__MsgBody__SetChargeEnState* dst);

/**
 * @brief Deserializes the set charge reg en state message body packed in |src| with little endianness into the
 *        struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__SET_CHARGE_REG_EN_STATE_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__SET_CHARGE_REG_EN_STATE_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__SET_CHARGE_REG_EN_STATE_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetChargeRegEnStateBody(const void* src,
                                              size_t srcLen,
                                              WdCmdMsgs__MsgBody__SetChargeRegEnState* dst);

/**
 * @brief Deserializes the set batt en state message body packed in |src| with little endianness into the
 *        struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__SET_BATT_EN_STATE_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__SET_BATT_EN_STATE_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__SET_BATT_EN_STATE_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetBattEnStateBody(const void* src,
                                         size_t srcLen,
                                         WdCmdMsgs__MsgBody__SetBattEnState* dst);

/**
 * @brief Deserializes the set batt control en state message body packed in |src| with little endianness into the
 *        struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__SET_BATT_CTRL_EN_STATE_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__SET_BATT_CTRL_EN_STATE_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__SET_BATT_CTRL_EN_STATE_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetBattCtrlEnStateBody(const void* src,
                                             size_t srcLen,
                                             WdCmdMsgs__MsgBody__SetBattCtrlEnState* dst);

/**
 * @brief Deserializes the set latch batt state message body packed in |src| with little endianness into the
 *        struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__SET_LATCH_BATT_STATE_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__SET_LATCH_BATT_STATE_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__SET_LATCH_BATT_STATE_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetLatchBattStateBody(const void* src,
                                            size_t srcLen,
                                            WdCmdMsgs__MsgBody__SetLatchBattState* dst);

/**
 * @brief Deserializes the latch set pulse low message body packed in |src| with little endianness into the
 *        struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__LATCH_SET_PULSE_LOW_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__LATCH_SET_PULSE_LOW_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__LATCH_SET_PULSE_LOW_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeLatchSetPulseLowBody(const void* src,
                                           size_t srcLen,
                                           WdCmdMsgs__MsgBody__LatchSetPulseLow* dst);

/**
 * @brief Deserializes the latch reset pulse low message body packed in |src| with little endianness into the
 *        struct |dst| with system endianness.
 *
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined by
 *            the value of |srcLen|, of at least WD_CMD_MSGS__PACKED_SIZE__LATCH_RESET_PULSE_LOW_BODY bytes.
 * @param srcLen [IN] The length of the buffer pointed to by |src|. Must be at least
 *               WD_CMD_MSGS__PACKED_SIZE__LATCH_RESET_PULSE_LOW_BODY.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return WdCmdMsgs__Status One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: |src| or |dst| was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: |srcLen| was less than
 *                                                  WD_CMD_MSGS__PACKED_SIZE__LATCH_RESET_PULSE_LOW_BODY.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
static WdCmdMsgs__Status
WdCmdMsgs__deserializeLatchResetPulseLowBody(const void* src,
                                             size_t srcLen,
                                             WdCmdMsgs__MsgBody__LatchResetPulseLow* dst);

//######################################################################################################################
// Public Function Definitions
//######################################################################################################################

WdCmdMsgs__Status WdCmdMsgs__serializeGroundResponse(const WdCmdMsgs__Response* src,
                                                     void* dst,
                                                     size_t dstLen)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (dstLen < WD_CMD_MSGS__PACKED_SIZE__RESPONSE_MSG) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* dstIntPtr = (uint8_t*) dst;
    short serializationResult = Serialization__serializeAs8Bit(&(src->magicNumber), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(src->magicNumber);

    uint8_t commandIdAsUint = (uint8_t) src->commandId;
    serializationResult = Serialization__serializeAs8Bit(&(commandIdAsUint), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);
    dstIntPtr += sizeof(commandIdAsUint);

    uint8_t statusCodeUint = (uint8_t) src->statusCode;
    serializationResult = Serialization__serializeAs8Bit(&(statusCodeUint), dstIntPtr, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(serializationResult);

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

WdCmdMsgs__Status
WdCmdMsgs__deserializeBody(WdCmdMsgs__CommandId srcMsgId,
                           const void* src,
                           size_t srcLen,
                           WdCmdMsgs__MessageBody* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    switch (srcMsgId) {
        case WD_CMD_MSGS__CMD_ID__RESET_SPECIFIC:
            return WdCmdMsgs__deserializeResetSpecificBody(src, srcLen, &(dst->resetSpecific));

        case WD_CMD_MSGS__CMD_ID__PREP_FOR_DEPLOY:
            return WdCmdMsgs__deserializePrepForDeployBody(src, srcLen, &(dst->prepForDeploy));

        case WD_CMD_MSGS__CMD_ID__DEPLOY:
            return WdCmdMsgs__deserializeDeployBody(src, srcLen, &(dst->deploy));

        case WD_CMD_MSGS__CMD_ID__SWITCH_CONN_MODE:
            return WdCmdMsgs__deserializeSwitchConnModeBody(src, srcLen, &(dst->switchConnMode));

        case WD_CMD_MSGS__CMD_ID__SET_HEATER_KP:
            return WdCmdMsgs__deserializeSetHeaterKpBody(src, srcLen, &(dst->setHeaterKp));

        case WD_CMD_MSGS__CMD_ID__SET_AUTO_HEATER_ON_VALUE:
            return WdCmdMsgs__deserializeSetAutoHeaterOnValueBody(src, srcLen, &(dst->setAutoHeaterOnValue));

        case WD_CMD_MSGS__CMD_ID__SET_AUTO_HEATER_OFF_VALUE:
            return WdCmdMsgs__deserializeSetAutoHeaterOffValueBody(src, srcLen, &(dst->setAutoHeaterOffValue));

        case WD_CMD_MSGS__CMD_ID__SET_HEATER_DUTY_CYCLE_MAX:
            return WdCmdMsgs__deserializeSetHeaterDutyCycleMaxBody(src, srcLen, &(dst->setHeaterDutyCycleMax));

        case WD_CMD_MSGS__CMD_ID__SET_HEATER_DUTY_CYCLE_PERIOD:
            return WdCmdMsgs__deserializeSetHeaterDutyCyclePeriodBody(src, srcLen, &(dst->setHeaterDutyCyclePeriod));

        case WD_CMD_MSGS__CMD_ID__SET_THERMISTER_V_SETPOINT:
            return WdCmdMsgs__deserializeSetThermisterVSetpointBody(src, srcLen, &(dst->setThermisterVSetpoint));

        case WD_CMD_MSGS__CMD_ID__ENTER_SLEEP_MODE:
            return WdCmdMsgs__deserializeEnterSleepModeBody(src, srcLen, &(dst->enterSleepMode));

        case WD_CMD_MSGS__CMD_ID__ENTER_KEEPALIVE_MODE:
            return WdCmdMsgs__deserializeEnterKeepAliveModeBody(src, srcLen, &(dst->enterKeepAliveMode));

        case WD_CMD_MSGS__CMD_ID__ENTER_SERVICE_MODE:
            return WdCmdMsgs__deserializeEnterServiceModeBody(src, srcLen, &(dst->enterServiceMode));

        case WD_CMD_MSGS__CMD_ID__DANGEROUS_FORCE_BATT_STATE:
            return WdCmdMsgs__deserializeDangForceBattStateBody(src, srcLen, &(dst->dangForceBattState));

        case WD_CMD_MSGS__CMD_ID__SET_CHARGE_EN_STATE:
            return WdCmdMsgs__deserializeSetChargeEnStateBody(src, srcLen, &(dst->setChargeEnState));

        case WD_CMD_MSGS__CMD_ID__SET_CHARGE_REG_EN_STATE:
            return WdCmdMsgs__deserializeSetChargeRegEnStateBody(src, srcLen, &(dst->setChargeRegEnState));

        case WD_CMD_MSGS__CMD_ID__SET_BATT_EN_STATE:
            return WdCmdMsgs__deserializeSetBattEnStateBody(src, srcLen, &(dst->setBattEnState));

        case WD_CMD_MSGS__CMD_ID__SET_BATT_CTRL_EN_STATE:
            return WdCmdMsgs__deserializeSetBattCtrlEnStateBody(src, srcLen, &(dst->setBattCtrlEnState));

        case WD_CMD_MSGS__CMD_ID__SET_LATCH_BATT_STATE:
            return WdCmdMsgs__deserializeSetLatchBattStateBody(src, srcLen, &(dst->setLatchBattState));

        case WD_CMD_MSGS__CMD_ID__LATCH_SET_PULSE_LOW:
            return WdCmdMsgs__deserializeLatchSetPulseLowBody(src, srcLen, &(dst->latchSetPulseLow));

        case WD_CMD_MSGS__CMD_ID__LATCH_RESET_PULSE_LOW:
            return WdCmdMsgs__deserializeLatchResetPulseLowBody(src, srcLen, &(dst->latchResetPulseLow));

        default:
            return WD_CMD_MSGS__STATUS__ERROR_UNKNOWN_MESSAGE_ID;
    }
}

WdCmdMsgs__Status
WdCmdMsgs__deserializeMessage(const void* src,
                              size_t srcLen,
                              WdCmdMsgs__Message* dst,
                              BOOL deserializeHeader)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    // Sanity check that the src buffer is large enough to hold at least a header and a command ID.
    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__SMALLEST_MSG) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;

    // First deserialize the header if requested. We don't confirm that the magic number indicates this is a
    // watchdog message.
    if (deserializeHeader) {
        CmdMsgs__Status deserHeaderStatus = CmdMsgs__deserializeHeader(srcIntPtr, srcLen, &(dst->commonHeader));

        if (CMD_MSGS__STATUS__SUCCESS != deserHeaderStatus) {
            return (WdCmdMsgs__Status) deserHeaderStatus;
        }
    }

    // Regardless of whether we actually deserialized the header or not, we need to move past it in the src buffer 
    srcIntPtr += (size_t) CMD_MSGS__PACKED_SIZE__COMMON_HEADER;
    size_t bodySrcLen = srcLen - (size_t) CMD_MSGS__PACKED_SIZE__COMMON_HEADER;

    // Next read the watchdog command ID
    uint16_t watchdogCmdIdUInt = 0;
    short deserializationResult =
        Serialization__deserializeAs16Bit(srcIntPtr, &(watchdogCmdIdUInt), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);    
    dst->commandId = (WdCmdMsgs__CommandId) watchdogCmdIdUInt;

    srcIntPtr += sizeof(watchdogCmdIdUInt);
    bodySrcLen -= sizeof(watchdogCmdIdUInt);

    WdCmdMsgs__Status deserBodyStatus = WdCmdMsgs__deserializeBody(dst->commandId,
                                                                   srcIntPtr,
                                                                   bodySrcLen,
                                                                   &(dst->body));
    
    if (WD_CMD_MSGS__STATUS__SUCCESS != deserBodyStatus) {
        return deserBodyStatus;
    }

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

//######################################################################################################################
// Private Function Definitions
//######################################################################################################################

static WdCmdMsgs__Status
WdCmdMsgs__deserializeResetSpecificBody(const void* src,
                                        size_t srcLen,
                                        WdCmdMsgs__MsgBody__ResetSpecific* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__RESET_SPECIFIC_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    uint8_t resetIdUInt = 0;
    short deserializationResult =
        Serialization__deserializeAs8Bit(srcIntPtr, &(resetIdUInt), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);

    dst->resetId = (WdCmdMsgs__ResetSpecificId) resetIdUInt;

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializePrepForDeployBody(const void* src,
                                        size_t srcLen,
                                        WdCmdMsgs__MsgBody__PrepForDeploy* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__PREP_FOR_DEPLOY_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    short deserializationResult = 
        Serialization__deserializeAs8Bit(srcIntPtr, &(dst->confirmationMagicNumber), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeDeployBody(const void* src,
                                 size_t srcLen,
                                 WdCmdMsgs__MsgBody__Deploy* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__DEPLOY_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    short deserializationResult = 
        Serialization__deserializeAs8Bit(srcIntPtr, &(dst->confirmationMagicNumber), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeSwitchConnModeBody(const void* src,
                                         size_t srcLen,
                                         WdCmdMsgs__MsgBody__SwitchConnMode* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__SWITCH_CONN_MODE_MSG) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    uint8_t modeUInt = 0;
    short deserializationResult = Serialization__deserializeAs8Bit(srcIntPtr, &(modeUInt), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);

    dst->connMode = (WdCmdMsgs__SwitchConnModeSelection) modeUInt;

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetHeaterKpBody(const void* src,
                                      size_t srcLen,
                                      WdCmdMsgs__MsgBody__SetHeaterKp* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_KP_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    short deserializationResult = 
        Serialization__deserializeAs16Bit(srcIntPtr, &(dst->kp), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetAutoHeaterOnValueBody(const void* src,
                                               size_t srcLen,
                                               WdCmdMsgs__MsgBody__SetAutoHeaterOnValue* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_ON_VALUE_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    short deserializationResult = 
        Serialization__deserializeAs16Bit(srcIntPtr, &(dst->heaterOnValue), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetAutoHeaterOffValueBody(const void* src,
                                                size_t srcLen,
                                                WdCmdMsgs__MsgBody__SetAutoHeaterOffValue* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_OFF_VALUE_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    short deserializationResult = 
        Serialization__deserializeAs16Bit(srcIntPtr, &(dst->heaterOffValue), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetHeaterDutyCycleMaxBody(const void* src,
                                                size_t srcLen,
                                                WdCmdMsgs__MsgBody__SetHeaterDutyCycleMax* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_MAX_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    short deserializationResult = 
        Serialization__deserializeAs16Bit(srcIntPtr, &(dst->dutyCycleMax), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetHeaterDutyCyclePeriodBody(const void* src,
                                                   size_t srcLen,
                                                   WdCmdMsgs__MsgBody__SetHeaterDutyCyclePeriod* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_PERIOD_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    short deserializationResult = 
        Serialization__deserializeAs16Bit(srcIntPtr, &(dst->dutyCyclePeriod), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetThermisterVSetpointBody(const void* src,
                                                 size_t srcLen,
                                                 WdCmdMsgs__MsgBody__SetThermisterVSetpoint* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__SET_THERMISTER_V_SETPOINT_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    short deserializationResult = 
        Serialization__deserializeAs16Bit(srcIntPtr, &(dst->thermisterVSetpoint), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeEnterSleepModeBody(const void* src,
                                         size_t srcLen,
                                         WdCmdMsgs__MsgBody__EnterSleepMode* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__ENTER_SLEEP_MODE_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    short deserializationResult = 
        Serialization__deserializeAs8Bit(srcIntPtr, &(dst->confirmationMagicNumber), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeEnterKeepAliveModeBody(const void* src,
                                             size_t srcLen,
                                             WdCmdMsgs__MsgBody__EnterKeepAliveMode* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__ENTER_KEEPALIVE_MODE_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    short deserializationResult = 
        Serialization__deserializeAs8Bit(srcIntPtr, &(dst->confirmationMagicNumber), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeEnterServiceModeBody(const void* src,
                                           size_t srcLen,
                                           WdCmdMsgs__MsgBody__EnterServiceMode* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__ENTER_SERVICE_MODE_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    short deserializationResult = 
        Serialization__deserializeAs8Bit(srcIntPtr, &(dst->confirmationMagicNumber), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeDangForceBattStateBody(const void* src,
                                             size_t srcLen,
                                             WdCmdMsgs__MsgBody__DangForceBattState* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__DANG_FORCE_BATT_STATE_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    short deserializationResult =
        Serialization__deserializeAs8Bit(srcIntPtr, &(dst->confirmationMagicNumberOne), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    srcIntPtr += sizeof(dst->confirmationMagicNumberOne);

    deserializationResult =
        Serialization__deserializeAs8Bit(srcIntPtr, &(dst->confirmationMagicNumberTwo), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    srcIntPtr += sizeof(dst->confirmationMagicNumberTwo);

    uint8_t enumAsUint = 0;
    deserializationResult =
        Serialization__deserializeAs8Bit(srcIntPtr, &enumAsUint, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    dst->state = (WdCmdMsgs__DangForceBattStateSelection) enumAsUint;

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetChargeEnStateBody(const void* src,
                                           size_t srcLen,
                                           WdCmdMsgs__MsgBody__SetChargeEnState* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__SET_CHARGE_EN_STATE_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    uint8_t enumAsUint = 0;
    short deserializationResult =
        Serialization__deserializeAs8Bit(srcIntPtr, &enumAsUint, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    dst->selection = (WdCmdMsgs__SetChargeEnSelection) enumAsUint;

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetChargeRegEnStateBody(const void* src,
                                              size_t srcLen,
                                              WdCmdMsgs__MsgBody__SetChargeRegEnState* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__SET_CHARGE_REG_EN_STATE_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    uint8_t enumAsUint = 0;
    short deserializationResult =
        Serialization__deserializeAs8Bit(srcIntPtr, &enumAsUint, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    dst->selection = (WdCmdMsgs__SetChargeRegEnSelection) enumAsUint;

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetBattEnStateBody(const void* src,
                                         size_t srcLen,
                                         WdCmdMsgs__MsgBody__SetBattEnState* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__SET_BATT_EN_STATE_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    uint8_t enumAsUint = 0;
    short deserializationResult =
        Serialization__deserializeAs8Bit(srcIntPtr, &enumAsUint, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    dst->selection = (WdCmdMsgs__SetBattEnSelection) enumAsUint;

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetBattCtrlEnStateBody(const void* src,
                                             size_t srcLen,
                                             WdCmdMsgs__MsgBody__SetBattCtrlEnState* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__SET_BATT_CTRL_EN_STATE_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    uint8_t enumAsUint = 0;
    short deserializationResult =
        Serialization__deserializeAs8Bit(srcIntPtr, &enumAsUint, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    dst->selection = (WdCmdMsgs__SetBattCtrlEnSelection) enumAsUint;

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeSetLatchBattStateBody(const void* src,
                                            size_t srcLen,
                                            WdCmdMsgs__MsgBody__SetLatchBattState* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__SET_LATCH_BATT_STATE_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    uint8_t enumAsUint = 0;
    short deserializationResult =
        Serialization__deserializeAs8Bit(srcIntPtr, &enumAsUint, SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);
    dst->selection = (WdCmdMsgs__SetLatchBattSelection) enumAsUint;

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeLatchSetPulseLowBody(const void* src,
                                           size_t srcLen,
                                           WdCmdMsgs__MsgBody__LatchSetPulseLow* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__LATCH_SET_PULSE_LOW_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    short deserializationResult =
        Serialization__deserializeAs8Bit(srcIntPtr, &(dst->confirmationMagicNumber), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);

    return WD_CMD_MSGS__STATUS__SUCCESS;
}

static WdCmdMsgs__Status
WdCmdMsgs__deserializeLatchResetPulseLowBody(const void* src,
                                             size_t srcLen,
                                             WdCmdMsgs__MsgBody__LatchResetPulseLow* dst)
{
    if (NULL == src || NULL == dst) {
        return WD_CMD_MSGS__STATUS__ERROR_NULL;
    }

    if (srcLen < WD_CMD_MSGS__PACKED_SIZE__LATCH_RESET_PULSE_LOW_BODY) {
        return WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    uint8_t* srcIntPtr = (uint8_t*) src;
    short deserializationResult =
        Serialization__deserializeAs8Bit(srcIntPtr, &(dst->confirmationMagicNumber), SERIALIZATION__LITTLE_ENDIAN);
    CHECK_SERIALIZATION_RESULT(deserializationResult);

    return WD_CMD_MSGS__STATUS__SUCCESS;
}
