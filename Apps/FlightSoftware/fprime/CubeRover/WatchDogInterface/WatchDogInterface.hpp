// ======================================================================
// \title  WatchDogInterfaceComponentImpl.hpp
// \author alec
// \brief  hpp file for WatchDogInterface component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// This file outlines all functions implemented in WatchDogInterface
// Contains all functions that send and receive data between WatchdogInterface and Watchdog
// Contains functions that only watchdog receives, functions are marked as such. Such functions only send a stroke to the Watchdog if ever received.
// ======================================================================

#ifndef WatchDogInterface_HPP
#define WatchDogInterface_HPP

#include "CubeRover/WatchDogInterface/WatchDogInterfaceComponentAc.hpp"
#include <CubeRover/WatchDogInterface/WatchDogRxTask.hpp>
#include "Include/FswPacket.hpp"
#include "FreeRTOS.h"
#include "os_portmacro.h"

#include "lin.h"
#include "adc.h"
#include "sci.h"

// The pin number for the deploment pin 2
static const U8 deploy_bit = 4;
// The number of thermistors on the SBC + number of externally wired thermistors
static const U8 number_thermistors = 16;
// The number of current sensors on the SBC:
static const U8 number_current_sensors = 7;
// Default size of zero sent to watchdog
static const U16 zero_size = 0x0000;
// Minimum size that should be received back from watchdog when receiving
static const U8 min_receive_size = 8;
// Magic Value first sent in the header between watchdog and hercules communication
static const U32 header_magic = 0x21B00B;

/**
 * This controls how long we'll wait for a response before allowing the next message of the same type
 * to be sent to the MSP430 watchdog.
 */
static const uint32_t COMMAND_TIMEOUT_MILLISECONDS = 2000;

// HOW OFTEN SHOULD WE READ ADC FOR THERMISTORS (minimum period, not particularly time sensitive):
static const uint32_t ADC_THERMISTOR_READ_PERIOD_MS = 2000;

// HOW OFTEN SHOULD WE READ ADC FOR CURRENT SENSORS (minimum period, not particularly time sensitive):
static const uint32_t ADC_CURRENT_READ_PERIOD_MS = 1500;

// How often the processor should wait before checking back in on dmaSend completion while polling for it.
// Since this is a high priority task, it's not a good idea for this to be 0 (though it *can* be zero) in order to prevent Task starvation.
// NOTE: FreeRTOS scheduler ticks are every 1ms.
static const TickType_t DMA_SEND_POLLING_CHECK_INTERVAL = 2 / portTICK_PERIOD_MS; // every 2ms (2 ticks)

// These three parameters control the setup of the task that handles data received from the MSP430 watchdog.
static const NATIVE_INT_TYPE WATCH_DOG_INTERFACE_RX_TASK_PRIORITY = 16;
// NOTE: Stack size is in words. Make sure there's enough room for the overhead (min task size) plus some overhead. Use `uxTaskGetStackHighWaterMark(NULL)` to tune.
static const NATIVE_INT_TYPE WATCH_DOG_INTERFACE_RX_TASK_STACK_SIZE = configMINIMAL_STACK_SIZE + 256;
static const NATIVE_INT_TYPE WATCH_DOG_INTERFACE_RX_TASK_CPU_AFFINITY = -1;

namespace CubeRover
{
    class WatchDogInterfaceComponentImpl : public WatchDogInterfaceComponentBase, public virtual WatchDogRxCallbackProcessor
    {
    public:
        friend class NetworkManagerComponentImpl;
        // ----------------------------------------------------------------------
        // Construction, initialization, and destruction
        // ----------------------------------------------------------------------

        //! Construct object WatchDogInterface
        //!
        WatchDogInterfaceComponentImpl(
#if FW_OBJECT_NAMES == 1
            const char *const compName /*!< The component name*/
#else
            void
#endif
        );

        //! Initialize object WatchDogInterface
        //!
        void init(
            const NATIVE_INT_TYPE queueDepth,  /*!< The queue depth*/
            const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
        );

