#ifndef CUBEROVER_WF121_WF121_HPP_
#define CUBEROVER_WF121_WF121_HPP_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include "sci.h"
#include "gio.h"

#define MAX_SIZE_PAYLOAD    256    // in byte
#define SCI_REG             sciREG
#define BLOCKING_TIMEOUT_US 10000    // in us

#define MAC_ADDRESS_SIZE      6       // in byte
#define HARDWARE_ADDRESS_SIZE 6       // in byte
#define IP_ADDRESS_V4_SIZE    4       // in byte
#define GATEWAY_SIZE          4       // in byte
#define NETMASK_SIZE          4       // in byte

#define __USE_CTS_RTS__       1

namespace Wf121{
  typedef enum CommandClass{
    CLASS_SYSTEM                  = 0x01,
    CLASS_CONFIGURATION           = 0x02,
    CLASS_WIFI                    = 0x03,
    CLASS_TCP_STACK               = 0x04,
    CLASS_ENDPOINT                = 0x05,
    CLASS_HARDWARE                = 0x06,
    CLASS_I2C                     = 0x08,
    CLASS_WIRED_ETHERNET          = 0x0A,
    CLASS_HTTP_SERVER             = 0x09,
    CLASS_PERSISTENT_STORE        = 0x07,
    CLASS_DEVICE_FIRMWARE_UPGRADE = 0x00
  }CommandClass;

  typedef enum ErrorCode{
    NO_ERROR                      = 0,
    INVALID_PARAMETER             = 0x0180,
    DEVICE_WRONG_STATE            = 0x0181,
    OUT_OF_MEMORY                 = 0x0182,
    FEATURE_NOT_IMPLEMENTED       = 0x0183,
    COMMAND_NOT_RECOGNIZED        = 0x0184,
    TIMEOUT                       = 0x0185,
    UNSPECIFIED_ERROR             = 0x0186,
    HARDWARE_FAILURE              = 0x0187,
    INTERNAL_BUFFER_FULL          = 0x0188,
    DISCONNECTED                  = 0x0189,
    TOO_MANY_REQUEST              = 0x018A,
    ACCESS_POINT_NOT_IN_SCANLIST  = 0x018B,
    INVALID_PASSWORD              = 0x018C,
    AUTHENTIFICATION_FAILURE      = 0x018D,
    OVERFLOW                      = 0x018E,
    MULTIPLE_PBC_SESSIONS         = 0x018F,
    ETHERNET_NOT_CONNECTED        = 0x0190,
    ETHERNET_ROUTE_NOT_SET        = 0x0191,
    WRONG_OPERATING_MODE          = 0x0192,
    PS_STORE_FULL                 = 0x0301,
    PS_KEY_NOT_FOUND              = 0x0302,
    I2C_WRITE_ALREADY_IN_PROGRESS = 0x0303,
    I2C_ACK_MISSING               = 0x0304,
    TCP_IP_SUCCESS                = 0x0200,
    TCP_IP_OUT_OF_MEMORY          = 0x0201,
    TCP_IP_BUFFER_ERROR           = 0x0202,
    TCP_IP_TIMEOUT                = 0x0203,
    TCP_IP_ROUTING                = 0x0204,
    TCP_IP_IN_PROGRESS            = 0x0205,
    TCP_IP_ILLEGAL_VALUE          = 0x0206,
    TCP_IP_WOULD_BLOCK            = 0x0207,
    TCP_IP_ADDRESS_IN_USE         = 0x0208,
    TCP_IP_ALREADY_CONNECTED      = 0x0209,
    TCP_IP_CONNECTION_ABORTED     = 0x020A,
    TCP_IP_CONNECTION_RESET       = 0x020B,
    TCP_IP_CONNECTION_CLOSED      = 0x020C,
    TCP_IP_NOT_CONNECTED          = 0x020D,
    TCP_IP_ILLEGAL_ARGUMENT       = 0x020E,
    TCP_IP_INTERFACE_LEVEL_ERROR  = 0x020F,
    TCP_IP_UNKNOWN_HOST           = 0x0280
  }ErrorCode; 

  typedef enum MsgType{
    CMD_RSP_TYPE = 0,
    EVENT_TYPE = 1
  }MsgType;

