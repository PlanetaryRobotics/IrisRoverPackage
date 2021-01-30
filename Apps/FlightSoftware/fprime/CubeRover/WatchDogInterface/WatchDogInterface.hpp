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

      //! Implementation for Reset Specific for init of cuberover
      //! Only difference between this and Reset_Specific_cmdHandler is no cmd response
      bool Reset_Specific_Handler(
          U8 reset_value /*!< 
                      U8 Value that represents which things need to be reset
                    */
      );

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

      //! Implementation for sending frame and checking that frame was sent to watchdog
      //! Sends a Frame start everytime data is sent from cuberover to watchdog
      bool Send_Frame(
          U32 stroke  // Type of frame we want to send
          );

      //! Implementation for checking the tempurature senors from the ADC
      bool Read_Temp();

      // frame struct
      struct WatchdogFrameHeader {
        uint32_t magic_value    :24;
        uint8_t parity          :8;
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
     } __attribute__((packed, aligned(8)));

      Receive_Frame(uint32_t *comm_error, WatchdogFrameHeader *header);

    public:

      // variable to store if DMA is busy or not
      bool watchdog_dma_busy;

    };

} // end namespace CubeRover

#endif
