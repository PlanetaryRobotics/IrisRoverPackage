#ifndef __WATCHDOG_WATCHDOG_CMD_MSGS_H___
#define __WATCHDOG_WATCHDOG_CMD_MSGS_H___

#include "comms/cmd_msgs.h"
#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup watchdogWatchdogCmdMsgs Watchdog Command Messages
 * @addtogroup watchdogWatchdogCmdMsgs
 * @{
 */

/**
 * @brief Possible return statuses of CmdMsgs functions.
 */
typedef enum WdCmdMsgs__Status
{
    WD_CMD_MSGS__STATUS__SUCCESS = CMD_MSGS__STATUS__SUCCESS, //!< Operation succeeded.

    /**
     * @brief A required argument or a member of an argument was NULL
     */
    WD_CMD_MSGS__STATUS__ERROR_NULL = CMD_MSGS__STATUS__ERROR_NULL,

    /**
     * @brief A given buffer was too small.
     */
    WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL = CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL,

    /**
     * @brief A serialization error occurred.
     */
    WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR = CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR, 
    WD_CMD_MSGS__STATUS__ERROR_UNKNOWN_MESSAGE_ID = -4, //!< Encountered an unknown command ID.

    /**
     * An unexpected error occurred.
     */
    WD_CMD_MSGS__STATUS__ERROR__INTERNAL = CMD_MSGS__STATUS__ERROR__INTERNAL
} WdCmdMsgs__Status;

//######################################################################################################################
// Message Field Types
//######################################################################################################################

/**
 * @brief Possible response status codes.
 */
typedef enum WdCmdMsgs__ResponseStatusCode
{
    WD_CMD_MSGS__RESPONSE_STATUS__SUCCESS = 0, //!< Command performed successfully.
    WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_PACKET_LENGTH = 1, //!< Wrong packet length for this command.
    WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_CHECKSUM = 2, //!< Checksum validation failed.
    WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_MODULE_ID = 3, //!< Unexpected module ID.
    WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_ID = 4, //!< Unexpected command ID.
    WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_PARAMETER = 5, //!< Unexpected parameter value.
    WD_CMD_MSGS__RESPONSE_STATUS__ERROR_BAD_COMMAND_SEQUENCE = 6, //!< Unexpected sequence of commands.
    
    WD_CMD_MSGS__RESPONSE_STATUS__DEPLOY = 96, //!< The watchdog just enabled its HDRM.
} WdCmdMsgs__ResponseStatusCode;

/**
 * @brief Possible watchdog command IDs
 */
typedef enum WdCmdMsgs__CommandId
{
    WD_CMD_MSGS__CMD_ID__RESET_SPECIFIC = 0x1000, //!< Reset Specific.
    WD_CMD_MSGS__CMD_ID__PREP_FOR_DEPLOY = 0x1001, //!< Prepare to Deploy.
    WD_CMD_MSGS__CMD_ID__DEPLOY = 0x1002, //!< Deploy.
    WD_CMD_MSGS__CMD_ID__SWITCH_CONN_MODE = 0x1004, //!< Switch Connection Mode.
    WD_CMD_MSGS__CMD_ID__SET_HEATER_KP = 0x10AA, //!< Set proportional constant of heater temperature controller.
    WD_CMD_MSGS__CMD_ID__SET_AUTO_HEATER_ON_VALUE = 0x10AB, //!< Set heater "ON" value used with auto heater controller.
    WD_CMD_MSGS__CMD_ID__SET_AUTO_HEATER_OFF_VALUE = 0x10AC, //!< Set heater "OFF" value used with auto heater controller.
    WD_CMD_MSGS__CMD_ID__SET_HEATER_DUTY_CYCLE_MAX = 0x10AD, //!< Set max duty cycle of heater PWM.
    WD_CMD_MSGS__CMD_ID__SET_HEATER_DUTY_CYCLE_PERIOD = 0x10AE, //!< Set duty period period of heater PWM.
    WD_CMD_MSGS__CMD_ID__SET_THERMISTOR_V_SETPOINT = 0x10DA, //!< Set the thermistor value setpoint.
    WD_CMD_MSGS__CMD_ID__ENTER_SLEEP_MODE = 0x10EA, //!< Enter "Sleep" mode.
    WD_CMD_MSGS__CMD_ID__ENTER_KEEPALIVE_MODE = 0x10EB, //!< Enter "Keep Alive" mode.
    WD_CMD_MSGS__CMD_ID__ENTER_SERVICE_MODE = 0x10EC, //!< Enter "Service" mode.
    WD_CMD_MSGS__CMD_ID__CLEAR_RESET_MEMORY = 0x10F4, //!< Clear the reset memory used in the detailed report.
    WD_CMD_MSGS__CMD_ID__DANGEROUS_FORCE_BATT_STATE = 0x10F5, //!< Dangerous: Force battery state.
    WD_CMD_MSGS__CMD_ID__REQUEST_DETAILED_REPORT = 0x10F6, //!< Request a detailed report
    WD_CMD_MSGS__CMD_ID__SET_CHARGE_EN_STATE = 0x10F7, //!< Set charging IC enable state.
    WD_CMD_MSGS__CMD_ID__SET_CHARGE_REG_EN_STATE = 0x10F8, //!< Set charging power 28V regulator enable state.
    WD_CMD_MSGS__CMD_ID__SET_BATT_EN_STATE = 0x10F9, //!< Set battery connection state.
    WD_CMD_MSGS__CMD_ID__SET_BATT_CTRL_EN_STATE = 0x10FA, //!< Set battery management control state.
    WD_CMD_MSGS__CMD_ID__SET_LATCH_BATT_STATE = 0x10FB, //!< Set battery latch state.
    WD_CMD_MSGS__CMD_ID__LATCH_SET_PULSE_LOW = 0x10FC, //!< Pulse battery latch "SET" override low.
    WD_CMD_MSGS__CMD_ID__LATCH_RESET_PULSE_LOW = 0x10FD, //!< Pulse battery latch "RESET" override low.
} WdCmdMsgs__CommandId;

/**
 * @brief Possible reset values, given for the reset specific command.
 */
typedef enum WdCmdMsgs__ResetSpecificId
{
    /**
     * @brief Don't reset anything.
     * @todo What is the point of this?
     */
    WD_CMD_MSGS__RESET_ID__NO_RESET = 0x00,

    WD_CMD_MSGS__RESET_ID__HERCULES_RESET = 0x01, //!< Reset the Hercules.
    WD_CMD_MSGS__RESET_ID__HERCULES_POWER_ON = 0x02, //!< Power on the Hercules.
    WD_CMD_MSGS__RESET_ID__HERCULES_POWER_OFF = 0x03, //!< Power off the Hercules.

    WD_CMD_MSGS__RESET_ID__RADIO_RESET = 0x04, //!< Reset the radio (the WiFi).
    WD_CMD_MSGS__RESET_ID__RADIO_POWER_ON = 0x05, //!< Power on the radio (the WiFi).
    WD_CMD_MSGS__RESET_ID__RADIO_POWER_OFF = 0x06, //!< Power off the radio (the WiFi).

    WD_CMD_MSGS__RESET_ID__CAM_FPGA_RESET = 0x07, //!< Reset the camera FPGA.
    WD_CMD_MSGS__RESET_ID__CAM_FPGA_POWER_ON = 0x08, //!< Power on the camera FPGA.
    WD_CMD_MSGS__RESET_ID__CAM_FPGA_POWER_OFF = 0x09, //!< Power off the camera FPGA.

    WD_CMD_MSGS__RESET_ID__MOTOR_1_RESET = 0x0A, //!< Reset motor 1.
    WD_CMD_MSGS__RESET_ID__MOTOR_2_RESET = 0x0B, //!< Reset motor 2.
    WD_CMD_MSGS__RESET_ID__MOTOR_3_RESET = 0x0C, //!< Reset motor 3.
    WD_CMD_MSGS__RESET_ID__MOTOR_4_RESET = 0x0D, //!< Reset motor 4.

    WD_CMD_MSGS__RESET_ID__ALL_MOTORS_RESET = 0x0E, //!< Reset all motors.
    WD_CMD_MSGS__RESET_ID__ALL_MOTORS_POWER_ON = 0x0F, //!< Power on all motors.
    WD_CMD_MSGS__RESET_ID__ALL_MOTORS_POWER_OFF = 0x10, //!< Power off all motors.

    WD_CMD_MSGS__RESET_ID__3_3V_EN_RESET = 0x11, //!< Reset the 3.3V line enable.
    WD_CMD_MSGS__RESET_ID__3_3V_EN_POWER_ON = 0x12, //!< Power on the 3.3V line enable.
    WD_CMD_MSGS__RESET_ID__3_3V_EN_POWER_OFF = 0x13, //!< Power off the 3.3V line enable.

    WD_CMD_MSGS__RESET_ID__24V_EN_RESET = 0x14, //!< Reset the 24V line enable.
    WD_CMD_MSGS__RESET_ID__24V_EN_POWER_ON = 0x15, //!< Power on the 24V line enable.
    WD_CMD_MSGS__RESET_ID__24V_EN_POWER_OFF = 0x16, //!< Power off the 24V line enable.

    WD_CMD_MSGS__RESET_ID__HDRM_DEPLOY_SIGNAL_POWER_OFF = 0x18, //!< Power off the HDRM.

    WD_CMD_MSGS__RESET_ID__FPGA_CAM_0_SELECT = 0x19, //!< Select camera 0.
    WD_CMD_MSGS__RESET_ID__FPGA_CAM_1_SELECT = 0x1A, //!< Select camera 1.

    WD_CMD_MSGS__RESET_ID__BATTERY_CHARGE_START = 0x1B, //!< Start charging the batteries.
    WD_CMD_MSGS__RESET_ID__BATTERY_CHARGE_STOP = 0x1C, //!< Stop charging the batteries.

    WD_CMD_MSGS__RESET_ID__RS422_UART_ENABLE = 0x1D, //!< Enable the RS422 UART.
    WD_CMD_MSGS__RESET_ID__RS422_UART_DISABLE = 0x1E, //!< Disable the RS422 UART.

    WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_ENABLE = 0x1F, //!< Enable the automatic heater controller.
    WD_CMD_MSGS__RESET_ID__AUTO_HEATER_CONTROLLER_DISABLE = 0x20, //!< Disable the automatic heater controller.

    WD_CMD_MSGS__RESET_ID__HERCULES_WATCHDOG_ENABLE = 0x21, //!< Enable the Hercules watchdog.
    WD_CMD_MSGS__RESET_ID__HERCULES_WATCHDOG_DISABLE = 0x22, //!< Disable the Hercules watchdog.

    WD_CMD_MSGS__RESET_ID__BATTERIES_ENABLE = 0x23, //!< Enable the batteries.
    WD_CMD_MSGS__RESET_ID__BATTERIES_DISABLE = 0x24, //!< Disable the batteries.

    WD_CMD_MSGS__RESET_ID__HDRM_DEPLOY_SIGNAL_POWER_ON = 0xEE //!< Power on the HDRM.

} WdCmdMsgs__ResetSpecificId;

/**
 * @brief Possible values of the parameter of the Switch Connection Mode command.
 */
typedef enum WdCmdMsgs__SwitchConnModeSelection
{
    WD_CMD_MSGS__CONN_MODE__WIRED = 0x01, //!< Wired (RS-422): Watchdog gets data, forwards to Hercules
    WD_CMD_MSGS__CONN_MODE__WIRELESS = 0x02 //!< Wireless: Hercules gets data, forwards to watchdog
} WdCmdMsgs__SwitchConnModeSelection;

/**
 * @brief Possible values of the parameter of the Dangerous Force Battery State command.
 */
typedef enum WdCmdMsgs__DangForceBattStateSelection
{
    WD_CMD_MSGS__DANG_FORCE_BATT_STATE__LOW = 0x00, //!< Force battery state low
    WD_CMD_MSGS__DANG_FORCE_BATT_STATE__HIGH = 0xFF, //!< Force battery state high
    WD_CMD_MSGS__DANG_FORCE_BATT_STATE__RESTORE = 0xAA //!< Restore battery state to an input
} WdCmdMsgs__DangForceBattStateSelection;

/**
 * @brief Possible values of the parameter of the Set Charge EN State command.
 */
typedef enum WdCmdMsgs__SetChargeEnSelection
{
    WD_CMD_MSGS__CHARGE_EN__ON = 0xFF, //!< Enable charging
    WD_CMD_MSGS__CHARGE_EN__OFF = 0x00, //!< Disable charging
    WD_CMD_MSGS__CHARGE_EN__FORCE_HIGH = 0x99 //!< Force high output on CHRG_EN pin
} WdCmdMsgs__SetChargeEnSelection;

/**
 * @brief Possible values of the parameter of the Set Charge EN State command.
 */
typedef enum WdCmdMsgs__SetChargeRegEnSelection
{
    WD_CMD_MSGS__CHARGE_REG_EN__ON = 0xFF, //!< Enable charging regulator
    WD_CMD_MSGS__CHARGE_REG_EN__OFF = 0x00 //!< Disable charging regulator
} WdCmdMsgs__SetChargeRegEnSelection;

/**
 * @brief Possible values of the parameter of the Set Battery EN State command.
 */
typedef enum WdCmdMsgs__SetBattEnSelection
{
    WD_CMD_MSGS__BATT_EN__ON = 0xFF, //!< Enable battery connection
    WD_CMD_MSGS__BATT_EN__OFF = 0x00 //!< Disable battery connection
} WdCmdMsgs__SetBattEnSelection;

/**
 * @brief Possible values of the parameter of the Set Battery Control EN State command.
 */
typedef enum WdCmdMsgs__SetBattCtrlEnSelection
{
    WD_CMD_MSGS__BATT_CTRL_EN__ON = 0xFF, //!< Enable battery connection
    WD_CMD_MSGS__BATT_CTRL_EN__OFF = 0x00, //!< Disable battery connection
    WD_CMD_MSGS__BATT_CTRL_EN__FORCE_HIGH = 0x99 //!< Force high output on BCTRLE pin
} WdCmdMsgs__SetBattCtrlEnSelection;

/**
 * @brief Possible values of the parameter of the Set Battery Control EN State command.
 */
typedef enum WdCmdMsgs__SetLatchBattSelection
{
    WD_CMD_MSGS__LATCH_BATT__ON = 0xFF, //!< Enable battery connection
    WD_CMD_MSGS__LATCH_BATT__OFF = 0x00, //!< Disable battery connection
    WD_CMD_MSGS__LATCH_BATT__UPDATE = 0xAA//!< Pulse latch low-high-low
} WdCmdMsgs__SetLatchBattSelection;

/**
 * @brief Possible values of the parameter of the Set Battery Control EN State command.
 */
typedef enum WdCmdMsgs__LatchSetResetSelection
{
    WD_CMD_MSGS__LATCH_SET_RESET__OFF = 0x00, //!< Set as input
    WD_CMD_MSGS__LATCH_SET_RESET__PULSE = 0x15, //!< Pulse high-low-high as an output
    WD_CMD_MSGS__LATCH_SET_RESET__FORCE_HIGH = 0xBB, //!< Make an output and pull high
    WD_CMD_MSGS__LATCH_SET_RESET__FORCE_LOW = 0xFF //!< Make an output and pull low
} WdCmdMsgs__LatchSetResetSelection;

/**
 * @brief The magic number that is expected as the parameter of all of the commands that change the mode.
 */
static const uint8_t WD_CMD_MSGS__CONFIRM_MODE_CHANGE_MAGIC_NUMBER = 0x77u;

/**
 * @brief The magic number that is expected as the parameter of the "Prepare to Deploy" and "Deploy" commands.
 */
static const uint8_t WD_CMD_MSGS__CONFIRM_DEPLOYMENT_MAGIC_NUMBER = 0x60u;

/**
 * @brief The first magic number expected as a parameter of the "Dangerous Force Battery State" command.
 */
static const uint8_t WD_CMD_MSGS__CONFIRM_DANG_FORCE_BATT_STATE_MAGIC_NUMBER_ONE = 0xF0u;

/**
 * @brief The second magic number expected as a parameter of the "Dangerous Force Battery State" command.
 */
static const uint8_t WD_CMD_MSGS__CONFIRM_DANG_FORCE_BATT_STATE_MAGIC_NUMBER_TWO = 0x01u;

/**
 * @brief The first magic number expected as a parameter of the "Clear Reset Memory" command.
 */
static const uint8_t WD_CMD_MSGS__CONFIRM_CLR_RST_MEM_MAGIC_NUMBER_ONE = 0xC7u;

/**
 * @brief The second magic number expected as a parameter of the "Clear Reset Memory" command.
 */
static const uint8_t WD_CMD_MSGS__CONFIRM_CLR_RST_MEM_MAGIC_NUMBER_TWO = 0x19u;

/**
 * @brief The magic number that is expected as the parameter of the "Request Detailed Report" command.
 */
static const uint8_t WD_CMD_MSGS__CONFIRM_REQ_DET_REPORT_MAGIC_NUMBER = 0x57u;

/**
 * @brief The magic number expected as the first byte of the response message.
 */
static const uint8_t WD_CMD_MSGS__RESPONSE_MAGIC_NUMBER = 0x0Au;

//######################################################################################################################
// Message Bodies
//######################################################################################################################

/**
 * @brief The body of a "Reset Specific" command.
 */
typedef struct WdCmdMsgs__MsgBody__ResetSpecific
{
    WdCmdMsgs__ResetSpecificId resetId; //!< The reset value.
} WdCmdMsgs__MsgBody__ResetSpecific;

/**
 * @brief The body of a "Prepare for Deploy" command.
 */
typedef struct WdCmdMsgs__MsgBody__PrepForDeploy
{
    uint8_t confirmationMagicNumber; //!< Must be the expected magic number for this command to be performed.
} WdCmdMsgs__MsgBody__PrepForDeploy;

/**
 * @brief The body of a "Deploy" command.
 */
typedef struct WdCmdMsgs__MsgBody__Deploy
{
    uint8_t confirmationMagicNumber; //!< Must be the expected magic number for this command to be performed.
} WdCmdMsgs__MsgBody__Deploy;

/**
 * @brief The body of a "Switch Connection Mode" command.
 */
typedef struct WdCmdMsgs__MsgBody__SwitchConnMode
{
    WdCmdMsgs__SwitchConnModeSelection connMode; //!< The connection mode to switch to.
} WdCmdMsgs__MsgBody__SwitchConnMode;

/**
 * @brief The body of a "Set Heater Kp" command.
 */
typedef struct WdCmdMsgs__MsgBody__SetHeaterKp
{
    uint16_t kp; //!< The value to use as the proportional coefficient.
} WdCmdMsgs__MsgBody__SetHeaterKp;

/**
 * @brief The body of a "Set Auto Heater On Value" command.
 */
typedef struct WdCmdMsgs__MsgBody__SetAutoHeaterOnValue
{
    uint16_t heaterOnValue; //!< The value to use as the automatic heater "on" value.
} WdCmdMsgs__MsgBody__SetAutoHeaterOnValue;

/**
 * @brief The body of a "Set Auto Heater Off Value" command.
 */
typedef struct WdCmdMsgs__MsgBody__SetAutoHeaterOffValue
{
    uint16_t heaterOffValue; //!< The value to use as the automatic heater "off" value.
} WdCmdMsgs__MsgBody__SetAutoHeaterOffValue;

/**
 * @brief The body of a "Set Heater Duty Cycle Maximum" command.
 */
typedef struct WdCmdMsgs__MsgBody__SetHeaterDutyCycleMax
{
    uint16_t dutyCycleMax; //!< The maximum duty cycle value.
} WdCmdMsgs__MsgBody__SetHeaterDutyCycleMax;

/**
 * @brief The body of a "Set Heater Duty Cycle Period" command.
 */
typedef struct WdCmdMsgs__MsgBody__SetHeaterDutyCyclePeriod
{
    uint16_t dutyCyclePeriod; //!< The duty cycle period value.
} WdCmdMsgs__MsgBody__SetHeaterDutyCyclePeriod;

/**
 * @brief The body of a "Set Thermister V Setpoint" command.
 */
typedef struct WdCmdMsgs__MsgBody__SetThermisterVSetpoint
{
    uint16_t thermisterVSetpoint; //!< The thermister V setpoint value.
} WdCmdMsgs__MsgBody__SetThermisterVSetpoint;

/**
 * @brief The body of an "Enter Sleep Mode" command.
 */
typedef struct WdCmdMsgs__MsgBody__EnterSleepMode
{
    uint8_t confirmationMagicNumber; //!< Must be the expected magic number for this command to be performed.
} WdCmdMsgs__MsgBody__EnterSleepMode;

/**
 * @brief The body of an "Enter Keep Alive Mode" command.
 */
typedef struct WdCmdMsgs__MsgBody__EnterKeepAliveMode
{
    uint8_t confirmationMagicNumber; //!< Must be the expected magic number for this command to be performed.
} WdCmdMsgs__MsgBody__EnterKeepAliveMode;

/**
 * @brief The body of an "Enter Service Mode" command.
 */
typedef struct WdCmdMsgs__MsgBody__EnterServiceMode
{
    uint8_t confirmationMagicNumber; //!< Must be the expected magic number for this command to be performed.
} WdCmdMsgs__MsgBody__EnterServiceMode;

/**
 * @brief The body of an "Dangerous Force Battery State" command.
 */
typedef struct WdCmdMsgs__MsgBody__DangForceBattState
{
    uint8_t confirmationMagicNumberOne; //!< Must be the first expected magic number for this command to be performed.
    uint8_t confirmationMagicNumberTwo; //!< Must be the second expected magic number for this command to be performed.
    WdCmdMsgs__DangForceBattStateSelection state; //!< The state to force.
} WdCmdMsgs__MsgBody__DangForceBattState;

/**
 * @brief The body of an "Set Charge En State" command.
 */
typedef struct WdCmdMsgs__MsgBody__SetChargeEnState
{
    WdCmdMsgs__SetChargeEnSelection selection; //!< The charge enable state to be set.
} WdCmdMsgs__MsgBody__SetChargeEnState;

/**
 * @brief The body of an "Set Charge Reg En State" command.
 */
typedef struct WdCmdMsgs__MsgBody__SetChargeRegEnState
{
    WdCmdMsgs__SetChargeRegEnSelection selection; //!< The charge regulator enable state to be set.
} WdCmdMsgs__MsgBody__SetChargeRegEnState;

/**
 * @brief The body of an "Set Batt En State" command.
 */
typedef struct WdCmdMsgs__MsgBody__SetBattEnState
{
    WdCmdMsgs__SetBattEnSelection selection; //!< The battery enable state to be set.
} WdCmdMsgs__MsgBody__SetBattEnState;

/**
 * @brief The body of an "Set Batt Ctrl En State" command.
 */
typedef struct WdCmdMsgs__MsgBody__SetBattCtrlEnState
{
    WdCmdMsgs__SetBattCtrlEnSelection selection; //!< The battery control enable state to be set.
} WdCmdMsgs__MsgBody__SetBattCtrlEnState;

/**
 * @brief The body of an "Set Latch Batt State" command.
 */
typedef struct WdCmdMsgs__MsgBody__SetLatchBattState
{
    WdCmdMsgs__SetLatchBattSelection selection; //!< The latch battery state to be set.
} WdCmdMsgs__MsgBody__SetLatchBattState;

/**
 * @brief The body of an "Latch Set Pulse Low" command.
 */
typedef struct WdCmdMsgs__MsgBody__LatchSetPulseLow
{
        WdCmdMsgs__LatchSetResetSelection selection; //!< The latch set state to be set.
} WdCmdMsgs__MsgBody__LatchSetPulseLow;

/**
 * @brief The body of an "Latch Reset Pulse Low" command.
 */
typedef struct WdCmdMsgs__MsgBody__LatchResetPulseLow
{
        WdCmdMsgs__LatchSetResetSelection selection; //!< The latch reset state to be set.
} WdCmdMsgs__MsgBody__LatchResetPulseLow;

/**
 * @brief The body of an "Clear Reset Memory" command.
 */
typedef struct WdCmdMsgs__MsgBody__ClearResetMemory
{
    uint8_t magicOne;
    uint8_t magicTwo;
} WdCmdMsgs__MsgBody__ClearResetMemory;

/**
 * @brief The body of an "Request Detailed Report" command.
 */
typedef struct WdCmdMsgs__MsgBody__RequestDetailedReport
{
    uint8_t magic;
} WdCmdMsgs__MsgBody__RequestDetailedReport;

//######################################################################################################################
// Overall Message Structure
//######################################################################################################################

/**
 * @brief A union of all message bodies.
 *
 * Only the body that corresponds to the command ID of a given command should be accessed.
 */
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
    WdCmdMsgs__MsgBody__DangForceBattState dangForceBattState;
    WdCmdMsgs__MsgBody__SetChargeEnState setChargeEnState;
    WdCmdMsgs__MsgBody__SetChargeRegEnState setChargeRegEnState;
    WdCmdMsgs__MsgBody__SetBattEnState setBattEnState;
    WdCmdMsgs__MsgBody__SetBattCtrlEnState setBattCtrlEnState;
    WdCmdMsgs__MsgBody__SetLatchBattState setLatchBattState;
    WdCmdMsgs__MsgBody__LatchSetPulseLow latchSetPulseLow;
    WdCmdMsgs__MsgBody__LatchResetPulseLow latchResetPulseLow;
    WdCmdMsgs__MsgBody__ClearResetMemory clearResetMem;
    WdCmdMsgs__MsgBody__RequestDetailedReport reqDetReport;
} WdCmdMsgs__MessageBody;

