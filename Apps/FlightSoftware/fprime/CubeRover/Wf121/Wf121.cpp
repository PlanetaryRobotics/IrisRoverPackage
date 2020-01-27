#include "Wf121.hpp"

using namespace Wf121;

Wf121Driver :: Wf121Driver(){
    m_processingCmd = false;
}


ErrorCode Wf121Driver :: Init(){
#if __USE_CTS_RTS__
  gioSetBit(gioPORTB,3,1);       // Pull high RTS : not ready to receive some data
#endif //#if __USE_CTS_RTS__

  m_processingCmd = false;

  return NO_ERROR;
}

/**
 * @brief      Transmit data to WF121 module
 *
 * @param      header   The header
 * @param      payload  The payload
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: transmitCommand(BgApiHeader *header,
                                         uint8_t *payload){

  // check if a command is already being sent out, if so, only one command can
  // be sent at a time.
  if(m_processingCmd){
    return TOO_MANY_REQUEST;
  } 
   
#if __USE_CTS_RTS__
  uint32_t timeout = 10000;
  while(gioGetBit(gioPORTB, 2) && --timeout); //block until CTS goes low
  if(!timeout) return TIMEOUT;
#endif //#if __USE_CTS_RTS__

  while(!sciIsTxReady(SCI_REG));
  sciSend(SCI_REG, sizeof(header), (uint8_t *)header);

  uint16_t payloadSize = getPayloadSizeFromHeader(header);

  if(payloadSize > 0){
    if(payload == NULL){
      return INVALID_PARAMETER;
    }

    while(!sciIsTxReady(SCI_REG));
    sciSend(SCI_REG, payloadSize, payload);
  }

  // Flag that a command is processing, cannot send a new command until the
  // current one is processed
  m_processingCmd = true;

  return NO_ERROR;
}


/**
 * @brief      Execute system callback
 *
 * @param      header       The header
 * @param      payload      The payload
 * @param[in]  payloadSize  The payload size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: executeSystemCallback(BgApiHeader *header,
                                               uint8_t *payload,
                                               const uint16_t payloadSize){
  uint16_t result;
  uint16_t major;
  uint16_t minor;
  uint16_t patch;
  uint16_t build;
  uint16_t bootloaderVersion;
  uint16_t tcpIpVersion;
  uint16_t hwVersion;
  uint32_t address;
  uint8_t type;
  PowerSavingState state;

  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.cmdId){
      case 0x00: // Sync 
        return cb_CommandSyncSystem();
      case 0x02: // Hello
        return cb_CommandHelloSystem();
      case 0x03: // Set Power Saving State
        memcpy(&result,
              payload,
              sizeof(result));
        return cb_CommandSetPowerSavingState(result);
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.cmdId){
      case 0x00: // Boot
        m_processingCmd = false; // the command is processed
        memcpy(&major,
               payload,
               sizeof(major));
        memcpy(&minor,
               payload + 2, /* sizeof minor */
               sizeof(minor));
        memcpy(&patch,
               payload + 4, /* sizeof minor, major */
               sizeof(patch));
        memcpy(&build,
               payload + 6, /* sizeof minor major, patch */
               sizeof(build));
        memcpy(&bootloaderVersion,
               payload + 8, /* sizeof minor, major, patch, build */
               sizeof(bootloaderVersion));
        memcpy(&tcpIpVersion,
               payload + 10, /* sizeof minor, major, patch, build, bootloader version */
              sizeof(tcpIpVersion));
        memcpy(&hwVersion,
               payload + 12,  /* sizeof minor, major, patch, build, bootloader version */
               sizeof(hwVersion));
        return cb_EventBoot(major,
                            minor,
                            patch,
                            build,
                            bootloaderVersion,
                            tcpIpVersion,
                            hwVersion);
      case 0x02: // Software Exception
        if(payloadSize < sizeof(address) + sizeof(type)){
          return UNSPECIFIED_ERROR; // should not happen at that point
        }
        memcpy(&address,
               payload,
               sizeof(address));
        memcpy(&type,
               payload + sizeof(address),
               sizeof(type));
        return cb_EventSoftwareException(address, type);
      case 0x03: // Power saving state
        state = (PowerSavingState) payload[0];
        return cb_EventPowerSavingState(state);
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  return NO_ERROR;
}


/**
 * @brief      Execute configuration callback
 *
 * @param      header       The header
 * @param      payload      The payload
 * @param[in]  payloadSize  The payload size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: executeConfigurationCallback(BgApiHeader *header,
                                                      uint8_t *payload,
                                                      const uint16_t payloadSize){
  uint16_t result;
  HardwareInterface interface;
  HardwareAddress hwAddr;

  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.cmdId){
      case 0x00:  // Get MAC address
        memcpy(&result,
               payload,
               sizeof(result));
        interface = (HardwareInterface) payload[sizeof(result)];
        return cb_CommandGetMacAddress(result, interface);
      case 0x01:  // Set MAC address
        memcpy(&result,
               payload,
               sizeof(result));
        interface = (HardwareInterface) payload[sizeof(result)];
        return cb_CommandSetMacAddress(result, interface);
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.cmdId){
      case 0x00: // MAC address
        interface = (HardwareInterface) payload[0];
        memcpy(&hwAddr,
              payload + 1 /* hardware interface */,
              sizeof(HardwareAddress));
        return cb_EventMacAddress(interface, hwAddr);
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }
  return NO_ERROR;    
}


