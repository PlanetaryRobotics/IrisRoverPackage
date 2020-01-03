#include "Wf121.hpp"

#define MAX_SIZE_PAYLOAD    2048

using namespace Wf121;

ErrorCode Wf121Driver :: HelloSystem(){
  BgApiHeader txHeader;
  BgApiHeader rxHeader;
  
  txHeader.bit.msgType = 0; // command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0); //no payload expected
  txHeader.bit.classId = CLASS_SYSTEM;
  txHeader.bit.cmdId = 0x02;

  transmitData(&txHeader);
  receiveData(&rxHeader);

  // transmitted header has to match received header
  // to check that the module is working properly
  return (txHeader.all == rxHeader.all) ? NO_ERROR : HARDWARE_FAILURE;
}

ErrorCode Wf121Driver :: resetSystemWifi(const BootMode bootMode){
  BgApiHeader txHeader;
  uint8_t payload[1];

  txHeader.bit.msgType = 0; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload)); // payload of size 1
  txHeader.bit.classId = CLASS_SYSTEM;
  txHeader.bit.cmdId = 0x01;

  payload[0] = bootMode;
  transmitData(&txHeader, payload);
  receiveData(&rxHeader);

}

/**
 * @brief      Transmit data to WF121 module
 *
 * @param      header   The header
 * @param      payload  The payload
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: transmitData(BgApiHeader *header, uint8_t *payload){

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
    return NO_ERROR;
}


/**
 * @brief      Receive data from WF121 module
 *
 * @param      header   The header
 * @param      payload  The payload
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: receiveData(BgApiHeader *header, uint8_t *payload){

#if __USE_CTS_RTS__
  gioSetBit(gioPORTB,3,0);       // Pull low RTS : ready to receive some data
#endif //#if __USE_CTS_RTS__

  // Always receive 4 bytes to start the message
  while(!sciIsRxReady(SCI_REG));
  sciReceive(SCI_REG, sizeof(header), (uint8_t*) header);

#if __USE_CTS_RTS__
  gioSetBit(gioPORTB,3,1);    // Release RTS
#endif //#if __USE_CTS_RTS__

  uint16_t payloadSize = getPayloadSizeFromHeader(header);

  // Check if there is more data to read
  if(payloadSize > 0){
    if(payload == NULL){
     return INVALID_PARAMETER;
    }

#if __USE_CTS_RTS__
  gioSetBit(gioPORTB,3,0);       // Pull low RTS : ready to receive some data
#endif //#if __USE_CTS_RTS__

    while(!sciIsRxReady(SCI_REG));
    sciReceive(SCI_REG, payloadSize, payload);
  }

#if __USE_CTS_RTS__
  gioSetBit(gioPORTB,3,1);    // Release RTS
#endif //#if __USE_CTS_RTS__

  return NO_ERROR;
}

uint16_t Wf121Driver :: getPayloadSizeFromHeader(BgApiHeader *header){
    return header->bit.lengthLow + header->bit.lengthHigh << 8;
}

void Wf121Driver :: setHeaderPayloadSize(BgApiHeader *header, const uint16_t size){
    header->bit.lengthLow = size & 0xFF;
    header->bit.lengthHigh = size >> 8 & 0x7;
}

}
