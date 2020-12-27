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
    FwOpcodeType opCode = 0; // Put as zero but may need to be changed as unsure what the value should be
    U32 cmdSeq = 0; // Put as zero but may need to be changed
    Reset_Specific_cmdHandler(opCode, cmdSeq, 0x04 /*RESET WIFI*/);
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
} // end namespace CubeRover
