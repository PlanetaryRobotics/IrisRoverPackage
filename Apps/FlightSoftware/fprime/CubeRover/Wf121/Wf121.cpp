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
  switch(header->bit.msgId){
    default:
      return COMMAND_NOT_RECOGNIZED;
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
  switch(header->bit.msgId){
    default:
      return COMMAND_NOT_RECOGNIZED;
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
  switch(header->bit.msgId){
    default:
      return COMMAND_NOT_RECOGNIZED;
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
    switch(header->bit.msgId){
      default:
        return COMMAND_NOT_RECOGNIZED;
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
    switch(header->bit.msgId){
      default:
      return COMMAND_NOT_RECOGNIZED;
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
  switch(header->bit.msgId){
    default:
      return COMMAND_NOT_RECOGNIZED;
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
  switch(header->bit.msgId){
    default:
      return COMMAND_NOT_RECOGNIZED;
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
  switch(header->bit.msgId){
    default:
      return COMMAND_NOT_RECOGNIZED;
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
  switch(header->bit.msgId){
    default:
      return COMMAND_NOT_RECOGNIZED;
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
  if(header->bit.msgType == CMD_RSP_TYPE){
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