        //! Destroy object WatchDogInterface
        //!
        ~WatchDogInterfaceComponentImpl(void);

        /**
         * @brief The callback invoked by the `WatchDogRxTask` when it has received a message.
         *
         * @param msg The parsed message received from the MSP430 watchdog.
         * @param goodParity Whether or not `msg` passed its parity check. If false, `msg` will contain only a header.
         */
        virtual void rxCallback(WatchDogMpsm::Message &msg, bool goodParity);

        // Usage during FSW initialization or when other components call it
        // Only difference between this is function and Reset_Specific_cmdHandler is lack of cmd response and using int for reset
        bool Reset_Specific_Handler(int reset_enum_number);

        PRIVATE :

            // ----------------------------------------------------------------------
            // Handler implementations for user-defined typed input ports
            // ----------------------------------------------------------------------

            //! Handler implementation for PingIn
            //!
            void
            Run_handler(
                const NATIVE_INT_TYPE portNum, /*!< The port number*/
                NATIVE_UINT_TYPE context       /*!< The call order*/
            );

        //! Handler implementation for CompResetRequest
        //!
        void CompResetRequest_handler(
            const NATIVE_INT_TYPE portNum, /*!< The port number*/
            CubeRoverPorts::ResetValue reset);

        //! Handler implementation for downlink
        //!
        void downlink_handler(
            const NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::Buffer &fwBuffer);

        //! Handler implementation for PingIn
        //!
        void PingIn_handler(
            const NATIVE_INT_TYPE portNum, /*!< The port number*/
            U32 key                        /*!< Value to return to pinger*/
        );

        //! Handler implementation for WdogStroke
        //!
        void WdogStroke_handler(
            const NATIVE_INT_TYPE portNum, /*!< The port number*/
            U32 code                       /*!< Watchdog stroke code*/
        );

        PRIVATE :

            // ----------------------------------------------------------------------
            // Command handler implementations
            // ----------------------------------------------------------------------

            //! Implementation for Reset Specific command handler
            //! Command to reset the specific parts of rover
            void
            Reset_Specific_cmdHandler(
                const FwOpcodeType opCode,        /*!< The opcode*/
                const U32 cmdSeq,                 /*!< The command sequence number*/
                reset_values_possible reset_value /*!<
                              ENUM Value that specifies which components or hardware need to be reset
                          */
            );

        //! Implementation for Disengage_From_Lander command handler
        //! Command to send signal to MSP430 that it should send a signal to lander to disengage, sets disengage pin high
        void Disengage_From_Lander_cmdHandler(
            const FwOpcodeType opCode, /*!< The opcode*/
            const U32 cmdSeq,          /*!< The command sequence number*/
            confirm_disengage confirm);

        //! Implementation for Engage_From_Lander command handler
        //! Command to send signal to MSP430 that it should send a signal to lander to engage, sets disengage pin low
        void Engage_From_Lander_cmdHandler(
            const FwOpcodeType opCode, /*!< The opcode*/
            const U32 cmdSeq           /*!< The command sequence number*/
        );

        /* Commands that Only Watchdog Processes */

        //! Handler for command Prepare_For_Deployment
        /* Command to send signal to MSP430 to prepare for deploying (may not be needed) */
        void Prepare_For_Deployment_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            confirm_prepare_for_deploy confirm);