/**
 * @brief      Execute wi-fi callback
 *
 * @param      header       The header
 * @param      payload      The payload
 * @param[in]  payloadSize  The payload size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: executeWifiCallback(BgApiHeader *header,
                                             uint8_t *payload,
                                             const uint16_t payloadSize){
  uint16_t result;
  uint16_t reason;
  uint8_t status;
  HardwareAddress address;
  HardwareInterface interface;
  int8_t channel;
  int16_t rssi;
  int8_t snr;
  uint8_t secure;
  Ssid *ssid;
  SsidSize ssidSize;

  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.cmdId){
      case 0x00: // Wifi ON
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandTurnOnWifi(result);
      case 0x01: // Wifi OFF
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandTurnOffWifi(result);
      case 0x09: // Set scan channels
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandSetScanChannels(result);       
      case 0x03: // Start scan channels
        memcpy(&result,
               payload,
               sizeof(result));     
        return cb_CommandStartScanChannels(result);
      case 0x04: // Stop scan channels
        memcpy(&result,
               payload,
               sizeof(result));     
        return cb_CommandStopScanChannels(result);
      case 0x06: // Connect BSSID
        memcpy(&result,
               payload,
               sizeof(result)); 
        interface = (HardwareInterface)payload[2];
        memcpy(&address,
               payload + 3, /* offset by interface + result */
               sizeof(HardwareAddress));
        return cb_CommandConnectBssid(result,
                                      interface,
                                      address);
      case 0x08: // Disconnect
        memcpy(&result,
               payload,
               sizeof(result));
        interface = (HardwareInterface)payload[2];
        return cb_CommandDisconnect(result,
                                    interface);
      case 0x0D: // Scan results
        memcpy(&result,
               payload,
               sizeof(result));       
        return cb_CommandScanResultsSortRssi(result);
      case 0x05: // Set password
        status = payload[0];
        return cb_CommandSetPassword(status);
      case 0x07: // Connect SSID
        memcpy(&result,
               payload,
               sizeof(result)); 
        interface = (HardwareInterface)payload[2];
        memcpy(&address,
               payload + 3, /* offset by interface + result */
               sizeof(HardwareAddress));
        return cb_CommandConnectSsid(result,
                                     interface,
                                     address);
      case 0x13: // Get signal quality
        memcpy(&result,
               payload,
               sizeof(result)); 
        interface = (HardwareInterface)payload[2];
        return cb_CommandGetSignalQuality(result,
                                          interface);
      case 0x14: // Start sssid scan
        memcpy(&result,
               payload,
               sizeof(result)); 
        return cb_CommandStartSsidScan(result);
      case 0x15: // set AP hidden
        memcpy(&result,
               payload,
               sizeof(result)); 
        interface = (HardwareInterface)payload[2];
        return cb_CommandSetApHidden(result,
                                     interface);
      case 0x16: // Set 11n mode
        memcpy(&result,
               payload,
               sizeof(result)); 
        interface = (HardwareInterface)payload[2];
        return cb_CommandSet11nMode(result,
                                    interface);
      case 0x17: // Set Ap client isolation
        memcpy(&result,
               payload,
               sizeof(result)); 
        interface = (HardwareInterface)payload[2];
        return cb_CommandSetApClientIsolation(result,
                                              interface);
      case 0x11: // Start WPS
        memcpy(&result,
               payload,
               sizeof(result)); 
        interface = (HardwareInterface)payload[2];
        return cb_CommandStartWps(result,
                                  interface);
      case 0x12: // Stop WPS
        memcpy(&result,
               payload,
               sizeof(result)); 
        interface = (HardwareInterface)payload[2];
        return cb_CommandStopWps(result,
                                 interface);
      case 0x0A: // Set operating mode
        memcpy(&result,
               payload,
               sizeof(result)); 
        return cb_CommandSetOperatingMode(result);
      case 0x10: // Set AP max clients
        memcpy(&result,
               payload,
               sizeof(result)); 
        interface = (HardwareInterface)payload[2];
        return cb_CommandSetApMaxClients(result,
                                        interface);
      case 0x0F: // Set AP password
        status = payload[0];
        return cb_CommandSetApPassword(status);
      case 0x0B: // Start AP mode
        memcpy(&result,
               payload,
               sizeof(result));
        interface = (HardwareInterface)payload[2];
        return cb_CommandStartApMode(result,
                                     interface);
      case 0x0C: // Stop AP mode
        memcpy(&result,
               payload,
               sizeof(result));
        interface = (HardwareInterface)payload[2];
        return cb_CommandStopApMode(result,
                                    interface);
      case 0x0E: // AP disconnect client
        memcpy(&result,
               payload,
               sizeof(result));
        interface = (HardwareInterface)payload[2];
        return cb_CommandDisconnectApClient(result,
                                            interface);
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.cmdId){
      case 0x00: // Wifi ON
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_EventWifiIsOn(result);
      case 0x01: // Wifi OFF
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_EventWifiIsOff(result);
      case 0x02: // Scan result
        memcpy(&address,
               payload,
               sizeof(address));
        memcpy(&channel,
               payload + sizeof(address),
               sizeof(channel));
        memcpy(&rssi,
               payload + sizeof(address) + sizeof(channel),
               sizeof(rssi));
        memcpy(&snr,
               payload + sizeof(address) + sizeof(channel) + sizeof(rssi),
               sizeof(snr));
        memcpy(&secure,
               payload + sizeof(address) + sizeof(channel) + sizeof(rssi) + sizeof(snr),
               sizeof(secure));
        memcpy(&ssidSize,
               payload + sizeof(address) + sizeof(channel) + sizeof(rssi) + sizeof(snr) + sizeof(secure),
               sizeof(ssidSize));
        ssid = payload + sizeof(address) + sizeof(channel) + sizeof(rssi) + sizeof(snr) + sizeof(secure) + sizeof(ssidSize);

        return cb_EventScanResult(address,
                                  channel,
                                  rssi,
                                  snr,
                                  secure,
                                  ssid,
                                  ssidSize);
      case 0x03: // Scan result drop
        memcpy(&address,
               payload,
               sizeof(HardwareAddress));
        return cb_EventScanResultDrop(address);
      case 0x04: // Scanned
        return cb_EventScanned(payload[0]);
      case 0x0F: // Scan sort result
        memcpy(&address,
               payload,
               sizeof(address));
        memcpy(&channel,
               payload + sizeof(address),
               sizeof(channel));
        memcpy(&rssi,
               payload + sizeof(address) + sizeof(channel),
               sizeof(rssi));
        memcpy(&snr,
               payload + sizeof(address) + sizeof(channel) + sizeof(rssi),
               sizeof(snr));
        memcpy(&secure,
               payload + sizeof(address) + sizeof(channel) + sizeof(rssi) + sizeof(snr),
               sizeof(secure));
        memcpy(&ssidSize,
               payload + sizeof(address) + sizeof(channel) + sizeof(rssi) + sizeof(snr) + sizeof(secure),
               sizeof(ssidSize));
        ssid = payload + sizeof(address) + sizeof(channel) + sizeof(rssi) + sizeof(snr) + sizeof(secure) + sizeof(ssidSize);
        return cb_EventScanSortResult(address,
                                      channel,
                                      rssi,
                                      snr,
                                      secure,
                                      ssid,
                                      ssidSize);
      case 0x10: // Scan sort finished
        return cb_EventScanSortFinished();
      case 0x05: // Connected
        status = payload[0];
        interface = (HardwareInterface)payload[1];
        ssidSize = payload[2];
        ssid = payload + 3;
        return cb_EventConnected(status,
                                 interface,
                                 ssid,
                                 ssidSize);
      case 0x09: // Connect retry
        interface = (HardwareInterface)payload[sizeof(reason)];
        return cb_EventConnectRetry(interface);     
      case 0x08: // Connect failed
        memcpy(&reason,
               payload,
               sizeof(reason));
        interface = (HardwareInterface)payload[sizeof(reason)];
        return cb_EventConnectFailed(reason,
                                     interface);
      case 0x06: // Disconnected
        memcpy(&reason,
               payload,
               sizeof(reason));
        interface = (HardwareInterface)payload[sizeof(reason)];
        return cb_EventDisconnected(reason,
                                    interface);
      case 0x14: // WPS credential SSID
        interface = (HardwareInterface)payload[0];
        memcpy(&ssidSize,
               payload + 1 ,
               sizeof(ssidSize));
        return cb_EventCredentialSsid(interface,
                                      payload + 2 /* offset by interface + ssidSize */,
                                      ssidSize);
      case 0x15: // WPS credential password
        return cb_EventWpsCredentialPassword(interface,
                                             payload + 1,
                                             payload[0]);
      case 0x12: // WPS completed
        interface = (HardwareInterface)payload[0];
        return cb_EventWpsCompleted(interface);
      case 0x13: // WPS failed
        memcpy(&reason,
               payload,
               sizeof(reason));
        interface = (HardwareInterface)payload[sizeof(reason)];
        return cb_EventWpsFailed(reason,
                                 interface);
      case 0x11: // WPS stopped
        interface = (HardwareInterface)payload[0];
        return cb_EventWpsStopped(interface);
      case 0x16: // signal quality
        rssi = payload[0];
        interface = (HardwareInterface)payload[1];
        return cb_EventSignalQuality(rssi,
                                     interface);
      case 0x0A: // AP mode started
        interface = (HardwareInterface)payload[0];
        return cb_EventApModeStarted(interface);
      case 0x0B: // AP mode stopped
        interface = (HardwareInterface)payload[0];
        return cb_EventApModeStopped(interface);
      case 0x0C: // AP mode failed
        memcpy(&reason,
               payload,
               sizeof(reason));
        interface = (HardwareInterface)payload[sizeof(reason)];
        return cb_EventApModeFailed(reason,
                                    interface);
      case 0x0D: // AP client joined
        memcpy(&address,
               payload,
               sizeof(HardwareAddress));
        interface = (HardwareInterface)payload[sizeof(HardwareAddress)];
        return cb_EventApClientJoined(address,
                                      interface);
      case 0x0E: // AP client left
        memcpy(&address,
               payload,
               sizeof(HardwareAddress));
        interface = (HardwareInterface)payload[sizeof(HardwareAddress)];
        return cb_EventApClientLeft(address,
                                      interface);
      case 0x07: // interface status
        interface = (HardwareInterface)payload[0];
        status = payload[1];
        return cb_EventInterfaceStatus(interface,
                                       status);
      default:
        return COMMAND_NOT_RECOGNIZED;
    }    
  }

  return NO_ERROR;
}


