#include "Wf121.hpp"

#define MAX_SIZE_PAYLOAD    2048

using namespace Wf121;


/**
 * @brief      Hello System to check if the hardware is connected properly
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: HelloSystem(){
  BgApiHeader txHeader;
  BgApiHeader rxHeader;
  ErrorCode err;

  txHeader.bit.msgType = 0; // command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0); //no payload expected
  txHeader.bit.classId = CLASS_SYSTEM;
  txHeader.bit.cmdId = 0x02;

  err = transmitCommand(&txHeader);

  if(err != NO_ERROR) return err;

  err = receiveCommand(&rxHeader);

  if(err != NO_ERROR) return err;

  // transmitted header has to match received header
  // to check that the module is working properly
  return (txHeader.all == rxHeader.all) ? NO_ERROR : HARDWARE_FAILURE;
}


/**
 * @brief      Reset the wi-fi card to either main entry point or firmware
 *             update mode. Return an event when command is complete
 *
 * @param[in]  bootMode  The boot mode
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: resetSystemWifi(const BootMode bootMode){
  BgApiHeader txHeader;
  uint8_t payload[1];
  ErrorCode err;

  //Prepare command header
  txHeader.bit.msgType = 0; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload)); // payload of size 1
  txHeader.bit.classId = CLASS_SYSTEM;
  txHeader.bit.cmdId = 0x01; // reset system command

  // Prepare payload
  payload[0] = bootMode;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);
}

/**
 * @brief      Transmit data to WF121 module
 *
 * @param      header   The header
 * @param      payload  The payload
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: transmitCommand(BgApiHeader *header, uint8_t *payload){

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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x00: // Sync 
        break;
      case 0x01: // Reset
        break;
      case 0x02: // Hello
        break;
      case 0x03: // Set Max Power Saving State
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x00: // Boot
        break;
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x00:  // Get MAC
        break;
      case 0x01:  // Set MAC
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x00: // MAC address
        break;
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x00: // Wifi ON
        break;
      case 0x01: // Wifi OFF
        break;
      case 0x09: // Set scan channels
        break;
      case 0x03: // Start scan channels
        break;
      case 0x04: // Stop scan channels
        break;
      case 0x06: // Connect BSSID
        break;
      case 0x08: // Disconnect
        break;
      case 0x0D: // Scan results
        break;
      case 0x05: // Set password
        break;
      case 0x07: // Connect SSID
        break;
      case 0x13: // Get signal quality
        break;
      case 0x11: // Start WPS
        break;
      case 0x12: // Stop WPS
        break;
      case 0x0A: // Set operating mode
        break;
      case 0x10: // Set AP max clients
        break;
      case 0x0F: // Set AP password
        break;
      case 0x0B: // Start AP mode
        break;
      case 0x0C: // Stop AP mode
        break;
      case 0x0E: // AP disconnect client
        break; 
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x00: // Wifi ON
        break;
      case 0x01: // Wifi OFF
        break;
      case 0x02: // Scan result
        break;
      case 0x03: // Scan result drop
        break;
      case 0x04: // Scanned
        break;
      case 0x0F: // Scan sort result
        break;
      case 0x10: // Scan sort finished
        break;
      case 0x05: // Connected
        break;
      case 0x09: // Connect retry
        break;
      case 0x08: // Connect failed
        break;
      case 0x06: // Disconnected
        break;
      case 0x14: // WPS credential SSID
        break;
      case 0x15: // WPS credential password
        break;
      case 0x12: // WPS completed
        break;
      case 0x13: // WPS failed
        break;
      case 0x11: // WPS stopped
        break;
      case 0x16: // signal quality
        break;
      case 0x0A: // AP mode started
        break;
      case 0x0B: // AP mode stopped
        break;
      case 0x0C: // AP mode failed
        break;
      case 0x0D: // AP client joined
        break;
      case 0x0E: // AP client left
        break;
      case 0x07: // interface status
        break;
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x02: // set active
        break;
      case 0x00: // send
        break;
      case 0x05: // set transmit size
        break;
      case 0x01: // set streaming
        break;
      case 0x03: // set streaming destination
        break;
      case 0x04: // close endpoint
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x02: // status
        break;
      case 0x01: // data
        break;
      case 0x03: // closing
        break;
      case 0x04: // error
        break;
      case 0x00: // syntax error
        break;
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x09: // ADC read
        break;
      case 0x02: // Change notification
        break;
      case 0x03: // Change notification pullup
        break;
      case 0x01: // External interrupt
        break;
      case 0x04: // IO port config direction
        break;
      case 0x05: // IO port config drain
        break;
      case 0x07: // IO port read
        break;
      case 0x06: // IO port write
        break;
      case 0x08: // IO port compare
        break;
      case 0x0A: // RTC init
        break;
      case 0x0B: // RTC set time
        break;
      case 0x0C: // RTC get time
        break;
      case 0x0D: // RTC set alarm
        break;
      case 0x00: // set soft timer
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x01: // change notification
        break;
      case 0x02: // external interrupt
        break;
      case 0x03: // RTC alarm
        break;
      case 0x00: // soft timer
        break;
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x04: // configure
        break;
      case 0x08: // DHCP set hostname
        break;
      case 0x05: // DNS configure
        break;
      case 0x06: // DNS get host by name
        break;
      case 0x01: // TCP connect
        break;
      case 0x00: // Start TCP server
        break;
      case 0x03: // UDP connect
        break;
      case 0x07: // UDP bind
        break;
      case 0x02: // start UDP server
        break; 
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x00: // configuration
        break;
      case 0x01: // configuration DNS
        break;
      case 0x03: // Get host by name result
        break;
      case 0x02: // endpoint status
        break;
      case 0x04: //UDP data
        break;
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x02: // connected
        break;
      case 0x00: // set data route
        break;
      case 0x01: // close
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x00: // link status
        break;
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x03: // PS save
        break;
      case 0x04: // PS load
        break;
      case 0x07: // PS dump
        break;
      case 0x00: // PS defrag
        break;
      case 0x05: // PS erase
        break;
      case 0x02: // PS erase all
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x01: // PS keyy changed
        break;
      case 0x00: // PS key
        break;
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x00: // enable
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x01: // button
        break;
      case 0x00: // on req
        break;
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
    switch(header->bit.msgId){
      case 0x00: // reset
        break;
      case 0x01: // flash set address
        break;
      case 0x02: // flash upload
        break;
      case 0x03: // flash upload finished
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x00: // boot
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x00: // start read
        break;
      case 0x01: // start write
        break;
      case 0x02: // stop
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
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
ErrorCode Wf121Driver :: executeCallbacks(){
  ErrorCode err = NO_ERROR;
  BgApiHeader header;
  uint16_t payloadSize;

  //Check if some data is available in receiving buffer
  if(select(BLOCKING_TIMEOUT_US) == false) return NO_ERROR;

  //Some data is available in the buffer, process it
  err = getReplyHeader(&header);
  if(err != NO_ERROR) return err;

  // Get the payload if any. 
  payloadSize = getPayloadSizeFromHeader(header);

  // Some data need to be processed
  // Should not block at that point
  if(payloadSize > 0){
    err = getReplyPayload(m_payloadBuffer, payloadSize);
    if(err != NO_ERROR) return err;
  }

  // Execute class specific callback
  // 
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
      err = executeHttpServerCallback(&header, m_payloadSize, payloadSize);
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

  if(err != NO_ERROR) return err;

  // the command response is fully completed, now we can accept more command
  if(header.bit.msgType == CMD_RSP_TYPE){
    m_processingCmd = false;
  }

  return NO_ERROR;
}


/**
 * @brief      Poll receiving buffer
 *
 * @return    true if some data is available
 */