  typedef enum TechnologyType{
    TT_BLUETOOTH = 0,
    TT_WIFI = 1
  }TechnologyType;

  typedef struct BgApiHeaderBits{
    uint32_t lengthHigh:3;            // bit 0..2
    uint32_t technologyType:4;       // Bit 3..6, 0000: bluetooth 4.0 single mode
                                     //           0001: wifi
    uint32_t msgType:1;              // Bit 7, 0: command/response
                                     //        1: event 
    uint32_t lengthLow:8;            // Bit 5..15, payload size
    uint32_t classId:8;              // Bit 16..23, command class ID
    uint32_t cmdId:8;                // Bit 24..31, command ID
  }BgApiHeaderBits;

  typedef union BgApiHeader{
    uint32_t all;
    BgApiHeaderBits bit;
  }BgApiHeader;

  typedef enum BootMode{
    SYSTEM_BOOT = 0x00, // Normal boot
    DFU_BOOT    = 0x01  // Firmware update boot
  }BootMode;

  typedef enum PowerSavingState{
    LOW_LATENCY = 0,
    SAVE_POWER  = 1,
    DEEP_SLEEP  = 2
  }PowerSavingState;

  typedef enum OperatingMode{
    STATION      = 1,
    ACCESS_POINT = 2
  }OperatingMode;

  typedef enum SecurityMode{
    OPEN_SECURITY = 0,
    WPA_SECURITY  = 1,
    WPA2_SECURITY = 2,
    WEP_SECURITY  = 3
  }SecurityMode;

  typedef enum HardwareInterface{
    WIFI = 0
  }HardwareInterface;

  typedef enum DnsIndex{
    PRIMARY_DNS_SERVER   = 0,
    SECONDARY_DNS_SERVER = 1
  }DnsIndex;

  typedef enum Streaming{
    BGAPI_INTERFACE       = 0,
    STREAMING_TO_OTHER_EP = 1
  }Streaming;

  typedef enum Protocol{
    TCP = 0,
    UDP = 1
  }Protocol;

  typedef enum Wf121IoPort{
    PORT_A = 0,
    PORT_B = 1,
    PORT_C = 2,
    PORT_D = 3,
    PORT_E = 4,
    PORT_F = 5,
    PORT_G = 6   
  }Wf121IoPort;

  typedef enum CompareModuleIndex{
    COMP_MODULE_1 =  1,
    COMP_MODULE_2 =  2,
    COMP_MODULE_3 =  3,
    COMP_MODULE_4 =  4,
    COMP_MODULE_5 =  5,
  }CompareModuleIndex;

  typedef enum CompareModuleTimer{
    COMP_TIMER_2 = 2,
    COMP_TIMER_3 = 3
  }CompareModuleTimer;

  typedef enum CompareModuleMode{
    COMP_MODULE_MODE_0 = 0,
    COMP_MODULE_MODE_1 = 1,
    COMP_MODULE_MODE_2 = 2,
    COMP_MODULE_MODE_3 = 3,
    COMP_MODULE_MODE_4 = 4,
    COMP_MODULE_MODE_5 = 5,
    COMP_MODULE_MODE_6 = 7,
    COMP_MODULE_MODE_7 = 7       
  }CompareModuleMode;

  typedef enum UartDataBit{
    DATA_8_BITS = 8,
    DATA_9_BITS = 9
  }UartDataBit;
 
  typedef enum UartStopBit{
    STOP_1_BIT = 1,
    STOP_2_BIT = 2
  }UartStopBit;

  typedef enum UartParity{
    PARITY_NONE = 0,
    PARITY_ODD = 1,
    PARITY_EVEN = 2
  }UartParity;
                                      
  typedef enum UartFlowCtl{
    NO_FLOW_CTL = 0,
    RTS_CTS = 1,
    RTS = 2
  }UartFlowCtl;

  typedef enum WiredEthernetRoute{
    ROUTE_OFF = 0,
    ROUTE_BRIDGE = 1,
    ROUTE_ETHERNET_SERVER = 2,
    ROUTE_ETHERNET_DEVICE = 3
  }WiredEthernetRoute;

  typedef uint32_t Timeout;

