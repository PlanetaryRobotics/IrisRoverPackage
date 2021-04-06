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
// ======================================================================

#ifndef WatchDogInterface_HPP
#define WatchDogInterface_HPP

#include "CubeRover/WatchDogInterface/WatchDogInterfaceComponentAc.hpp"

#include "lin.h"
#include "adc.h"
#include "sci.h"

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
          U8 reset_value /*!< 
                    	U8 Value that represents which things need to be reset
                    */
      );

      //! Implementation for Disengage_From_Lander command handler
      //! Command to send signal to MSP430 that it should send a signal to lander to disengage
      void Disengage_From_Lander_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      // frame struct
      struct WatchdogFrameHeader {
        uint32_t magic_value    :24;
        uint32_t parity         :8;
        uint16_t payload_length;
        uint16_t reset_val;
      } __attribute__((packed, aligned(8)));

     struct WatchdogTelemetry {
         int16_t voltage_2V5;
         int16_t voltage_2V8;
         int16_t voltage_24V;
         int16_t voltage_28V;
         int8_t battery_thermistor;
         int8_t sys_status;
         int16_t battery_level;
         int32_t battery_current;
     } __attribute__((packed, aligned(8)));

     // Incorrect Response Possible Values
     enum resp_error : U8
     {
         bad_parity = 1,
         bad_size_received = 2,
         bad_reset_value = 3,
         bad_magic_value = 4,
         not_enough_bytes = 5
     };


     int Receive_Frame(uint32_t *comm_error, WatchdogFrameHeader *header);

     // Sends a Frame start everytime data is sent from cuberover to watchdog
     bool Send_Frame(
         U16 payload_length,    // stroke if 0x0000 or UDP data size if larger than
         U16 reset_value        // reset value for watchdog
         );

     bool Read_Temp();          // Checking the temperature sensors from the ADC

     void pollDMAReceiveFinished();
     void pollDMASendFinished();
     bool dmaReceive(void *buffer, int size, bool blocking=true);
     bool dmaSend(void *buffer, int size, bool blocking=true);

      // Usage during FSW initialization
      // Only difference between this is lack of cmd response
      bool Reset_Specific_Handler(U8 reset_value);


      sciBASE_t *m_sci;
      adcData_t m_thermistor_buffer[6];
      bool m_finished_initializing;     // Flag set when this component is fully initialized and interrupt DMA can be used (otherwise polling DMA)

    };

} // end namespace CubeRover

#endif