bool Wf121Driver :: select(const Timeout timeout){
  bool dataReady = false;
  Timeout counter;

#if __USE_CTS_RTS__
  gioSetBit(gioPORTB,3,0);       // Pull low RTS : ready to receive some data
#endif //#if __USE_CTS_RTS__

  for(counter = timeout; counter > 0; counter--){
    if(sciIsRxReady(SCI_REG)){
      dataReady = true;
      break;
    }
  }

 #if __USE_CTS_RTS__
  gioSetBit(gioPORTB,3,1);    // Release RTS
#endif //#if __USE_CTS_RTS__   

  return dataReady;
}


/**
 * @brief      Gets the reply header.
 *
 * @param      header  The header
 *
 * @return     The reply header.
 */
ErrorCode Wf121Driver :: getReplyHeader(BgApiHeader *header){
#if __USE_CTS_RTS__
  gioSetBit(gioPORTB,3,0);       // Pull low RTS : ready to receive some data
#endif //#if __USE_CTS_RTS__

  // Always receive 4 bytes to start the message
  sciReceive(SCI_REG, sizeof(header), (uint8_t*) header);

#if __USE_CTS_RTS__
  gioSetBit(gioPORTB,3,1);    // Release RTS
#endif //#if __USE_CTS_RTS__
}

/**
 * @brief      Receive data payload from WF121 module
 *
 * @param      payload      The payload
 * @param[in]  payloadSize  The payload size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: getReplyPayload(uint8_t *payload, const uint16_t payloadSize){

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
    return header->bit.lengthLow + header->bit.lengthHigh << 8;
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

