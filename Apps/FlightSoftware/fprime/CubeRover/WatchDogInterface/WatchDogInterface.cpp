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
#include <Drv/FreeRtosSerialDriver/FreeRtosSerialDriverComponentImpl.hpp>
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
    // Setup scilinReg port
    sciBASE_t * m_sci = scilinREG;
    sciEnterResetState(m_sci);
    sciSetBaudrate(m_sci, 9600);
    sciExitResetState(m_sci);

    // RESET THE WIFI CHIP ON STARTUP
    Reset_Specific_initHandler(0x04 /*RESET WIFI*/);
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

    // Update Thermistor Telemetry
    Read_Temp();

    // Sends U32 to the watchdog as defined in design document. Checks stroke value as to what to send watchdog
    U32 watchdog_stroke = 0x00000000;
    // Send stroke once Tx ready
    int tries = 10;
    while(--tries && !sciIsTxReady(scilinREG));
    if(tries == 0)
    {
    	this->log_WARNING_HI_WatchDogTimedOut();
    	return;
    }

    // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
    if(!Send_Frame(header))
      return;

    sciSend(scilinREG, sizeof(watchdog_stroke), (uint8_t *)&watchdog_stroke);

    if(!Send_Frame(footer))
      return;

    // Check for Response from MSP430 Watchdog
    U32 watchdog_reponse;
    //Blocking until timeout or data available
    int32_t size_read = 0;
    tries = 10;
    while(--tries && !sciIsRxReady(scilinREG));
    if(tries == 0)
    {
    	this->log_WARNING_HI_WatchDogTimedOut();
    	return;
    }
    size_read = sciReceiveWithTimeout(scilinREG,
                                 sizeof(watchdog_reponse),
                                 (uint8_t *)&watchdog_reponse,
                                 0x00002710); /*10 second timeout*/

    U32 comm_error = sciRxError(scilinREG);

    // Good read:
    if (size_read > 0)
    {
        if(size_read < 4)
        {
            this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
            return;
        }
        // Check that response is the same as what was sent
        else if(watchdog_reponse != watchdog_stroke)
        {
            // Check if the watchdog response includes UDP data
            if((uint16_t)(watchdog_reponse) != 0x0000)
            {
                // Forward all data after header and current data to ground interface
                // Blocking until timeout or data available
                Fw::Buffer buff;
                int stat = 0;
                uint16_t UDP_size = (U16)(watchdog_reponse);
                tries = 10;
                while(--tries && !sciIsRxReady(scilinREG));
                if(tries == 0)
			    {
			    	this->log_WARNING_HI_WatchDogTimedOut();
			    	return;
			    }
                stat = sciReceiveWithTimeout(scilinREG,
                                             UDP_size,
                                             reinterpret_cast<unsigned char*>(buff.getdata()),
                                             0x00002710); /*10 second timeout*/
                comm_error = sciRxError(scilinREG);
                // Good read:
                if (stat > 0)
                {
                   uplink_out(0, buff);
                }
                // check for timeout
                if (stat == 0)
                {
                    this->log_WARNING_HI_WatchDogTimedOut();
                }
                // Return other Error
                else
                {
                   this->log_WARNING_HI_WatchDogCommError(comm_error);
                }
            }
            else
                this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
        }
        else if(watchdog_reponse == watchdog_stroke)
        {
            //Foward telemetry data
            //Blocking until timeout or data available
            U16 buff[5];
            int stat = 0;
            tries = 10;
            while(--tries && !sciIsRxReady(scilinREG));
            if(tries == 0)
			{
			    this->log_WARNING_HI_WatchDogTimedOut();
			    return;
			}
            stat = sciReceiveWithTimeout(scilinREG,
                                         10,	//10 bytes is how big total telemetry should be
                                         (uint8_t *)&buff,
                                         0x00002710); /*10 second timeout*/
            comm_error = sciRxError(scilinREG);
            // Good read:
            if (stat >= 10)
            {
                int16_t voltage_tlm = 0;
                memcpy(&voltage_tlm, &(buff), 2); // Copy two bytes for tlm value
                this->tlmWrite_VOLTAGE_2_5V(voltage_tlm);
                memcpy(&voltage_tlm, &(buff)+2, 2); // Copy two bytes for tlm value
                this->tlmWrite_VOLTAGE_2_8V(voltage_tlm);
                memcpy(&voltage_tlm, &(buff)+4, 2); // Copy two bytes for tlm value
                this->tlmWrite_VOLTAGE_24V(voltage_tlm);
                memcpy(&voltage_tlm, &(buff)+6, 2);    // Copy two bytes for tlm value
                this->tlmWrite_VOLTAGE_28V(voltage_tlm);
                U8 bat_therm = 0;
                memcpy(&bat_therm, &(buff)+8, 1);    // Copy a byte for thermistor value
                this->tlmWrite_BATTERY_THERMISTOR(bat_therm);
                int8_t sys_stat = 0;
                memcpy(&sys_stat, &(buff)+9, 1);    // Copy a byte for system status value
                this->tlmWrite_SYSTEM_STATUS(sys_stat);
             }
             // check for timeout
             if (stat == 0)
             {
                 this->log_WARNING_HI_WatchDogTimedOut();
             }
             // Return other Error
             else
             {
                this->log_WARNING_HI_WatchDogCommError(comm_error);
             }
        }
    }
    // check for timeout
    if (size_read == 0)
    {
        this->log_WARNING_HI_WatchDogTimedOut();
    }
    // quit if other error or data
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
    uint16_t payloadSize = fwBuffer.getsize(); 
    // Send header of reset 0x0000 and UDP data size
    U32 watchdog_stroke = 0x0000FFFF & payloadSize;
    // Send stroke once Tx ready

    int tries = 10;
    while(--tries && !sciIsTxReady(scilinREG));
    if(tries == 0)
    {
    	this->log_WARNING_HI_WatchDogTimedOut();
    	return;
    }

    // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
    if(!Send_Frame(header))
      return;

    sciSend(scilinREG, sizeof(watchdog_stroke), (uint8_t *)&watchdog_stroke);

    // Check for Response from MSP430 Watchdog
    U32 watchdog_reponse;
    //Blocking until timeout or data available
    int32_t size_read = 0;
    tries = 10;
    while(--tries && !sciIsRxReady(scilinREG));
    if(tries == 0)
    {
    	this->log_WARNING_HI_WatchDogTimedOut();
    	return;
    }
    size_read = sciReceiveWithTimeout(scilinREG,
                                 sizeof(watchdog_reponse),
                                 (uint8_t *)&watchdog_reponse,
                                 0x00002710); /*10 second timeout*/

    U32 comm_error = sciRxError(scilinREG);

    // Good read:
    if (size_read > 0)
    {
        if(size_read < 4)
        {
            this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
            return;
        }
        // Check that response is the same as what was sent
        else if(watchdog_reponse != watchdog_stroke)
        {
            this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
        }
        else if(watchdog_reponse == watchdog_stroke)
        {
            tries = 10;
    		    while(--tries && !sciIsTxReady(scilinREG));
    		    if(tries == 0)
    		    {
    		    	this->log_WARNING_HI_WatchDogTimedOut();
    		    	return;
    		    }
    		    sciSend(scilinREG, payloadSize, reinterpret_cast<unsigned char*>(fwBuffer.getdata()));

            if(!Send_Frame(footer))
              return;
        }
    }
    // check for timeout
    if (size_read == 0)
    {
        this->log_WARNING_HI_WatchDogTimedOut();
    }
    // quit if other error or data
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
  	char command_type[24] = "Reset Specific:";
  	char reset_val_char[8];
  	sprintf(reset_val_char, "%u", reset_value);
  	strcat(command_type, reset_val_char);
  	Fw::LogStringArg command_type_log = command_type;
  	//Fw::TlmString command_type_tlm = command_type;
  	this->log_ACTIVITY_HI_WatchDogCmdReceived(command_type_log);
  	//this->tlmWrite_LAST_COMMAND(command_type_tlm);

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
      	U32 watchdog_stroke = (static_cast<U32>(reset_value) << 16);
	    // Send stroke once Tx ready

	    int tries = 10;
	    while(--tries && !sciIsTxReady(scilinREG));
	    if(tries == 0)
	    {
	    	this->log_WARNING_HI_WatchDogTimedOut();
	    	this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_BUSY);
	    	return;
	    }

      // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
      if(!Send_Frame(header))
        return;

	    sciSend(scilinREG, sizeof(watchdog_stroke), (uint8_t *)&watchdog_stroke);

      if(!Send_Frame(footer))
      return;

	    // Check for Response from MSP430 Watchdog
	    U32 watchdog_reponse;
	    //Blocking until timeout or data available
	    int32_t size_read = 0;
	    tries = 10;
	    while(--tries && !sciIsRxReady(scilinREG));
	    if(tries == 0)
	    {
	    	this->log_WARNING_HI_WatchDogTimedOut();
	    	this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_BUSY);
	    	return;
	    }
	    size_read = sciReceiveWithTimeout(scilinREG,
	                                 sizeof(watchdog_reponse),
	                                 (uint8_t *)&watchdog_reponse,
	                                 0x00002710); /*10 second timeout*/

	    U32 comm_error = sciRxError(scilinREG);

	    // Good read:
	    if (size_read > 0)
	    {
	        if(size_read < 4)
	        {
	            this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
	            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
	            return;
	        }
	        // Check that response is the same as what was sent
	        else if(watchdog_reponse != watchdog_stroke)
	        {
	            // Check if the watchdog response includes UDP data
	            if((uint16_t)(watchdog_reponse) != 0x0000)
	            {
	                // Forward all data after header and current data to ground interface
	                // Blocking until timeout or data available
	                Fw::Buffer buff;
	                int stat = 0;
	                uint16_t UDP_size = (U16)(watchdog_reponse);
	                tries = 10;
	                while(--tries && !sciIsRxReady(scilinREG));
	                if(tries == 0)
				    {
				    	this->log_WARNING_HI_WatchDogTimedOut();
				    	this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_BUSY);
				    	return;
				    }
	                stat = sciReceiveWithTimeout(scilinREG,
	                                             UDP_size,
	                                             reinterpret_cast<unsigned char*>(buff.getdata()),
	                                             0x00002710); /*10 second timeout*/
	                comm_error = sciRxError(scilinREG);
	                // Good read:
	                if (stat > 0)
	                {
	                   uplink_out(0, buff);
	                }
	                // check for timeout
	                if (stat == 0)
	                {
	                    this->log_WARNING_HI_WatchDogTimedOut();
	                }
	                // Return other Error
	                else
	                {
	                   this->log_WARNING_HI_WatchDogCommError(comm_error);
	                }
	            }
	            else
	                this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
	        }
	        else if(watchdog_reponse == watchdog_stroke)
	        {
	            //Foward telemetry data
	            //Blocking until timeout or data available
	            U16 buff[5];
	            int stat = 0;
	            tries = 10;
	            while(--tries && !sciIsRxReady(scilinREG));
	            if(tries == 0)
				{
				    this->log_WARNING_HI_WatchDogTimedOut();
				    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_BUSY);
				    return;
				}
	            stat = sciReceiveWithTimeout(scilinREG,
	                                         10,	//10 bytes is how big total telemetry should be
	                                         (uint8_t *)&buff,
	                                         0x00002710); /*10 second timeout*/
	            comm_error = sciRxError(scilinREG);
	            // Good read:
	            if (stat >= 10)
	            {
	                int16_t voltage_tlm = 0;
	                memcpy(&voltage_tlm, &(buff), 2); // Copy two bytes for tlm value
	                this->tlmWrite_VOLTAGE_2_5V(voltage_tlm);
	                memcpy(&voltage_tlm, &(buff)+2, 2); // Copy two bytes for tlm value
	                this->tlmWrite_VOLTAGE_2_8V(voltage_tlm);
	                memcpy(&voltage_tlm, &(buff)+4, 2); // Copy two bytes for tlm value
	                this->tlmWrite_VOLTAGE_24V(voltage_tlm);
	                memcpy(&voltage_tlm, &(buff)+6, 2);    // Copy two bytes for tlm value
	                this->tlmWrite_VOLTAGE_28V(voltage_tlm);
	                U8 bat_therm = 0;
	                memcpy(&bat_therm, &(buff)+8, 1);    // Copy a byte for thermistor value
	                this->tlmWrite_BATTERY_THERMISTOR(bat_therm);
	                int8_t sys_stat = 0;
	                memcpy(&sys_stat, &(buff)+9, 1);    // Copy a byte for system status value
	                this->tlmWrite_SYSTEM_STATUS(sys_stat);
	             }
	             // check for timeout
	             if (stat == 0)
	             {
	                 this->log_WARNING_HI_WatchDogTimedOut();
	             }
	             // Return other Error
	             else
	             {
	                this->log_WARNING_HI_WatchDogCommError(comm_error);
	             }
	        }
	    }
	    // check for timeout
	    if (size_read == 0)
	    {
	        this->log_WARNING_HI_WatchDogTimedOut();
	    }
	    // quit if other error or data
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
	  	// Send Activity Log/tlm to know watchdog recieved command
	  	char command_type[24] = "Disengage From Rover";
	  	Fw::LogStringArg command_type_log = command_type;
	  	//Fw::TlmString command_type_tlm = command_type;
	  	this->log_ACTIVITY_HI_WatchDogCmdReceived(command_type_log);
	  	//this->tlmWrite_LAST_COMMAND(command_type_tlm);

      U32 watchdog_stroke = 0x00EE0000;
	    // Send stroke once Tx ready

	    int tries = 10;
	    while(--tries && !sciIsTxReady(scilinREG));
	    if(tries == 0)
	    {
	    	this->log_WARNING_HI_WatchDogTimedOut();
	    	this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_BUSY);
	    	return;
	    }

      // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
      if(!Send_Frame(header))
        return;

	    sciSend(scilinREG, sizeof(watchdog_stroke), (uint8_t *)&watchdog_stroke);

      if(!Send_Frame(footer))
      return;

	    // Check for Response from MSP430 Watchdog
	    U32 watchdog_reponse;
	    //Blocking until timeout or data available
	    int32_t size_read = 0;
	    tries = 10;
	    while(--tries && !sciIsRxReady(scilinREG));
	    if(tries == 0)
	    {
	    	this->log_WARNING_HI_WatchDogTimedOut();
	    	this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_BUSY);
	    	return;
	    }
	    size_read = sciReceiveWithTimeout(scilinREG,
	                                 sizeof(watchdog_reponse),
	                                 (uint8_t *)&watchdog_reponse,
	                                 0x00002710); /*10 second timeout*/

	    U32 comm_error = sciRxError(scilinREG);

	    // Good read:
	    if (size_read > 0)
	    {
	        if(size_read < 4)
	        {
	            this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
	            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
	            return;
	        }
	        // Check that response is the same as what was sent
	        else if(watchdog_reponse != watchdog_stroke)
	        {
	            // Check if the watchdog response includes UDP data
	            if((uint16_t)(watchdog_reponse) != 0x0000)
	            {
	                // Forward all data after header and current data to ground interface
	                // Blocking until timeout or data available
	                Fw::Buffer buff;
	                int stat = 0;
	                uint16_t UDP_size = (U16)(watchdog_reponse);
	                tries = 10;
	                while(--tries && !sciIsRxReady(scilinREG));
	                if(tries == 0)
				    {
				    	this->log_WARNING_HI_WatchDogTimedOut();
				    	this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_BUSY);
				    	return;
				    }
	                stat = sciReceiveWithTimeout(scilinREG,
	                                             UDP_size,
	                                             reinterpret_cast<unsigned char*>(buff.getdata()),
	                                             0x00002710); /*10 second timeout*/
	                comm_error = sciRxError(scilinREG);
	                // Good read:
	                if (stat > 0)
	                {
	                   uplink_out(0, buff);
	                }
	                // check for timeout
	                if (stat == 0)
	                {
	                    this->log_WARNING_HI_WatchDogTimedOut();
	                }
	                // Return other Error
	                else
	                {
	                   this->log_WARNING_HI_WatchDogCommError(comm_error);
	                }
	            }
	            else
	                this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
	        }
	        else if(watchdog_reponse == watchdog_stroke)
	        {
	            //Foward telemetry data
	            //Blocking until timeout or data available
	            U16 buff[5];
	            int stat = 0;
	            tries = 10;
	            while(--tries && !sciIsRxReady(scilinREG));
	            if(tries == 0)
				{
				    this->log_WARNING_HI_WatchDogTimedOut();
				    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_BUSY);
				    return;
				}
	            stat = sciReceiveWithTimeout(scilinREG,
	                                         10,	//10 bytes is how big total telemetry should be
	                                         (uint8_t *)&buff,
	                                         0x00002710); /*10 second timeout*/
	            comm_error = sciRxError(scilinREG);
	            // Good read:
	            if (stat >= 10)
	            {
	                int16_t voltage_tlm = 0;
	                memcpy(&voltage_tlm, &(buff), 2); // Copy two bytes for tlm value
	                this->tlmWrite_VOLTAGE_2_5V(voltage_tlm);
	                memcpy(&voltage_tlm, &(buff)+2, 2); // Copy two bytes for tlm value
	                this->tlmWrite_VOLTAGE_2_8V(voltage_tlm);
	                memcpy(&voltage_tlm, &(buff)+4, 2); // Copy two bytes for tlm value
	                this->tlmWrite_VOLTAGE_24V(voltage_tlm);
	                memcpy(&voltage_tlm, &(buff)+6, 2);    // Copy two bytes for tlm value
	                this->tlmWrite_VOLTAGE_28V(voltage_tlm);
	                U8 bat_therm = 0;
	                memcpy(&bat_therm, &(buff)+8, 1);    // Copy a byte for thermistor value
	                this->tlmWrite_BATTERY_THERMISTOR(bat_therm);
	                int8_t sys_stat = 0;
	                memcpy(&sys_stat, &(buff)+9, 1);    // Copy a byte for system status value
	                this->tlmWrite_SYSTEM_STATUS(sys_stat);
	             }
	             // check for timeout
	             if (stat == 0)
	             {
	                 this->log_WARNING_HI_WatchDogTimedOut();
	             }
	             // Return other Error
	             else
	             {
	                this->log_WARNING_HI_WatchDogCommError(comm_error);
	             }
	        }
	    }
	    // check for timeout
	    if (size_read == 0)
	    {
	        this->log_WARNING_HI_WatchDogTimedOut();
	    }
	    // quit if other error or data
	    else
	    {
	        this->log_WARNING_HI_WatchDogCommError(comm_error);
	    }
	this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }


  bool WatchDogInterfaceComponentImpl ::
    Reset_Specific_initHandler(
        U8 reset_value
    )
  {
    // Send Activity Log/tlm to know watchdog recieved command
    char command_type[24] = "Reset Specific:";
    char reset_val_char[8];
    sprintf(reset_val_char, "%u", reset_value);
    strcat(command_type, reset_val_char);
    Fw::LogStringArg command_type_log = command_type;
    //Fw::TlmString command_type_tlm = command_type;
    this->log_ACTIVITY_HI_WatchDogCmdReceived(command_type_log);
    //this->tlmWrite_LAST_COMMAND(command_type_tlm);

    // Sends a command to watchdog to reset specified devices. Can be hardware through watchdog or component

    // If reset_value is greater than 0x1B, we are resetting a software component
    if(reset_value > 0x1B)
    {
      // Reset Components in software
      // TODO
      return true;
    }
    // If reset_value less than or equal to 0x1B, we are resetting hardware
    else
    {
      // Copy reset value and shift it left by 4 bytes to get 0x0000 as our first byte and reset_value as our second byte
      U32 watchdog_stroke = (static_cast<U32>(reset_value) << 16);
      
      // Send stroke once Tx ready
      int tries = 10;
      while(--tries && !sciIsTxReady(scilinREG));
      if(tries == 0)
      {
        this->log_WARNING_HI_WatchDogTimedOut();
        return false;
      }

      // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
      if(!Send_Frame(header))
        return false;
      
      sciSend(scilinREG, sizeof(watchdog_stroke), (uint8_t *)&watchdog_stroke);

      if(!Send_Frame(footer))
      return;

      // Check for Response from MSP430 Watchdog
      U32 watchdog_reponse;
      //Blocking until timeout or data available
      int32_t size_read = 0;
      tries = 10;
      while(--tries && !sciIsRxReady(scilinREG));
      if(tries == 0)
      {
        this->log_WARNING_HI_WatchDogTimedOut();
        return false;
      }
      size_read = sciReceiveWithTimeout(scilinREG,
                                   sizeof(watchdog_reponse),
                                   (uint8_t *)&watchdog_reponse,
                                   0x00002710); /*10 second timeout*/

      U32 comm_error = sciRxError(scilinREG);

      // Good read:
      if (size_read > 0)
      {
          if(size_read < 4)
          {
              this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
              return false;
          }
          // Check that response is the same as what was sent
          else if(watchdog_reponse != watchdog_stroke)
          {
              // Check if the watchdog response includes UDP data
              if((uint16_t)(watchdog_reponse) != 0x0000)
              {
                  // Forward all data after header and current data to ground interface
                  // Blocking until timeout or data available
                  Fw::Buffer buff;
                  int stat = 0;
                  uint16_t UDP_size = (U16)(watchdog_reponse);
                  tries = 10;
                  while(--tries && !sciIsRxReady(scilinREG));
                  if(tries == 0)
                  {
                    this->log_WARNING_HI_WatchDogTimedOut();
                    return false;
                  }
                  stat = sciReceiveWithTimeout(scilinREG,
                                               UDP_size,
                                               reinterpret_cast<unsigned char*>(buff.getdata()),
                                               0x00002710); /*10 second timeout*/
                  comm_error = sciRxError(scilinREG);
                  // Good read:
                  if (stat > 0)
                  {
                     uplink_out(0, buff);
                  }
                  // check for timeout
                  if (stat == 0)
                  {
                      this->log_WARNING_HI_WatchDogTimedOut();
                      return false;
                  }
                  // Return other Error
                  else
                  {
                     this->log_WARNING_HI_WatchDogCommError(comm_error);
                     return false;
                  }
              }
              else
              {
                  this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
                  return false;
              }
          }
          else if(watchdog_reponse == watchdog_stroke)
          {
              //Foward telemetry data
              //Blocking until timeout or data available
              U16 buff[5];
              int stat = 0;
              tries = 10;
              while(--tries && !sciIsRxReady(scilinREG));
              if(tries == 0)
              {
                  this->log_WARNING_HI_WatchDogTimedOut();
                  return false;
              }
              stat = sciReceiveWithTimeout(scilinREG,
                                           10,  //10 bytes is how big total telemetry should be
                                           (uint8_t *)&buff,
                                           0x00002710); /*10 second timeout*/
              comm_error = sciRxError(scilinREG);
              // Good read:
              if (stat >= 10)
              {
                  int16_t voltage_tlm = 0;
                  memcpy(&voltage_tlm, &(buff), 2); // Copy two bytes for tlm value
                  this->tlmWrite_VOLTAGE_2_5V(voltage_tlm);
                  memcpy(&voltage_tlm, &(buff)+2, 2); // Copy two bytes for tlm value
                  this->tlmWrite_VOLTAGE_2_8V(voltage_tlm);
                  memcpy(&voltage_tlm, &(buff)+4, 2); // Copy two bytes for tlm value
                  this->tlmWrite_VOLTAGE_24V(voltage_tlm);
                  memcpy(&voltage_tlm, &(buff)+6, 2);    // Copy two bytes for tlm value
                  this->tlmWrite_VOLTAGE_28V(voltage_tlm);
                  U8 bat_therm = 0;
                  memcpy(&bat_therm, &(buff)+8, 1);    // Copy a byte for thermistor value
                  this->tlmWrite_BATTERY_THERMISTOR(bat_therm);
                  int8_t sys_stat = 0;
                  memcpy(&sys_stat, &(buff)+9, 1);    // Copy a byte for system status value
                  this->tlmWrite_SYSTEM_STATUS(sys_stat);
               }
               // check for timeout
               if (stat == 0)
               {
                   this->log_WARNING_HI_WatchDogTimedOut();
                   return false;
               }
               // Return other Error
               else
               {
                  this->log_WARNING_HI_WatchDogCommError(comm_error);
                  return false;
               }
          }
      }
      // check for timeout
      if (size_read == 0)
      {
          this->log_WARNING_HI_WatchDogTimedOut();
          return false;
      }
      // quit if other error or data
      else
      {
          this->log_WARNING_HI_WatchDogCommError(comm_error);
          return false;
      }
      return true;
    }
  }

  bool WatchDogInterfaceComponentImpl :: Send_Frame(frame_type frame_input)
  {
      //Create Value for frame
      U32 frame;
      if(frame_input == header)
        frame = 0x2A465F8B;
      else
        frame = 0xB8F564A2;

      int tries = 10;
      while(--tries && !sciIsTxReady(scilinREG));
      if(tries == 0)
      {
        this->log_WARNING_HI_WatchDogTimedOut();
        return false;
      }
      sciSend(scilinREG, sizeof(frame), (uint8_t *)&frame);

      // Check for Response from MSP430 Watchdog
      U32 frame_reponse;
      //Blocking until timeout or data available
      int32_t size_read = 0;
      tries = 10;
      while(--tries && !sciIsRxReady(scilinREG));
      if(tries == 0)
      {
        this->log_WARNING_HI_WatchDogTimedOut();
        return false;
      }
      size_read = sciReceiveWithTimeout(scilinREG,
                                   sizeof(frame_reponse),
                                   (uint8_t *)&frame_reponse,
                                   0x00002710); /*10 second timeout*/

      U32 comm_error = sciRxError(scilinREG);

      // Good read:
      if (size_read > 0)
      {
        if(frame != frame_reponse)
        {
          this->log_WARNING_HI_WatchDogMSP430IncorrectResp();
          return false;
        }
        else
          return true;
      }
      // check for timeout
      if (size_read == 0)
      {
          this->log_WARNING_HI_WatchDogTimedOut();
          return false;
      }
      // quit if other error or data
      else
      {
          this->log_WARNING_HI_WatchDogCommError(comm_error);
          return false;
      }

      // Return false by default, should never get to this point
      return false;
  }

  bool WatchDogInterfaceComponentImpl :: Read_Temp()
  {
    // Start ADC Conversions for all thermistors
    adcStartConversion(adcREG1, adcGROUP1);

    // Check if all ADC Conversions are done
    int tries = 10;
    while(--tries && !adcIsConversionComplete(adcREG1, adcGROUP1));
    if(tries == 0)
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

      // Create char array of size 24 for all possible Thermistor Values. Size is 24 as 12-bit conversion, 12 bits -> 1.5 bytes, 16 thermistors * 1.5 bytes = 24 bytes. Char is 1 byte
      char data[24];
      adcData_t* data_ptr = &data;

      // adcGetData returns how many conversions happened, saves data into data_ptr
      U32 num_conversions = adcGetData(adcREG1, adcGROUP1, data_ptr);

      if(num_conversions >= 6)
      {
        // Report tempurature as telemetry
        U16 temp_data;    // Create temp var to hold 12 bit (must use 16 bits in this case) thermistor value

        // Send Thermistor 12 bit values to Telemetry
        memcpy(&temp_data, &(data)+(12*0), 2);   // Copy two bytes for thermistor value
        temp_data = temp_data & 0x0FFF;       // Remove last 4 bits as are from other thermistor value
        this->tlmWrite_THERM_0(temp_data);

        memcpy(&temp_data, &(data)+(12*1), 2);   // Copy two bytes for thermistor value
        temp_data = temp_data & 0x0FFF;       // Remove last 4 bits as are from other thermistor value
        this->tlmWrite_THERM_1(temp_data);

        memcpy(&temp_data, &(data)+(12*2), 2);   // Copy two bytes for thermistor value
        temp_data = temp_data & 0x0FFF;       // Remove last 4 bits as are from other thermistor value
        this->tlmWrite_THERM_2(temp_data);

        memcpy(&temp_data, &(data)+(12*3), 2);   // Copy two bytes for thermistor value
        temp_data = temp_data & 0x0FFF;       // Remove last 4 bits as are from other thermistor value
        this->tlmWrite_THERM_3(temp_data);

        memcpy(&temp_data, &(data)+(12*4), 2);   // Copy two bytes for thermistor value
        temp_data = temp_data & 0x0FFF;       // Remove last 4 bits as are from other thermistor value
        this->tlmWrite_THERM_4(temp_data);

        memcpy(&temp_data, &(data)+(12*5), 2);   // Copy two bytes for thermistor value
        temp_data = temp_data & 0x0FFF;       // Remove last 4 bits as are from other thermistor value
        this->tlmWrite_THERM_5(temp_data);
      }
      else
      {
        // Log Error in conversion from ADC
        this->log_WARNING_HI_ADCThermistorError();
        return false;
      }
    }
    return true;
  }
} // end namespace CubeRover