  typedef uint8_t IpAddress[IP_ADDRESS_V4_SIZE];
  typedef uint8_t Gateway[GATEWAY_SIZE];
  typedef uint8_t Netmask[NETMASK_SIZE];
  typedef uint8_t MacAddress[MAC_ADDRESS_SIZE];
  typedef uint8_t HardwareAddress[HARDWARE_ADDRESS_SIZE];

  typedef uint8_t Channel;
  typedef uint8_t ChannelList;
  typedef uint8_t ChannelListSize;

  typedef uint8_t Password;
  typedef uint8_t PasswordSize;

  typedef uint8_t Ssid;
  typedef uint8_t SsidSize;

  typedef uint8_t DhcpHostName;
  typedef uint8_t DhcpHostNameSize;

  typedef uint8_t ServiceAttribute;
  typedef uint8_t ServiceAttributeSize;

  typedef uint8_t MdnsHostName;
  typedef uint8_t MdnsHostNameSize;

  typedef uint8_t ServiceName;
  typedef uint8_t ServiceNameSize;

  typedef uint8_t ServerPath;
  typedef uint8_t ServerPathSize;

  typedef uint8_t HttpResponseData;
  typedef uint8_t HttpResponseDataSize;

  typedef uint8_t KeyValue;
  typedef uint8_t KeyValueSize;

  typedef uint16_t TcpPort;
  typedef uint16_t UdpPort;

  typedef uint8_t DataSize;

  typedef int8_t StreamingDestination;

  typedef uint8_t HandleTimer;
  typedef uint32_t TimeMs;

  typedef uint8_t InterruptMask;

  typedef uint8_t Endpoint;

  class Wf121Driver{
    public:
      Wf121Driver();
      ~Wf121Driver();
      ErrorCode Init();

