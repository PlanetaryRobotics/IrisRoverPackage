#ifndef CUBEROVER_WF121_WF121_HPP_
#define CUBEROVER_WF121_WF121_HPP_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include "sci.h"
#include "gio.h"

#define MAX_SIZE_PAYLOAD    2048
#define SCI_REG             sciREG

#define __USE_CTS_RTS__     1

namespace Wf121{
  typedef enum CommandClass{
    CLASS_SYSTEM = 0x01,
    CLASS_CONFIGURATION = 0x02,
    CLASS_WIFI = 0x03,
    CLASS_TCP_STACK = 0x04,
    CLASS_ENDPOINT = 0x05,
    CLASS_HARDWARE = 0x06,
    CLASS_I2C = 0x08,
    CLASS_WIRED_ETHERNET = 0xA,
    CLASS_HTTP_SERVER = 0x09,
    CLASS_PERSISTENT_STORE = 0x07,
    CLASS_DEVICE_FIRMWARE_UPGRADE = 0x00
  }CommandClass;

  typedef enum ErrorCode{
    NO_ERROR = 0,
    INVALID_PARAMETER = 0x0180,
    DEVICE_WRONG_STATE = 0x0181,
    OUT_OF_MEMORY = 0x0182,
    FEATURE_NOT_IMPLEMENTED = 0x0183,
    COMMAND_NOT_RECOGNIZED = 0x0184,
    TIMEOUT = 0x0185,
    UNSPECIFIED_ERROR = 0x0186,
    HARDWARE_FAILURE = 0x0187,
    INTERNAL_BUFFER_FULL = 0x0188,
    DISCONNECTED = 0x0189,
    TOO_MANY_REQUEST = 0x018A,
    ACCESS_POINT_NOT_IN_SCANLIST = 0x018B,
    INVALID_PASSWORD = 0x018C,
    AUTHENTIFICATION_FAILURE = 0x018D,
    OVERFLOW = 0x018E,
    MULTIPLE_PBC_SESSIONS = 0x018F,
    ETHERNET_NOT_CONNECTED = 0x0190,
    ETHERNET_ROUTE_NOT_SET = 0x0191,
    WRONG_OPERATING_MODE = 0x0192,
    PS_STORE_FULL = 0x0301,
    PS_KEY_NOT_FOUND = 0x0302,
    I2C_WRITE_ALREADY_IN_PROGRESS = 0x0303,
    I2C_ACK_MISSING = 0x0304,
    TCP_IP_SUCCESS = 0x0200,
    TCP_IP_OUT_OF_MEMORY = 0x0201,
    TCP_IP_BUFFER_ERROR = 0x0202,
    TCP_IP_TIMEOUT = 0x0203,
    TCP_IP_ROUTING = 0x0204,
    TCP_IP_IN_PROGRESS = 0x0205,
    TCP_IP_ILLEGAL_VALUE = 0x0206,
    TCP_IP_WOULD_BLOCK = 0x0207,
    TCP_IP_ADDRESS_IN_USE = 0x0208,
    TCP_IP_ALREADY_CONNECTED = 0x0209,
    TCP_IP_CONNECTION_ABORTED = 0x020A,
    TCP_IP_CONNECTION_RESET = 0x020B,
    TCP_IP_CONNECTION_CLOSED = 0x020C,
    TCP_IP_NOT_CONNECTED = 0x020D,
    TCP_IP_ILLEGAL_ARGUMENT = 0x020E,
    TCP_IP_INTERFACE_LEVEL_ERROR = 0x020F,
    TCP_IP_UNKNOWN_HOST = 0x0280
  }ErrorCode; 

  typedef struct BgApiHeaderBits{
    uint32_t lengthHigh:3;            // bit 0..2
    uint32_t technologyType:4;       // Bit 3..6, 0000: bluetooth 4.0 single mode
                                     //           0001: wifi
    uint32_t msgType:1;              // Bit 7, 0: command/response
                                     //        1: event 
    uint32_t lengthLow:8;              // Bit 5..15, payload size
    uint32_t classId:8;              // Bit 16..23, command class ID
    uint32_t cmdId:8;                // Bit 24..31, command ID
  }BgApiHeaderBits;

  typedef union BgApiHeader{
    uint32_t all;
    BgApiHeaderBits bit;
  }BgApiHeader;

  class Wf121Driver{
    public:
      ErrorCode HelloSystem();
      ErrorCode resetSystemWifi(const BootMode bootMode);
    private:
      ErrorCode transmitData(BgApiHeader *header, uint8_t *payload = NULL);
      ErrorCode receiveData(BgApiHeader *header, uint8_t *payload = NULL);
      uint16_t getPayloadSizeFromHeader(BgApiHeader *header);
      void setHeaderPayloadSize(BgApiHeader *header, const uint16_t size);

     // uint8_t m_rxBuffer[4 /*header*/ + MAX_SIZE_PAYLOAD];
  };
}  // Wf121

#endif /* CUBEROVER_WF121_WF121_HPP_ */
