// ======================================================================
// \title  WatchDogInterfaceComponentImpl.cpp
// \author alec, justin, mschnur, cwcolomb
// \brief  cpp file for WatchDogInterface component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <CubeRover/WatchDogInterface/WatchDogInterface.hpp>
#include <Drv/FreeRtosSerialDriver/FreeRtosSerialDriverComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <stdio.h>
#include <string.h>
#include <cassert>

#include "Include/FswPacket.hpp"
#include "Include/Version.h"
#include "sys_dma.h"
#include <App/DMA.h>
#include "gio.h"
#include "reg_spi.h"
#include <HAL/include/FreeRTOS.h>
#include <HAL/include/os_task.h>
#include <HAL/include/os_portmacro.h>
#include <Os/Mutex.hpp>

static volatile bool dmaWriteBusy = false;

extern "C" void SCILIN_TX_DMA_ISR(dmaInterrupt_t inttype)
{
    dmaWriteBusy = false;
}

namespace CubeRover
{

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    WatchDogInterfaceComponentImpl ::
#if FW_OBJECT_NAMES == 1
        WatchDogInterfaceComponentImpl(
            const char *const compName) : WatchDogInterfaceComponentBase(compName)
#else
        WatchDogInterfaceComponentImpl(void)
#endif
                                          ,
                                          m_sci(scilinREG), m_finished_initializing(false), m_txCmdArray(), m_rxTask(), m_downlinkSequenceNumber(0), m_skippedStrokes(0), m_missedStrokeResponses(0), m_lastThermistorReadTime(0), m_lastCurrentReadTime(0)
    {
        m_txCmdArray.commands[COMMAND_INDEX__STROKE].opcode = static_cast<FwOpcodeType>(STROKE_OPCODE);
        m_txCmdArray.commands[COMMAND_INDEX__DOWNLINK].opcode = static_cast<FwOpcodeType>(DOWNLINK_OPCODE);
        m_txCmdArray.commands[COMMAND_INDEX__RESET_SPECIFIC].opcode =
            static_cast<uint16_t>(WatchDogInterfaceComponentBase::OPCODE_RESET_SPECIFIC);
        m_txCmdArray.commands[COMMAND_INDEX__DISENGAGE_FROM_LANDER].opcode =
            static_cast<uint16_t>(WatchDogInterfaceComponentBase::OPCODE_DISENGAGE_FROM_LANDER);
        m_txCmdArray.commands[COMMAND_INDEX__ENGAGE_FROM_LANDER].opcode =
            static_cast<uint16_t>(WatchDogInterfaceComponentBase::OPCODE_ENGAGE_FROM_LANDER);
    }

    void WatchDogInterfaceComponentImpl ::
        init(
            const NATIVE_INT_TYPE queueDepth,
            const NATIVE_INT_TYPE instance)
    {
        WatchDogInterfaceComponentBase::init(queueDepth, instance);
        sciEnterResetState(m_sci);
        sciSetBaudrate(m_sci, 57600);
        sciExitResetState(m_sci);

        // Configure and up the receiving task
        m_rxTask.registerCallback(this);
        ::Os::Task::TaskStatus taskStat = m_rxTask.startTask(WATCH_DOG_INTERFACE_RX_TASK_PRIORITY,
                                                             WATCH_DOG_INTERFACE_RX_TASK_STACK_SIZE,
                                                             WATCH_DOG_INTERFACE_RX_TASK_CPU_AFFINITY);
        // Assert that this will always be started successfully. If it isn't, we're screwed.
        configASSERT(taskStat == Os::Task::TASK_OK);

        gioSetBit(spiPORT3, deploy_bit, 0);

        Read_Temp();
        Read_Current();

        // Let the Watchdog know we've booted, incl. current software version (useful for later Hercules Remote Programming):
        debugPrintfToWatchdog("Hercules Boot v.%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION);

        m_finished_initializing = true;
    }

    WatchDogInterfaceComponentImpl ::
        ~WatchDogInterfaceComponentImpl(void)
    {
        // Do Not Use
    }

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    // Timed function that runs every 1Hz
    void WatchDogInterfaceComponentImpl ::
        Run_handler(
            const NATIVE_INT_TYPE portNum,
            NATIVE_UINT_TYPE context)
    {
        static uint16_t sequenceNumber = 0;
        static uint32_t lastFailedStrokeMsgSendTime = 0;

        Fw::Time now = getTime();
        uint32_t nowMillis = static_cast<uint32_t>(now.get_time_ms());

        if ((nowMillis - m_lastThermistorReadTime) > ADC_THERMISTOR_READ_PERIOD_MS)
        {
            // Update Thermistor Telemetry no more frequently than `ADC_THERMISTOR_READ_PERIOD_MS`:
            m_lastThermistorReadTime = nowMillis;
            Read_Temp();
        }

        if ((nowMillis - m_lastCurrentReadTime) > ADC_CURRENT_READ_PERIOD_MS)
        {
            // Update Current Telemetry no more frequently than `ADC_CURRENT_READ_PERIOD_MS`:
            m_lastCurrentReadTime = nowMillis;
            Read_Current();
        }

        bool success = txCommand(STROKE_OPCODE, sequenceNumber, static_cast<uint16_t>(No_Reset), nullptr, 0, true);

        if (success)
        {
            sequenceNumber++;
        }
        else
        {
            // debugPrintfToWatchdog("Failed to send stroke\n");
            //  TODO: Add logging error
        }

        if (nowMillis - lastFailedStrokeMsgSendTime >= 10000)
        {
            if (debugPrintfToWatchdog("Missed responses: %u, skipped sends: %u\n",
                                      m_missedStrokeResponses, m_skippedStrokes))
            {
                lastFailedStrokeMsgSendTime = nowMillis;
            }
        }

        return;
    }

    void WatchDogInterfaceComponentImpl ::
        CompResetRequest_handler(
            const NATIVE_INT_TYPE portNum,
            CubeRoverPorts::ResetValue reset)
    {
        // Deprecated, not needed
    }

    void WatchDogInterfaceComponentImpl ::
        downlink_handler(
            const NATIVE_INT_TYPE portNum,
            Fw::Buffer &fwBuffer)
    {
        bool success = txCommand(DOWNLINK_OPCODE,
                                 m_downlinkSequenceNumber,
                                 static_cast<uint16_t>(No_Reset),
#pragma diag_push
#pragma diag_suppress 1107
                                 // This casts an integer to a pointer with a smaller size than the source integer,
                                 // and CCS warns about doing this. However, in this case it is necessary and should
                                 // be fine, so I disable the warning
                                 reinterpret_cast<uint8_t *>(fwBuffer.getdata()),
#pragma diag_pop
                                 static_cast<size_t>(fwBuffer.getsize()),
                                 true);

        if (success)
        {
            m_downlinkSequenceNumber++;
        }
        else
        {
            // TODO: Add logging error
        }

        if (static_cast<size_t>(fwBuffer.getsize()) > 650)
        {
            debugPrintfToWatchdog("fwBuffer has size %u\n", static_cast<size_t>(fwBuffer.getsize()));
        }

        return;
    }

    void WatchDogInterfaceComponentImpl ::
        PingIn_handler(
            const NATIVE_INT_TYPE portNum,
            U32 key)
    {
        this->PingOut_out(0, key);
    }

    void WatchDogInterfaceComponentImpl ::
        WdogStroke_handler(
            const NATIVE_INT_TYPE portNum,
            U32 code)
    {
        // Depricated, not used currently
    }

    // ----------------------------------------------------------------------
    // Command handler implementations
    // ----------------------------------------------------------------------

    // Function called by cmd handler to send a reset to watchdog
    void WatchDogInterfaceComponentImpl ::Reset_Specific_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        reset_values_possible reset_value)
    {
        bool success = logAndSendResetSpecific(opCode - this->getIdBase(), cmdSeq, reset_value, true);
        if (success)
        {
            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
        }
    }