/**
 * @brief      Execute endpoint callback
 *
 * @param      header       The header
 * @param      payload      The payload
 * @param[in]  payloadSize  The payload size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: executeEndpointCallback(BgApiHeader *header,
                                                 uint8_t *payload,
                                                 const uint16_t payloadSize){
  uint16_t result;
  Endpoint endpoint;
  uint8_t * data;
  DataSize8 dataSize8;
  uint32_t endpointType;
  uint8_t streaming;
  int8_t destination;
  uint8_t active;

  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.cmdId){
      case 0x02: // set active
        memcpy(&result,
               payload,
               sizeof(result)); 
        endpoint = payload[2];
        return cb_CommandSetActiveEndpoint(result,
                                           endpoint); 
      case 0x00: // send
        memcpy(&result,
               payload,
               sizeof(result)); 
        endpoint = payload[2];
        return cb_CommandSendEndpoint(result,
                                      endpoint);  
      case 0x05: // set transmit size
        memcpy(&result,
               payload,
               sizeof(result)); 
        endpoint = payload[2];
        return cb_CommandSetTransmitSize(result,
                                         endpoint); 
      case 0x01: // set streaming
        memcpy(&result,
               payload,
               sizeof(result)); 
        endpoint = payload[2];
        return cb_CommandSetStreaming(result,
                                      endpoint); 
      case 0x03: // set streaming destination
        memcpy(&result,
               payload,
               sizeof(result)); 
        endpoint = payload[2];
        return cb_CommandSetStreamingDestination(result,
                                                 endpoint); 
      case 0x04: // close endpoint
        memcpy(&result,
               payload,
               sizeof(result)); 
        endpoint = payload[2];
        return cb_CommandCloseEndpoint(result,
                                       endpoint);
      case 0x06: // disable endpoint
        memcpy(&result,
               payload,
               sizeof(result)); 
        endpoint = payload[2];
        return cb_CommandDisableEndpoint(result,
                                         endpoint);         
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.cmdId){
      case 0x02: // status
      endpoint = payload[0];
      memcpy(&endpointType,
              payload + sizeof(endpoint),
              sizeof(endpointType));
      streaming = payload[sizeof(endpoint) + sizeof(endpointType) + sizeof(endpointType)];
      destination = payload[sizeof(endpoint) + sizeof(endpointType) + sizeof(endpointType) + sizeof(streaming)];
      active = payload[sizeof(endpointType) + sizeof(endpointType) + sizeof(streaming) + sizeof(destination)];
      return cb_EventEndpointStatus(endpoint,
                                    (EndpointType)endpointType,
                                    streaming,
                                    destination,
                                    active);
      case 0x01: // data
        endpoint = payload[0];
        dataSize8 = payload[1];
        data = payload + sizeof(dataSize8) + sizeof(endpoint);
        return cb_EventDataEndpoint(endpoint,
                                    data,
                                    dataSize8);
      case 0x03: // closing
        memcpy(&result,
               payload,
               sizeof(result)); 
        endpoint = payload[2];
        return cb_EventClosingEndpoint(result,
                                       endpoint);   
      case 0x04: // error
        memcpy(&result,
               payload,
               sizeof(result)); 
        endpoint = payload[2];
        return cb_EventErrorEndpoint(result,
                                     endpoint);  
      case 0x00: // syntax error
          memcpy(&result,
                 payload,
                 sizeof(result));
          endpoint = payload[sizeof(result)];
          return cb_EventEndpointSyntaxError(result, endpoint);
      default:
        return COMMAND_NOT_RECOGNIZED;
    }    
  }

  return NO_ERROR;
}


/**
 * @brief      Execute hardware callback
 *
 * @param      header       The header
 * @param      payload      The payload
 * @param[in]  payloadSize  The payload size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: executeHardwareCallback(BgApiHeader *header,
                                                 uint8_t *payload,
                                                 const uint16_t payloadSize){
  uint16_t result;
  uint8_t input;
  uint16_t value;
  uint8_t handle;
  uint32_t changeNotification;
  uint8_t irq;
  uint32_t timestamp;
  uint8_t id;
  uint32_t rate;
  uint8_t dataBits;
  uint8_t stopBits;
  uint8_t parity;
  uint8_t flowCtl;
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t weekday;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;

  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.cmdId){
      case 0x09: // ADC read
        memcpy(&result,
               payload,
               sizeof(result));     
        input = payload[result];
        memcpy(&value,
               payload + sizeof(result) + sizeof(input),
               sizeof(value));
        return cb_CommandAdcRead(result,
                                 input,
                                 value); 
      case 0x02: // Change notification
        break;
      case 0x03: // Change notification pullup
        memcpy(&result,
               payload,
               sizeof(result));     
        return cb_CommandConfigureChangeNotification(result);
      case 0x01: // External interrupt
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandConfigureExternalInterrupt(result);
      case 0x04: // IO port config direction
        memcpy(&result,
               payload,
               sizeof(result));     
        return cb_CommandConfigureIoPortDirection(result);
      case 0x05: // IO port config drain
        memcpy(&result,
               payload,
               sizeof(result));          
        return cb_CommandConfigureIoOpenDrain(result);
      case 0x07: // IO port read
        memcpy(&result,
               payload,
               sizeof(result)); 
        return cb_CommandReadIoPort(result);
      case 0x06: // IO port write
        memcpy(&result,
               payload,
               sizeof(result));      
        return cb_CommandWriteIoPort(result);
      case 0x08: // IO port compare
        memcpy(&result,
               payload,
               sizeof(result)); 
        return cb_CommandOutputCompare(result);
      case 0x0A: // RTC init
         memcpy(&result,
               payload,
               sizeof(result));     
        return cb_CommandRtcInit(result);
      case 0x0B: // RTC set time
         memcpy(&result,
               payload,
               sizeof(result));         
        return cb_CommandRtcSetTime(result);
      case 0x0C: // RTC get time
        memcpy(&result,
               payload,
               sizeof(result)); 
        memcpy(&year,
               payload + sizeof(result),
               sizeof(year));
        month = payload[sizeof(result) + sizeof(year)];
        day = payload[sizeof(result) + sizeof(year) + sizeof(month)];
        weekday = payload[sizeof(result)+ sizeof(year) + sizeof(month) + sizeof(day)];
        hour = payload[sizeof(result)+ sizeof(year) + sizeof(month) +
                       sizeof(day) + sizeof(weekday)];
        minute = payload[sizeof(result)+ sizeof(year) + sizeof(month) + 
                         sizeof(day) + sizeof(weekday) + sizeof(hour)];
        second = payload[sizeof(result)+ sizeof(year) + sizeof(month) + 
                         sizeof(day) + sizeof(weekday) + sizeof(hour) + sizeof(minute)];          
        return cb_CommandRtcGetTime(result,
                                    year,
                                    month,
                                    day,
                                    weekday,
                                    hour,
                                    minute,
                                    second);
      case 0x0D: // RTC set alarm
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandSetAlarm(result);
      case 0x0E: // Configure UART
        memcpy(&result,
               payload,
               sizeof(result));       
        return cb_CommandConfigureUart(result);
      case 0x0F: // Get UART configuration
        memcpy(&result,
               payload,
               sizeof(result));       
        return cb_CommandGetUartConfiguration(result);      
      case 0x00: // set soft timer
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandSetSoftTimer(result);
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.cmdId){
      case 0x01: // change notification
        memcpy(&changeNotification,
               payload,
               sizeof(changeNotification));
        return cb_EventChangeNotification(changeNotification);
      case 0x02: // external interrupt
        irq = payload[0];
        memcpy(&timestamp,
               payload,
               sizeof(timestamp));
        return cb_EventExternalInterrupt(irq,
                                         timestamp);
      case 0x03: // RTC alarm
        return cb_EventRtcAlarm();
      case 0x00: // soft timer
        handle = payload[0];
        return cb_EventSoftTimer(handle);
      case 0x04: // UART configuration
        id = payload[0];
        memcpy(&rate,
               payload,
               sizeof(rate));
        dataBits = payload[sizeof(id) + sizeof(rate)];
        stopBits = payload[sizeof(id) + sizeof(rate) + sizeof(dataBits)];
        parity = payload[sizeof(id) + sizeof(rate) + sizeof(dataBits) + sizeof(stopBits)];
        flowCtl = payload[sizeof(id) + sizeof(rate) + sizeof(dataBits) + sizeof(stopBits) + sizeof(parity)];

        return cb_EventUartConfiguration(id,
                                         rate,
                                         dataBits,
                                         stopBits,
                                         parity,
                                         flowCtl);
      default:
        return COMMAND_NOT_RECOGNIZED;
    }    
  }

  return NO_ERROR;
}


/**
 * @brief      Execute TCP stack callback
 *
 * @param      header       The header
 * @param      payload      The payload
 * @param[in]  payloadSize  The payload size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: executeTcpStackCallback(BgApiHeader *header,
                                                 uint8_t *payload,
                                                 const uint16_t payloadSize){
  uint16_t result;
  uint8_t index;
  uint8_t clientCount;
  Endpoint endpoint;
  IpAddress localIp;
  uint16_t localPort;
  IpAddress remoteIp;
  uint16_t remotePort;
  IpAddress address;
  DnsName * dnsName;
  DnsNameSize dnsNameSize;
  uint8_t * data;
  DataSize16 dataSize16;
  uint8_t routingEnabled;
  uint32_t leaseTime;
  Netmask subnetMask;
  HardwareAddress hwAddress;
  Gateway gateway;
  uint8_t useDhcp;

  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.cmdId){
      case 0x04: // TCP configure
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandConfigureTcpIp(result);
      case 0x08: // DHCP set hostname
        memcpy(&result,
               payload,
               sizeof(result));      
        return cb_CommandSetDhcpHostName(result);
      case 0x05: // DNS configure
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandDnsConfigure(result);
      case 0x06: // DNS get host by name
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandGetDnsHostByName(result);
      case 0x01: // TCP connect
        memcpy(&result,
               payload,
               sizeof(result));
        endpoint = payload[sizeof(result)];
        return cb_CommandTcpConnect(result,
                                    endpoint);
      case 0x00: // Start TCP server
        memcpy(&result,
               payload,
               sizeof(result));
        endpoint = payload[sizeof(result)];
        return cb_CommandStartTcpServer(result,
                                        endpoint);
      case 0x03: // UDP connect
        memcpy(&result,
               payload,
               sizeof(result));
        endpoint = payload[sizeof(result)];
        return cb_CommandUdpConnect(result,
                                    endpoint);
      case 0x07: // UDP bind
        memcpy(&result,
               payload,
               sizeof(result));     
        return cb_CommandUdpBind(result);
      case 0x02: // start UDP server
        memcpy(&result,
               payload,
               sizeof(result));
        endpoint = payload[sizeof(result)];
        return cb_CommandStartUdpServer(result,
                                        endpoint);
      case 0x09: // dhcp enable routing
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandDhcpEnableRouting(result);
      case 0x0A: // set mDNS host name
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandSetMdnsHostName(result);
      case 0x0B: // start mDNS network
        memcpy(&result,
               payload,
               sizeof(result));      
        return cb_CommandStartMDns(result); 
      case 0x0C: // stop mDNS network
        memcpy(&result,
               payload,
               sizeof(result));         
        return cb_CommandStopMDns(result);
      case 0x0D: // sd add service
        memcpy(&result,
               payload,
               sizeof(result));
        endpoint = payload[sizeof(result)];  
        return cb_CommandDnsSdAddService(result,
                                         endpoint);
      case 0x0E: // sd add service instance
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandDnsSdAddServiceInstance(result); 
      case 0x0F: // DNS Sd Add Service attribute
        memcpy(&result,
               payload,
               sizeof(result));       
        return cb_CommandDnsSdAddServiceAttribute(result);
      case 0x10: // DNS Sd Remove Service
        memcpy(&result,
               payload,
               sizeof(result));          
        return cb_CommandDnsSdRemoveService(result);
      case 0x11: // DNS SD Start Service
        memcpy(&result,
               payload,
               sizeof(result));          
        return cb_CommandDnsSdStartService(result);
      case 0x12: // DSN SD Stop Service
        memcpy(&result,
               payload,
               sizeof(result));          
        return cb_CommandDnsSdStopService(result);
      case 0x13: // Multicast join
        memcpy(&result,
               payload,
               sizeof(result));          
        return cb_CommandMulticastJoin(result);   
      case 0x14: // Multicast leave
        memcpy(&result,
               payload,
               sizeof(result));          
        return cb_CommandMulticastLeave(result);
      case 0x15: // DHCP configure
        memcpy(&result,
               payload,
               sizeof(result));          
        return cb_CommandDhcpConfigure(result);  
      case 0x16: // list DHCP clients
        memcpy(&result,
               payload,
               sizeof(result));          
        clientCount = payload[sizeof(result)];
        return cb_CommandDhcpClients(result,
                                     clientCount);                          
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.cmdId){
      case 0x00: // configuration
        memcpy(address,
               payload,
               sizeof(address));
        memcpy(&subnetMask,
               payload + sizeof(address),
               sizeof(subnetMask));
        memcpy(gateway,
               payload + sizeof(address) + sizeof(subnetMask),
               sizeof(gateway));
        useDhcp = payload[sizeof(address) + sizeof(subnetMask) + sizeof(gateway)];
        return cb_EventConfigureTcpIp(address,
                                      subnetMask,
                                      gateway,
                                      useDhcp);
      case 0x01: // configuration DNS
        index = payload[0];
        memcpy(address,
               payload + sizeof(index),
               sizeof(address));        
        return cb_EventDnsConfigureTcpIp(index,
                                         address);
      case 0x03: // Get host by name result
        memcpy(&result,
               payload,
               sizeof(result));
        memcpy(address,
               payload + sizeof(result),
               sizeof(address));
        memcpy(&dnsNameSize,
               payload + sizeof(result) + sizeof(address),
               sizeof(dnsNameSize));
        dnsName = payload + sizeof(result) + sizeof(address) + sizeof(dnsNameSize);
        return cb_EventGetDnsHostByName(result,
                                        address,
                                        dnsName,
                                        dnsNameSize);
      case 0x02: // endpoint status
        endpoint = payload[0];
        memcpy(localIp,
               payload + sizeof(endpoint),
               sizeof(localIp));
        memcpy(&localPort,
               payload + sizeof(endpoint) + sizeof(localIp),
               sizeof(localPort));
        memcpy(remoteIp,
               payload + sizeof(endpoint) + sizeof(localIp) + sizeof(localPort),
               sizeof(remoteIp));
        memcpy(&remotePort,
               payload + sizeof(endpoint) + sizeof(localIp) + sizeof(localPort) + sizeof(remoteIp),
               sizeof(remotePort));
        return cb_EventTcpIpEndpointStatus(endpoint,
                                          localIp,
                                          localPort,
                                          remoteIp,
                                          remotePort);
      case 0x04: //UDP data
        memcpy(&endpoint,
               payload,
               sizeof(endpoint));
        memcpy(&remoteIp,
               payload + sizeof(endpoint),
               sizeof(IpAddress));
        memcpy(&remotePort,
               payload + sizeof(endpoint) + sizeof(IpAddress),
               sizeof(remotePort));
        memcpy(&dataSize16,
               payload + sizeof(endpoint) + sizeof(IpAddress) + sizeof(remotePort),
               sizeof(dataSize16));
        data = payload + sizeof(endpoint) + sizeof(IpAddress) + sizeof(remotePort) + sizeof(dataSize16);

        return cb_EventUdpData(endpoint,
                               remoteIp,
                               remotePort,
                               data,
                               dataSize16);
      case 0x05: //mDSN started
        return cb_EventMDnsStarted();
      case 0x06: //mDSN failed
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_EventMDnsFailed(result);
      case 0x07: //mDSN stopped
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_EventMDnsStopped(result); 
      case 0x08:
        index = payload[0];
        return cb_EventDnsSdServiceStarted(index); 
      case 0x09:
        memcpy(&result,
               payload,
               sizeof(result));
        index = payload[sizeof(result)];
        return cb_EventDnsSdServiceFailed(result,
                                          index);
      case 0x0A:
        memcpy(&result,
               payload,
               sizeof(result));
        index = payload[sizeof(result)];
        return cb_EventDnsSdServiceStopped(result,
                                           index); 
      case 0x0B:
        routingEnabled = payload[0];
        memcpy(&address,
               payload + sizeof(routingEnabled),
               sizeof(address));
        memcpy(subnetMask,
               payload + sizeof(routingEnabled) + sizeof(address),
               sizeof(subnetMask));
        memcpy(&leaseTime,
               payload + sizeof(routingEnabled) + sizeof(address) + sizeof(subnetMask),
               sizeof(leaseTime));
        return cb_EventDhcpConfiguration(routingEnabled,
                                         address,
                                         subnetMask,
                                         leaseTime);
      case 0x0C:
        memcpy(address,
               payload,
               sizeof(address));
        memcpy(hwAddress,
               payload + sizeof(address),
               sizeof(hwAddress));
        return cb_EventDhcpClient(address,
                                  hwAddress);
      default:
        return COMMAND_NOT_RECOGNIZED;
    }    
  }

  return NO_ERROR;
}




/**
 * @brief      Execute wired ethernet callback
 *
 * @param      header       The header
 * @param      payload      The payload
 * @param[in]  payloadSize  The payload size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: executeWiredEthernetCallback(BgApiHeader *header,
                                                      uint8_t *payload,
                                                      const uint16_t payloadSize){
  uint16_t result;
  uint8_t state;

  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.cmdId){
      case 0x02: // connected
        state = payload[0]; 
        return cb_CommandRouteConnected(state);
      case 0x00: // set data route
        memcpy(&result,
               payload,
               sizeof(result));   
        return cb_CommandSetDataRoute(result); 
      case 0x01: // close
      memcpy(&result,
             payload,
             sizeof(result));
        return cb_CommandCloseRoute(result);
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.cmdId){
      case 0x00: // link status
        state = payload[0];
        return cb_EventLinkStatus(state);
      default:
        return COMMAND_NOT_RECOGNIZED;
    }    
  }

  return NO_ERROR;
}


/**
 * @brief      Execute persistent store callback
 *
 * @param      header       The header
 * @param      payload      The payload
 * @param[in]  payloadSize  The payload size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: executePersistentStoreCallback(BgApiHeader *header,
                                                      uint8_t *payload,
                                                      const uint16_t payloadSize){
  uint16_t result;
  uint8_t dataSize;
  uint8_t *data;

  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.cmdId){
      case 0x03: // PS save
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandDefragPersistentStore(result);
      case 0x04: // PS load
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandLoadPersistentStore(result);
      case 0x07: // PS dump
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandDumpPersistentStore(result);
      case 0x00: // PS defrag
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandDefragPersistentStore(result);
      case 0x05: // PS erase
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandEraseAllPersistentStore(result);
      case 0x02: // PS erase all
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandErasePersistentStore(result);
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.cmdId){
      case 0x01: // PS key changed
        memcpy(&result,
               payload,
               sizeof(result)); 
        return cb_EventPersistentStoreKeyChanged(result);
      case 0x00: // PS key
        memcpy(&result,
               payload,
               sizeof(result));
        dataSize = payload[sizeof(result)]; 
        data =  payload + sizeof(result) + sizeof(dataSize);
        return cb_EventPersistentStoreKey(result,
                                          dataSize,
                                          data);
      case 0x02: // low voltage
        return cb_EventLowVoltageFlash();
      default:
        return COMMAND_NOT_RECOGNIZED;
    }    
  }

  return NO_ERROR;
}


/**
 * @brief      Execute http server callback
 *
 * @param      header       The header
 * @param      payload      The payload
 * @param[in]  payloadSize  The payload size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: executeHttpServerCallback(BgApiHeader *header,
                                                    uint8_t *payload,
                                                    const uint16_t payloadSize){
  uint16_t result;
  uint32_t request;
  uint8_t method;
  uint8_t resourceSize;
  uint8_t *resource;
  uint8_t dataSize;
  uint8_t *data;

  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.cmdId){
      case 0x00: // enable
        memcpy(&result,
               payload,
               sizeof(result)); 
        return cb_CommandEnableServers(result);
      case 0x01: // add path
        memcpy(&result,
               payload,
               sizeof(result));       
        return cb_CommandAddServerPath(result);
      case 0x02: // Api response
        memcpy(&result,
               payload,
               sizeof(result)); 
        return cb_CommandApiResponse(result);
      case 0x03: // Api response finish
        memcpy(&result,
               payload,
               sizeof(result));        
        return cb_CommandApiResponseFinsh(result);
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.cmdId){
      case 0x02: // Api request
        memcpy(&request,
               payload,
               sizeof(request));
        method = payload[sizeof(request)];
        resourceSize = payload[sizeof(request) + sizeof(method)];
        resource = payload + sizeof(request) + sizeof(method) + sizeof(resourceSize);
        return cb_EventApiRequest(request,
                                  method,
                                  resourceSize,
                                  resource);
      case 0x04: // Api request data
        memcpy(&request,
               payload,
               sizeof(request)); 
        dataSize = payload[sizeof(request)];
        data = payload + sizeof(request) + sizeof(dataSize);
        return cb_EventApiRequestData(request,
                                      dataSize,
                                      data);
      case 0x05: // Api request finished
        memcpy(&request,
               payload,
               sizeof(request)); 
        return cb_EventApiRequestFinished(request);
      default:

        return COMMAND_NOT_RECOGNIZED;
    }    
  }

  return NO_ERROR;
}


/**
 * @brief      Execute firmware upgrade callback
 *
 * @param      header       The header
 * @param      payload      The payload
 * @param[in]  payloadSize  The payload size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: executeDeviceFirmwareUpgradeCallback(BgApiHeader *header,
                                                              uint8_t *payload,
                                                              const uint16_t payloadSize){
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.cmdId){
      default:
        return FEATURE_NOT_IMPLEMENTED;   // FIRMWARE UPGRADE COMMAND NOT SUPPORTED IN CURRENT API
                                          // IMPLEMENTATION
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.cmdId){
      default:
        return FEATURE_NOT_IMPLEMENTED;   // FIRMWARE UPGRADE COMMAND NOT SUPPORTED IN CURRENT API
                                          // IMPLEMENTATION
    }    
  }

  return NO_ERROR;
}


/**
 * @brief      Execute i2c callback
 *
 * @param      header       The header
 * @param      payload      The payload
 * @param[in]  payloadSize  The payload size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: executeI2cCallback(BgApiHeader *header,
                                            uint8_t *payload,
                                            const uint16_t payloadSize){
  uint16_t result;

  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.cmdId){
      case 0x00: // start read
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandI2cRead(result); 
      case 0x01: // start write
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandI2cWrite(result); 
      case 0x02: // stop
        memcpy(&result,
               payload,
               sizeof(result));
        return cb_CommandI2cStop(result); 
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.cmdId){
      default:
        return COMMAND_NOT_RECOGNIZED;
    }    
  }

  return NO_ERROR;
}

/**
 * @brief      Execute any pending callbacks
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ExecuteCallbacks(){
  ErrorCode err = NO_ERROR;
  BgApiHeader header;
  uint16_t payloadSize;
  ErrorCode cmdResult;

  //Some data is available in the buffer, process it
  err = getReplyHeader(&header);
  if(err != NO_ERROR) return err;

  // Get the payload if any. 
  payloadSize = getPayloadSizeFromHeader(&header);

  // Some data need to be processed
  // Should not block at that point
  if(payloadSize > 0){
    err = getReplyPayload(m_payloadBuffer, payloadSize);
    if(err != NO_ERROR){
        return err;
    }
  }

  // If the command is a response, then the first two bytes of the reply are
  // result of the command.
  if(header.bit.msgType == CMD_RSP_TYPE){
    // If payload > 0 than zero, it means the result of the command is in the payload
    // except for some cases (like "hello system" that acknowledge the command without payload)
    if(payloadSize > 0){
        // Process special command cases
        if(header.bit.classId == CLASS_WIFI && header.bit.cmdId == 0x05 /* password set */){
        }
        else{         // end of special command cases
            memcpy(&cmdResult,
                   m_payloadBuffer,
                   sizeof(cmdResult));

            // If there is an error, return the command failure immediately
            if(cmdResult != NO_ERROR){
              m_processingCmd = false;
              return cmdResult;
            }
        }
    }
    m_processingCmd = false;
  }

  // Execute class specific callback
  // Some payload data is expected at that point
  switch(header.bit.classId){
    case CLASS_SYSTEM:
      err = executeSystemCallback(&header, m_payloadBuffer, payloadSize);
      break;
    case CLASS_CONFIGURATION:
      err = executeConfigurationCallback(&header, m_payloadBuffer, payloadSize);
      break;
    case CLASS_WIFI:
      err = executeWifiCallback(&header, m_payloadBuffer, payloadSize);
      break;
    case CLASS_TCP_STACK:
      err = executeTcpStackCallback(&header, m_payloadBuffer, payloadSize);
      break;
    case CLASS_ENDPOINT:
      err = executeEndpointCallback(&header, m_payloadBuffer, payloadSize);
      break;
    case CLASS_HARDWARE:
      err = executeHardwareCallback(&header, m_payloadBuffer, payloadSize);
      break;
    case CLASS_I2C:
      err = executeI2cCallback(&header, m_payloadBuffer, payloadSize);
      break;
    case CLASS_WIRED_ETHERNET:
      err = executeWiredEthernetCallback(&header, m_payloadBuffer, payloadSize);
      break;
    case CLASS_HTTP_SERVER:
      err = executeHttpServerCallback(&header, m_payloadBuffer, payloadSize);
      break;
    case CLASS_PERSISTENT_STORE:
      err = executePersistentStoreCallback(&header, m_payloadBuffer, payloadSize);
      break;
    case CLASS_DEVICE_FIRMWARE_UPGRADE:
      err = executeDeviceFirmwareUpgradeCallback(&header, m_payloadBuffer, payloadSize);
      break;
    default:
      return COMMAND_NOT_RECOGNIZED;
  }

  return err;
}