        //! Handler for command Switch_Connection_Mode
        /* Command to send signal to MSP430 that we switch the current connection mode. NOTE: This is currently deprecated behavior. Watchdog now sends data to all available and active interfaces in any given state. */
        void Switch_Connection_Mode_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            watchdog_connection_mode mode);

        //! Handler for command Set_Debug_Comms_State
        /* Turn Watchdog DEBUG comms messages ON or OFF (should default to ON). */
        void Set_Debug_Comms_State_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            confirm_change_debug confirm,
            debug_comms_state state);

        //! Handler for command Set_Auto_Heater_On_Value
        /* Set the ON threshold for the auto heater controller on the Watchdog. */
        void Set_Auto_Heater_On_Value_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            U16 on);

        //! Handler for command Set_Auto_Heater_Off_Value
        /* Set the OFF threshold for the auto heater controller on the Watchdog. */
        void Set_Auto_Heater_Off_Value_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            U16 off);

        //! Handler for command Set_Heater_Duty_Cycle
        /* Set the PWM duty cycle of the auto heater controller on the Watchdog. */
        void Set_Heater_Duty_Cycle_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            U16 duty);

        //! Handler for command Set_Heater_Duty_Cycle_Period
        /* Set the PWM period of the auto heater controller on the Watchdog. */
        void Set_Heater_Duty_Cycle_Period_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            U16 period);

        //! Handler for command Set_VSAE_State
        /* Tells the Watchdog to manually set the state for the V_SYS_ALL_ENABLE line on the BLiMP. */
        void Set_VSAE_State_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            confirm_vsae_change_enum confirm,
            vsae_state state);

        //! Handler for command Switch_to_Sleep_Mode
        /* Command to send signal to MSP430 that it should go into Sleep Mode */
        void Switch_to_Sleep_Mode_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            confirm_sleep_mode confirm);

        //! Handler for command Switch_to_Keep_Alive_Mode
        /* Command to send signal to MSP430 that it should go into Keep Alive Mode */
        void Switch_to_Keep_Alive_Mode_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            confirm_alive_mode confirm);

        //! Handler for command Switch_to_Service_Mode
        /* Command to send signal to MSP430 that it should go into Service Mode */
        void Switch_to_Service_Mode_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            confirm_service_mode confirm);

        //! Handler for command Clear_Reset_Memory
        /* Clear the reset memory used in the Watchdog's Detailed Status Report. */
        void Clear_Reset_Memory_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            confirm_clear_reset_memory_1 confirm_1,
            confirm_clear_reset_memory_2 confirm_2);

        //! Handler for command DANGEROUS_Force_Battery_State_DANGEROUS
        /* **DANGEROUS**: Tells the Watchdog to set the BLiMP's BSTAT pin (normally an input) to OUTPUT the given value. This is designed to be used as a last ditch effort to connect the batteries in case any of the components driving BSTAT die. If they aren't dead, this will have the effect of blowing up the BSTAT circuitry and maybe a port on the Watchdog if not the whole Watchdog. This is **ONLY** to be used if the Mission will be over if you don't. You've got to be really sure you want to do this. */
        void DANGEROUS_Force_Battery_State_DANGEROUS_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            dangerous_confirm_force_bstat_enum_1 confirm_1,
            dangerous_confirm_force_bstat_enum_2 confirm_2,
            bstat_state state);

        //! Handler for command Request_Status_Report
        /* Request the Watchdog to send a Detailed Status Report. */
        void Request_Status_Report_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            confirm_status_request confirm);

        //! Handler for command Set_Charger_Enable
        /* Manually set charging IC enable state: CE. (normally you should just use the start and stop charging commands in reset specific.) */
        void Set_Charger_Enable_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            charge_en_states charge_en);

        //! Handler for command Set_Charger_Power_Connection
        /* Manually set charging power 28V regulator enable state: REGE. (normally you should just use the start and stop charging commands in reset specific.) */
        void Set_Charger_Power_Connection_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            bool v_lander_reg_en);

        //! Handler for command Set_Battery_Connection
        /* Manually set battery connection state: BE. (normally you should just use the batteries enable/disable command in reset specific.) */
        void Set_Battery_Connection_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            bool batt_en);

        //! Handler for command Set_Battery_Control_Enable
        /* Manually set the state of the battery control circuitry: BCTRLE. On Iris FM1 this line (should be) disconnected so this *should effectively be a no-op. To be used if the engineers believe this connection may have reformed somehow. */
        void Set_Battery_Control_Enable_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            batt_ctrl_en_states batt_ctrl_en);

        //! Handler for command Set_Battery_Latch
        /* Manually set battery latch state: LB. (normally you should just use the batteries enable/disable command in reset specific.) */
        void Set_Battery_Latch_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            latch_batt_states latch_batt);

        //! Handler for command Set_Latch_Set
        /* Control the battery latch "SET" override. This line *should* be severed on Iris FM1, so this *should* effectively be a no-op. */
        void Set_Latch_Set_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            latch_set_states latch_set);

        //! Handler for command Set_Latch_Reset
        /* Control the battery latch "RESET" override. This line *should* be severed on Iris FM1, so this *should* effectively be a no-op. */
        void Set_Latch_Reset_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            latch_reset_states latch_reset);

        //! Handler for command Echo
        /* Echo the given fixed length string (technically can send a string of any length up to the max length). */
        void Echo_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            U8 length,
            const Fw::CmdStringArg &message /*!< Message for the Watchdog to Echo back to us.*/
        );

        /* End of Commands that Only Watchdog Processes*/

        // The Header Frame structure sent and received between Hercules and Watchdog
        struct WatchdogFrameHeader
        {
            uint32_t magic_value : 24; // 24 bit magic value that is constant and sent at the begining of every send
            uint32_t parity : 8;       // 8 bit parity of whole header, initially calculated with parity equal to 0
            uint16_t payload_length;   // The length of the payload being sent
            uint16_t reset_val;        // The reset value being sent to watchdog
            uint16_t sequence_number;  // The lower 16 bits of the fprime command sequence number
            uint16_t opcode;           // The lower 16 bits of the fprime command opcode
        } __attribute__((packed, aligned(8)));

        struct WatchdogTelemetry
        {
            int16_t voltage_2V5;       // Current measured voltage of the 2.5V line as read from the Watchdog
            int16_t voltage_2V8;       // Current measured voltage of the 2.8V line as read from the Watchdog
            int16_t voltage_24V;       // Current measured voltage of the 24V line as read from the Watchdog
            int16_t voltage_28V;       // Current measured voltage of the 28V line as read from the Watchdog
            int8_t battery_thermistor; // Current measured voltage of the watchdog battery thermistor as read from the Watchdog
            int8_t sys_status;         // 8 bit systems status where each bit represents a watchdog status
            int16_t battery_level;     // Current measured battery mAH as read from the Watchdog
            int16_t battery_current;   // Current measured battery current as read from the Watchdog
            int16_t battery_voltage;   // Voltage measured battery current as read from the Watchdog
        } __attribute__((packed, aligned(8)));

        // Incorrect Response Possible Values
        enum resp_error : U8
        {
            bad_parity = 1,        // Error code for response having bad parity value
            bad_size_received = 2, // Error code for response having bad data size received
            bad_reset_value = 3,   // Error code for response having a bad reset value
            bad_magic_value = 4,   // Error code for response having a bad magic value
            not_enough_bytes = 5   // Error code for response not having enough bytes (must be over min_receive_size)
        };

        enum disengage_command : U16
        {
            Disengage = 0x00EE // Reset value for the disengagement command sent to Watchdog
        };

        /**
         * The opcode sent in the header with stroke messages to the MSP430 watchdog.
         */
        static const uint16_t STROKE_OPCODE = 0x0100u;

        /**
         * The opcode sent in the header with downlink messages to the MSP430 watchdog.
         */
        static const uint16_t DOWNLINK_OPCODE = 0x0101u;

        /**
         * The opcode send in the header with uplink messages from the MSP430 watchdog.
         */
        static const uint16_t UPLINK_OPCODE = 0x0102u;

        /**
         * The opcode send in the header with debug log messages to the MSP430 watchdog.
         */
        static const uint16_t DEBUG_OPCODE = 0x0103u;

        /**
         * The opcode sent in the header with downlink messages from the MSP430 watchdog.
         */
        static const uint16_t DOWNLINK_TO_WIFI_OPCODE = 0x0107u;

        /**
         * The indices in the TxCommandArray of the TxCommandStatus for each type of command that is sent to the
         * MSP430 watchdog.
         */
        enum TxCommandIndex
        {
            COMMAND_INDEX__STROKE = 0,
            COMMAND_INDEX__DOWNLINK = 1,
            COMMAND_INDEX__RESET_SPECIFIC = 2,
            COMMAND_INDEX__DISENGAGE_FROM_LANDER = 3,
            COMMAND_INDEX__ENGAGE_FROM_LANDER = 4,
            COMMAND_INDEX__NUM_COMMANDS = 5
        };

        /**
         * Holds the status of a command transmitted by this WatchDogInterface to the MSP430 watchdog.
         *
         * The fields of this structure have been selected so that we can send a response to the CmdDispatcher
         * after we receive a response for that command from the MSP430 watchdog. We determine if a given response
         * is for a particular command based on the opcode and sequence number that is included in the response.
         */
        struct TxCommandStatus
        {
            /**
             * The opcode, EXCLUDING the ID offset (i.e. not the value passed to cmdHandler's, instead that value
             * minus the baseId of the base component), of the transmitted command.
             */
            FwOpcodeType opcode;
            U32 seqNum;            //!< The sequence number of the transmitted command.
            uint32_t txTimeMillis; //!< The time (from Fw::Time) that the command was transmitted, in milliseconds.
            bool active;           //!< If true, the transmitted command has not yet received a response from the watchdog.

            /**
             * Whether or not we want to send a response to the CmdDispatcher after a response is received from the
             * watchdog.
             */
            bool sendResponse;

            TxCommandStatus() : opcode(0)
            {
                reset();
            }

            void reset()
            {
                seqNum = 0;
                txTimeMillis = 0;
                active = false;
                sendResponse = true;
            }
        };

        /**
         * An array of TxCommandStatus structs, used to track the status of the last transmitted command for all
         * commands types that are sent to the MSP430 watchdog.
         *
         * Additionally, contains a mutex that should be locked anytime anything in the `commands` field is read or
         * modified.
         */
        struct TxCommandArray
        {
            TxCommandStatus commands[COMMAND_INDEX__NUM_COMMANDS];
            ::Os::Mutex cmdMutex;
        };

        bool Read_Temp();    // Checking the temperature sensors from the ADC
        bool Read_Current(); // Checking the current sensors from the ADC

        void pollDMASendFinished(); // Polls DMA send finish to see if we've finished our DMA sending

        // Perform a DMA send function
        bool dmaSend(void *buffer,          // The buffer of data to send to watchdog
                     int size,              // The size of the data to send to watchdog
                     bool blocking = true); // Check variable to see if we need to block other DMA requests

        /**
         * @brief Logs an event about sending this Reset_specific command, then invokes `sendResetSpecific`.
         *
         * @param opCode The parameter, excluding the base ID (i.e. without the offset specified in Top), of the
         *               command that is sending the reset specific command.
         * @param cmdSeq The sequence number of the command to be transmitted.
         * @param resetValue The reset value of the reset specific command.
         * @param sendResponse Whether or not to send a response to the cmdDispatcher when a response for the
         *                     sent command is received from the MSP430 watchdog.
         *
         * @return true if transmitting the command succeeded, otherwise false.
         */
        bool logAndSendResetSpecific(FwOpcodeType opCode,
                                     U32 cmdSeq,
                                     reset_values_possible resetValue,
                                     bool sendResponse);

        /**
         * Transmits the "reset specific" command to the MSP430.
         *
         * If unable to transmit the command, this function wills send a response to the CmdDispatcher indicating
         * the command failed due to Fw::COMMAND_EXECUTION_ERROR.
         *
         * @param opCode The parameter, excluding the base ID (i.e. without the offset specified in Top), of the
         *               command that is sending the reset specific command.
         * @param cmdSeq The sequence number of the command to be transmitted.
         * @param resetValue The reset value of the reset specific command.
         * @param sendResponse Whether or not to send a response to the cmdDispatcher when a response for the
         *                     sent command is received from the MSP430 watchdog.
         *
         * @return true if transmitting the command succeeded, otherwise false.
         */
        bool sendResetSpecific(FwOpcodeType opCode,
                               U32 cmdSeq,
                               reset_values_possible resetValue,
                               bool sendResponse);

        /**
         * Standard handler for any commands that only the WatchDog MSP430
         * processes.
         *
         * Currently, this function only sends a response to the CmdDispatcher
         * indicating the command couldn't be executed (b/c this isn't the
         * MSP430) due to Fw::COMMAND_EXECUTION_ERROR.
         *
         * @param opCode The parameter, excluding the base ID (i.e. without the offset specified in Top), of the
         *               command that is sending the reset specific command.
         * @param cmdSeq The sequence number of the command to be transmitted.
         */
        void handleWatchDogOnlyCommand(FwOpcodeType opCode,
                                       U32 cmdSeq);

        /**
         * @brief Handles a message containing uplink data received from the MSP430 watchdog.
         *
         * Handling this message involves sending the received uplink data out the uplink port.
         *
         * @msg The received uplink message.
         */
        void handleUplinkMsg(WatchDogMpsm::Message &msg);
        void handleDownlinkMsg(WatchDogMpsm::Message &msg);

        /**
         * @brief Handles a message containing telemetry data received from the MSP430 watchdog.
         *
         * Handling this message involves updating the telemetry with the values from the received message.
         *
         * @msg The received telemetry message.
         */
        void handleTelemetryMsg(WatchDogMpsm::Message &msg);

        /**
         * @brief Gets the TxCommandStatus structure (or rather, a pointer to it) from the TxCommandArray based on the
         *        full opcode given.
         *
         * The opcode parameter is "full" only in the sense that it's type is FwOpcodeType, which may be wider than
         * the uint16_t values that are transmitted in the header of messages to/from the MSP430 watchdog. However,
         * this opcode should not contain the base offset. This function simply casts the given value to a uint16_t
         * then invokes `getTxCommandStatus`.
         *
         * @param opCode The "full" opcode used to determine the TxCommandStatus to return
         *
         * @return A pointer to the appropriate TxCommandStatus for the given opcode, or nullptr if an appropriate
         *         TxCommandStatus was not found.
         */
        TxCommandStatus *getTxCommandStatusFullOpcode(FwOpcodeType opCode);

        /**
         * @brief Gets the TxCommandStatus structure (or rather, a pointer to it) from the TxCommandArray based on the
         *        opcode given.
         *
         * The given opcode should not contain the base offset.
         *
         * @param opCode The opcode used to determine the TxCommandStatus to return.
         *
         * @return A pointer to the appropriate TxCommandStatus for the given opcode, or nullptr if an appropriate
         *         TxCommandStatus was not found.
         */
        TxCommandStatus *getTxCommandStatus(uint16_t opCode);

        /**
         * @brief Transmits the specified command to the MSP430 watchdog.
         *
         * The command to be transmitted is constructed by populating the header with the lower 16 bits of the opcode
         * and sequence number, as well as the reset value and the data length. From these a parity is calculated,
         * and that is the last field needed in the header. If any data is passed, it will be transmitted following
         * the header.
         *
         * Prior to being transmitted, we check if a command of this type is already active (i.e., waiting for a
         * response from the MSP430 watchdog). If the previously transmitted command was transmitted so long ago
         * (based on `COMMAND_TIMEOUT_MILLISECONDS`) that it is considered timed out, then this function will
         * clear the TxCommandStatus structure and send a response for the previously transmitted command to
         * the CmdDispatcher (unless the previously transmitted command was sent with `sendResponse` set to false).
         * However, if the previously transmitted command hasn't timed out, we will return from this function
         * without sending the current command. If this function sends the command successfully, it will then
         * update the TxCommandStatus structure for this command type to indicate that we've sent this command
         * and that we're now waiting for a response.
         *
         * @param opCode The opcode of the command being transmitted (excluding any base ID offset).
         * @param cmdSeq The command number of the command being transmitted.
         * @param resetValue The reset value to pass in the command being transmitted.
         * @param dataBuffer The buffer containing the payload of the message to be transmitted, if there is any.
         *                   This field can be nullptr so long as `dataLen` is zero.
         * @param dataLen The length of the payload data in `dataBuffer`.
         * @param sendResponse Whether or not we want to send a response for this command to the CmdDispatcher
         *                     once we get a response from the MSP430 for this command.
         *
         * @return Whether or not the command was successfully transmitted.
         */
        bool txCommand(FwOpcodeType opCode,
                       U32 cmdSeq,
                       uint16_t resetValue,
                       uint8_t *dataBuffer = nullptr,
                       size_t dataLen = 0,
                       bool sendResponse = true);

    public:
        bool debugPrintfToWatchdog(const char *fmt, ...);
        // Sends the given buffer as a debug printf message to the watchdog. Be careful with the size on this one.
        bool debugPrintfBuffer(uint8_t *buffer, size_t bufferLen);
        // Same as `debugPrintfBuffer` but appends a prefix buffer before the main buffer (helpful for labelling messages):
        bool debugPrintfBufferWithPrefix(uint8_t *prefixBuffer, size_t prefixBufferLen, uint8_t *buffer, size_t bufferLen);
        PRIVATE :

            sciBASE_t *m_sci;                               // The sci base used to initialize the watchdog interface connection
        adcData_t m_thermistor_buffer[number_thermistors];  // Location to store current data for thermistors
        adcData_t m_current_buffer[number_current_sensors]; // Location to store current data for current sensors
        bool m_finished_initializing;                       // Flag set when this component is fully initialized and interrupt DMA can be used (otherwise polling DMA)

        /**
         * The array of structures that allow us to track the status of previously transmitted commands.
         */
        TxCommandArray m_txCmdArray;

        /**
         * The task that handles receiving messages from the MSP430 watchdog.
         */
        WatchDogRxTask m_rxTask;

        uint16_t m_downlinkSequenceNumber;

        char m_printBuffer[256];

        uint32_t m_skippedStrokes;
        uint32_t m_missedStrokeResponses;

        // Time we last read the temperature sensors:
        uint32_t m_lastThermistorReadTime;
        // Time we last read the current sensors
        uint32_t m_lastCurrentReadTime;

        // Mutex allowing external agents to safely access IMU data while
        // circumventing FPrime:
        ::Os::Mutex m_extDataMutex;
        // Protected, externally accessible data:
        // Voltage28V, raw ADC reading:
        int16_t m_extVoltage28VRaw;

    public:
        // Getters and Setters for Mutex-protected data:
        void setExt28VRaw(int16_t voltage);
        int16_t getExt28VRaw();
    };

    // Define empty Friend class (which autogenerated and uneditable C++ `WatchDogInterfaceComponentBase` Class already friends)
    // so NetworkManager and Camera can access protected & private enums from FPrime XML by inheriting `WatchDogInterfaceComponentBaseFriend`:
    class WatchDogInterfaceComponentBaseFriend
    {
    public:
        // Grab any members we want from `WatchDogInterfaceComponentBase`:
        // ID to pass to WatchDog ResetSpecific to reset the radio:
        static const uint8_t WD_RADIO_RESET_ID = WatchDogInterfaceComponentBase::reset_values_possible::Reset_Radio; // 0x04;
        // ID to pass to WatchDog ResetSpecific to reset the Hercules:
        static const uint8_t WD_HERCULES_RESET_ID = WatchDogInterfaceComponentBase::reset_values_possible::Reset_Hercules; // 0x01;

        // IDs to trigger a camera select:
        static const uint8_t WD_CAM_SEL_0_RESET_ID = WatchDogInterfaceComponentBase::reset_values_possible::FPGA_Cam_0;
        static const uint8_t WD_CAM_SEL_1_RESET_ID = WatchDogInterfaceComponentBase::reset_values_possible::FPGA_Cam_1;

        // IDs for motor control:
        static const uint8_t WD_ALL_MOTORS_ON_RESET_ID = WatchDogInterfaceComponentBase::reset_values_possible::All_Motors_On;
        static const uint8_t WD_ALL_MOTORS_OFF_RESET_ID = WatchDogInterfaceComponentBase::reset_values_possible::All_Motors_Off;
    };

} // end namespace CubeRover

#endif
