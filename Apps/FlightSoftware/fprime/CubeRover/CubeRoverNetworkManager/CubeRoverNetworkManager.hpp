#ifndef __CUBEROVER_NETWORK_MANAGER__
#define __CUBEROVER_NETWORK_MANAGER__

#include "CubeRover/Wf121/Wf121.hpp"

#define TRIES_EXECUTE_CALLBACK    50000
#define PRESCALE_CHECK_SIGNAL     0         // Used to throttle how often wifi signal quality need to checked
                                            // 0: for no prescaling

#define ROVER_IP_ADDRESS        {192, 168, 1, 2}
#define ROVER_MASK_ADDRESS      {255, 255, 255, 0}
#define ROVER_GATEWAY_ADDRESS   {192, 168, 1, 1}
#define GATEWAY_PORT            8080
#define ROVER_UDP_PORT          8080 

#define LANDER_SSID                     "PeregrineLander"
#define LANDER_NETWORK_PASSWORD         "Hello123"
#define MAX_SIZE_SSID_NAME              32
#define MAX_NUMBER_CHANNEL_PER_NETWORK  11
#define MAX_SORTING_LIST_SIZE           5

#define RX_RING_BUFFER_SIZE              MAX_SIZE_PAYLOAD

using namespace Wf121;

typedef enum CubeRoverNetworkStateMachine{
  UNINITIALIZED,
  INITIALIZED,
  WIFI_SLEEP,
  WIFI_TURNING_ON,
  WIFI_ON,
  SCANNING,
  SCANNED,
  CONNECTING,
  CONNECTED,
  DISCONNECTING,
  UDP_CONNECTED
}CubeRoverNetworkStateMachine;

typedef enum UdpReadMode{
  WAIT_UNTIL_READY = 0x01,// wait until data is available 
  NORMAL_READ = 0x02,     // read the data and dequeue buffer
  PEEK_READ = 0x04        // read the data without modifying the ring buffer
}UdpReadMode;

typedef struct WifiNetworkChannel{
  HardwareAddress bssid;
  int16_t rssi;
  int8_t snr;
  int8_t channel;
  bool active;
}WifiNetworkChannel;

typedef struct WifiNetwork{
  Ssid ssid[MAX_SIZE_SSID_NAME];
  WifiNetworkChannel channel[MAX_NUMBER_CHANNEL_PER_NETWORK];
}WifiNetwork;

typedef enum CubeRoverSignalLevels{
  VERY_GOOD = -10,
  OKAY      = -70,
  NOT_GOOD  = -80,
  UNUSABLE  = -90,
  NOT_DEFINED = 127
}CubeRoverSignalLevels;

typedef void(*NetworkManagerUserCbFunctionPtr)(void);

uint8_t g_rxRingBuffer[RX_RING_BUFFER_SIZE];

class CubeRoverNetworkManager : public Wf121Driver{
public:
  CubeRoverNetworkManager();
  ErrorCode UpdateNetworkManager();
  CubeRoverNetworkStateMachine GetState();
  int8_t GetSignalRssi();
  CubeRoverSignalLevels GetSignalLevel();

  void ConnectCallback(const CubeRoverSignalLevels signal,
                       NetworkManagerUserCbFunctionPtr cb);
  ErrorCode SendUdpData(uint8_t * data,
                        const uint32_t size,
                        const uint32_t timeoutus);
  ErrorCode ReceiveUdpData(uint8_t * data,
                          const uint16_t dataSize,
                          uint16_t * dataRead,
                          const uint8_t mode,
                          const uint32_t timeout);

  // Callback event
  ErrorCode cb_EventBoot(const uint16_t major,
                         const uint16_t minor,
                         const uint16_t patch,
                         const uint16_t build,
                         const uint16_t bootloaderVersion,
                         const uint16_t tcpIpVersion,
                         const uint16_t hwVersion);
  ErrorCode cb_EventPowerSavingState(const PowerSavingState state);
  ErrorCode cb_EventMacAddress(const HardwareInterface interface,
                               const HardwareAddress hwAddr);
  ErrorCode cb_EventWifiIsOn(const uint16_t result);
  ErrorCode cb_EventScanSortResult(const HardwareAddress address,
                                   const int8_t channel,
                                   const int16_t rssi,
                                   const int8_t snr,
                                   const uint8_t secure,
                                   const Ssid *ssid,
                                   const SsidSize ssidSize);
  ErrorCode cb_EventScanned(const int8_t scanStatus);
  ErrorCode cb_EventScanSortFinished();
  ErrorCode cb_EventConnected(const int8_t status,
                              const HardwareInterface hwInterface,
                              const Ssid *bssid,
                              const SsidSize bssidSize);
  ErrorCode cb_EventDisconnected(const uint16_t reason,
                                 const HardwareInterface hwInterface);
  ErrorCode cb_EventConnectFailed(const uint16_t reason,
                                  const HardwareInterface hwInterface);
  ErrorCode cb_EventSignalQuality(const int8_t rssi,
                                  const HardwareInterface hwInterface);
  ErrorCode cb_EventUdpData(const Endpoint endpoint,
                            const IpAddress srcAddress,
                            const uint16_t srcPort,
                            uint8_t * data,
                            const DataSize16 dataSize);
  ErrorCode cb_EventTcpIpEndpointStatus(const uint8_t endpoint,
                                        const IpAddress localIp,
                                        const uint16_t localPort,
                                        const IpAddress remoteIp,
                                        const uint16_t remotePort);
  ErrorCode cb_EventConfigureTcpIp(const IpAddress address,
                                   const Netmask netmask,
                                   const Gateway gateway,
                                   const uint8_t useDhcp);

