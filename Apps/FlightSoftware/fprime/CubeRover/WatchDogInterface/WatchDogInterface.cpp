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
#include "sys_dma.h"
#include <App/DMA.h>
#include "gio.h"
#include "reg_spi.h"

static volatile bool dmaWriteBusy = false;
static volatile bool dmaReadBusy = false;

extern "C" void dmaCh0_ISR(dmaInterrupt_t inttype) {
    dmaReadBusy = false;
}

extern "C" void dmaCh1_ISR(dmaInterrupt_t inttype) {
    dmaWriteBusy = false;
}

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
    , m_sci(scilinREG), m_finished_initializing(false)
  {

  }

  void WatchDogInterfaceComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    WatchDogInterfaceComponentBase::init(queueDepth, instance);
    sciEnterResetState(m_sci);
    sciSetBaudrate(m_sci, 9600);
    sciExitResetState(m_sci);

    Read_Temp();

    Reset_Specific_Handler(Reset_Radio);           // Reset WF121
    // There would normally be a 400000000 cycle delay for WF121 to finish resetting.. but that actualyl breaks things... weird
    m_finished_initializing = true;
  }

  WatchDogInterfaceComponentImpl ::
    ~WatchDogInterfaceComponentImpl(void)
  {
      // Do Not Use
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  // Timed function that runs every 1Hz
  void WatchDogInterfaceComponentImpl ::
    Run_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {

    // Update Thermistor Telemetry
    Read_Temp();

    // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
    if(!Send_Frame(zero_size, No_Reset))
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
    // Depricated, not needed
  }

  void WatchDogInterfaceComponentImpl ::
    downlink_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    // Sends payload and reset value to MSP430
    U16 payload_length = fwBuffer.getsize();

    // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
    if(!Send_Frame(payload_length, No_Reset))
      return;
     
    dmaSend(reinterpret_cast<void *>(fwBuffer.getdata()), payload_length);  // FIXME: What is DMA send failed? *TUrn blocking off when we use Mutexes **DO the same for other DMA sends and receives
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
    // Depricated, not used currently
  }
  
  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  // Function called by cmd handler to send a reset to watchdog
  void WatchDogInterfaceComponentImpl :: Reset_Specific_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        reset_values_possible reset_value
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

    // Sends a command to watchdog to reset specified devices

      // Check that reset_value is correct
      if(reset_value >= No_Reset && reset_value <= max_reset_value)
      {
        this->log_WARNING_LO_WatchDogIncorrectResetValue();
        return;
      }

      // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
      if(!Send_Frame(zero_size, reset_value))
        return;
	    
      // Receive frame back from MSP430
      U32 comm_error;
      WatchdogFrameHeader frame;
      int32_t size_read = Receive_Frame(&comm_error, &frame);

      if(size_read < min_receive_size)
      {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;
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

      // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
	  if(!Send_Frame(zero_size, Disengage))
	      return;
	     
      // Receive frame back from MSP430
      U32 comm_error;
      WatchdogFrameHeader frame;
      int32_t size_read = Receive_Frame(&comm_error, &frame);

      if(size_read < min_receive_size)
      {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;
      }

      // Set Deployment Bit High
      // Deployment2 signal is on MIBSPI3NCS_4 which is setup as a GPIO pin with default 0 and no pull up/down resistor.
      // Use Bit 5 as MIBSPI3NCS_4 is the 5th (start at 0) pin from the start of SPI3 Port 
      gioSetBit(spiPORT3, deploy_bit, 1);

	this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void WatchDogInterfaceComponentImpl ::
    Engage_From_Lander_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
        // Send Activity Log/tlm to know watchdog recieved command
    char command_type[24] = "Engage From Rover";
    Fw::LogStringArg command_type_log = command_type;
    this->log_ACTIVITY_HI_WatchDogCmdReceived(command_type_log);

      // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
    if(!Send_Frame(zero_size, HDRM_Off))
        return;
       
      // Receive frame back from MSP430
      U32 comm_error;
      WatchdogFrameHeader frame;
      int32_t size_read = Receive_Frame(&comm_error, &frame);

      if(size_read < min_receive_size)
      {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;
      }

      // Set Deployment Bit High
      // Deployment2 signal is on MIBSPI3NCS_4 which is setup as a GPIO pin with default 0 and no pull up/down resistor.
      // Use Bit 5 as MIBSPI3NCS_4 is the 5th (start at 0) pin from the start of SPI3 Port 
      gioSetBit(spiPORT3, deploy_bit, 0);

    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  bool WatchDogInterfaceComponentImpl ::
    Reset_Specific_Handler(
        reset_values_possible reset_value
    )
  {
    // Send Activity Log/tlm to know watchdog recieved command
    char command_type[24] = "Reset Specific:";
    char reset_val_char[8];
    sprintf(reset_val_char, "%u", reset_value);
    strcat(command_type, reset_val_char);
    Fw::LogStringArg command_type_log = command_type;
    this->log_ACTIVITY_HI_WatchDogCmdReceived(command_type_log);

    // Sends a command to watchdog to reset specified devices

      // Check that reset_value is correct
      if(reset_value >= No_Reset && reset_value <= max_reset_value)
      {
        this->log_WARNING_LO_WatchDogIncorrectResetValue();
        return;
      }
      
      // Send frame to watchdog. If returns false, communication with MSP430 is bad and should not send anymore data. Errors logged in Send_Frame()
      if(!Send_Frame(zero_size, reset_value))
        return false;

      // Receive frame back from MSP430
      U32 comm_error;
      WatchdogFrameHeader frame = {0};
      int32_t size_read = Receive_Frame(&comm_error, &frame);

      if(size_read < min_receive_size)
          return false;

      return true;
  }

  /* Commands that Only Watchdog Processes */
  void WatchDogInterfaceComponentImpl ::
    Prepare_For_Deployment_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    Reset_Specific_Handler(No_Reset);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void WatchDogInterfaceComponentImpl ::
    Switch_Connection_Mode_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    Reset_Specific_Handler(No_Reset);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void WatchDogInterfaceComponentImpl ::
    Set_Kp_Specific_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    Reset_Specific_Handler(No_Reset);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void WatchDogInterfaceComponentImpl ::
    Set_Heater_On_value_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    Reset_Specific_Handler(No_Reset);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void WatchDogInterfaceComponentImpl ::
    Set_Heater_Off_value_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    Reset_Specific_Handler(No_Reset);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void WatchDogInterfaceComponentImpl ::
    Set_Heater_Duty_Cycle_Max_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    Reset_Specific_Handler(No_Reset);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void WatchDogInterfaceComponentImpl ::
    Set_Heater_Duty_Cycle_Period_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    Reset_Specific_Handler(No_Reset);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void WatchDogInterfaceComponentImpl ::
    Set_V_Setpoint_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    Reset_Specific_Handler(No_Reset);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void WatchDogInterfaceComponentImpl ::
    Switch_to_Sleep_Mode_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    Reset_Specific_Handler(No_Reset);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void WatchDogInterfaceComponentImpl ::
    Switch_to_Keep_Alive_Mode_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    Reset_Specific_Handler(No_Reset);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void WatchDogInterfaceComponentImpl ::
    Switch_to_Service_Mode_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    Reset_Specific_Handler(No_Reset);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /* End of Commands that Only Watchdog Processes*/

  bool WatchDogInterfaceComponentImpl :: Send_Frame(U16 payload_length, U16 reset_value)
  {
      struct WatchdogFrameHeader frame;
      frame.magic_value = 0x0021B00B;
      frame.parity = 0;
      frame.payload_length = static_cast<U16>(payload_length);
      frame.reset_val = static_cast<U16>(reset_value);

      uint64_t frame_bin = *((uint64_t *)&frame);
      frame.parity = ~(((frame_bin & 0x00000000000000FFL) >> 0)  +
                       ((frame_bin & 0x000000000000FF00L) >> 8)  +
                       ((frame_bin & 0x0000000000FF0000L) >> 16) +
                       ((frame_bin & 0x00000000FF000000L) >> 24) +
                       ((frame_bin & 0x000000FF00000000L) >> 32) +
                       ((frame_bin & 0x0000FF0000000000L) >> 40) +
                       ((frame_bin & 0x00FF000000000000L) >> 48) +
                       ((frame_bin & 0xFF00000000000000L) >> 56));
      dmaSend(&frame, sizeof(frame));

      return true;
  }

  bool WatchDogInterfaceComponentImpl :: Read_Temp()
  {
    // Start ADC Conversions for all thermistors
    adcStartConversion(adcREG1, adcGROUP1);

    // Check if all ADC Conversions are done
    // From testing tries ALMOST ALWAYS ~= 10 - 12  ==>  38 - 40 cycles to convert data.. OK to poll
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
      U32 num_conversions = adcGetData(adcREG1, adcGROUP1, m_thermistor_buffer);
      if(num_conversions >= number_thermistors)
      {
        this->tlmWrite_THERM_0(m_thermistor_buffer[0].value);
        this->tlmWrite_THERM_1(m_thermistor_buffer[1].value);
        this->tlmWrite_THERM_2(m_thermistor_buffer[2].value);
        this->tlmWrite_THERM_3(m_thermistor_buffer[3].value);
        this->tlmWrite_THERM_4(m_thermistor_buffer[4].value);
        this->tlmWrite_THERM_5(m_thermistor_buffer[5].value);
      }
      else
      {
        this->log_WARNING_HI_ADCThermistorError();
        return false;
      }
    }
    return true;
  }

  int WatchDogInterfaceComponentImpl::Receive_Frame(uint32_t *comm_error, struct WatchdogFrameHeader *header)
  {
    dmaReceive(header, sizeof(*header));

    int size_read = sizeof(*header);
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
    if(header->magic_value != header_magic)
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

    int payload_read = 0;
    if (header->payload_length == 0) // Received a WD echo, always expect telemtry
    {
        struct WatchdogTelemetry buff;
        dmaReceive(&buff, sizeof(buff));

        payload_read = sizeof(buff);
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
          this->tlmWrite_BATTERY_CURRENT(buff.battery_current);
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
        dmaReceive(reinterpret_cast<void *>(uplinked_data.getdata()), header->payload_length);
        payload_read = header->payload_length;
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

  // FIXME: Add timeout to escape polling loop
  void WatchDogInterfaceComponentImpl::pollDMAReceiveFinished() {
      if (!m_finished_initializing) {
          while (!((getDMAIntStatus(BTC) >> SCILIN_RX_DMA_CH) & 0x01U));
          dmaReadBusy = false;
          sciDMARecvCleanup(SCILIN_RX_DMA_CH);
      }
      while (dmaReadBusy);      // TODO: Mutex to allow multiprogramming & TIMEOOUT
  }

  // Returns negative on error
  bool WatchDogInterfaceComponentImpl::dmaReceive(void *buffer, int size, bool blocking) {
      if (blocking)
          while (dmaReadBusy);
      else if (dmaReadBusy)
          return false;
      sciDMARecv(SCILIN_RX_DMA_CH, static_cast<char *>(buffer), size, ACCESS_8_BIT, &dmaReadBusy);
      if (blocking)
          pollDMAReceiveFinished();
      return true;
  }

  // FIXME: Add timeout to escape polling loop
  void WatchDogInterfaceComponentImpl::pollDMASendFinished() {
      if (!m_finished_initializing) {
          while (!((getDMAIntStatus(BTC) >> SCILIN_TX_DMA_CH) & 0x01U));
          dmaWriteBusy = false;
          sciDMASendCleanup(SCILIN_TX_DMA_CH);
      }
      while (dmaWriteBusy);    // TODO: Mutex to allow multiprogramming & TIMEOOUT
  }

  // Returns negative on error
  bool WatchDogInterfaceComponentImpl::dmaSend(void *buffer, int size, bool blocking) {
      if (blocking)
          while (dmaWriteBusy);
      else if (dmaWriteBusy)
          return false;
      sciDMASend(SCILIN_TX_DMA_CH, static_cast<char *>(buffer), size, ACCESS_8_BIT, &dmaWriteBusy);
      if (blocking)
          pollDMASendFinished();
      return true;
  }

} // end namespace CubeRover