    // Special function for other functions to call. Cannot have command response as not a command from ground
    bool WatchDogInterfaceComponentImpl ::
        Reset_Specific_Handler(
            int reset_enum_number)
    {
        static Os::Mutex sloppyResourceProtectionMutex; // quick and dirty. keeps multiple tasks from doing this at once.

        sloppyResourceProtectionMutex.lock();
        // Convert int into reset_values_possible
        reset_values_possible reset_value = (reset_values_possible)reset_enum_number;
        // Check that reset_value is correct
        if (reset_value >= reset_values_possible_MAX)
        {
            this->log_WARNING_LO_WatchDogIncorrectResetValue();
            sloppyResourceProtectionMutex.unLock();
            return false;
        }
        // Send command to watchdog, put 0 for opcode and cmdseq
        bool success = logAndSendResetSpecific(WatchDogInterfaceComponentBase::OPCODE_RESET_SPECIFIC, 0, reset_value, false);

        sloppyResourceProtectionMutex.unLock();
        return success;
    }

    void WatchDogInterfaceComponentImpl ::
        Disengage_From_Lander_cmdHandler(
            const FwOpcodeType opCode,
            const U32 cmdSeq,
            confirm_disengage confirm)
    {
        // Send Activity Log/tlm to know watchdog received command
        char command_type[24] = "Disengage From Rover";
        Fw::LogStringArg command_type_log = command_type;
        this->log_ACTIVITY_HI_WatchDogCmdReceived(command_type_log);

        bool success = sendResetSpecific(opCode - this->getIdBase(),
                                         cmdSeq,
                                         static_cast<reset_values_possible>(Disengage),
                                         true);

        debugPrintfToWatchdog("Hercules Triggering Deployment Interlock...");
        // Set Deployment Bit High
        // Deployment2 signal is on MIBSPI3NCS_4 which is setup as a GPIO pin with default 0 and no pull up/down resistor.
        // Use Bit 5 as MIBSPI3NCS_4 is the 5th (start at 0) pin from the start of SPI3 Port
        gioSetBit(spiPORT3, deploy_bit, 1);
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

    void WatchDogInterfaceComponentImpl ::
        Engage_From_Lander_cmdHandler(
            const FwOpcodeType opCode,
            const U32 cmdSeq)
    {
        // Send Activity Log/tlm to know watchdog received command
        char command_type[24] = "Engage From Rover";
        Fw::LogStringArg command_type_log = command_type;
        this->log_ACTIVITY_HI_WatchDogCmdReceived(command_type_log);

        bool success = sendResetSpecific(opCode - this->getIdBase(), cmdSeq, HDRM_Off, true);

        if (success)
        {
            // Set Deployment Bit low
            // Deployment2 signal is on MIBSPI3NCS_4 which is setup as a GPIO pin with default 0 and no pull up/down resistor.
            // Use Bit 5 as MIBSPI3NCS_4 is the 5th (start at 0) pin from the start of SPI3 Port
            gioSetBit(spiPORT3, deploy_bit, 0);
            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
        }
    }

    bool WatchDogInterfaceComponentImpl::logAndSendResetSpecific(FwOpcodeType opCode,
                                                                 U32 cmdSeq,
                                                                 reset_values_possible resetValue,
                                                                 bool sendResponse)
    {
        static char command_type[24] = "Reset Specific:";
        static Os::Mutex sloppyResourceProtectionMutex; // quick and dirty. keeps multiple tasks from doing this at once.

        sloppyResourceProtectionMutex.lock();
        // Send Activity Log/tlm to know watchdog received command:
        char reset_val_char[8];
        sprintf(reset_val_char, "%u", resetValue);
        strcat(command_type, reset_val_char);
        Fw::LogStringArg command_type_log = command_type;
        this->log_ACTIVITY_HI_WatchDogCmdReceived(command_type_log);

        sloppyResourceProtectionMutex.unLock();
        return sendResetSpecific(opCode, cmdSeq, resetValue, sendResponse);
    }

    bool WatchDogInterfaceComponentImpl::sendResetSpecific(FwOpcodeType opCode,
                                                           U32 cmdSeq,
                                                           reset_values_possible resetValue,
                                                           bool sendResponse)
    {
        // Check that reset_value is correct
        if (resetValue >= reset_values_possible_MAX)
        {
            this->log_WARNING_LO_WatchDogIncorrectResetValue();
            return false;
        }

        bool success = txCommand(opCode, cmdSeq, static_cast<uint16_t>(resetValue), nullptr, 0, sendResponse);

        if (!success && sendResponse)
        {
            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
        }

        return success;
    }

    /* Commands that Only Watchdog Processes */

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
    void WatchDogInterfaceComponentImpl::handleWatchDogOnlyCommand(FwOpcodeType opCode, U32 cmdSeq)
    {
        // TODO (consider fwd'ing these to WD somehow...).
        // Would need to account for vargs and would need Matt Schnur's help
        // passing the data through the stroking protocol.
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
    }

    //! Handler for command Prepare_For_Deployment
    /* Command to send signal to MSP430 to prepare for deploying (may not be needed) */
    void WatchDogInterfaceComponentImpl::Prepare_For_Deployment_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        confirm_prepare_for_deploy confirm)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Switch_Connection_Mode
    /* Command to send signal to MSP430 that we switch the current connection mode. NOTE: This is currently deprecated behavior. Watchdog now sends data to all available and active interfaces in any given state. */
    void WatchDogInterfaceComponentImpl::Switch_Connection_Mode_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        watchdog_connection_mode mode)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Set_Debug_Comms_State
    /* Turn Watchdog DEBUG comms messages ON or OFF (should default to ON). */
    void WatchDogInterfaceComponentImpl::Set_Debug_Comms_State_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        confirm_change_debug confirm,
        debug_comms_state state)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Set_Auto_Heater_On_Value
    /* Set the ON threshold for the auto heater controller on the Watchdog. */
    void WatchDogInterfaceComponentImpl::Set_Auto_Heater_On_Value_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        U16 on)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Set_Auto_Heater_Off_Value
    /* Set the OFF threshold for the auto heater controller on the Watchdog. */
    void WatchDogInterfaceComponentImpl::Set_Auto_Heater_Off_Value_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        U16 off)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Set_Heater_Duty_Cycle
    /* Set the PWM duty cycle of the auto heater controller on the Watchdog. */
    void WatchDogInterfaceComponentImpl::Set_Heater_Duty_Cycle_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        U16 duty)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Set_Heater_Duty_Cycle_Period
    /* Set the PWM period of the auto heater controller on the Watchdog. */
    void WatchDogInterfaceComponentImpl::Set_Heater_Duty_Cycle_Period_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        U16 period)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Set_VSAE_State
    /* Tells the Watchdog to manually set the state for the V_SYS_ALL_ENABLE line on the BLiMP. */
    void WatchDogInterfaceComponentImpl::Set_VSAE_State_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        confirm_vsae_change_enum confirm,
        vsae_state state)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Switch_to_Sleep_Mode
    /* Command to send signal to MSP430 that it should go into Sleep Mode */
    void WatchDogInterfaceComponentImpl::Switch_to_Sleep_Mode_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        confirm_sleep_mode confirm)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Switch_to_Keep_Alive_Mode
    /* Command to send signal to MSP430 that it should go into Keep Alive Mode */
    void WatchDogInterfaceComponentImpl::Switch_to_Keep_Alive_Mode_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        confirm_alive_mode confirm)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Switch_to_Service_Mode
    /* Command to send signal to MSP430 that it should go into Service Mode */
    void WatchDogInterfaceComponentImpl::Switch_to_Service_Mode_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        confirm_service_mode confirm)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Clear_Reset_Memory
    /* Clear the reset memory used in the Watchdog's Detailed Status Report. */
    void WatchDogInterfaceComponentImpl::Clear_Reset_Memory_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        confirm_clear_reset_memory_1 confirm_1,
        confirm_clear_reset_memory_2 confirm_2)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command DANGEROUS_Force_Battery_State_DANGEROUS
    /* **DANGEROUS**: Tells the Watchdog to set the BLiMP's BSTAT pin (normally an input) to OUTPUT the given value. This is designed to be used as a last ditch effort to connect the batteries in case any of the components driving BSTAT die. If they aren't dead, this will have the effect of blowing up the BSTAT circuitry and maybe a port on the Watchdog if not the whole Watchdog. This is **ONLY** to be used if the Mission will be over if you don't. You've got to be really sure you want to do this. */
    void WatchDogInterfaceComponentImpl::DANGEROUS_Force_Battery_State_DANGEROUS_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        dangerous_confirm_force_bstat_enum_1 confirm_1,
        dangerous_confirm_force_bstat_enum_2 confirm_2,
        bstat_state state)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Request_Status_Report
    /* Request the Watchdog to send a Detailed Status Report. */
    void WatchDogInterfaceComponentImpl::Request_Status_Report_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        confirm_status_request confirm)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Set_Charger_Enable
    /* Manually set charging IC enable state: CE. (normally you should just use the start and stop charging commands in reset specific.) */
    void WatchDogInterfaceComponentImpl::Set_Charger_Enable_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        charge_en_states charge_en)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Set_Charger_Power_Connection
    /* Manually set charging power 28V regulator enable state: REGE. (normally you should just use the start and stop charging commands in reset specific.) */
    void WatchDogInterfaceComponentImpl::Set_Charger_Power_Connection_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        bool v_lander_reg_en)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Set_Battery_Connection
    /* Manually set battery connection state: BE. (normally you should just use the batteries enable/disable command in reset specific.) */
    void WatchDogInterfaceComponentImpl::Set_Battery_Connection_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        bool batt_en)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Set_Battery_Control_Enable
    /* Manually set the state of the battery control circuitry: BCTRLE. On Iris FM1 this line (should be) disconnected so this *should effectively be a no-op. To be used if the engineers believe this connection may have reformed somehow. */
    void WatchDogInterfaceComponentImpl::Set_Battery_Control_Enable_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        batt_ctrl_en_states batt_ctrl_en)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Set_Battery_Latch
    /* Manually set battery latch state: LB. (normally you should just use the batteries enable/disable command in reset specific.) */
    void WatchDogInterfaceComponentImpl::Set_Battery_Latch_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        latch_batt_states latch_batt)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Set_Latch_Set
    /* Control the battery latch "SET" override. This line *should* be severed on Iris FM1, so this *should* effectively be a no-op. */
    void WatchDogInterfaceComponentImpl::Set_Latch_Set_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        latch_set_states latch_set)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Set_Latch_Reset
    /* Control the battery latch "RESET" override. This line *should* be severed on Iris FM1, so this *should* effectively be a no-op. */
    void WatchDogInterfaceComponentImpl::Set_Latch_Reset_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        latch_reset_states latch_reset)
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    //! Handler for command Echo
    /* Echo the given fixed length string (technically can send a string of any length up to the max length). */
    void WatchDogInterfaceComponentImpl::Echo_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        U8 length,
        const Fw::CmdStringArg &message /*!< Message for the Watchdog to Echo back to us.*/
    )
    {
        this->handleWatchDogOnlyCommand(opCode, cmdSeq);
    }

    /* End of Commands that Only Watchdog Processes*/

    bool WatchDogInterfaceComponentImpl::Read_Temp()
    {
        // Start ADC Conversions for all thermistors
        adcStartConversion(adcREG1, adcGROUP1);

        // Check if all ADC Conversions are done
        // From testing tries ALMOST ALWAYS ~= 10 - 12  ==>  38 - 40 cycles to convert data.. OK to poll
        int tries = 135; // Num thermistors bumped from 6 to 16, so bumped this up from 50 accordingly
        while (--tries && !adcIsConversionComplete(adcREG1, adcGROUP1))
            ;

        if (tries == 0)
        {
            // Safety stop for conversion to prevent a hangup
            adcStopConversion(adcREG1, adcGROUP1);

            this->log_WARNING_HI_ADCThermistorError();
            return false;
        }
        else
        {
            // Conversion SHOULD end automatically once all ADC values have been converted but this should end it otherwise
            adcStopConversion(adcREG1, adcGROUP1);
            U32 num_conversions = adcGetData(adcREG1, adcGROUP1, m_thermistor_buffer);
            if (num_conversions >= number_thermistors)
            {
                this->tlmWrite_THERM_0(m_thermistor_buffer[0].value);
                this->tlmWrite_THERM_1(m_thermistor_buffer[1].value);
                this->tlmWrite_THERM_2(m_thermistor_buffer[2].value);
                this->tlmWrite_THERM_3(m_thermistor_buffer[3].value);
                this->tlmWrite_THERM_4(m_thermistor_buffer[4].value);
                this->tlmWrite_THERM_5(m_thermistor_buffer[5].value);
                this->tlmWrite_THERM_6(m_thermistor_buffer[6].value);
                this->tlmWrite_THERM_7(m_thermistor_buffer[7].value);
                this->tlmWrite_THERM_8(m_thermistor_buffer[8].value);
                this->tlmWrite_THERM_9(m_thermistor_buffer[9].value);
                this->tlmWrite_THERM_10(m_thermistor_buffer[10].value);
                this->tlmWrite_THERM_11(m_thermistor_buffer[11].value);
                this->tlmWrite_THERM_12(m_thermistor_buffer[12].value);
                this->tlmWrite_THERM_13(m_thermistor_buffer[13].value);
                this->tlmWrite_THERM_14(m_thermistor_buffer[14].value);
                this->tlmWrite_THERM_15(m_thermistor_buffer[15].value);
            }
            else
            {
                this->log_WARNING_HI_ADCThermistorError();
                return false;
            }
        }

        // Ensure a small min wait period before we could sample any other ADCs (e.g. for currents):
        // Must wait at least 2 ADC cycles between each read when using two ADCs with shared pins
        // (per pg.935 in TI SPNU514C).
        // The ADCs have shared pins, though we don't share them across groups. Just in case, we'll
        // add this wait.
        // Both ADCs are configured in HALCoGen with a cycle time of 100ns.
        // Since 16MHz clock = 62.5ns per clock, we need to wait 4 cycles:
        int waitCycles = 4;
        while (--waitCycles)
            ;

        return true;
    }

    bool WatchDogInterfaceComponentImpl::Read_Current()
    {
        // Start ADC Conversions for all thermistors
        adcStartConversion(adcREG2, adcGROUP1);

        // Check if all ADC Conversions are done
        int tries = 60; // Based on data used in Read_Temp. 50 was sufficient for 6 inputs, we're reading 7 here, so we'll max out at 60
        while (--tries && !adcIsConversionComplete(adcREG2, adcGROUP1))
            ;

        if (tries == 0)
        {
            // Safety stop for conversion to prevent a hangup
            adcStopConversion(adcREG2, adcGROUP1);

            this->log_WARNING_HI_ADCCurrentError();
            return false;
        }
        else
        {
            // Conversion SHOULD end automatically once all ADC values have been converted but this should end it otherwise
            adcStopConversion(adcREG2, adcGROUP1);
            U32 num_conversions = adcGetData(adcREG2, adcGROUP1, m_current_buffer);
            if (num_conversions >= number_current_sensors)
            {
                // Emit a Current Readings Report:
                log_ACTIVITY_HI_AdcCurrentSensorReadingsReport(
                    m_current_buffer[0].value, // CURRENT_3V3_FPGA
                    m_current_buffer[1].value, // CURRENT_3V3_RADIO
                    m_current_buffer[2].value, // CURRENT_3V3
                    m_current_buffer[3].value, // CURRENT_3V3_HERCULES
                    m_current_buffer[4].value, // CURRENT_1V2_HERCULES
                    m_current_buffer[5].value, // CURRENT_1V2_FPGA
                    m_current_buffer[6].value  // CURRENT_24V
                );
            }
            else
            {
                this->log_WARNING_HI_ADCCurrentError();
                return false;
            }
        }

        // Ensure a small min wait period before we could sample any other ADCs (e.g. for currents):
        // Must wait at least 2 ADC cycles between each read when using two ADCs with shared pins
        // (per pg.935 in TI SPNU514C).
        // The ADCs have shared pins, though we don't share them across groups. Just in case, we'll
        // add this wait.
        // Both ADCs are configured in HALCoGen with a cycle time of 100ns.
        // Since 16MHz clock = 62.5ns per clock, we need to wait 4 cycles:
        int waitCycles = 4;
        while (--waitCycles)
            ;

        return true;
    }

    void WatchDogInterfaceComponentImpl::rxCallback(WatchDogMpsm::Message &msg, bool goodParity)
    {
        // If the message failed its parity check we don't even try to match it against any previously
        // sent messages. Instead, just warn about this occurring. We'll rely on timeout to eventually
        // clear up the tx command status structure for whatever tx command resulted in this
        // message with bad parity (if it was even a command to begin with)
        if (!goodParity)
        {
            this->log_WARNING_HI_WatchDogIncorrectResp(bad_parity);
        }

        Fw::Time now = getTime();

        // Uplink messages are different (specifically, they aren't a response to a Hercules command)
        // so we handle them separately
        if (msg.parsedHeader.lowerOpCode == UPLINK_OPCODE)
        {
            handleUplinkMsg(msg);
            return;
        }

        // Downlink messages are different (specifically, they aren't a response to a Hercules command)
        // so we handle them separately
        if (msg.parsedHeader.lowerOpCode == DOWNLINK_TO_WIFI_OPCODE)
        {
            handleDownlinkMsg(msg);
            return;
        }

        // Try to get the transmit status structure for the received message
        TxCommandStatus *cmdStatus = getTxCommandStatus(msg.parsedHeader.lowerOpCode);

        if (NULL == cmdStatus)
        {
            // TODO: Log error (which error?)
            debugPrintfToWatchdog("NULL cmdStatus\n");
            return;
        }

        // If we got a message, but the tx command status structure indicates that it is inactive
        // (i.e. that there was no transmitted message that is waiting for a response) then we want
        // to log an error, but continue on anyway so that we handle the message if it's telemetry.
        // In this case, since the tx command status structure is already inactive there is no need
        // to change it.
        //
        // On the other hand, if the status structure indicates that it *is* active but the sequence
        // number in the status structure is different than the sequence number of the received message,
        // then our handling depends on the ordering of the sequence numbers. If the received sequence
        // number is greater than the sequence number in the status structure (not sure how this could
        // happen), then we will reset the status structure and send a response for it, but we'll also
        // log an error about the incorrect sequence number. If the received sequence number is less
        // than the sequence number in the status structure, then presumably what happened is that
        // a previous command of the same type timed out while waiting for a response from WD, and
        // Hercules sent another command of the same type. Getting this response now, for the previous
        // message, means that for some reason WD was delayed in responding. In this case, we will not
        // clear the status structure, as the hope is that the WD will send another response for the
        // subsequent command that was sent. We'll still log an error in this case as well, but since
        // we don't know the correct sequence number to respond for the older tx message we will not
        // send a response. In either case (a sequence number mismatch of either ordering) we will
        // still handle the message (i.e. we'll process it if it is telemetry).
        //
        // In the nominal case, where everything in this response is what we would expect given the
        // contents of the tx status structure, we'll send a response for the tx command in question,
        // reset the tx status structure, and then handle the message (if it's telemetry).
        //
        // In all three of the above cases, any tx command structure manipulation will happen in the
        // section below in which the mutex is locked. However, any logging, responding to any
        // existing command, or handling of the message will be delayed until after the locked section.

        bool cmdInactive = false;
        bool rxOlderSeqNum = false;
        bool rxNewerSeqNum = false;
        FwOpcodeType txCmdOpCode = 0;
        U32 txCmdSeqNum = 0;
        bool txCmdSendResponse = false;
        uint32_t txTimeMillis = 0;

        { // The mutex must be unlocked before we exit this scope
            m_txCmdArray.cmdMutex.lock();

            if (!cmdStatus->active)
            {
                // Inactive tx command
                cmdInactive = true;
            }
            else
            {
                // Store the contents of the tx command structure prior to possible reset and so we can use
                // them outside of this block
                txCmdOpCode = cmdStatus->opcode;
                txCmdSeqNum = cmdStatus->seqNum;
                txCmdSendResponse = cmdStatus->sendResponse;
                txTimeMillis = cmdStatus->txTimeMillis;

                uint16_t ushortTxSeqNum = static_cast<uint16_t>(txCmdSeqNum);

                if (msg.parsedHeader.lowerSeqNum == ushortTxSeqNum)
                {
                    // Matching sequence numbers. Just need to reset the tx status structure
                    cmdStatus->reset();
                }
                else if (msg.parsedHeader.lowerSeqNum < ushortTxSeqNum)
                {
                    // Received sequence number is less than (older than) the one in the tx status structure
                    rxOlderSeqNum = true;
                }
                else
                {
                    // Received sequence number is greater than (newer than) the one in the tx status structure
                    rxNewerSeqNum = true;
                    cmdStatus->reset();
                }
            }

            m_txCmdArray.cmdMutex.unLock();
        }

        if (cmdInactive)
        {
            // TODO: LOG ERROR (which error to log?)
            debugPrintfToWatchdog("cmdInactive: %d\n", msg.parsedHeader.lowerOpCode);
        }
        else if (rxOlderSeqNum)
        {
            // TODO: LOG ERROR (which error to log?)
            debugPrintfToWatchdog("rxOlderSeqNum: %d\n", msg.parsedHeader.lowerSeqNum);
        }
        else if (rxNewerSeqNum)
        {
            // TODO: LOG ERROR (which error to log?)
            debugPrintfToWatchdog("rxNewerSeqNum: %d\n", msg.parsedHeader.lowerSeqNum);

            // We want to respond to the old tx message. Make sure we don't try to send a response about any
            // of our fake opcodes, and don't send a response if we didn't want to send one when we sent the message
            if (txCmdOpCode != STROKE_OPCODE && txCmdOpCode != DOWNLINK_OPCODE && txCmdSendResponse)
            {
                this->cmdResponse_out(txCmdOpCode, txCmdSeqNum, Fw::COMMAND_EXECUTION_ERROR);
            }
        }
        else
        {
            if (txCmdOpCode == STROKE_OPCODE)
            {
                uint32_t nowMillis = static_cast<uint32_t>(now.get_time_ms());
                debugPrintfToWatchdog("RTT: %u ms\n", nowMillis - txTimeMillis);
            }

            // We want to respond positively about the tx message. Make sure we don't try to send a response about any
            // of our fake opcodes, and don't send a response if we didn't want to send one when we sent the message
            if (txCmdOpCode != STROKE_OPCODE && txCmdOpCode != DOWNLINK_OPCODE && txCmdSendResponse)
            {
                this->cmdResponse_out(txCmdOpCode, txCmdSeqNum, Fw::COMMAND_OK);
            }
        }

        // Now we want to handle this message if it contains telemetry. For Reset_Specific and Downlink,
        // we're only looking for an ACK so we don't need to worry about doing anything with the responses for
        // those. However, the responses for the strokes will contain telemetry, and we do want to handle that.
        if (msg.parsedHeader.lowerOpCode == STROKE_OPCODE)
        {
            handleTelemetryMsg(msg);
        }

        return;
    }

    void WatchDogInterfaceComponentImpl::handleUplinkMsg(WatchDogMpsm::Message &msg)
    {
        // Before anything else, make sure we have enough data (and since the payload is variable size,
        // in this case "enough" just means non-zero and not over the maximum)
        if (msg.accumulatedDataSize <= 0 || msg.accumulatedDataSize > WATCHDOG_MAX_PAYLOAD)
        {
            this->log_WARNING_HI_WatchDogIncorrectResp(bad_size_received);
            return;
        }

// This block of code can be enabled to debug bad data being received from WD
#if 0
        bool hasBadass = false;
        for (size_t i = 0; i < msg.accumulatedDataSize - 2; ++i) {
            if (msg.dataBuffer[i] == 0x55 &&
                    msg.dataBuffer[i+1] == 0xDA &&
                    msg.dataBuffer[i+2] == 0xBA) {
                hasBadass = true;
                break;
            }
        }

        if (!hasBadass) {
            m_rxTask.printRxUpdates();
        }
#endif

        // TODO: Verify that the MTU for wired connection is the same as Wifi
        Fw::Buffer uplinked_data;
        uplinked_data.setdata(reinterpret_cast<U64>(msg.dataBuffer));
        uplinked_data.setsize(static_cast<U32>(msg.accumulatedDataSize));
        uplink_out(0, uplinked_data);
    }

    void WatchDogInterfaceComponentImpl::handleDownlinkMsg(WatchDogMpsm::Message &msg)
    {
        // Before anything else, make sure we have enough data (and since the payload is variable size,
        // in this case "enough" just means non-zero and not over the maximum)
        if (msg.accumulatedDataSize <= 0 || msg.accumulatedDataSize > WATCHDOG_MAX_PAYLOAD)
        {
            this->log_WARNING_HI_WatchDogIncorrectResp(bad_size_received);
            return;
        }

// This block of code can be enabled to debug bad data being received from WD
#if 0
        bool hasBadass = false;
        for (size_t i = 0; i < msg.accumulatedDataSize - 2; ++i) {
            if (msg.dataBuffer[i] == 0x55 &&
                    msg.dataBuffer[i+1] == 0xDA &&
                    msg.dataBuffer[i+2] == 0xBA) {
                hasBadass = true;
                break;
            }
        }

        if (!hasBadass) {
            m_rxTask.printRxUpdates();
        }
#endif

        // TODO: Verify that the MTU for wired connection is the same as Wifi
        Fw::Buffer downlinked_data;
        downlinked_data.setdata(reinterpret_cast<U64>(msg.dataBuffer));
        downlinked_data.setsize(static_cast<U32>(msg.accumulatedDataSize));
        downlinkBufferSend_out(0, downlinked_data);
    }

    void WatchDogInterfaceComponentImpl::handleTelemetryMsg(WatchDogMpsm::Message &msg)
    {
        // Before anything else, make sure we have enough data
        if (msg.accumulatedDataSize != sizeof(struct WatchdogTelemetry))
        {
            this->log_WARNING_HI_WatchDogIncorrectResp(bad_size_received);
            return;
        }

        struct WatchdogTelemetry *buff =
            reinterpret_cast<struct WatchdogTelemetry *>(msg.dataBuffer);

        //        this->tlmWrite_VOLTAGE_2_5V(buff->voltage_2V5); // DEPRECATED TELEM FIELD (see note in XML)
        //        this->tlmWrite_VOLTAGE_2_8V(buff->voltage_2V8); // DEPRECATED TELEM FIELD (see note in XML)
        //        this->tlmWrite_VOLTAGE_24V(buff->voltage_24V); // DEPRECATED TELEM FIELD (see note in XML)
        this->tlmWrite_VOLTAGE_28V(buff->voltage_28V);
        this->tlmWrite_BATTERY_THERMISTOR(buff->battery_thermistor);
        // this->tlmWrite_SYSTEM_STATUS(buff->sys_status);        // Not currently impl. (we get this from WD->Herc packet forwarding anyway).
        // this->tlmWrite_BATTERY_LEVEL(buff->battery_level);     //  Not currently impl. (we get this from WD->Herc packet forwarding anyway).
        // this->tlmWrite_BATTERY_CURRENT(buff->battery_current); //  Not currently impl. (we get this from WD->Herc packet forwarding anyway).
        // this->tlmWrite_BATTERY_VOLTAGE(buff->battery_voltage); // Not currently impl. (we get this from WD->Herc packet forwarding anyway)
        setExt28VRaw(buff->voltage_28V);
        return;
    }

    WatchDogInterfaceComponentImpl::TxCommandStatus *
    WatchDogInterfaceComponentImpl::getTxCommandStatusFullOpcode(FwOpcodeType opCode)
    {
        return getTxCommandStatus(static_cast<uint16_t>(opCode));
    }

    WatchDogInterfaceComponentImpl::TxCommandStatus *
    WatchDogInterfaceComponentImpl::getTxCommandStatus(uint16_t opCode)
    {
        static const uint16_t RESET_SPECIFIC_OPCODE_USHORT =
            static_cast<uint16_t>(WatchDogInterfaceComponentBase::OPCODE_RESET_SPECIFIC);
        static const uint16_t DISENGAGE_FROM_LANDER_USHORT =
            static_cast<uint16_t>(WatchDogInterfaceComponentBase::OPCODE_DISENGAGE_FROM_LANDER);
        static const uint16_t ENGAGE_FROM_LANDER_USHORT =
            static_cast<uint16_t>(WatchDogInterfaceComponentBase::OPCODE_ENGAGE_FROM_LANDER);

        size_t index = 0;

        switch (opCode)
        {
        case STROKE_OPCODE:
            index = COMMAND_INDEX__STROKE;
            break;

        case DOWNLINK_OPCODE:
            index = COMMAND_INDEX__DOWNLINK;
            break;

        case RESET_SPECIFIC_OPCODE_USHORT:
            index = COMMAND_INDEX__RESET_SPECIFIC;
            break;

        case DISENGAGE_FROM_LANDER_USHORT:
            index = COMMAND_INDEX__DISENGAGE_FROM_LANDER;
            break;

        case ENGAGE_FROM_LANDER_USHORT:
            index = COMMAND_INDEX__ENGAGE_FROM_LANDER;
            break;

        default:
            // Should never happen;
            //            configASSERT(false);
        }

        // This is equivalent to &(m_txCmdArray.commands[index])
        return m_txCmdArray.commands + index;
    }

    bool WatchDogInterfaceComponentImpl::txCommand(FwOpcodeType opCode,
                                                   U32 cmdSeq,
                                                   uint16_t resetValue,
                                                   uint8_t *dataBuffer,
                                                   size_t dataLen,
                                                   bool sendResponse)
    {
        static Os::Mutex sloppyResourceProtectionMutex; // quick and dirty. keeps multiple tasks from doing this at once.

        sloppyResourceProtectionMutex.lock();
        struct WatchdogFrameHeader frame;
        frame.magic_value = header_magic;
        frame.parity = 0;
        frame.payload_length = static_cast<uint16_t>(dataLen);
        frame.reset_val = resetValue;
        frame.sequence_number = static_cast<uint16_t>(cmdSeq);
        frame.opcode = static_cast<uint16_t>(opCode);

        uint8_t *frameBytes = reinterpret_cast<uint8_t *>(&frame);
        uint8_t runningParity = 0;

        for (size_t i = 0; i < 12; ++i)
        {
            runningParity += frameBytes[i];
        }

        frame.parity = ~runningParity;

        // Skip all response checking logic for debug messages, just send it
        if (frame.opcode != DEBUG_OPCODE)
        {
            TxCommandStatus *cmdStatus = getTxCommandStatusFullOpcode(opCode);

            if (NULL == cmdStatus)
            {
                sloppyResourceProtectionMutex.unLock();
                return false;
            }

            // Get the current time
            Fw::Time now = getTime();
            uint32_t nowMillis = static_cast<uint32_t>(now.get_time_ms());
            bool previousStillWaiting = false;
            bool timeout = false;
            FwOpcodeType timedOutOpcode = 0;
            U32 timedOutSeqNum = 0;
            bool timedOutSendResponse = false;

            { // We shouldn't leave this scope without unlocking the mutex
                m_txCmdArray.cmdMutex.lock();

                // Note: We don't want to do any comms (or anything else long or complicated) with the mutex locked.

                // First, check if a command of this type is already active (i.e. waiting for a response)
                if (cmdStatus->active)
                {
                    // One is active, so check if it has timed out (this will still work even if the time wraps)
                    if (nowMillis - cmdStatus->txTimeMillis >= COMMAND_TIMEOUT_MILLISECONDS)
                    {
                        // We're timing out the previous command.
                        timeout = true;
                        timedOutOpcode = cmdStatus->opcode;
                        timedOutSeqNum = cmdStatus->seqNum;
                        timedOutSendResponse = cmdStatus->sendResponse;
                        cmdStatus->reset();
                    }
                    else
                    {
                        // The previous tx of this command type has not timed out yet, so we won't send this command
                        previousStillWaiting = true;
                    }
                }

                // Update the tx command structure with the current data (only if we don't already have a command
                // of this type waiting for a response)
                if (!previousStillWaiting)
                {
                    cmdStatus->active = true;
                    cmdStatus->sendResponse = sendResponse;
                    cmdStatus->seqNum = cmdSeq;
                    cmdStatus->txTimeMillis = nowMillis;
                }

                m_txCmdArray.cmdMutex.unLock();
            }

            // Now actually send the response about the previous iteration of this command timing out
            if (timeout)
            {
                this->log_WARNING_HI_WatchDogTimedOut();

                if (cmdStatus->opcode == STROKE_OPCODE)
                {
                    m_missedStrokeResponses++;
                }

                // Make sure we don't try to send a response about any of our fake opcodes, and don't
                // send a response if we didn't want to send one when we sent the message
                if (timedOutOpcode != STROKE_OPCODE && timedOutOpcode != DOWNLINK_OPCODE && timedOutSendResponse)
                {
                    this->cmdResponse_out(timedOutOpcode, timedOutSeqNum, Fw::COMMAND_EXECUTION_ERROR);
                }
            }

            // If we're not sending this command we don't want to continue
            if (previousStillWaiting)
            {
                if (cmdStatus->opcode == STROKE_OPCODE)
                {
                    m_skippedStrokes++;
                }

                // TODO: LOG ERROR (which error?)
                // debugPrintfToWatchdog("previousStillWaiting: %d\n", cmdStatus->opcode);
                sloppyResourceProtectionMutex.unLock();
                return false;
            }
        }

        // Finally we're ready to transmit. If this is header-only we can transmit it all in one go (since it's in
        // one "buffer"), and since we can transmit it all in one go we can also transmit it non-blocking. On the
        // other hand, if there is also data then we need to transmit the header with blocking, then we can
        // transmit the data non-blocking. However, with the non-blocking send of the header we need to make
        // sure dmaSend doesn't return false, because if it does that means a previous send is still active.
        // If that's the case then we'll return the send as blocking (even if there is no data).
        bool success = dmaSend((void *)&frame, 12, true); //(dataLen != 0));

        if (!success)
        {
            dmaSend((void *)&frame, 12, true);
        }

        if (dataLen > 0)
        {
            // If we're sending data then we always know that the previous blocking send of the header is complete,
            // which means that this should always succeed. However, we'll handle that case anyway just in case.
            success = dmaSend(dataBuffer, dataLen, true);

            if (!success)
            {
                dmaSend(dataBuffer, dataLen, true);
            }
        }

        sloppyResourceProtectionMutex.unLock();
        return true;
    }

    bool WatchDogInterfaceComponentImpl::debugPrintfToWatchdog(const char *fmt, ...)
    {
        static Os::Mutex sloppyResourceProtectionMutex; // quick and dirty. keeps multiple tasks from doing this at once.
        if (fmt == NULL)
        {
            return false;
        }

        sloppyResourceProtectionMutex.lock();
        memset(m_printBuffer, 0, sizeof(m_printBuffer));
        sprintf(m_printBuffer, "DEBUG");
        va_list args;
        va_start(args, fmt); // @suppress("Function cannot be resolved")
        vsnprintf(m_printBuffer + 5, sizeof(m_printBuffer) - 5, fmt, args);
        va_end(args);

        bool success = txCommand(DEBUG_OPCODE,
                                 m_downlinkSequenceNumber,
                                 static_cast<uint16_t>(No_Reset),
                                 reinterpret_cast<uint8_t *>(m_printBuffer),
                                 static_cast<size_t>(strnlen(m_printBuffer, sizeof(m_printBuffer))),
                                 false);

        if (success)
        {
            m_downlinkSequenceNumber++;
        }
        sloppyResourceProtectionMutex.unLock();

        return success;
    }

    bool WatchDogInterfaceComponentImpl::debugPrintfBuffer(uint8_t *buffer, size_t bufferLen)
    {
        static Os::Mutex sloppyResourceProtectionMutex; // quick and dirty. keeps multiple tasks from doing this at once.
        if (bufferLen == 0)
        {
            return false;
        }
        sloppyResourceProtectionMutex.lock();
        memset(m_printBuffer, 0, sizeof(m_printBuffer));
        sprintf(m_printBuffer, "DEBUG");
        size_t bytesToSend = (bufferLen > (sizeof(m_printBuffer) - 5)) ? (sizeof(m_printBuffer) - 5) : bufferLen;
        memcpy(m_printBuffer + 5, buffer, bytesToSend);

        bool success = txCommand(DEBUG_OPCODE,
                                 m_downlinkSequenceNumber,
                                 static_cast<uint16_t>(No_Reset),
                                 reinterpret_cast<uint8_t *>(m_printBuffer),
                                 (5 + bytesToSend),
                                 false);

        if (success)
        {
            m_downlinkSequenceNumber++;
        }
        sloppyResourceProtectionMutex.unLock();

        return success;
    }

    bool WatchDogInterfaceComponentImpl::debugPrintfBufferWithPrefix(uint8_t *prefixBuffer, size_t prefixBufferLen, uint8_t *buffer, size_t bufferLen)
    {
        static Os::Mutex sloppyResourceProtectionMutex; // quick and dirty. keeps multiple tasks from doing this at once.
        if (bufferLen == 0 || prefixBufferLen == 0)
        {
            return false;
        }
        sloppyResourceProtectionMutex.lock();
        memset(m_printBuffer, 0, sizeof(m_printBuffer));
        sprintf(m_printBuffer, "DEBUG");

        // Safety check the buffer sizes:
        size_t prefixBytesToSend, mainBytesToSend;

        // If prefix is too big, we just send that:
        if (prefixBufferLen > (sizeof(m_printBuffer) - 5))
        {
            prefixBytesToSend = (sizeof(m_printBuffer) - 5);
            mainBytesToSend = 0;
        }
        else
        {
            prefixBytesToSend = prefixBufferLen;
        }

        // Clip main buffer:
        if (bufferLen > (sizeof(m_printBuffer) - 5 - prefixBytesToSend))
        {
            mainBytesToSend = (sizeof(m_printBuffer) - 5 - prefixBytesToSend);
        }
        else
        {
            mainBytesToSend = bufferLen;
        }

        // Copy data:
        memcpy(m_printBuffer + 5, prefixBuffer, prefixBytesToSend);
        memcpy(m_printBuffer + 5 + prefixBytesToSend, buffer, mainBytesToSend);

        // Send:
        bool success = txCommand(DEBUG_OPCODE,
                                 m_downlinkSequenceNumber,
                                 static_cast<uint16_t>(No_Reset),
                                 reinterpret_cast<uint8_t *>(m_printBuffer),
                                 (5 + prefixBytesToSend + mainBytesToSend),
                                 false);

        if (success)
        {
            m_downlinkSequenceNumber++;
        }
        sloppyResourceProtectionMutex.unLock();

        return success;
    }

    // FIXME: Add timeout to escape polling loop
    void WatchDogInterfaceComponentImpl::pollDMASendFinished()
    {
        if (!m_finished_initializing)
        {
            while (!((getDMAIntStatus(BTC) >> SCILIN_TX_DMA_CH) & 0x01U))
            {
                vTaskDelay(DMA_SEND_POLLING_CHECK_INTERVAL); // Check back in every X ticks (don't hog the processor)
            };
            dmaWriteBusy = false;
            sciDMASendCleanup(SCILIN_TX_DMA_CH);
        }
        while (dmaWriteBusy)
        {
            vTaskDelay(DMA_SEND_POLLING_CHECK_INTERVAL); // Check back in every X ticks (don't hog the processor)
        };
        ; // TODO: Mutex to allow multiprogramming & TIMEOOUT
    }

    // Returns negative on error
    bool WatchDogInterfaceComponentImpl::dmaSend(void *buffer, int size, bool blocking)
    {
        static Os::Mutex resourceProtectionMutex; // quick and dirty. prevents multiple Tasks from trying this at once
        // sciSend(m_sci, size, static_cast<uint8_t *>(buffer));
        // return true;
        resourceProtectionMutex.lock();

        if (blocking)
        {
            while (dmaWriteBusy)
            {
                vTaskDelay(DMA_SEND_POLLING_CHECK_INTERVAL); // Check back in every X ticks (don't hog the processor)
            };
        }
        else if (dmaWriteBusy)
        {
            return false;
        }
        sciDMASend(SCILIN_TX_DMA_CH, static_cast<char *>(buffer), size, ACCESS_8_BIT, &dmaWriteBusy);
        if (blocking)
            pollDMASendFinished();

        resourceProtectionMutex.unLock();
        return true;
    }

    void WatchDogInterfaceComponentImpl::setExt28VRaw(int16_t voltage)
    {
        this->m_extDataMutex.lock();
        this->m_extVoltage28VRaw = voltage;
        this->m_extDataMutex.unLock();
    }

    int16_t WatchDogInterfaceComponentImpl::getExt28VRaw()
    {
        int16_t reading;
        this->m_extDataMutex.lock();
        reading = this->m_extVoltage28VRaw;
        this->m_extDataMutex.unLock();
        return reading;
    }

} // end namespace CubeRover
