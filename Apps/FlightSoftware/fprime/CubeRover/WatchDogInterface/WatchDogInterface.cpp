// ======================================================================
// \title  WatchDogInterfaceComponentImpl.cpp
// \author alec and justin
// \brief  cpp file for WatchDogInterface component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/WatchDogInterface/WatchDogInterface.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <stdio.h>
#include <string.h>

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
    Run_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // This run handler happens every 1-100 hz (overview says 100 hz, specific says 1 hz)
    // Sends U16 to the watchdog as defined in design document. Checks stroke value as to what to send watchdog
    U16 watchdog_stroke = 0x00000000;
    // Send stroke
    linSend(linREG, (uint8_t *)&watchdog_stroke);

    // Check for Response from MSP430 Watchdog
    U8 watchdog_reponse;
    linGetData(linREG, (uint8_t *)&watchdog_reponse);
    void* watchdog_resp_add = (uint8_t *)&watchdog_reponse;
    U8* watchdog_resp_add2 = (uint8_t *)&watchdog_reponse;

    U32 comm_error = linGetStatusFlag(linREG);
    U32 watchdog_resp_header = 0x00000000;
    // Check to make sure that we have recieved at least four bytes
    if(sizeof(*watchdog_resp_add2) >= 4)
        memcpy(&watchdog_resp_header, watchdog_resp_add, 4);	// Copy first four bytes of response
    else
    {
    	this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
    	return;
    }

    // Check for timeout of response
    if(comm_error == LIN_TO_INT)
    {
    	this->log_WARNING_HI_WatchDogTimedOut();
    }
    // Check that response is the same as what was sent
    else if(watchdog_resp_header != watchdog_stroke)
    {
    	// Check if the watchdog response includes UDP data
    	if((watchdog_resp_header & 0xFFFF0000) == 0x00000000)
    	{
    		// Forward all data after header and current data to ground interface
    		U64 UDP_ptr = (U64)(watchdog_resp_add+4); // Create a pointer to start of the UDP data 32 bits after start of watchdog_response
    		uint16_t UDP_size = (U16)(watchdog_resp_header);
    		Fw::Buffer recv_Buffer(this->getInstance(), 0, UDP_ptr, UDP_size); 
    		uplink_out(0, recv_Buffer);
    	}
    	else
    		this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
    }
    else if(watchdog_resp_header == watchdog_stroke)
    {
    	//Foward telemetry data
    	      	
      	if(sizeof(*watchdog_resp_add2) >= 14)
      	{
      	  int16_t voltage_tlm = 0;
      	  memcpy(&voltage_tlm, watchdog_resp_add+4, 2);	// Copy two bytes for tlm value
    	  this->tlmWrite_VOLTAGE_2_5V(voltage_tlm);
    	  memcpy(&voltage_tlm, watchdog_resp_add+6, 2);	// Copy two bytes for tlm value
    	  this->tlmWrite_VOLTAGE_2_8V(voltage_tlm);
    	  memcpy(&voltage_tlm, watchdog_resp_add+8, 2);	// Copy two bytes for tlm value
    	  this->tlmWrite_VOLTAGE_24V(voltage_tlm);
    	  memcpy(&voltage_tlm, watchdog_resp_add+10, 2);	// Copy two bytes for tlm value
    	  this->tlmWrite_VOLTAGE_28V(voltage_tlm);
    	  U8 bat_therm = 0;
    	  memcpy(&voltage_tlm, watchdog_resp_add+12, 1);	// Copy a byte for thermistor value
    	  this->tlmWrite_BATTERY_THERMISTOR(bat_therm);
    	  int8_t sys_stat = 0;
    	  memcpy(&voltage_tlm, watchdog_resp_add+13, 1);	// Copy a byte for system status value
    	  this->tlmWrite_SYSTEM_STATUS(sys_stat);
		}
		else
		{
			this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
		}
    }
	// Check for any watchdog errors (NOTE: not sure that comm_error should be zero if no errors)
    else
    {
    	this->log_WARNING_HI_WatchDogCommError(comm_error);
    }
  }
  
  void WatchDogInterfaceComponentImpl ::
    downlink_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    uint8_t *buffer = reinterpret_cast<uint8_t *>(fwBuffer.getdata());
    uint16_t payloadSize = fwBuffer.getsize(); 
    // Send header of reset 0x0000 and UDP data size
    U32 downlink_header = 0x0000FFFF & payloadSize;
    linSend(linREG, (uint8_t *)&downlink_header);
    // Send UDP Data
    linSend(linREG, buffer);

    // Check for Response from MSP430 Watchdog
    U8 watchdog_reponse;
    linGetData(linREG, (uint8_t *)&watchdog_reponse);
    void* watchdog_resp_add = (uint8_t *)&watchdog_reponse;
    U8* watchdog_resp_add2 = (uint8_t *)&watchdog_reponse;

    U32 comm_error = linGetStatusFlag(linREG);
    U32 watchdog_resp_header = 0x00000000;
    // Check to make sure that we have recieved at least four bytes
    if(sizeof(*watchdog_resp_add2) >= 4)
        memcpy(&watchdog_resp_header, watchdog_resp_add, 4);	// Copy first four bytes of response
    else
    {
    	this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
    	return;
    }

    // Check for timeout of response
    if(comm_error == LIN_TO_INT)
    {
    	this->log_WARNING_HI_WatchDogTimedOut();
    }
    // Check that response is the same as what was sent
    else if(watchdog_resp_header != downlink_header)
    {
    	this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
    }
	// Check for any watchdog errors (NOTE: not sure that comm_error should be zero if no errors)
    else
    {
    	this->log_WARNING_HI_WatchDogCommError(comm_error);
    }
  }
  
  void WatchDogInterfaceComponentImpl ::
    PingIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    this->PingOut_out(0, key);
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
    Reset_Specific_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U8 reset_value
    )
  {
  	// Send Activity Log/tlm to know watchdog recieved command
  	char command_type[32] = "Reset Specific:";
  	char reset_val_char[8];
  	sprintf(reset_val_char, "%u", reset_val_char);
  	strcat(command_type, reset_val_char);
  	Fw::LogStringArg command_type_log = command_type;
  	Fw::TlmString command_type_tlm = command_type;
  	this->log_ACTIVITY_HI_WatchDogCmdReceived(command_type_log);
  	this->tlmWrite_LAST_COMMAND(command_type_tlm);

    // Sends a command to watchdog to reset specified devices. Can be hardware through watchdog or component

    // If reset_value is greater than 0x1B, we are resetting a software component
    if(reset_value > 0x1B)
    {
      // Reset Components in software
      // TODO
    }
    // If reset_value less than or equal to 0x1B, we are resetting hardware
    else
    {
      // Copy reset value and shift it left by 4 bytes to get 0x0000 as our first byte and reset_value as our second byte
      U32 watchdog_reset_val = (static_cast<U32>(reset_value) << 16);
      // Send watchdog_reset to MSP430 watchdog
      linSend(linREG, (uint8_t *)&watchdog_reset_val);

	  // Check for Response from MSP430 Watchdog
	  U8 watchdog_reponse;
      linGetData(linREG, (uint8_t *)&watchdog_reponse);
      void* watchdog_resp_add = (uint8_t *)&watchdog_reponse;
      U8* watchdog_resp_add2 = (uint8_t *)&watchdog_reponse;

      U32 comm_error = linGetStatusFlag(linREG);
      U32 watchdog_resp_header = 0x00000000;
      // Check to make sure that we have recieved at least four bytes
      if(sizeof(*watchdog_resp_add2) >= 4)
          memcpy(&watchdog_resp_header, watchdog_resp_add, 4);	// Copy first four bytes of response
	  else
      {
    	  this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
    	  this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
    	  return;
      }

	  // Check for timeout of response
	  if(comm_error == LIN_TO_INT)
	  {
	    this->log_WARNING_HI_WatchDogTimedOut();
	  }
	  // Check that response is the same as what was sent
      else if(watchdog_resp_header != watchdog_reset_val)
	  {
	    // Check if the watchdog response includes UDP data
    	if((watchdog_resp_header & 0x0000) == watchdog_reset_val) 
    	{
    		// Forward all data after header and current data to ground interface
    		U64 UDP_ptr = (U64)(watchdog_resp_add+4); // Create a pointer to start of the UDP data 32 bits after start of watchdog_response
    		uint16_t UDP_size = (U16)(watchdog_resp_header);
    		Fw::Buffer recv_Buffer(this->getInstance(), 0, UDP_ptr, UDP_size); 
    		uplink_out(0, recv_Buffer);
    	}
    	else
    		this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
	  }
	  else if(watchdog_resp_header == watchdog_reset_val)
      {
      	//Foward telemetry data
      	
      	if(sizeof(*watchdog_resp_add2) >= 14)
      	{
      	  int16_t voltage_tlm = 0;
      	  memcpy(&voltage_tlm, watchdog_resp_add+4, 2);	// Copy two bytes for tlm value
    	  this->tlmWrite_VOLTAGE_2_5V(voltage_tlm);
    	  memcpy(&voltage_tlm, watchdog_resp_add+6, 2);	// Copy two bytes for tlm value
    	  this->tlmWrite_VOLTAGE_2_8V(voltage_tlm);
    	  memcpy(&voltage_tlm, watchdog_resp_add+8, 2);	// Copy two bytes for tlm value
    	  this->tlmWrite_VOLTAGE_24V(voltage_tlm);
    	  memcpy(&voltage_tlm, watchdog_resp_add+10, 2);	// Copy two bytes for tlm value
    	  this->tlmWrite_VOLTAGE_28V(voltage_tlm);
    	  U8 bat_therm = 0;
    	  memcpy(&voltage_tlm, watchdog_resp_add+12, 1);	// Copy a byte for thermistor value
    	  this->tlmWrite_BATTERY_THERMISTOR(bat_therm);
    	  int8_t sys_stat = 0;
    	  memcpy(&voltage_tlm, watchdog_resp_add+13, 1);	// Copy a byte for system status value
    	  this->tlmWrite_SYSTEM_STATUS(sys_stat);
		}
		else
		{
			this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
		}
		
      }
	  // Check for any watchdog errors (NOTE: not sure that comm_error should be zero if no errors)
	  else
	  {
	  	this->log_WARNING_HI_WatchDogCommError(comm_error);
	  }
    }
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void WatchDogInterfaceComponentImpl ::
    Disengage_From_Lander_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
      U32 watchdog_reset_val = 0x00EE0000;
      // Send watchdog_reset to MSP430 watchdog
      linSend(linREG, (uint8_t *)&watchdog_reset_val);

	  // Check for Response from MSP430 Watchdog
	  U8 watchdog_reponse;
      linGetData(linREG, (uint8_t *)&watchdog_reponse);
      void* watchdog_resp_add = (uint8_t *)&watchdog_reponse;
      U8* watchdog_resp_add2 = (uint8_t *)&watchdog_reponse;

      U32 comm_error = linGetStatusFlag(linREG);
      U32 watchdog_resp_header = 0x00000000;
      // Check to make sure that we have recieved at least four bytes
      if(sizeof(*watchdog_resp_add2) >= 4)
          memcpy(&watchdog_resp_header, watchdog_resp_add, 4);	// Copy first four bytes of response
	  else
      {
    	 this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
    	 this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
    	 return;
      }

	  // Check for timeout of response
	  if(comm_error == LIN_TO_INT)
	  {
	    this->log_WARNING_HI_WatchDogTimedOut();
	  }
	  // Check that response is the same as what was sent
      else if(watchdog_resp_header != watchdog_reset_val)
	  {
	    // Check if the watchdog response includes UDP data
    	if((watchdog_resp_header & 0x0000) == watchdog_reset_val) 
    	{
    		// Forward all data after header and current data to ground interface
    		U64 UDP_ptr = (U64)(watchdog_resp_add+4); // Create a pointer to start of the UDP data 32 bits after start of watchdog_response
    		uint16_t UDP_size = (U16)(watchdog_resp_header);
    		Fw::Buffer recv_Buffer(this->getInstance(), 0, UDP_ptr, UDP_size); 
    		uplink_out(0, recv_Buffer);
    	}
    	else
    		this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
	  }
	  else if(watchdog_resp_header == watchdog_reset_val)
      {
    	  //Foward telemetry data
      	      	
      	if(sizeof(*watchdog_resp_add2) >= 14)
      	{
      	  int16_t voltage_tlm = 0;
      	  memcpy(&voltage_tlm, watchdog_resp_add+4, 2);	// Copy two bytes for tlm value
    	  this->tlmWrite_VOLTAGE_2_5V(voltage_tlm);
    	  memcpy(&voltage_tlm, watchdog_resp_add+6, 2);	// Copy two bytes for tlm value
    	  this->tlmWrite_VOLTAGE_2_8V(voltage_tlm);
    	  memcpy(&voltage_tlm, watchdog_resp_add+8, 2);	// Copy two bytes for tlm value
    	  this->tlmWrite_VOLTAGE_24V(voltage_tlm);
    	  memcpy(&voltage_tlm, watchdog_resp_add+10, 2);	// Copy two bytes for tlm value
    	  this->tlmWrite_VOLTAGE_28V(voltage_tlm);
    	  U8 bat_therm = 0;
    	  memcpy(&voltage_tlm, watchdog_resp_add+12, 1);	// Copy a byte for thermistor value
    	  this->tlmWrite_BATTERY_THERMISTOR(bat_therm);
    	  int8_t sys_stat = 0;
    	  memcpy(&voltage_tlm, watchdog_resp_add+13, 1);	// Copy a byte for system status value
    	  this->tlmWrite_SYSTEM_STATUS(sys_stat);
		}
		else
		{
			this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
		}
      }
	  // Check for any watchdog errors (NOTE: not sure that comm_error should be zero if no errors)
	  else
	  {
	  	this->log_WARNING_HI_WatchDogCommError(comm_error);
	  }

    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }
} // end namespace CubeRover