  // Callback command
  ErrorCode cb_CommandHelloSystem();
  ErrorCode cb_CommandTurnOnWifi(const uint16_t result);
  ErrorCode cb_CommandSetPowerSavingState(const uint16_t result);
  ErrorCode cb_CommandConfigureTcpIp(const uint16_t result);
  ErrorCode cb_CommandStartScanChannels(const uint16_t result);
  ErrorCode cb_CommandConnectBssid(const uint16_t result,
                                   const HardwareInterface interface,
                                   const HardwareAddress address);
  ErrorCode cb_CommandSetPassword(const uint8_t status);
  ErrorCode cb_CommandGetSignalQuality(const uint16_t result,
                                       const HardwareInterface interface); 
  ErrorCode cb_CommandDisconnect(const uint16_t result,
                                 const HardwareInterface interface);
  ErrorCode cb_CommandUdpConnect(const uint16_t result,
                                 const uint8_t endpoint);
  ErrorCode cb_CommandUdpBind(const uint16_t result);
  ErrorCode cb_CommandStartUdpServer(const uint16_t result,
                                     const uint8_t endpoint);
  ErrorCode cb_CommandSendEndpoint(const uint16_t result,
                                   const Endpoint endpoint);
  ErrorCode cb_CommandSetTransmitSize(const uint16_t result,
                                      const Endpoint endpoint);

private:
  ErrorCode initializeNetworkManager();
  ErrorCode turnOnWifiAdapter();
  ErrorCode scanWifiNetwork();
  ErrorCode connectToWifiNetwork();
  ErrorCode manageSignalStrength();
  ErrorCode disconnectFromWifiNetwork();
  ErrorCode startUdpServer();
  bool ipAddressesMatch(const IpAddress addr1,
                        const IpAddress addr2);

  CubeRoverNetworkStateMachine m_state;
  bool m_wifiModuleDetected;
  PowerSavingState m_powerSavingState;
  bool m_wifiModuleIdentified;
  bool m_macAddressIdentified;
  HardwareAddress m_macAddress;
  bool m_powerSavingStateSet;
  bool m_ipConfigurationSet;
  bool m_passwordSet;
  bool m_commandSignalQualitySet;
  bool m_udpConnectSet;
  bool m_connectBssidSet;
  bool m_udpServerStarted;
  bool m_udpBindSet;
  bool m_commandSendEndpointSet;
  bool m_commandTransmitSizeSet;

  IpAddress m_roverIpAddress = ROVER_IP_ADDRESS;
  Netmask m_roverMaskAddress = ROVER_MASK_ADDRESS;
  Gateway m_udpGatewayAddress = ROVER_GATEWAY_ADDRESS;
  WifiNetwork m_landerWifi;
  uint8_t m_scanIndex;
  uint8_t m_connectIndex;
  uint8_t m_nbChannelFound;
  uint16_t m_preScaleCheckSignalCtr;
  CubeRoverSignalLevels m_signalLevel;
  NetworkManagerUserCbFunctionPtr m_userCbVeryGoodSignal;
  NetworkManagerUserCbFunctionPtr m_userCbOkaySignal;
  NetworkManagerUserCbFunctionPtr m_userCbNotGoodSignal;
  NetworkManagerUserCbFunctionPtr m_userCbUnusableSignal;
  uint32_t m_logNbOfBytesReceived;
  uint32_t m_logNbOfBytesSent;
  uint32_t m_rxUdpFifoBytesCount;
  uint32_t m_txUdpFifoBytesCount;
  uint16_t m_rxUdpFifoHeadPointer;
  uint16_t m_rxUdpFifoTailPointer;
  uint8_t m_udpSendEndpoint;
};

#endif
