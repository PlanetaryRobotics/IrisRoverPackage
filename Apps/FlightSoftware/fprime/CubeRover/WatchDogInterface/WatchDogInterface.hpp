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

#include "lin.h"
#include "adc.h"
#include "sci.h"

// The pin number for the deploment pin 2
const U8 deploy_bit = 5;
// The number of thermistors on the SBC
const U8 number_thermistors = 6;
// Default size of zero sent to watchdog
const U16 zero_size = 0x0000;
// Minimum size that should be received back from watchdog when receiving
const U8 min_receive_size = 8;
// Magic Value first sent in the header between watchdog and hercules communication
const U32 header_magic = 0x21B00B;

namespace CubeRover {

  class WatchDogInterfaceComponentImpl :
    public WatchDogInterfaceComponentBase
  {

    public:

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
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object WatchDogInterface
      //!
      ~WatchDogInterfaceComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for PingIn
      //!
      void Run_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

      //! Handler implementation for CompResetRequest
      //!
      void CompResetRequest_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          CubeRoverPorts::ResetValue reset 
      );

      //! Handler implementation for downlink
      //!
      void downlink_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Handler implementation for PingIn
      //!
      void PingIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

      //! Handler implementation for WdogStroke
      //!
      void WdogStroke_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 code /*!< Watchdog stroke code*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for Reset Specific command handler
      //! Command to reset the specific parts of rover
      void Reset_Specific_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          reset_values_possible reset_value /*!< 
                    	ENUM Value that specifies which components or hardware need to be reset
                    */
      );

      //! Implementation for Disengage_From_Lander command handler
      //! Command to send signal to MSP430 that it should send a signal to lander to disengage, sets disengage pin high
      void Disengage_From_Lander_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          confirm_disengage confirm 
      );

      //! Implementation for Engage_From_Lander command handler
      //! Command to send signal to MSP430 that it should send a signal to lander to engage, sets disengage pin low
      void Engage_From_Lander_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      /* Commands that Only Watchdog Processes */

      //! Implementation for Prepare_For_Deployment command handler
      //! Command to send signal to MSP430 to prepare for deploying (may not be needed)
      void Prepare_For_Deployment_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          confirm_prepare_for_deploy confirm 
      );

      //! Implementation for Switch_Connection_Mode command handler
      //! Command to send signal to MSP430 that we switch the current connection mode
      void Switch_Connection_Mode_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Implementation for Set_Kp_Specific command handler
      //! Command to send signal to MSP430 that it should set Kp to a specific value
      void Set_Kp_Specific_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U16 value 
      );

      //! Implementation for Set_Heater_Duty_Cycle_Max command handler
      //! Command to send signal to MSP430 that it should set the max possible Duty Cycle value for the heater
      void Set_Heater_Duty_Cycle_Max_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Implementation for Set_Heater_Duty_Cycle_Period command handler
      //! Command to send signal to MSP430 that it should set the period the Duty Cycle for the heater is at
      void Set_Heater_Duty_Cycle_Period_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U16 period 
      );

      //! Implementation for Set_Heater_Window command handler
      //! Set the Half-Width of the heater on/off deadband window around the setpoint (in thermistor ADC values). Between setpoint - half_width and setpoint + half_width, heater is off.
      void Set_Heater_Window_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U16 adc_half_width 
      );

      //! Implementation for Set_Heater_Setpoint command handler
      //! Command to send signal to MSP430 that it should set V to a specific value
      void Set_Heater_Setpoint_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U16 adc_setpoint 
      );

      //! Implementation for Switch_to_Sleep_Mode command handler
      //! Command to send signal to MSP430 that it should go into Sleep Mode
      void Switch_to_Sleep_Mode_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          confirm_sleep_mode confirm 
      );

      //! Implementation for Switch_to_Keep_Alive_Mode command handler
      //! Command to send signal to MSP430 that it should go into Keep Alive Mode
      void Switch_to_Keep_Alive_Mode_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          confirm_alive_mode confirm 
      );

      //! Implementation for Switch_to_Service_Mode command handler
      //! Command to send signal to MSP430 that it should go into Service Mode
      void Switch_to_Service_Mode_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          confirm_service_mode confirm 
      );

      /* End of Commands that Only Watchdog Processes*/

      // The Header Frame structure sent and received between Hercules and Watchdog
      struct WatchdogFrameHeader {
        uint32_t magic_value    :24;  // 24 bit magic value that is constant and sent at the begining of every send
        uint32_t parity         :8;   // 8 bit partity of whole header, initially calculated with parity equal to 0
        uint16_t payload_length;      // The length of the payload being sent
        uint16_t reset_val;           // The reset value being sent to watchdog
      } __attribute__((packed, aligned(8)));

     struct WatchdogTelemetry {
         int16_t voltage_2V5;   // Current measured voltage of the 2.5V line as read from the Watchdog
         int16_t voltage_2V8;   // Current measured voltage of the 2.8V line as read from the Watchdog
         int16_t voltage_24V;   // Current measured voltage of the 24V line as read from the Watchdog
         int16_t voltage_28V;   // Current measured voltage of the 28V line as read from the Watchdog
         int8_t battery_thermistor;   // Current measured voltage of the watchdog battery thermistor as read from the Watchdog
         int8_t sys_status;   // 8 bit systems status where each bit represents a watchdog status
         int16_t battery_level;   // Current measured battery voltage as read from the Watchdog
         int32_t battery_current;   // Current measured battery current as read from the Watchdog
     } __attribute__((packed, aligned(8)));

     // Incorrect Response Possible Values
     enum resp_error : U8
     {
         bad_parity = 1,    // Error code for response having bad parity value
         bad_size_received = 2,   // Error code for response having bad data size received
         bad_reset_value = 3,   // Error code for response having a bad reset value
         bad_magic_value = 4,   // Error code for response having a bad magic value
         not_enough_bytes = 5   // Error code for response not having enough bytes (must be over min_receive_size)
     };

     enum disengage_command : U16
     {
        Disengage = 0x00EE    // Reset value for the disengagement command sent to Watchdog
     };

     // Receives a Frame start everytime data is sent from watchdog to hercules
     int Receive_Frame(uint32_t *comm_error, // Error returned from receiving frame
                      WatchdogFrameHeader *header  // Location to save data into
                      );

     // Sends a Frame start everytime data is sent from hercules to watchdog
     bool Send_Frame(
         U16 payload_length,    // stroke if 0x0000 or UDP data size if larger than
         U16 reset_value        // reset value for watchdog
         );

     bool Read_Temp();          // Checking the temperature sensors from the ADC

     void pollDMAReceiveFinished();   // Polls DMA recieve finish to see if we've finished our DMA receiving
     void pollDMASendFinished();      // Polls DMA send finish to see if we've finished our DMA sending
     // Perform a DMA receive function
     bool dmaReceive(void *buffer,          // The buffer to put all received data into
                      int size,             // The size that is received from watchdog
                      bool blocking=true);  // Check variable to see if we need to block other DMA requests
     // Perform a DMA send function
     bool dmaSend(void *buffer,           // The buffer of data to send to watchdog
                  int size,               // The size of the data to send to watchdog
                  bool blocking=true);    // Check variable to see if we need to block other DMA requests

      // Usage during FSW initialization
      // Only difference between this is function and Reset_Specific_cmdHandler is lack of cmd response
      bool Reset_Specific_Handler(reset_values_possible reset_value);


      sciBASE_t *m_sci;   // The sci base used to initialize the watchdog interface connection 
      adcData_t m_thermistor_buffer[number_thermistors];  // Location to store current data for thermistors
      bool m_finished_initializing;     // Flag set when this component is fully initialized and interrupt DMA can be used (otherwise polling DMA)

    };

} // end namespace CubeRover

#endif
