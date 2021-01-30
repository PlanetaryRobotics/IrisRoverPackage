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

    Read_Temp();

    Reset_Specific_Handler(0x04);    // Reset WF121
    for (unsigned i = 400000000; i; --i);
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

    // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
    if(!Send_Frame(watchdog_stroke))
      return;
     
    for(unsigned i = 100000000; i>0; --i); // Wait loop to make sure sci sends out all data

    // Receive frame back from MSP430
    U32 comm_error;
    alignas(8) watchdog_frame frame;
    int32_t size_read = Receive_Watchdog_Frame(&comm_error, &frame);

    // Good read:
    if (size_read > 0)
    {
        if(size_read < 8)
        {
            this->log_WARNING_HI_WatchDogIncorrectResp();
            return;
        }
        // Check that response is the same as what was sent
        else if(frame.stroke_data_size != (uint16_t)watchdog_stroke)
        {
            // Check if the watchdog response includes UDP data
            if(frame.stroke_data_size != 0x0000)
            {
                // Forward all data after header and current data to ground interface
                // Blocking until timeout or data available
                Fw::Buffer buff;
                int stat = 0;
                stat = sciReceiveWithTimeout(scilinREG,
                                             frame.stroke_data_size,
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
                this->log_WARNING_HI_WatchDogIncorrectResp();
        }
        else if(frame.stroke_data_size == (uint16_t)watchdog_stroke && frame.reset_val == (uint16_t)*((&watchdog_stroke)+2))
        {
            //Foward telemetry data
            //Blocking until timeout or data available
            U16 buff[6];
            int stat = 0;
            stat = sciReceiveWithTimeout(scilinREG,
                                         12,	//12 bytes is how big total telemetry should be
                                         (uint8_t *)&buff,
                                         0x00002710); /*10 second timeout*/
            comm_error = sciRxError(scilinREG);
            // Good read:
            if (stat >= 12)
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
                U16 bat_level = 0;
                memcpy(&bat_level, &(buff)+10, 2);    // Copy two bytes for battery voltage level value
                this->tlmWrite_BATTERY_LEVEL(bat_level);
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

    // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
    if(!Send_Frame(watchdog_stroke))
      return;
     
    for(unsigned i = 100000000; i>0; --i); // Wait loop to make sure sci sends out all data

    // Receive frame back from MSP430
    U32 comm_error;
    alignas(8) watchdog_frame frame;
    int32_t size_read = Receive_Watchdog_Frame(&comm_error, &frame);

    // Good read:
    if (size_read > 0)
    {
        if(size_read < 8)
        {
            this->log_WARNING_HI_WatchDogIncorrectResp();
            return;
        }
        // Check that response is the same as what was sent
        else if(frame.stroke_data_size != (uint16_t)watchdog_stroke)
        {
            this->log_WARNING_HI_WatchDogIncorrectResp();
        }
        else if(frame.stroke_data_size == (uint16_t)watchdog_stroke && frame.reset_val == (uint16_t)*((&watchdog_stroke)+2))
        {
            int tries = 100000000;
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

      	// Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
      	if(!Send_Frame(watchdog_stroke))
        	return;
	    
      for(unsigned i = 100000000; i>0; --i); // Wait loop to make sure sci sends out all data

      // Receive frame back from MSP430
      U32 comm_error;
      alignas(8) watchdog_frame frame;
      int32_t size_read = Receive_Watchdog_Frame(&comm_error, &frame);

	    // Good read:
	    if (size_read > 0)
	    {
	        if(size_read < 8)
	        {
	            this->log_WARNING_HI_WatchDogIncorrectResp();
	            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
	            return;
	        }
	        // Check that response is the same as what was sent
	        else if(frame.stroke_data_size != (uint16_t)watchdog_stroke)
	        {
	            // Check if the watchdog response includes UDP data
	            if(frame.stroke_data_size != 0x0000)
	            {
	                // Forward all data after header and current data to ground interface
	                // Blocking until timeout or data available
	                Fw::Buffer buff;
	                int stat = 0;
	                stat = sciReceiveWithTimeout(scilinREG,
	                                             frame.stroke_data_size,
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
	                this->log_WARNING_HI_WatchDogIncorrectResp();
	        }
	        else if(frame.stroke_data_size == (uint16_t)watchdog_stroke && frame.reset_val == (uint16_t)*((&watchdog_stroke)+2))
	        {
	            //Foward telemetry data
	            //Blocking until timeout or data available
	            U16 buff[6];
	            int stat = 0;
	            stat = sciReceiveWithTimeout(scilinREG,
	                                         12,	//12 bytes is how big total telemetry should be
	                                         (uint8_t *)&buff,
	                                         0x00002710); /*10 second timeout*/
	            comm_error = sciRxError(scilinREG);
	            // Good read:
	            if (stat >= 12)
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
                  U16 bat_level = 0;
                  memcpy(&bat_level, &(buff)+10, 2);    // Copy two bytes for battery voltage level value
                  this->tlmWrite_BATTERY_LEVEL(bat_level);
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

      // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
	    if(!Send_Frame(watchdog_stroke))
	      return;
	     
      for(unsigned i = 100000000; i>0; --i); // Wait loop to make sure sci sends out all data

      // Receive frame back from MSP430
      U32 comm_error;
      alignas(8) watchdog_frame frame;
      int32_t size_read = Receive_Watchdog_Frame(&comm_error, &frame);

	    // Good read:
	    if (size_read > 0)
	    {
	        if(size_read < 8)
	        {
	            this->log_WARNING_HI_WatchDogIncorrectResp();
	            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
	            return;
	        }
	        // Check that response is the same as what was sent
	        else if(frame.stroke_data_size != (uint16_t)watchdog_stroke)
	        {
	            // Check if the watchdog response includes UDP data
	            if(frame.stroke_data_size != 0x0000)
	            {
	                // Forward all data after header and current data to ground interface
	                // Blocking until timeout or data available
	                Fw::Buffer buff;
	                int stat = 0;
	                stat = sciReceiveWithTimeout(scilinREG,
	                                             frame.stroke_data_size,
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
	                this->log_WARNING_HI_WatchDogIncorrectResp();
	        }
	        else if(frame.stroke_data_size == (uint16_t)watchdog_stroke && frame.reset_val == (uint16_t)*((&watchdog_stroke)+2))
	        {
	            //Foward telemetry data
	            //Blocking until timeout or data available
	            U16 buff[6];
	            int stat = 0;
	            stat = sciReceiveWithTimeout(scilinREG,
	                                         12,	//12 bytes is how big total telemetry should be
	                                         (uint8_t *)&buff,
	                                         0x00002710); /*10 second timeout*/
	            comm_error = sciRxError(scilinREG);
	            // Good read:
	            if (stat >= 12)
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
                  U16 bat_level = 0;
                  memcpy(&bat_level, &(buff)+10, 2);    // Copy two bytes for battery voltage level value
                  this->tlmWrite_BATTERY_LEVEL(bat_level);
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
    Reset_Specific_Handler(
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
      
      // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
      if(!Send_Frame(watchdog_stroke))
        return false;
       
      for(unsigned i = 100000000; i>0; --i); // Wait loop to make sure sci sends out all data

      // Receive frame back from MSP430
      U32 comm_error;
      alignas(8) watchdog_frame frame;
      int32_t size_read = Receive_Watchdog_Frame(&comm_error, &frame);

      // Good read:
      if (size_read > 0)
      {
          if(size_read < 8)
          {
              this->log_WARNING_HI_WatchDogIncorrectResp();
              return false;
          }
          // Check that response is the same as what was sent
          else if(frame.stroke_data_size != (uint16_t)watchdog_stroke)
          {
              // Check if the watchdog response includes UDP data
              if(frame.stroke_data_size != 0x0000)
              {
                  // Forward all data after header and current data to ground interface
                  // Blocking until timeout or data available
                  Fw::Buffer buff;
                  int stat = 0;
                  stat = sciReceiveWithTimeout(scilinREG,
                                               frame.stroke_data_size,
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
                  this->log_WARNING_HI_WatchDogIncorrectResp();
                  return false;
              }
          }
          else if(frame.stroke_data_size == (uint16_t)watchdog_stroke && frame.reset_val == (uint16_t)*((&watchdog_stroke)+2))
          {
              //Foward telemetry data
              //Blocking until timeout or data available
              U16 buff[6];
              int stat = 0;
              stat = sciReceiveWithTimeout(scilinREG,
                                           12,  //12 bytes is how big total telemetry should be
                                           (uint8_t *)&buff,
                                           0x00002710); /*10 second timeout*/
              comm_error = sciRxError(scilinREG);
              // Good read:
              if (stat >= 12)
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
                  U16 bat_level = 0;
                  memcpy(&bat_level, &(buff)+10, 2);    // Copy two bytes for battery voltage level value
                  this->tlmWrite_BATTERY_LEVEL(bat_level);
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

  bool WatchDogInterfaceComponentImpl :: Send_Frame(U32 stroke)
  {
      // Create Value for frame
      U32 test_frame = 0x0021B00B;

      // Calculate Parity for Message by summing each byte then Inversing it
      U8 parity = 0;
      parity = ~((test_frame&0x000000FF) +
      			 ((test_frame&0x0000FF00) >> 8) + 
      			 ((test_frame&0x00FF0000) >> 16) +
      			 ((test_frame&0xFF000000) >> 24) + 
      			 (stroke&0x000000FF) +
      			 ((stroke&0x0000FF00) >> 8) +
      			 ((stroke&0x00FF0000) >> 16) +
      			 ((stroke&0xFF000000) >> 24));

      // Add Parity to frame
      test_frame = (parity << 24) | test_frame;

      alignas(8) watchdog_frame frame;
      frame.magic_value = 0x21B00B;
      frame.parity = parity;
      frame.stroke_data_size = (uint16_t)stroke;
      frame.reset_val = (uint16_t)*((&stroke)+2);


      int tries = 100000000;
      while(--tries && !sciIsTxReady(scilinREG));
      if(tries == 0)
      {
        this->log_WARNING_HI_WatchDogTimedOut();
        return;
      }
      sciSend(scilinREG, sizeof(frame), (uint8_t *)&frame);

      return true;
  }

  int32_t WatchDogInterfaceComponentImpl :: Receive_Watchdog_Frame(U32* comm_error, watchdog_frame* frame)
  {
    uint64_t watchdog_return;
    int32_t size_read = 0;
    size_read = sciReceiveWithTimeout(scilinREG,
                                 sizeof(watchdog_return),
                                 (uint8_t *)&watchdog_return,
                                 0x00002710); /*10 second timeout*/

    *comm_error = sciRxError(scilinREG);

    if(size_read >= 8)
    {
      uint16_t temp;
      memcpy(&(temp), &(watchdog_return), 3); // Copy magic bytes from returned value to struct magic value
      (*frame).magic_value = temp;
      memcpy(&((*frame).parity), &(watchdog_return)+3, 1); // Copy parity bytes from returned value to struct parity value
      memcpy(&((*frame).stroke_data_size), &(watchdog_return)+4, 2); // Copy stroke/data size bytes from returned value to struct stroke/data size value
      memcpy(&((*frame).reset_val), &(watchdog_return)+6, 2); // Copy reset_val bytes from returned value to struct reset_val value
    }

    return size_read;
  }

  bool WatchDogInterfaceComponentImpl :: Read_Temp()
  {
    // Start ADC Conversions for all thermistors
    adcStartConversion(adcREG1, adcGROUP1);

    // Check if all ADC Conversions are done
    int tries = 50;
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

      // Create adcData_t array of size 6 for all Thermistors
      adcData_t data[6];
      adcData_t* data_ptr = (adcData_t*)&data;

      // adcGetData returns how many conversions happened, saves data into data_ptr
      U32 num_conversions = adcGetData(adcREG1, adcGROUP1, data_ptr);

      if(num_conversions >= 6)
      {
        // Report tempurature as telemetry

        // Send Thermistor 12 bit values to Telemetry
        this->tlmWrite_THERM_0(data[0].value);

        this->tlmWrite_THERM_1(data[1].value);

        this->tlmWrite_THERM_2(data[2].value);

        this->tlmWrite_THERM_3(data[3].value);

        this->tlmWrite_THERM_4(data[4].value);

        this->tlmWrite_THERM_5(data[5].value);
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
