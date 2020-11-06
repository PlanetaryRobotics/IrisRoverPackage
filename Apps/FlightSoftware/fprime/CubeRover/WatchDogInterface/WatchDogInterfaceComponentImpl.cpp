// ======================================================================
// \title  WatchDogInterfaceComponentImpl.cpp
// \author alec
// \brief  cpp file for WatchDogInterface component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/WatchDogInterface/WatchDogInterfaceComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  WatchDogInterfaceComponentImpl ::
#if FW_OBJECT_NAMES == 1
    WatchDogInterfaceComponentImpl(
        const char *const compName
    ) :
      WatchDogInterfaceComponentBase(compName)
#else
    WatchDogInterfaceComponentImpl(void)
#endif
  {

  }

  void WatchDogInterfaceComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    // Configure UART with watchdog MSP430
    Configure_UART(UART_id, UART_rate, UART_data, UART_stop, UART_parity, UART_flow);
    // Pull high RTS: not ready to receive data
    gioSetBit(gioPORTB, 3, 1);  //NOTE: THIS IS THE WIFI PORT, *****MUST***** CHANGE THIS TO CORRECT WATCHDOG PORT
    WatchDogInterfaceComponentBase::init(queueDepth, instance);
  }

  WatchDogInterfaceComponentImpl ::
    ~WatchDogInterfaceComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void WatchDogInterfaceComponentImpl ::
    PingIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    this->pingOut_out(0, key);
  }

  void WatchDogInterfaceComponentImpl ::
    CmdInput_handler(
        const NATIVE_INT_TYPE portNum,
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CmdArgBuffer &args
    )
  {
    // TODO
    // how is this different than the command functions defined below?
  }

  void WatchDogInterfaceComponentImpl ::
    Run_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // TODO
    // This run handler happens every 1-100 hz (overview says 100 hz, specific says 1 hz)
    // Sends a U32 to the watchdog as defined in design document. Checks stroke value as to what to send watchdog
    // May need to wait to send command everytime it runs as could be a lot of commands
  }

  void WatchDogInterfaceComponentImpl ::
    WdogStroke_handler(
        const NATIVE_INT_TYPE portNum,
        U32 code
    )
  {
    // TODO
    // Happens everytime Health sends a watchdog code to Watchdog interface
    // Watchdog code will have info on what components status is (MUST FIGURE OUT HOW CODE WORKS)
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void WatchDogInterfaceComponentImpl ::
    Reset Everything_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // Sends a command to watchdog to reset all devices AND resets all components (how to do that is TBD)
    // Set watchdog_reset to all 1's from bits 1-17, 0's for bit's 0 and 18-31
    U32 watchdog_reset = 0x7FFFC000;
    // Send watchdog_reset to MSP430 watchdog
    // TODO
    // Reset all Components (UNDECIDED YET)
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void WatchDogInterfaceComponentImpl ::
    Reset Specific_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U32 reset_value
    )
  {
    // Sends a command to watchdog to reset specified devices. Can be hardware through watchdog or component
    // Isolate the first bit of the reset_value to know if we're reseting hardware or component
    U32 reset_choice = reset_value & 1;
    // If reset_choice == 1, we are resetting components
    if(reset_choice)
    {
      // Reset Components (UNDECIDED YET)
      // TODO
    }
    // If reset_choice == 0, we are resetting hardware
    else
    {
      // Copy value of reset_value but set 0 bit and bits 18-31 to zero
      U32 watchdog_reset = reset_value & 0x7FFFC000;
      // Send watchdog_reset to MSP430 watchdog
      // TODO
    }
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  // UART Configuration Implementation
  void Configure_UART(
      const uint8_t uartId,       //UART ID value
      const uint32_t baudrate,    //UART Baudrate
      const uint8_t dataBits,     //UART # of data bits
      const uint8_t stopBits,     //UART # of stop bits
      const uint8_t parity,       //UART odd, even, or no parity
      const uint8_t flowCtrl      //UART flow control
    )
  {
    uint8_t payload[1 /* uartId size*/ + 4 /*baudrate size*/ + 4 /*4 bits for data*/ + 3 /*3 bits for stop/parity/flow*/];
    //prepare payload
    //copy uartID
    payload[0] = uartId;
    //copy baudrate
    memcpy(payload+1, &baudrate, sizeof(baudrate));
    //copy baudrate
    memcpy(payload+5, &dataBits, sizeof(dataBits));
    //copy other bits
    payload[9] = stopBits;
    payload[10] = parity;
    payload[11] = flowCtrl;

    //transmit payload to configure UART
    //TODO
  }

} // end namespace CubeRover