/**
 * @brief A watchdog command message.
 */
typedef struct WdCmdMsgs__Message
{
    CmdMsgs__CommonHeader commonHeader; //!< The FSW common header.
    WdCmdMsgs__CommandId commandId; //!< The command ID.
    WdCmdMsgs__MessageBody body; //!< The message body, if `commandId` is a message with a parameter.
} WdCmdMsgs__Message;

/**
 * @brief A watchdog command response message.
 */
typedef struct WdCmdMsgs__Response
{
    uint8_t magicNumber; //!< Should always be set to `WD_CMD_MSGS__RESPONSE_MAGIC_NUMBER`.
    WdCmdMsgs__CommandId commandId; //!< The command ID of the message to which this is a response.
    WdCmdMsgs__ResponseStatusCode statusCode; //!< The status of performing the message to which this is a reponse.
} WdCmdMsgs__Response;

//######################################################################################################################
// Packed Sizes
//######################################################################################################################

/**
 * @brief The packed sizes of the body and full message for the various command types as well as the command response.
 */
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
    WD_CMD_MSGS__PACKED_SIZE__DANG_FORCE_BATT_STATE_BODY = 3 * sizeof(uint8_t),
    WD_CMD_MSGS__PACKED_SIZE__SET_CHARGE_EN_STATE_BODY = sizeof(uint8_t),
    WD_CMD_MSGS__PACKED_SIZE__SET_CHARGE_REG_EN_STATE_BODY = sizeof(uint8_t),
    WD_CMD_MSGS__PACKED_SIZE__SET_BATT_EN_STATE_BODY = sizeof(uint8_t),
    WD_CMD_MSGS__PACKED_SIZE__SET_BATT_CTRL_EN_STATE_BODY = sizeof(uint8_t),
    WD_CMD_MSGS__PACKED_SIZE__SET_LATCH_BATT_STATE_BODY = sizeof(uint8_t),
    WD_CMD_MSGS__PACKED_SIZE__LATCH_SET_PULSE_LOW_BODY = sizeof(uint8_t),
    WD_CMD_MSGS__PACKED_SIZE__LATCH_RESET_PULSE_LOW_BODY = sizeof(uint8_t),
    WD_CMD_MSGS__PACKED_SIZE__CLEAR_RESET_MEMORY_BODY = 2 * sizeof(uint8_t),
    WD_CMD_MSGS__PACKED_SIZE__REQUEST_DETAILED_REPORT_BODY = sizeof(uint8_t),


    // Full Messages

    WD_CMD_MSGS__PACKED_SIZE__RESPONSE_MSG = (3 * sizeof(uint8_t)),

    WD_CMD_MSGS__PACKED_SIZE__RESET_SPECIFIC_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__RESET_SPECIFIC_BODY,
    WD_CMD_MSGS__PACKED_SIZE__PREP_FOR_DEPLOY_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__PREP_FOR_DEPLOY_BODY,
    WD_CMD_MSGS__PACKED_SIZE__DEPLOY_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__DEPLOY_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SWITCH_CONN_MODE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__SWITCH_CONN_MODE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_KP_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_KP_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_ON_VALUE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_ON_VALUE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_OFF_VALUE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__SET_AUTO_HEATER_OFF_VALUE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_MAX_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_MAX_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_PERIOD_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__SET_HEATER_DUTY_CYCLE_PERIOD_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_THERMISTER_V_SETPOINT_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__SET_THERMISTER_V_SETPOINT_BODY,
    WD_CMD_MSGS__PACKED_SIZE__ENTER_SLEEP_MODE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__ENTER_SLEEP_MODE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__ENTER_KEEPALIVE_MODE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__ENTER_KEEPALIVE_MODE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__ENTER_SERVICE_MODE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__ENTER_SERVICE_MODE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__DANG_FORCE_BATT_STATE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__DANG_FORCE_BATT_STATE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_CHARGE_EN_STATE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__SET_CHARGE_EN_STATE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_CHARGE_REG_EN_STATE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__SET_CHARGE_REG_EN_STATE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_BATT_EN_STATE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__SET_BATT_EN_STATE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_BATT_CTRL_EN_STATE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__SET_BATT_CTRL_EN_STATE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__SET_LATCH_BATT_STATE_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__SET_LATCH_BATT_STATE_BODY,
    WD_CMD_MSGS__PACKED_SIZE__LATCH_SET_PULSE_LOW_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__LATCH_SET_PULSE_LOW_BODY,
    WD_CMD_MSGS__PACKED_SIZE__LATCH_RESET_PULSE_LOW_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__LATCH_RESET_PULSE_LOW_BODY,
    WD_CMD_MSGS__PACKED_SIZE__CLEAR_RESET_MEMORY_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__CLEAR_RESET_MEMORY_BODY,
    WD_CMD_MSGS__PACKED_SIZE__REQUEST_DETAILED_REPORT_MSG = WD_CMD_MSGS__PACKED_SIZE__COMMON_HEADER + sizeof(uint16_t) + WD_CMD_MSGS__PACKED_SIZE__REQUEST_DETAILED_REPORT_BODY,

    WD_CMD_MSGS__PACKED_SIZE__SMALLEST_MSG = WD_CMD_MSGS__PACKED_SIZE__RESET_SPECIFIC_MSG,
    WD_CMD_MSGS__PACKED_SIZE__LARGEST_MSG = WD_CMD_MSGS__PACKED_SIZE__DANG_FORCE_BATT_STATE_MSG
} WdCmdMsgs__PackedSize;

