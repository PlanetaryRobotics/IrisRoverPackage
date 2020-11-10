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
    // This run handler happens every 1-100 hz (overview says 100 hz, specific says 1 hz)
    // Sends a U32 to the watchdog as defined in design document. Checks stroke value as to what to send watchdog
    // May need to wait to send command everytime it runs as could be a lot of commands
	// Create watchdog stroke equal to 1 as first bit 1 to tell watchdog scheduled, rest bits 0 to not reset anything
    U32 watchdog_stroke = 1;
    // May want to change wattchdog_stoke sent value to accumulated stroke value?
    linSend(linREG, &watchdog_stroke);
    // Check for Response from MSP430 Watchdog
    U32 watchdog_reponse;
    linGetData(linREG, &watchdog_reponse);
    // Check for timeout of response
    U32 comm_error = linGetStatusFlag(linREG);
    if(comm_error == LIN_TO_INT)
    {
    	this->log_WARNING_HI_WatchDogTimedOut();
    }
    // Check that response is the same as what was sent
    if(watchdog_reponse != watchdog_reset)
    {
    	this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
    }
	// Check for any watchdog errors (NOTE: not sure that comm_error should be zero if no errors)
    if(comm_error)
    {
    	this->log_WARNING_HI_WatchDogCommError(comm_error);
    }
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
  	// Send Activity Log to know watchdog recieved command
  	char *command_type = "Reset Everything"
  	this->log_WARNING_HI_WatchDogCmdReceived(command_type);
    // Sends a command to watchdog to reset all devices AND resets all components (how to do that is TBD)
    // Set watchdog_reset to all 1's from bits 1-17, 0's for bit's 0 and 18-31
    U32 watchdog_reset = 0x7FFFC000;
    // Send watchdog_reset to MSP430 watchdog
    linSend(linREG, &watchdog_reset);
    // Reset all Components (UNDECIDED YET)
    // TODO
    // Check for Response from MSP430 Watchdog
    U32 watchdog_reponse;
    linGetData(linREG, &watchdog_reponse);
    // Check for timeout of response
    U32 comm_error = linGetStatusFlag(linREG);
    if(comm_error == LIN_TO_INT)
    {
    	this->log_WARNING_HI_WatchDogTimedOut();
    }
    // Check that response is the same as what was sent
    if(watchdog_reponse != watchdog_reset)
    {
    	this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
    }
	// Check for any watchdog errors (NOTE: not sure that comm_error should be zero if no errors)
    if(comm_error)
    {
    	this->log_WARNING_HI_WatchDogCommError(comm_error);
    }
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void WatchDogInterfaceComponentImpl ::
    Reset Specific_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U32 reset_value
    )
  {
  	// Send Activity Log to know watchdog recieved command
  	char *command_type = "Reset Specific"
  	this->log_WARNING_HI_WatchDogCmdReceived(command_type);
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
      	linSend(linREG, &watchdog_reset);
		// Check for Response from MSP430 Watchdog
	    U32 watchdog_reponse;
	    linGetData(linREG, &watchdog_reponse);
	    // Check for timeout of response
	    U32 comm_error = linGetStatusFlag(linREG);
	    if(comm_error == LIN_TO_INT)
	    {
	    	this->log_WARNING_HI_WatchDogTimedOut();
	    }
	    // Check that response is the same as what was sent
	    if(watchdog_reponse != watchdog_reset)
	    {
	    	this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
	    }
		// Check for any watchdog errors (NOTE: not sure that comm_error should be zero if no errors)
	    if(comm_error)
	    {
	    	this->log_WARNING_HI_WatchDogCommError(comm_error);
	    }
    }
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace CubeRover