/**
 * @brief      Gets the reply header.
 *
 * @param      header  The header
 *
 * @return     The reply header.
 */
ErrorCode Wf121Driver :: getReplyHeader(BgApiHeader *header){
  Timeout counter;
  bool dataReady = false;

#if __USE_CTS_RTS__
  gioSetBit(gioPORTB,3,0);       // Pull low RTS : ready to receive some data
#endif //#if __USE_CTS_RTS__

  for(counter = BLOCKING_TIMEOUT_US; counter > 0; counter--){
    if(sciIsRxReady(SCI_REG)){
      dataReady = true;
      break;
    }
  }

  if(dataReady == false){
#if __USE_CTS_RTS__
    gioSetBit(gioPORTB,3,1);       // Pull low RTS : ready to receive some data
#endif //#if __USE_CTS_RTS__
    return TRY_AGAIN;
  }

  // Always receive 4 bytes to start the message
  sciReceive(SCI_REG, sizeof(header), (uint8_t*) header);

#if __USE_CTS_RTS__
  gioSetBit(gioPORTB,3,1);    // Release RTS
#endif //#if __USE_CTS_RTS__

  // Check for consistency of the message received
  if(header->bit.technologyType != TT_BLUETOOTH && header->bit.technologyType != TT_WIFI){
      return COMMAND_NOT_RECOGNIZED;
  }

  if(header->bit.msgType != CMD_RSP_TYPE && header->bit.msgType != EVENT_TYPE){
      return COMMAND_NOT_RECOGNIZED;
  }

  if(header->bit.classId > CLASS_WIRED_ETHERNET){
      return COMMAND_NOT_RECOGNIZED;
  }

  return NO_ERROR;
}