//######################################################################################################################
// Message Serialization and Deserialization Functions
//######################################################################################################################


/**
 * @brief Serializes the message in `src` with system endianness into the buffer `dst` with little endianness.
 *
 * @param src [IN] The response to be serialized.
 * @param dst [OUT] The buffer into which the message will be serialized. Must have a large enough size, as determined
 *            by the value of `dstLen`, of at least WD_CMD_MSGS__PACKED_SIZE__RESPONSE_MSG bytes.
 * @param dstLen [IN] The length of the buffer pointed to by `dst`.
 *
 * @return One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: `src` or `dst` was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: `dst` was too small to hold the serialized message.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
WdCmdMsgs__Status WdCmdMsgs__serializeGroundResponse(const WdCmdMsgs__Response* src,
                                                     void* dst,
                                                     size_t dstLen);

/**
 * @brief Deserializes the message body packed in `src` with little endianness into the struct `dst` with system
 *        endianness.
 *
 * The type of body in `src` is determined by `srcMsgId`.
 *
 * @param srcMsgId [IN] The message ID of the message body type packed in `src`.
 * @param src [IN] The buffer containing the packed message body to be deserialized. Must have a size, as determined
 *            by the value of `srcLen`, that is large enough to fit the message body with the type specified by
 *            `srcMsgId`. The exact size required varies based on the type of the message body.
 * @param srcLen [IN] The length of the buffer pointed to by `src`.
 * @param dst [OUT] The struct into which the message body will be deserialized.
 *
 * @return One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: `src` or `dst` was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: `srcLen` was too small to hold the message body.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
WdCmdMsgs__Status
WdCmdMsgs__deserializeBody(WdCmdMsgs__CommandId srcMsgId,
                           const void* src,
                           size_t srcLen,
                           WdCmdMsgs__MessageBody* dst);

/**
 * @brief Deserializes the message packed in `src` with little endianness into the struct `dst` with system endianness.
 *
 * @param src [IN] The buffer containing the packed message to be deserialized. Must have a size, as determined by the
 *            value of `srcLen`, that is large enough to fit a message header and the message body whose type is
 *            specified in the message header. The exact size required varies based on the type of the message. At
 *            a minimum it must be WD_CMD_MSGS__PACKED_SIZE__SMALLEST_MSG, but it may need to be as large as 
 *            WD_CMD_MSGS__PACKED_SIZE__LARGEST_MSG.
 * @param srcLen [IN] The length of the buffer pointed to by `src`.
 * @param dst [OUT] The struct into which the message will be deserialized.
 * @param deserializeHeader [IN] Whether or not to deserialize the header into `dst->commonHeader`. This can be set to
 *                           false if (for instance) the command header has already been deserialized prior to calling
 *                           this function.
 *
 * @return One of the following:
 *   - WD_CMD_MSGS__STATUS__SUCCESS: The function was successful.
 *   - WD_CMD_MSGS__STATUS__ERROR_NULL: `src` or `dst` was NULL.
 *   - WD_CMD_MSGS__STATUS__ERROR_BUFFER_TOO_SMALL: `srcLen` was too small to hold the message.
 *   - WD_CMD_MSGS__STATUS__ERROR_SERIALIZATION_ERROR: A Serialization function call returned an error.
 */
WdCmdMsgs__Status
WdCmdMsgs__deserializeMessage(const void* src,
                              size_t srcLen,
                              WdCmdMsgs__Message* dst,
                              BOOL deserializeHeader);

/**
 * @}
 */

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __WATCHDOG_WATCHDOG_CMD_MSGS_H___ */
