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

#include "Include/FswPacket.hpp"

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
      watchdog_dma_busy = false;
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

    // Sends payload and reset value to MSP430
    U16 payload_length = 0x0000;
    U16 reset_value = 0x0000;

    // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
    if(!Send_Frame(payload_length, reset_value))
      return;
     
    // Receive frame back from MSP430
    U32 comm_error;
    WatchdogFrameHeader frame;
    Receive_Frame(&comm_error, &frame);
  }
  
  void WatchDogInterfaceComponentImpl ::
    CompResetRequest_handler(
        const NATIVE_INT_TYPE portNum,
        CubeRoverPorts::ResetValue reset
    )
  {
    // TODO
  }

  void WatchDogInterfaceComponentImpl ::
    downlink_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    // Sends payload and reset value to MSP430
    U16 payload_length = fwBuffer.getsize();
    U16 reset_value = 0x0000;

    // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
    if(!Send_Frame(payload_length, reset_value))
      return;
     
    // Receive frame back from MSP430
    U32 comm_error;
    WatchdogFrameHeader frame;
    int32_t size_read =  Receive_Frame(&comm_error, &frame);

    // Good read:
    if (size_read >= 8)
    {
        if(frame.payload_length == payload_length && frame.reset_val == 0x0000)
        {
            int tries = 100000000;
    		while(--tries && !sciIsTxReady(scilinREG));
    		if(tries == 0)
    		{
    		    this->log_WARNING_HI_WatchDogTimedOut();
    		    return;
    		}
    		sciSend(scilinREG, payload_length, reinterpret_cast<unsigned char*>(fwBuffer.getdata()));
        }
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

  void WatchDogInterfaceComponentImpl :: Reset_Specific_cmdHandler(
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
      // Sends payload and reset value to MSP430
      U16 payload_length = 0x0000;
      U16 wd_reset_value = reset_value;

      // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
      if(!Send_Frame(payload_length, wd_reset_value))
        return;
	    
      // Receive frame back from MSP430
      U32 comm_error;
      WatchdogFrameHeader frame;
      int32_t size_read = Receive_Frame(&comm_error, &frame);

      if(size_read < 8)
      {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;
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
	  this->log_ACTIVITY_HI_WatchDogCmdReceived(command_type_log);

      // Sends payload and reset value to MSP430
      U16 payload_length = 0x0000;
      U16 reset_value = 0x00EE;

      // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
	  if(!Send_Frame(payload_length, reset_value))
	      return;
	     
      // Receive frame back from MSP430
      U32 comm_error;
      WatchdogFrameHeader frame;
      int32_t size_read = Receive_Frame(&comm_error, &frame);

      if(size_read < 8)
      {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;
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
    this->log_ACTIVITY_HI_WatchDogCmdReceived(command_type_log);

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
      // Sends payload and reset value to MSP430
      U16 payload_length = 0x0000;
      U16 wd_reset_value = reset_value;
      
      // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
      if(!Send_Frame(payload_length, wd_reset_value))
        return false;

      // Receive frame back from MSP430
      U32 comm_error;
      WatchdogFrameHeader frame;
      int32_t size_read = Receive_Frame(&comm_error, &frame);

      if(size_read < 8)
          return false;

      return true;
    }
  }

  bool WatchDogInterfaceComponentImpl :: Send_Frame(U16 payload_length, U16 reset_value)
  {
      struct WatchdogFrameHeader frame;
      frame.magic_value = 0x21B00B;
      frame.parity = 0;
      frame.payload_length = payload_length;
      frame.reset_val = reset_value;

      uint64_t frame_bin = *((uint64_t *)&frame);
      frame.parity = ~(((frame_bin & 0x00000000000000FFL) >> 0)  +
                       ((frame_bin & 0x000000000000FF00L) >> 8)  +
                       ((frame_bin & 0x0000000000FF0000L) >> 16) +
                       ((frame_bin & 0x00000000FF000000L) >> 24) +
                       ((frame_bin & 0x000000FF00000000L) >> 32) +
                       ((frame_bin & 0x0000FF0000000000L) >> 40) +
                       ((frame_bin & 0x00FF000000000000L) >> 48) +
                       ((frame_bin & 0xFF00000000000000L) >> 56));


      int tries = 100000000;
      while(--tries && !sciIsTxReady(scilinREG));
      if(tries == 0)
      {
        this->log_WARNING_HI_WatchDogTimedOut();
        return false;
      }
      sciSend(scilinREG, sizeof(frame), (uint8_t *)&frame);

      return true;
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

  int WatchDogInterfaceComponentImpl::Receive_Frame(uint32_t *comm_error, struct WatchdogFrameHeader *header)
  {
    int size_read = sciReceiveWithTimeout(scilinREG, sizeof(*header), (uint8_t *)header, 100000000);
    *comm_error = 0;

    if (size_read == 0)
    {
        this->log_WARNING_HI_WatchDogTimedOut();
        return size_read;
    }
    else if(size_read < 0)
    {
        *comm_error = ~size_read;
        this->log_WARNING_HI_WatchDogCommError(*comm_error);
        return size_read;
    }
    else if (size_read != sizeof(*header))
    {
        this->log_WARNING_HI_WatchDogIncorrectResp(not_enough_bytes);
        return size_read;
    }

    // Check that a magic value, parity and reset_value of frame was sent back
    if(header->magic_value != 0x21B00B)
    {
        this->log_WARNING_HI_WatchDogIncorrectResp(bad_magic_value);
        return size_read;
    }

    uint64_t frame_bin = *((uint64_t *)header);
    // 0 if parity in packet is correct
    uint8_t check_parity = ~(((frame_bin & 0x00000000000000FFL) >> 0)  +
                             ((frame_bin & 0x000000000000FF00L) >> 8)  +
                             ((frame_bin & 0x0000000000FF0000L) >> 16) +
                             ((frame_bin & 0x00000000FF000000L) >> 24) +
                             ((frame_bin & 0x000000FF00000000L) >> 32) +
                             ((frame_bin & 0x0000FF0000000000L) >> 40) +
                             ((frame_bin & 0x00FF000000000000L) >> 48) +
                             ((frame_bin & 0xFF00000000000000L) >> 56));

    if(check_parity)
    {
        this->log_WARNING_HI_WatchDogIncorrectResp(bad_parity);
        return size_read;
    }

    if(header->reset_val > 0x0020 && header->reset_val != 0x00EE)
    {
        this->log_WARNING_HI_WatchDogIncorrectResp(bad_reset_value);
    }

    int payload_read = 0;
    if (header->payload_length == 0) // Received a WD echo
    {
        struct WatchdogTelemetry buff;
        payload_read = sciReceiveWithTimeout(scilinREG, sizeof(buff), (uint8_t *)&buff, 100000000);
        *comm_error = 0;

        if (payload_read == sizeof(buff))
        {
          this->tlmWrite_VOLTAGE_2_5V(buff.voltage_2V5);
          this->tlmWrite_VOLTAGE_2_8V(buff.voltage_2V8);
          this->tlmWrite_VOLTAGE_24V(buff.voltage_24V);
          this->tlmWrite_VOLTAGE_28V(buff.voltage_28V);
          this->tlmWrite_BATTERY_THERMISTOR(buff.battery_thermistor);
          this->tlmWrite_SYSTEM_STATUS(buff.sys_status);
          this->tlmWrite_BATTERY_LEVEL(buff.battery_level);
          size_read += payload_read;
        }
        else if(payload_read < 0)
        {
            *comm_error = ~payload_read;
            this->log_WARNING_HI_WatchDogCommError(*comm_error);
        }
        else
        {
            this->log_WARNING_HI_WatchDogTimedOut();
        }
    }
    else if (0 < header->payload_length and header->payload_length < UDP_MAX_PAYLOAD)  // Received uplinked data
    {
        // UDP_MAX_PAYLOAD defined in FlightMCU/Include/FswPacket.hpp
        // TODO: Verify that the MTU for wired connection is the same as Wifi
        Fw::Buffer uplinked_data;
        payload_read = sciReceiveWithTimeout(scilinREG, header->payload_length,
                                             reinterpret_cast<uint8_t *>(uplinked_data.getdata()), 100000000);
        *comm_error = 0;

        if (payload_read == header->payload_length)
        {
            uplink_out(0, uplinked_data);
            size_read += payload_read;
        }
        else if(size_read < 0)
        {
            *comm_error = ~payload_read;
            this->log_WARNING_HI_WatchDogCommError(*comm_error);
        }
        else
            this->log_WARNING_HI_WatchDogTimedOut();
    }
    else
    {
        this->log_WARNING_HI_WatchDogIncorrectResp(bad_size_received);
    }

    return size_read;
  }

} // end namespace CubeRover