      // List of commands
      ErrorCode HelloSystem();
      ErrorCode ResetSystemWifi(const BootMode bootMode);
      ErrorCode SetMaxPowerSavingState(const PowerSavingState state);
      ErrorCode SyncSystem();
      ErrorCode GetMacAddress(const HardwareInterface interface);
      ErrorCode SetMacAddress(const HardwareInterface interface,
                              const MacAddress mac);
      ErrorCode TurnOnWifi();
      ErrorCode TurnOffWifi();
      ErrorCode SetScanChannels(const HardwareInterface interface, 
                                ChannelList *list,
                                const ChannelListSize channelListSize);
      ErrorCode StartScanChannels(const HardwareInterface interface, 
                                  ChannelList *list,
                                  const ChannelListSize channelListSize);
      ErrorCode StopScanChannels();
      ErrorCode ConnectBssid(const HardwareAddress bssid);
      ErrorCode Disconnect();
      ErrorCode ScanResultsSortRssi(const uint8_t amount);
      ErrorCode StartSsidScan(const Ssid *ssid, 
                              const SsidSize ssidSize);
      ErrorCode SetApHidden(const bool hidden);
      ErrorCode SetPassword(const Password *pwd,
                            const PasswordSize pwdSize);
      ErrorCode ConnectSsid(const Ssid *ssid,
                            const SsidSize ssidSize);
      ErrorCode GetSignalQuality();
      ErrorCode StartWps();
      ErrorCode StopWps(); 
      ErrorCode SetOperatingMode(const OperatingMode mode);
      ErrorCode Set11nMode(const bool mode);
      ErrorCode SetApClientIsolation(const bool isolation);
      ErrorCode SetApMaxClient(const uint8_t maxClients);
      ErrorCode SetApPassword(const Password *pwd,
                              const PasswordSize pwdSize);
      ErrorCode StartApMode(const Channel chan, 
                            const SecurityMode sm,
                            const Ssid *ssid,
                            const SsidSize ssidSize);
      ErrorCode StopApMode();
      ErrorCode DisconnectApClient(const HardwareAddress hwAddr);
      ErrorCode ConfigureTcpIp(const IpAddress ip, 
                               const Netmask mask,
                               const Gateway gateway,
                               const bool useDhcp); 
      ErrorCode SetDhcpHostName(DhcpHostName  *hostName,
                                const DhcpHostNameSize hostNameSize);
      ErrorCode ConfigureDns(const DnsIndex index, 
                             IpAddress *ip);
      ErrorCode GetDnsHostByName(DhcpHostName * name,
                                 const DhcpHostNameSize size);
      ErrorCode SetMdnsHostName(MdnsHostName * name,
                                const MdnsHostNameSize size);
      ErrorCode StartMDns();
      ErrorCode StopMDns();
      ErrorCode DnsSdAddService(const TcpPort port,
                                const Protocol protocol,
                                const ServiceName *serviceName,
                                const ServiceNameSize serviceNameSize);
      ErrorCode DnsSdAddServiceInstance(const uint8_t index,
                                        const ServiceName *serviceName,
                                        const ServiceNameSize serviceNameSize);
      ErrorCode DnsSdAddServiceAttribute(const uint8_t index,
                                         const ServiceAttribute *serviceAttribute,
                                         const ServiceAttributeSize serviceAttributeSize);
      ErrorCode DnsSdRemoveService(const uint8_t index);
      ErrorCode DnsSdStartService(const uint8_t index);
      ErrorCode DnsSdStopService(const uint8_t index);
      ErrorCode MulticastJoin(IpAddress *ip); 
      ErrorCode MulticastLeave(IpAddress *ip);
      ErrorCode DhcpConfigure(IpAddress *ip,
                              Netmask *netmask,
                              const uint32_t leaseTime);
      ErrorCode DhcpClients(); 
      ErrorCode TcpConnect(IpAddress *ip, 
                           const TcpPort port,
                           const int8_t routing);
      ErrorCode StartTcpServer(const TcpPort port,
                               const int8_t defaultDestination);
      ErrorCode UdpConnect(IpAddress *ip, 
                           const UdpPort port,
                           const int8_t routing);
      ErrorCode UdpBind(const Endpoint endpoint, 
                        const UdpPort port);
      ErrorCode StartUdpServer(const UdpPort port,
                               const uint8_t defaultDestination);
      ErrorCode DhcpEnableRouting(const bool enable);
      ErrorCode SetActiveEndpoint(const Endpoint endpoint,
                                  const bool endpointStatus);
      ErrorCode SendEndpoint(const Endpoint endpoint,
                              uint8_t *data,
                              const DataSize dataSize);
      ErrorCode SetTransmitSize(const Endpoint endpoint,
                                 const uint16_t transmitSize);
      ErrorCode SetStreaming(const Endpoint endpoint,
                             const Streaming streaming);
      ErrorCode SetStreamingDestination(const Endpoint endpoint,
                                        const StreamingDestination dest);
      ErrorCode CloseEndpoint(const Endpoint endpoint);
      ErrorCode DisableEndpoint(const Endpoint endpoint);
      ErrorCode SetSoftTimer(const TimeMs timeMs,
                             const HandleTimer handle,
                              const bool singleShot);
      ErrorCode ConfigureExternalInterrupt(const InterruptMask enable,
                                           const InterruptMask polarity);
      ErrorCode ConfigureChangeNotification(const uint32_t enable);
      ErrorCode ChangeNotificationPullup(const uint32_t pullup);
      ErrorCode ConfigureIoPort(const Wf121IoPort port,
                                const uint16_t bitMask,
                                const uint16_t bitDirection);
      ErrorCode ConfigureIoOpenDrain(const Wf121IoPort port,
                                     const uint16_t bitMask,
                                     const uint16_t openDrain);
      ErrorCode WriteIoPort(const Wf121IoPort port,
                            const uint16_t bitMask,
                            const uint16_t val);
      ErrorCode ReadIoPort(const Wf121IoPort port,
                           const uint16_t bitMask);
      ErrorCode OutputCompare(const CompareModuleIndex index,
                              const bool bit32,
                              const CompareModuleTimer timer,
                              const CompareModuleMode mode,
                              const uint32_t compareValue);
      ErrorCode AdcRead(const uint8_t adcInput);
      ErrorCode RtcInit(const bool enable,
                        const int16_t drift);
      ErrorCode RtcSetTime(const int16_t year,
                           const int8_t month,
                           const int8_t day,
                           const int8_t weekday,
                           const int8_t hour,
                           const int8_t minute,
                           const int8_t second);
      ErrorCode RtcGetTime();
      ErrorCode RtcSetAlarm(const int16_t year,
                            const int8_t month,
                            const int8_t day,
                            const int8_t weekday,
                            const int8_t hour,
                            const int8_t minute,
                            const int8_t second,
                            const uint8_t repeatMask,
                            const uint16_t repeatCount);
      ErrorCode ConfigureUart(const uint8_t uartId,
                              const uint32_t baudrate,
                              const UartDataBit format,
                              const UartStopBit stop,
                              const UartParity parity,
                              const UartFlowCtl flowCtl);
      ErrorCode GetHardwareConfiguration(const uint8_t uartId);
      ErrorCode I2cStartRead(const uint8_t endpoint,
                             const uint16_t slaveAddress,
                             const uint8_t length);
      ErrorCode I2cStartwrite(const uint8_t endpoint,
                              const uint16_t slaveAddress);
      ErrorCode I2cStop(const uint8_t endpoint);
      ErrorCode SetDataRoute(const WiredEthernetRoute route);
      ErrorCode CloseRoute();
      ErrorCode Connected();
      ErrorCode EnableServers(const bool https,
                              const bool dhcps,
                              const bool dnss);
      ErrorCode AddServerPath(const uint8_t device,
                              ServerPath *path,
                              const ServerPathSize pathSize);
      ErrorCode ApiResponse(const uint32_t request,
                            HttpResponseData *data,
                            const HttpResponseDataSize dataSize);
      ErrorCode ApiResponseFinish(const uint32_t request);
      ErrorCode DefragPersistentStore();
      ErrorCode EraseAllPersistentStore();
      ErrorCode SavePersistentStore(const uint16_t key,
                                    KeyValue *keyVal,
                                    const KeyValueSize keyValSize);
      ErrorCode LoadPersistentStore(const uint16_t key);
      ErrorCode DumpPersistentStore();
      ErrorCode ErasePersistentStore(const uint16_t key);