/**
 * @brief      Receive data payload from WF121 module
 *
 * @param      payload      The payload
 * @param[in]  payloadSize  The payload size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: getReplyPayload(uint8_t *payload,
                                         const uint16_t payloadSize){

  // Check if there is more data to read
  if(payload == NULL){
    return INVALID_PARAMETER;
  }

#if __USE_CTS_RTS__
  gioSetBit(gioPORTB,3,0);       // Pull low RTS : ready to receive some data
#endif //#if __USE_CTS_RTS__

  while(!sciIsRxReady(SCI_REG));
  sciReceive(SCI_REG, payloadSize, payload);

#if __USE_CTS_RTS__
  gioSetBit(gioPORTB,3,1);    // Release RTS
#endif //#if __USE_CTS_RTS__

  return NO_ERROR;
}


/**
 * @brief      Gets the payload size from header.
 *
 * @param      header  The header
 *
 * @return     The payload size from header.
 */
uint16_t Wf121Driver :: getPayloadSizeFromHeader(BgApiHeader *header){
    return header->bit.lengthLow + (header->bit.lengthHigh << 8);
}


/**
 * @brief      Sets the header payload size.
 *
 * @param      header  The header
 * @param[in]  size    The size
 */
void Wf121Driver :: setHeaderPayloadSize(BgApiHeader *header, const uint16_t size){
    header->bit.lengthLow = size & 0xFF;
    header->bit.lengthHigh = size >> 8 & 0x7;
}


/**
 * @brief      Return if a command is processing or not
 *
 * @return     If a command is processing then the function return true,
 *             otherwise it returns false.
 */
bool Wf121Driver :: CommandIsProcessing(){
  return m_processingCmd;
}

ErrorCode Wf121Driver :: cb_EventEndpointSyntaxError(const uint16_t result,
                                                     const Endpoint endpoint){
    if(result != NO_ERROR){
        m_processingCmd = false;
    }

    return (ErrorCode) result;
}

