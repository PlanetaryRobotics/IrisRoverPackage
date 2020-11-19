#ifndef CUBEROVER_WF121_WF121_HPP_
#define CUBEROVER_WF121_WF121_HPP_

/**
 *  Implementation of the Bluegiga-Wifi- Software API 3.0 
 *  https://www.silabs.com/documents/public/reference-manuals/Bluegiga-WiFi-Software-3.0-API-RM.pdf
 *  
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <math.h>

#include "sci.h"
#include "gio.h"
#include "Include/FswPacket.hpp"      // PrimaryFlightController/FlightMCU/Include

#define MAX_SIZE_PAYLOAD    UDP_MAX_PAYLOAD    // in byte
#define SCI_REG             sciREG
#define BLOCKING_TIMEOUT_US 1000    // in us

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
    TRY_AGAIN                     = 0x0001,
    INVALID_PARAMETER             = 0x0180, // This error code indicates that a command contained an invalid
                                            // parameter
    DEVICE_WRONG_STATE            = 0x0181, // This error code indicates that the device is in wrong state to
                                            // accept commands.
    OUT_OF_MEMORY                 = 0x0182, // This error indicates that the device has run out of memory.
    FEATURE_NOT_IMPLEMENTED       = 0x0183, // This error indicates that the feature in question has not been
                                            // implemented.
    COMMAND_NOT_RECOGNIZED        = 0x0184, // This error indicates that the issued command was not recognized.
    TIMEOUT                       = 0x0185, // This error indicates that a command or procedure failed due to
                                            // timeout.This error code is generated e.g. if you send an
                                            // incomplete command to the Wi-Fi module - after the timeout of 1
                                            // second this error code is sent to the host transported by the
                                            // event .endpoint_syntax_errorThis error code is generated also
                                            // e.g. when the maximum number of retry attempts (10) to try to
                                            // connect to a wireless network have been executed. A typical
                                            // example of such a case might be when the issued password is
                                            // invalid, in which case the error code is transported by the event
                                            // .wifi_evt_sme_connect_failed
    UNSPECIFIED_ERROR             = 0x0186, // This error code is generated when an unspecified error is
                                            // detected.
    HARDWARE_FAILURE              = 0x0187, // This error code is generated when a hardware failure is detected.
    INTERNAL_BUFFER_FULL          = 0x0188, // This error code is generated the a command was not accepted due
                                            // to full internal buffers.
    DISCONNECTED                  = 0x0189, // This error code is generated when a command or procedure has
                                            // failed due to disconnection.
    TOO_MANY_REQUEST              = 0x018A, // This error code is generated when there are too many simultaneous
                                            // requests.
    ACCESS_POINT_NOT_IN_SCANLIST  = 0x018B, // This error code is generated when the defined Access Point is not
                                            // found from the scanlist.
    INVALID_PASSWORD              = 0x018C, // This error code is generated in the following cases:1) you try to
                                            // connect to a secured network without setting the password or the
                                            // password is too short2) you try to start a secured AP without
                                            // setting the password or the password is too short3) you try to
                                            // set a 64-character PSK with non-hex characters4) you try to set
                                            // an invalid WEP key (invalid characters or invalid length)Notice
                                            // that WPA does not contain any standard way for the Access Point
                                            // to communicate to the station that the password is invalid. The
                                            // Access Point just disconnects the client during authentication if
                                            // the password is found invalid. Some stations take an educated
                                            // guess that this probably means the password is incorrect. WF121
                                            // simply retries the authentication until it exceeds the maximum
                                            // amount of retries (10) which then causes the 0x0185 Timeout
                                            // Error.
    AUTHENTIFICATION_FAILURE      = 0x018D, // This error code is generated when the WPA/WPA2 authentication has
                                            // failed.
    OVERFLOW                      = 0x018E, // This error code is generated when an overflow has been detected.
    MULTIPLE_PBC_SESSIONS         = 0x018F, // This error code is generated when multiple PBC (Push Button
                                            // Configuration) sessions have been detected.
    ETHERNET_NOT_CONNECTED        = 0x0190, // This error code is generated when the Ethernet cable is not
                                            // connected
    ETHERNET_ROUTE_NOT_SET        = 0x0191, // This error code is generated if the Ethernet route is not set
    WRONG_OPERATING_MODE          = 0x0192, // This error code is generated if the operating mode is wrong for
                                            // the issued command
    WIFI_NOT_FOUND                = 0x0193, // This error code is generated if the requested resource was not
                                            // found.
    WIFI_NOT_ALREADY_EXIST        = 0x0194, // This error is generated if the requested resource already exists.
    WIFI_INVALID_CONFIGURATION    = 0x0195, // This error code is generated if the current configuration is
                                            // invalid.
    WIFI_ACCESS_POINT_LOST        = 0x0196, // This error code is generated if the connection to an Access Point
                                            // is lost.
    PS_STORE_FULL                 = 0x0301, // This error is generated if the Flash which is reserved for the PS
                                            // (Persistent Store) is full.
    PS_KEY_NOT_FOUND              = 0x0302, // This error is generated if the PS key (Persistent Store key) was
                                            // not found.
    I2C_WRITE_ALREADY_IN_PROGRESS = 0x0303, // This error is generated if I2C transmission was initiated while a
                                            // transmission was already in progress.
    I2C_ACK_MISSING               = 0x0304, // This error is generated if an acknowledgement for I2C was not
                                            // received.
    FLASH_WRITE_FAILED            = 0x0308, // This error code is generated if writing to Flash failed.
    FILE_NOT_OPENED               = 0x0305, // This error code is generated if an access was attempted to an
                                            // unopened file.
    FILE_NOT_FOUND                = 0x0306, // This error code is generated if the requested file was not found
                                            // in the SD card.
    DISK_ERROR                    = 0x0307, // This error code is generated if an SD card error was detected of
                                            // if the SD card is full
    TCP_IP_SUCCESS                = 0x0200, // This code indicates that no error was detected.
    TCP_IP_OUT_OF_MEMORY          = 0x0201, // This error code is generated when the system has run out of
                                            // memory.
    TCP_IP_BUFFER_ERROR           = 0x0202, // This error code is generated when handling of buffers has failed
    TCP_IP_TIMEOUT                = 0x0203, // This error code is generated when a timeout has been detected.
    TCP_IP_ROUTING                = 0x0204, // This error code is generated when a route could not be found.
    TCP_IP_IN_PROGRESS            = 0x0205, // This error code is generated when an operation is in progress
    TCP_IP_ILLEGAL_VALUE          = 0x0206, // This error code is generated when the issued value is deemed
                                            // illegal.
    TCP_IP_WOULD_BLOCK            = 0x0207, // This error code is generated when an operation blocks.
    TCP_IP_ADDRESS_IN_USE         = 0x0208, // This error code is generated when the issued address is already
                                            // in use
    TCP_IP_ALREADY_CONNECTED      = 0x0209, // This error code is generated when the Wi-Fi module is already
                                            // connected
    TCP_IP_CONNECTION_ABORTED     = 0x020A, // This error code is generated when a connection is aborted.
    TCP_IP_CONNECTION_RESET       = 0x020B, // This error code is generated when a connection has been reset
    TCP_IP_CONNECTION_CLOSED      = 0x020C, // This error code is generated when a connection has been closed.
    TCP_IP_NOT_CONNECTED          = 0x020D, // This error code is generated when the Wi-Fi module is not connected.
    TCP_IP_ILLEGAL_ARGUMENT       = 0x020E, // This error code is generated if an illegal argument was issued.
    TCP_IP_INTERFACE_LEVEL_ERROR  = 0x020F, // This error code is generated if an interface error was detected
    TCP_IP_UNKNOWN_HOST           = 0x0280, // This error is generated if an unknown host is detected.
    SERVICE_NOT_RUNNING           = 0x0210, // This error code is generated if the specified service is not
                                            // running.
    SERVICE_RUNNING               = 0x0211, // This error code is generated if the specified service is already
                                            // running
    HOSTNAME_NOT_SET              = 0x0212, // This error code is generated if the hostname has not been set
    HOSTNAME_CONFLICT             = 0x0213  // This error code is generated if a hostname conflict was detected.
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
    DEEP_SLEEP  = 2,
    NOT_DEFINED = 3
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

  typedef enum EndpointType{
    ENDPOINT_FREE       = 0,
    ENDPOINT_UART       = 1,
    ENDPOINT_USB        = 2,
    ENDPOINT_TCP        = 4,
    ENDPOINT_TCP_SERVER = 8,
    ENDPOINT_UDP        = 16,
    ENDPOINT_UDP_SERVER = 32,
    ENDPOINT_SCRIPT     = 64,
    ENDPOINT_WAIT_CLOSE = 128,
    ENDPOINT_SPI        = 256,
    ENDPOINT_I2C        = 512,
    ENFPOINT_DROP       = 1024
  }EndpointType;

  typedef enum RtcAlarmRepeat{
    HARDWARE_ALARM_EVERY_HALF_SECOND = 0,
    HARDWARE_ALARM_EVERY_SECOND = 1,
    HARDWARE_ALARM_EVERY_TEN_SECONDS = 2,
    HARDWARE_ALARM_EVERY_MINUTE = 3,
    HARDWARE_ALARM_EVERY_TEN_MINUTES = 4,
    HARDWARE_ALARM_EVERY_HOUR = 5,
    HARDWARE_ALARM_EVERY_DAY = 6,
    HARDWARE_ALARM_EVERY_WEEK = 7,
    HARDWARE_ALARM_EVERY_MONTH = 8,
    HARDWARE_ALARM_EVERY_YEAR = 9
  }RtcAlarmRepeat;

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

  typedef uint8_t DnsName;
  typedef uint8_t DnsNameSize;

  typedef uint8_t ServerPath;
  typedef uint8_t ServerPathSize;

  typedef uint8_t HttpResponseData;
  typedef uint8_t HttpResponseDataSize;

  typedef uint8_t KeyValue;
  typedef uint8_t KeyValueSize;

  typedef uint16_t TcpPort;
  typedef uint16_t UdpPort;

  typedef uint8_t DataSize8;
  typedef uint16_t DataSize16;

  typedef int8_t StreamingDestination;

  typedef uint8_t HandleTimer;
  typedef uint32_t TimeMs;

  typedef uint8_t InterruptMask;

  typedef uint8_t Endpoint;

  static uint8_t g_payloadBuffer[MAX_SIZE_PAYLOAD];

  class Wf121Driver{
    public:
      Wf121Driver();
      virtual ~Wf121Driver() {}
      ErrorCode Init();

      // List of commands
      // System class commands
      ErrorCode HelloSystem();
      ErrorCode ResetSystemWifi(const BootMode bootMode);
      ErrorCode SetPowerSavingState(const PowerSavingState state);
      ErrorCode SyncSystem();

      // Configuration wi-fi commands
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
                               const int8_t defaultDestination);
      ErrorCode DhcpEnableRouting(const bool enable);
      ErrorCode SetActiveEndpoint(const Endpoint endpoint,
                                  const bool endpointStatus);
      ErrorCode SendEndpoint(const Endpoint endpoint,
                              uint8_t *data,
                              const DataSize8 dataSize);
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
      ErrorCode ConfigureIoPortDirection(const Wf121IoPort port,
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

      // -------------------------------------------------------------------------------------
      // List of event callbacks
      //--------------------------------------------------------------------------------------

      /**
       * @brief      This event indicates that a protocol error was detected in
       *             BGAPI command parser. This event is triggered if a BGAPI
       *             command from the host contains syntax error(s), or if a
       *             command is only partially sent. Then the BGAPI parser has a
       *             1 second command timeout and if a valid command is not
       *             transmitted within this timeout an error is raised and the
       *             partial or wrong command will be ignored
       *
       * @param[in]  result    Error reasonTypical errors are:
       *                       - 0x0184 Command Not Recognized
       *                       - 0x0185 TimeoutFor these and other
       *                       error codes refer to the  documentation
       * @param[in]  endpoint  The endpoint
       *
       * @return     The error code.
       */
       ErrorCode cb_EventEndpointSyntaxError(const uint16_t result,
                                             const Endpoint endpoint);

      /**
       * @brief      This event indicates that the device has started and is
       *             ready to receive commands.
       *
       * @param[in]  major              The major
       * @param[in]  minor              The minor
       * @param[in]  patch              The patch
       * @param[in]  build              The build
       * @param[in]  bootloaderVersion  The bootloader version
       * @param[in]  tcpIpVersion       The tcp ip version
       * @param[in]  hwVersion          The hardware version
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventBoot(const uint16_t major,
                                     const uint16_t minor,
                                     const uint16_t patch,
                                     const uint16_t build,
                                     const uint16_t bootloaderVersion,
                                     const uint16_t tcpIpVersion,
                                     const uint16_t hwVersion) { return NO_ERROR; }

      /**
       * @brief      This event indicates that a software exception has
       *             occurred.
       *
       * @param[in]  address  Address where exception occurred
       * @param[in]  type     Type of exception.
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventSoftwareException(const uint32_t address,
                                                  const uint8_t type) { return NO_ERROR; }


      /**
       * @brief      This event indicates the power saving state into which the
       *             module has entered
       *
       * @param[in]  state  The state
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventPowerSavingState(const PowerSavingState state) { return NO_ERROR; }


      /**
       * @brief      This event indicates the current MAC address of the device
       *
       * @param[in]  interface  The hardware interface to use 0: Wi-Fi
       * @param[in]  hwAddr     The current MAC address
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventMacAddress(const HardwareInterface interface,
                                           const HardwareAddress hwAddr) { return NO_ERROR; }

      
      /**
       * @brief      This event indicates that the 802.11 radio is powered up
       *             and ready to receive commands.
       *
       * @param[in]  result  Return code 0 : success non-zero : An error
       *                     occurred
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventWifiIsOn(const uint16_t result) { return NO_ERROR; }


 
      /**
       * @brief      This command can be used to turn off the 802.11 radio.
       *
       * @param[in]  result  The result
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventWifiIsOff(const uint16_t result) { return NO_ERROR; }



      /**
       * @brief      This event indicates Access Point scan results. After a
       *             scan has been started these events are sent every time an
       *             Access Point is discovered. Access Point is also added to
       *             internal scan list and it is possible to connect to it.
       *
       * @param[in]  hwAddr    The BSSID of an Access Point which was found
       * @param[in]  channel   The channel on which an Access Point was seen
       * @param[in]  rssi      The received signal strength indication of the
       *                       found Access Point, in dBm.The RSSI values are
       *                       reported in 1dBm steps.This event is triggered
       *                       only if the RSSI value corresponds to -88dBm or
       *                       above.
       * @param[in]  snr       The signal to noise ratio of an Access Point. The
       *                       values are reported in 1dB steps
       * @param[in]  secure    Access Point security status as a bitmask.bit 0:
       *                       whether the AP supports secure connectionsbit 1:
       *                       whether the AP supports WPS
       * @param[in]  ssid      The SSID of the network the Access Point belongs
       *                       to
       * @param[in]  ssidSize  The ssid size
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventScanResult(const HardwareAddress hwAddr,
                                           const int8_t channelIndex,
                                           const int16_t rssi,
                                           const int8_t snr,
                                           const uint8_t secure,
                                           const Ssid *ssid,
                                           const SsidSize ssidSize) { return NO_ERROR; }

      /**
       * @brief      This event indicates that the Access Point was dropped from
       *             the internal scan list. It is not possible to connect to it
       *             anymore.
       *
       * @param[in]  hwAddr  The BSSID of the Access Point that was dropped
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventScanResultDrop(const HardwareAddress hwAddr) { return NO_ERROR; }



      /**
       * @brief      This event indicates the Access Point scan is finished.
       *
       * @param[in]  scanStatus  Scan status 0 : scan finished normally
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventScanned(const int8_t scanStatus) { return NO_ERROR; }



      /**
       * @brief      This event indicates a successful connection to an Access Point.
       *
       * @param[in]  status       Wi-Fi connection status 0: Wi-Fi is connected 1:Wi-Fi is not connected
       * @param[in]  hwInterface  The hardware interface
       * @param[in]  ssid         The BSSID of the device that the module connected to
       * @param[in]  ssidSize     The ssid size
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventConnected(const int8_t status,
                                          const HardwareInterface hwInterface,
                                          const Ssid *bssid,
                                          const SsidSize bssidSize) { return NO_ERROR; }


      /**
       * @brief      This event indicates a disconnection from an Access Point.
       *             The event occurs either because sme_disconnectcommand was
       *             issued or connection to an Access Point was lost.
       *             Connection loss could occur because the Access Point has
       *             been switched off or the user has moved out of coverage.
       *             The timeout for detecting a lost connection is 50 beacons
       *             which under typical network configuration translates to
       *             roughly 5 seconds.
       *
       * @param[in]  reason       Disconnect reason For values refer to the
       *                          error code documentation
       * @param[in]  hwInterface  The hardware interface
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventDisconnected(const uint16_t reason,
                                             const HardwareInterface hwInterface) { return NO_ERROR; }


      /**
       * @brief      This event indicates the current network status. If for
       *             example DHCP has successfully finished and the module has
       *             an IP address, it will send this event with status = 1.
       *
       * @param[in]  hwInterface  The hardware interface
       * @param[in]  status       Network status 0 : network down 1 : network up
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventInterfaceStatus(const HardwareInterface hwInterface,
                                                const uint8_t status) { return NO_ERROR; }


      /**
       * @brief      This event indicates a failed connection to an Access
       *             Point. This event may occur if the device is unable to
       *             establish a connection to an Access Point or if the Access
       *             Point disconnects the device during the connection.
       *
       * @param[in]  reason       The reason
       * @param[in]  hwInterface  The hardware interface
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventConnectFailed(const uint16_t reason,
                                              const HardwareInterface hwInterface) { return NO_ERROR; }


      /**
       * @brief      This event indicates that a connection attempt failed,
       *             which will eventually lead to an automatic retry. This
       *             event appears typically when the module is commanded to
       *             connect to a wireless network but the given password is
       *             wrong. The amount of retries is fixed to 10 and the retries
       *             can be stopped with the command
       *
       * @param[in]  hwInterface  The hardware interface
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventConnectRetry(const HardwareInterface hwInterface) { return NO_ERROR; }



      /**
       * @brief      This event indicates that the Wi-Fi Access Point mode has
       *             been successfully started.
       *
       * @param[in]  hwInterface  The hardware interface
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventApModeStarted(const HardwareInterface hwInterface) { return NO_ERROR; }



      /**
       * @brief      This event indicates that the Wi-Fi Access Point mode has
       *             been stopped
       *
       * @param[in]  hwInterface  The hardware interface
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventApModeStopped(const HardwareInterface hwInterface) { return NO_ERROR; }



      /**
       * @brief      This event indicates that the Wi-Fi Access Point mode has failed.
       *
       * @param[in]  reason       The reason
       * @param[in]  hwInterface  The hardware interface
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventApModeFailed(const uint16_t reason,
                                             const HardwareInterface hwInterface) { return NO_ERROR; }


      /**
       * @brief      This event indicates that a Wi-Fi client has joined the network
       *
       * @param[in]  address      MAC address of the station
       * @param[in]  hwInterface  Hardware interface 0 : Wi-Fi
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventApClientJoined(const HardwareAddress address,
                                               const HardwareInterface hwInterface) { return NO_ERROR; }


      /**
       * @brief      This event indicates that a Wi-Fi client (client), has left
       *             the Access Point.In case a station moves out of range or is
       *             abruptly powered off, it might take from 180 to 270 seconds
       *             for this event to be issued at the module operating as the
       *             Access Point. This timeout range is due to the fact that
       *             the remote station is not sending any message to indicate
       *             that it is going to disconnect, while at the module side
       *             the conditions to declare that a client has left are the
       *             following: considering slots of 90 seconds from the moment
       *             the client connected, if in one slot there has been no
       *             message from the client, then the next slot is used to send
       *             empty frames to the client (at the interval of 15 seconds)
       *             and if there is still no response from the client then the
       *             connection is considered down, at which time this event is
       *             generated.
       *
       * @param[in]  address      MAC address of the station
       * @param[in]  hwInterface  The hardware interface
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventApClientLeft(const HardwareAddress address,
                                             const HardwareInterface hwInterface) { return NO_ERROR; }     


      /**
       * @brief      This event indicates the scan result.
       *
       * @param[in]  address   The address
       * @param[in]  channel   The channel
       * @param[in]  rssi      The rssi
       * @param[in]  snr       The snr
       * @param[in]  secure    The secure
       * @param[in]  ssid      The ssid
       * @param[in]  ssidSize  The ssid size
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventScanSortResult(const HardwareAddress address,
                                               const int8_t channel,
                                               const int16_t rssi,
                                               const int8_t snr,
                                               const uint8_t secure,
                                               const Ssid *ssid,
                                               const SsidSize ssidSize) { return NO_ERROR; }


      /**
       * @brief      This event indicates that the scan result sort is finished.
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventScanSortFinished() { return NO_ERROR; }



      /**
       * @brief      This event indicates that the Wi-Fi Protected Setup (WPS)
       *             session was stopped.
       *
       * @param[in]  hwInterface  The hardware interface
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventWpsStopped(const HardwareInterface hwInterface) { return NO_ERROR; }



      /**
       * @brief      This event indicates that the Wi-Fi Protected Setup (WPS)
       *             session was completed successfully.
       *
       * @param[in]  hwInterface  The hardware interface
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventWpsCompleted(const HardwareInterface hwInterface) { return NO_ERROR; }



      /**
       * @brief      This event indicates that the Wi-Fi Protected Setup (WPS)
       *             session failed
       *
       * @param[in]  reason       The reason
       * @param[in]  hwInterface  The hardware interface
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventWpsFailed(const uint16_t reason,
                                          const HardwareInterface hwInterface) { return NO_ERROR; }


      /**
       * @brief      This event indicates the SSID of the network in relation to
       *             Wi-Fi Protected Setup (WPS).
       *
       * @param[in]  hwInterface  The hardware interface
       * @param[in]  ssid         The ssid
       * @param[in]  ssidSize     The ssid size
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventCredentialSsid(const HardwareInterface hwInterface,
                                               const Ssid *ssid,
                                               const SsidSize ssidSize) { return NO_ERROR; }


      /**
       * @brief      This event indicates the password of then network in
       *             relation to Wi-Fi Protected Setup (WPS).
       *
       * @param[in]  hwInterface   The hardware interface
       * @param[in]  password      The password
       * @param[in]  passwordSize  The password size
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventWpsCredentialPassword(const HardwareInterface hwInterface,
                                                      const Password *password,
                                                      const PasswordSize passwordSize) { return NO_ERROR; }


      /**
       * @brief      This event indicates the signal quality (RSSI value) of the
       *             connection in dBm units
       *
       * @param[in]  rssi         The received signal strength indication (RSSI)
       *                          in dBm units
       * @param[in]  hwInterface  The hardware interface
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventSignalQuality(const int8_t rssi,
                                              const HardwareInterface hwInterface) { return NO_ERROR; }

 
      /**
       * @brief      This event indicates TCP/IP configuration status
       *
       * @param[in]  address  The address
       * @param[in]  netmask  The netmask
       * @param[in]  gateway  The gateway
       * @param[in]  useDhcp  DHCP used 0 = DHCP Client not being used 1 = DHCP Client being used
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventConfigureTcpIp(const IpAddress address,
                                               const Netmask netmask,
                                               const Gateway gateway,
                                               const uint8_t useDhcp) { return NO_ERROR; }


      /**
       * @brief      This event indicates DNS configuration status.Note that if
       *             static IP configuration is in use and no DNS configuration
       *             has been provided by the user, the primary DNS server
       *             defaults to 208.67.222.222 (resolver1.opendns.com)
       *
       * @param[in]  index    DNS server ID 0: primary DNS server 1: secondary DNS server
       * @param[in]  address  The address
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventDnsConfigureTcpIp(const uint8_t index,
                                                  const IpAddress address) { return NO_ERROR; }


      /**
       * @brief      This event indicates the current status of a TCP/IP
       *             endpoint.
       *
       * @param[in]  endpoint    The endpoint index this message describes
       * @param[in]  localIp     The local IP address of this endpoint
       * @param[in]  localPort   The local port of this endpoint
       * @param[in]  remoteIp    The remote IP address of this endpoint. For
       *                         server endpoints (which have no client), this
       *                         will not contain any valid value
       * @param[in]  remotePort  The port of the remote device. For server
       *                         endpoints (which have no client), this will not
       *                         contain any valid value.
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventTcpIpEndpointStatus(const uint8_t endpoint,
                                                    const IpAddress localIp,
                                                    const uint16_t localPort,
                                                    const IpAddress remoteIp,
                                                    const uint16_t remotePort) { return NO_ERROR; }

      /**
       * @brief      This event is generated as a response to a  command. If the
       *             procedure is successful, this DNS Gethostbynamemessage
       *             contains the IP address of the queried address.
       *
       * @param[in]  result       The result
       * @param[in]  address      The resolved IP address of the server
       * @param[in]  dnsName      The dns name
       * @param[in]  dnsNameSize  Name of the server whose IP address was resolved
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventGetDnsHostByName(const uint16_t result,
                                                 const IpAddress address,
                                                 const DnsName * dnsName,
                                                 const DnsNameSize dnsNameSize) { return NO_ERROR; }

      /**
       * @brief      This event indicates incoming data from an UDP endpoint. In
       *             order to receive this event, instead of the Endpoint event,
       *             use -1 as the default destination in the  command.
       *
       * @param[in]  endpoint    The endpoint which received this data, i.e. to
       *                         which it was sent
       * @param[in]  srcAddress  IP address of the client that sent this data
       * @param[in]  srcPort     Client UDP port where this data was sent from
       * @param[in]  data        The data
       * @param[in]  dataSize    The data size
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventUdpData(const Endpoint endpoint,
                                        const IpAddress srcAddress,
                                        const uint16_t srcPort,
                                        uint8_t * data,
                                        const DataSize16 dataSize) { return NO_ERROR; }

      /**
       * @brief      This event indicates that a mDNS service has been
       *             successfully started
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventMDnsStarted() { return NO_ERROR; }



      /**
       * @brief      This event indicates that a mDNS service has failed.
       *
       * @param[in]  reason  For values refer to the error code documentation
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventMDnsFailed(const uint16_t reason) { return NO_ERROR; }



      /**
       * @brief      This event indicates that a mDNS service has been stopped.
       *
       * @param[in]  reason  For values refer to the error code documentation
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventMDnsStopped(const uint16_t reason) { return NO_ERROR; } 


      /**
       * @brief      This event indicates that a DNS-SD service has been
       *             successfully started.
       *
       * @param[in]  index  Service index
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventDnsSdServiceStarted(const uint8_t index) { return NO_ERROR; }


      /**
       * @brief      This event indicates that a DNS-SD service has failed.
       *
       * @param[in]  reason  Failure reason For values refer to the error code
       *                     documentation
       * @param[in]  index   Service index
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventDnsSdServiceFailed(const uint16_t reason,
                                                    const uint8_t index) { return NO_ERROR; }


      /**
       * @brief      This event indicates that a DNS-SD service has stopped
       *
       * @param[in]  reason  Failure reasonFor values refer to the error code
       *                     documentation
       * @param[in]  index   The index
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventDnsSdServiceStopped(const uint16_t reason,
                                                    const uint8_t index) { return NO_ERROR; }


      /**
       * @brief      This event contains DHCP Server configuration.
       *
       * @param[in]  routingEnabled  DHCP server routing enabled0 = DHCP server
       *                             responses  include gateway and DNS
       *                             don'tserver information1 = Gateway and DNS
       *                             server information are included in
       *                             responses
       * @param[in]  address         First address of DHCP Server address pool
       * @param[in]  subnetMask      Subnetwork mask
       * @param[in]  leaseTime       DHCP address lease timeout
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventDhcpConfiguration(const uint8_t routingEnabled,
                                                  const IpAddress address,
                                                  const Netmask subnetMask,
                                                  const uint32_t leaseTime) { return NO_ERROR; }


      /**
       * @brief      This event contains IPv4 address and MAC address of one
       *             client connected to WF121 Access Point. Message is sent for
       *             each client one after another
       *
       * @param[in]  address  The address
       * @param[in]  address  The address
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventDhcpClient(const IpAddress ipAddress,
                                           const HardwareAddress hwAddress) { return NO_ERROR; }


      /**
       * @brief      This event indicates incoming data from an endpoint.
       *
       * @param[in]  endpoint  The endpoint which received this data, i.e. to
       *                       which it was sent.
       * @param      data      The raw data
       * @param[in]  dataSize  The data size
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventDataEndpoint(const Endpoint endpoint,
                                             uint8_t * data,
                                             const DataSize8 dataSize) { return NO_ERROR; }


 
      /**
       * @brief      cb_EventTcpIpEndpointStatus
       *
       * @param[in]  endpoint      The index of the endpoint whose status this
       *                           event describe
       * @param[in]  endpointType  The type of endpoint, see the endpoint type
       *                           enumeration
       * @param[in]  streaming     Endpoint mode 0 : Endpoint is connected to
       *                           BGAPI 1 : Endpoint is streaming to another
       *                           endpoint
       * @param[in]  destination   The index of the endpoint to which the
       *                           incoming data goes
       * @param[in]  active        Endpoint status 0 : receiving and sending of
       *                           data is blocked 1 : receiving and sending is
       *                           allowed.
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventEndpointStatus(const Endpoint endpoint,
                                               const EndpointType endpointType,
                                               const uint8_t streaming,
                                               const int8_t destination,
                                               const uint8_t active) { return NO_ERROR; }



      /**
       * @brief      This event indicates an endpoint is closing or indicates
       *             that the remote end has terminated the connection. The
       *             event should be acknowledged by calling the  command or
       *             otherwise the software will not re-use endpoint closethe
       *             endpoint index.
       *
       * @param[in]  reason    Zero indicates success. For other values refer to
       *                       the error code documentation
       * @param[in]  endpoint  The endpoint which is closing
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventClosingEndpoint(const uint16_t reason,
                                                const Endpoint endpoint) { return NO_ERROR; }



      /**
       * @brief      This event indicates an error in an endpoint
       *
       * @param[in]  reason    Error reason For values refer to the error code
       *                       documentation
       * @param[in]  endpoint  The endpoint where the error occurred
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventErrorEndpoint(const uint16_t reason,
                                              const Endpoint endpoint) { return NO_ERROR; }


      /**
       * @brief      This event indicates that a software timer has reached the
       *             defined count (elapsed)
       *
       * @param[in]  handle  The handle
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventSoftTimer(const uint8_t handle) { return NO_ERROR; }


      /**
       * @brief      This event indicates an IO port status change and provides
       *             a time stamp when the change occurred
       *
       * @param[in]  handle  Timestamp of when the change occurred
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventChangeNotification(const uint32_t handle) { return NO_ERROR; }


      /**
       * @brief      This event is generated when an external interrupt occurs
       *             and provides a time stamp and IRQ. The IRQ's and their
       *             corresponding pins are documented in the WF121 datasheet,
       *             page 9, table 2.
       *
       * @param[in]  irq        IRQ index
       * @param[in]  timestamp  Time stamp of when the interrupt occurred
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventExternalInterrupt(const uint8_t irq,
                                                  const uint32_t timestamp) { return NO_ERROR; }


      /**
       * @brief      This event indicates and alarm generated from the internal
       *             Real Time Clock (RTC).
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventRtcAlarm() { return NO_ERROR; }


      /**
       * @brief      This event reports the current configuration of a UART
       *             interface. It follows the command .hardware_uart_conf_get
       *
       * @param[in]  id        UART id
       * @param[in]  rate      Baud rate in bps e.g. 115200
       * @param[in]  dataBits  Data bits 8 or 9
       * @param[in]  stopBits  Stop bits 1 or 2
       * @param[in]  parity    Parity 0=none, 1=odd, 2=even
       * @param[in]  flowCtl   Flow control 0=none, 1=rts/cts 2=rts
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventUartConfiguration(const uint8_t id,
                                                  const uint32_t rate,
                                                  const uint8_t dataBits,
                                                  const uint8_t stopBits,
                                                  const uint8_t parity,
                                                  const uint8_t flowCtl) { return NO_ERROR; }


      /**
       * @brief      This event indicates the status changes of Ethernet link
       *             state
       *
       * @param[in]  status  Link state 0: link is downlink is up
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventLinkStatus(const uint8_t status) { return NO_ERROR; }



      /**
       * @brief      This command can be used to send HTTP response data to a
       *             pending HTTP request.
       *
       * @param[in]  request       The request
       * @param[in]  method        The method
       * @param[in]  resourceSize  The resource size
       * @param[in]  resource      The resource
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventApiRequest(const uint32_t request,
                                           const uint8_t method,
                                           const uint8_t resourceSize,
                                           const uint8_t * resource) { return NO_ERROR; }


      /**
       * @brief      This event includes the HTTP header data of a particular
       *             HTTP request. At least one event is expected to be
       *             generated, but multiple events may be also generated.
       *
       * @param[in]  request   Request number
       * @param[in]  dataSize  Request header data
       * @param[in]  data      The data
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventApiRequestHeader(const uint32_t request,
                                                 const uint8_t dataSize,
                                                 const uint8_t * data) { return NO_ERROR; }


      /**
       * @brief      This event includes HTTP payload data of a particular HTTP
       *             request. Multiple events may be generated, or none if the
       *             request does not carry data at all.
       *
       * @param[in]  request   Request number
       * @param[in]  dataSize  Request data
       * @param[in]  data      The data
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventApiRequestData(const uint32_t request,
                                               const uint8_t dataSize,
                                               const uint8_t * data) { return NO_ERROR; }



      /**
       * @brief      This event indicates that all HTTP header data, and payload
       *             data if any, have been fully delivered for a particular
       *             HTTP request. The HTTP request is left pending until  has
       *             been called or a API Response Finishtimeout occurs
       *
       * @param[in]  request  The request
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventApiRequestFinished(const uint32_t request) { return NO_ERROR; }




      /**
       * @brief      This event is generated when PS keys are dumped from the
       *             Persistent Store
       *
       * @param[in]  key    Key index 65535 : Last key
       * @param[in]  size   The size
       * @param[in]  value  The value
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventPersistentStoreKey(const uint16_t key,
                                                   const uint8_t size,
                                                   const uint8_t *value) { return NO_ERROR; }

      /**
       * @brief      This event indicates that a PS key has been changed
       *
       * @param[in]  key   The key
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventPersistentStoreKeyChanged(const uint16_t key) { return NO_ERROR; }



      /**
       * @brief      This event indicates that a PS key has been changed
       *
       * @return     The error code.
       */
      virtual ErrorCode cb_EventLowVoltageFlash() { return NO_ERROR; }

      // -------------------------------------------------------------------------------------
      // List of command callbacks
      //--------------------------------------------------------------------------------------

      // System class callbacks
      virtual ErrorCode cb_CommandHelloSystem() { return NO_ERROR; }
      virtual ErrorCode cb_CommandSyncSystem() { return NO_ERROR; }
      virtual ErrorCode cb_CommandSetPowerSavingState(const uint16_t result) { return NO_ERROR; }

      // Configuration wi-fi callbacks
      virtual ErrorCode cb_CommandGetMacAddress(const uint16_t result,
                                                const HardwareInterface interface) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSetMacAddress(const uint16_t result,
                                                const HardwareInterface interface) { return NO_ERROR; }

      // Wi-fi callbacks
      virtual ErrorCode cb_CommandTurnOnWifi(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandTurnOffWifi(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandStartScanChannels(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandStopScanChannels(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSetPassword(const uint8_t status) { return NO_ERROR; }
      virtual ErrorCode cb_CommandConnectBssid(const uint16_t result,
                                               const HardwareInterface interface,
                                               const HardwareAddress address) { return NO_ERROR; }
      virtual ErrorCode cb_CommandConnectSsid(const uint16_t result,
                                              const HardwareInterface interface,
                                              const HardwareAddress address) { return NO_ERROR; }
      virtual ErrorCode cb_CommandDisconnect(const uint16_t result,
                                             const HardwareInterface interface) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSetScanChannels(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSetOperatingMode(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandStartApMode(const uint16_t result,
                                              const HardwareInterface interface) { return NO_ERROR; }
      virtual ErrorCode cb_CommandStopApMode(const uint16_t result,
                                             const HardwareInterface interface) { return NO_ERROR; }
      virtual ErrorCode cb_CommandScanResultsSortRssi(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandDisconnectApClient(const uint16_t result,
                                                     const HardwareInterface interface) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSetApPassword(const uint8_t status) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSetApMaxClients(const uint16_t result,
                                                  const HardwareInterface interface) { return NO_ERROR; }
      virtual ErrorCode cb_CommandStartWps(const uint16_t result,
                                           const HardwareInterface interface) { return NO_ERROR; }
      virtual ErrorCode cb_CommandStopWps(const uint16_t result,
                                         const HardwareInterface interface) { return NO_ERROR; }
      virtual ErrorCode cb_CommandGetSignalQuality(const uint16_t result,
                                                   const HardwareInterface interface) { return NO_ERROR; }
      virtual ErrorCode cb_CommandStartSsidScan(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSetApHidden(const uint16_t result,
                                              const HardwareInterface interface) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSet11nMode(const uint16_t result,
                                              const HardwareInterface interface) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSetApClientIsolation(const uint16_t result,
                                                       const HardwareInterface interface) { return NO_ERROR; }
      virtual ErrorCode cb_CommandStartTcpServer(const uint16_t result,
                                                 const uint8_t endpoint) { return NO_ERROR; }
      virtual ErrorCode cb_CommandTcpConnect(const uint16_t result,
                                             const uint8_t endpoint) { return NO_ERROR; }
      virtual ErrorCode cb_CommandStartUdpServer(const uint16_t result,
                                                 const uint8_t endpoint) { return NO_ERROR; }
      virtual ErrorCode cb_CommandUdpConnect(const uint16_t result,
                                             const uint8_t endpoint) { return NO_ERROR; }
      virtual ErrorCode cb_CommandConfigureTcpIp(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandDnsConfigure(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandGetDnsHostByName(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandUdpBind(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSetDhcpHostName(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandDhcpEnableRouting(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSetMdnsHostName(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandStartMDns(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandStopMDns(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandDnsSdAddService(const uint16_t result,
                                                  const uint8_t endpoint) { return NO_ERROR; }
      virtual ErrorCode cb_CommandDnsSdAddServiceInstance(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandDnsSdAddServiceAttribute(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandDnsSdRemoveService(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandDnsSdStartService(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandDnsSdStopService(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandMulticastJoin(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandMulticastLeave(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandDhcpConfigure(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandDhcpClients(const uint16_t result,
                                              const uint8_t clientCount) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSendEndpoint(const uint16_t result,
                                               const Endpoint endpoint) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSetStreaming(const uint16_t result,
                                                          const Endpoint endpoint) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSetActiveEndpoint(const uint16_t result,
                                                    const Endpoint endpoint) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSetStreamingDestination(const uint16_t result,
                                                          const Endpoint endpoint) { return NO_ERROR; }
      virtual ErrorCode cb_CommandCloseEndpoint(const uint16_t result,
                                                const Endpoint endpoint) { return NO_ERROR; } 
      virtual ErrorCode cb_CommandSetTransmitSize(const uint16_t result,
                                                  const Endpoint endpoint) { return NO_ERROR; } 
      virtual ErrorCode cb_CommandDisableEndpoint(const uint16_t result,
                                                  const Endpoint endpoint) { return NO_ERROR; } 
      virtual ErrorCode cb_CommandSetSoftTimer(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandConfigureExternalInterrupt(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandConfigureChangeNotification(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandConfigureIoPortDirection(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandConfigureIoOpenDrain(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandWriteIoPort(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandReadIoPort(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandOutputCompare(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandAdcRead(const uint16_t result,
                                          const uint8_t input,
                                          const uint16_t value) { return NO_ERROR; }
      virtual ErrorCode cb_CommandRtcInit(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandRtcSetTime(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandRtcGetTime(const uint16_t result,
                                             const int16_t year,
                                             const int8_t month,
                                             const int8_t day,
                                             const int8_t weekday,
                                             const int8_t hour,
                                             const int8_t minute,
                                             const int8_t second) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSetAlarm(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandConfigureUart(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandGetUartConfiguration(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandI2cWrite(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandI2cRead(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandI2cStop(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSetDataRoute(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandCloseRoute(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandRouteConnected(const uint8_t state) { return NO_ERROR; }
      virtual ErrorCode cb_CommandEnableServers(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandAddServerPath(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandApiResponse(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandApiResponseFinsh(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandDefragPersistentStore(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandEraseAllPersistentStore(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandSavePersistentStore(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandLoadPersistentStore(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandDumpPersistentStore(const uint16_t result) { return NO_ERROR; }
      virtual ErrorCode cb_CommandErasePersistentStore(const uint16_t result) { return NO_ERROR; }

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
      bool m_processingCmd;
  };
}  // Wf121

#endif /* CUBEROVER_WF121_WF121_HPP_ */