      // List of events

      // Other functions
      ErrorCode ExecuteCallbacks();
      bool CommandIsProcessing();
    private:

      ErrorCode transmitCommand(BgApiHeader *header,
                                uint8_t *payload = NULL);
      ErrorCode receiveCommand(BgApiHeader *header,
                               uint8_t *payload = NULL);
      uint16_t getPayloadSizeFromHeader(BgApiHeader *header);
      ErrorCode getReplyHeader(BgApiHeader *header);
      ErrorCode getReplyPayload(uint8_t *payload,
                                const uint16_t payloadSize);
      void setHeaderPayloadSize(BgApiHeader *header,
                                const uint16_t size);

      // Callbacks
      ErrorCode executeSystemCallback(BgApiHeader *header,
                                      uint8_t *payload,
                                      const uint16_t payloadSize);
      ErrorCode executeConfigurationCallback(BgApiHeader *header,
                                             uint8_t *payload,
                                             const uint16_t payloadSize);
      ErrorCode executeWifiCallback(BgApiHeader *header,
                                    uint8_t *payload,
                                    const uint16_t payloadSize);
      ErrorCode executeEndpointCallback(BgApiHeader *header,
                                        uint8_t *payload,
                                        const uint16_t payloadSize);
      ErrorCode executeHardwareCallback(BgApiHeader *header,
                                        uint8_t *payload,
                                        const uint16_t payloadSize);
      ErrorCode executeTcpStackCallback(BgApiHeader *header,
                                        uint8_t *payload,
                                        const uint16_t payloadSize);
      ErrorCode executeWiredEthernetCallback(BgApiHeader *header,
                                             uint8_t *payload,
                                             const uint16_t payloadSize);
      ErrorCode executePersistentStoreCallback(BgApiHeader *header,
                                               uint8_t *payload,
                                               const uint16_t payloadSize);
      ErrorCode executeHttpServerCallback(BgApiHeader *header,
                                          uint8_t *payload,
                                          const uint16_t payloadSize);
      ErrorCode executeDeviceFirmwareUpgradeCallback(BgApiHeader *header,
                                                     uint8_t *payload,
                                                     const uint16_t payloadSize);
      ErrorCode executeI2cCallback(BgApiHeader *header,
                                   uint8_t *payload,
                                   const uint16_t payloadSize);
      // Private members
      uint8_t m_payloadBuffer[MAX_SIZE_PAYLOAD];
      bool m_processingCmd;
  };
}  // Wf121

#endif /* CUBEROVER_WF121_WF121_HPP_ */
