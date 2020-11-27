#ifndef __CUBEROVER_NETWORK_MANAGER__
#define __CUBEROVER_NETWORK_MANAGER__

#include "CubeRover/Wf121/Wf121.hpp"
#include "Include/FswPacket.hpp"      // PrimaryFlightController/FlightMCU/Include

#define TRIES_EXECUTE_CALLBACK    50000
#define PRESCALE_CHECK_SIGNAL     0         // Used to throttle how often wifi signal quality need to checked
                                            // 0: for no prescaling

#define ROVER_IP_ADDRESS        {192, 168, 1, 2}
#define ROVER_MASK_ADDRESS      {255, 255, 255, 0}
#define ROVER_GATEWAY_ADDRESS   {192, 168, 1, 1}
#define GATEWAY_PORT            8080
#define ROVER_UDP_PORT          8080 

#define LANDER_SSID                     "Houston"
#define LANDER_NETWORK_PASSWORD         "redr0ver"
#define MAX_SIZE_SSID_NAME              32
#define MAX_NUMBER_CHANNEL_PER_NETWORK  11
#define MAX_SORTING_LIST_SIZE           5

#define RX_RING_BUFFER_SIZE              MAX_SIZE_PAYLOAD

#define MAX_FSM_NO_TRANSITION_COUNT 1024000000            // TODO: TUNE ME

namespace CubeRoverNetworkManager {

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
  Wf121::HardwareAddress bssid;
  int16_t rssi;
  int8_t snr;
  int8_t channel;
  bool active;
}WifiNetworkChannel;

typedef struct WifiNetwork{
  Wf121::Ssid ssid[MAX_SIZE_SSID_NAME];
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

class CubeRoverNetworkManager : public Wf121::Wf121Driver{
public:
  CubeRoverNetworkManager();
  Wf121::ErrorCode UpdateNetworkManager();
  CubeRoverNetworkStateMachine GetState();
  int8_t GetSignalRssi();
  int8_t GetSignalNoiseRatio();
  uint32_t GetNbOfBytesReceived();
  uint32_t GetNbOfBytesSent();
  CubeRoverSignalLevels GetSignalLevel();

  void ConnectCallback(const CubeRoverSignalLevels signal,
                       NetworkManagerUserCbFunctionPtr cb);
  Wf121::ErrorCode SendUdpData(uint8_t * data,
                        const uint32_t size,
                        const uint32_t timeoutus);
  Wf121::ErrorCode ReceiveUdpData(uint8_t * data,
                          const uint16_t dataSize,
                          uint16_t * dataRead,
                          const uint8_t mode,
                          const uint32_t timeout);

  // Callback event
  Wf121::ErrorCode cb_EventBoot(const uint16_t major,
                         const uint16_t minor,
                         const uint16_t patch,
                         const uint16_t build,
                         const uint16_t bootloaderVersion,
                         const uint16_t tcpIpVersion,
                         const uint16_t hwVersion);
  Wf121::ErrorCode cb_EventPowerSavingState(const Wf121::PowerSavingState state);
  Wf121::ErrorCode cb_EventMacAddress(const Wf121::HardwareInterface interface,
                               const Wf121::HardwareAddress hwAddr);
  Wf121::ErrorCode cb_EventWifiIsOn(const uint16_t result);
  Wf121::ErrorCode cb_EventScanSortResult(const Wf121::HardwareAddress address,
                                   const int8_t channel,
                                   const int16_t rssi,
                                   const int8_t snr,
                                   const uint8_t secure,
                                   const Wf121::Ssid *ssid,
                                   const Wf121::SsidSize ssidSize);
  Wf121::ErrorCode cb_EventScanned(const int8_t scanStatus);
  Wf121::ErrorCode cb_EventScanSortFinished();
  Wf121::ErrorCode cb_EventConnected(const int8_t status,
                              const Wf121::HardwareInterface hwInterface,
                              const Wf121::Ssid *bssid,
                              const Wf121::SsidSize bssidSize);
  Wf121::ErrorCode cb_EventDisconnected(const uint16_t reason,
                                 const Wf121::HardwareInterface hwInterface);
  Wf121::ErrorCode cb_EventConnectFailed(const uint16_t reason,
                                  const Wf121::HardwareInterface hwInterface);
  Wf121::ErrorCode cb_EventSignalQuality(const int8_t rssi,
                                  const Wf121::HardwareInterface hwInterface);
  Wf121::ErrorCode cb_EventUdpData(const Wf121::Endpoint endpoint,
                            const Wf121::IpAddress srcAddress,
                            const uint16_t srcPort,
                            uint8_t * data,
                            const Wf121::DataSize16 dataSize);
  Wf121::ErrorCode cb_EventTcpIpEndpointStatus(const uint8_t endpoint,
                                        const Wf121::IpAddress localIp,
                                        const uint16_t localPort,
                                        const Wf121::IpAddress remoteIp,
                                        const uint16_t remotePort);
  Wf121::ErrorCode cb_EventConfigureTcpIp(const Wf121::IpAddress address,
                                   const Wf121::Netmask netmask,
                                   const Wf121::Gateway gateway,
                                   const uint8_t useDhcp);

  // Callback command
  Wf121::ErrorCode cb_CommandHelloSystem();
  Wf121::ErrorCode cb_CommandTurnOnWifi(const uint16_t result);
  Wf121::ErrorCode cb_CommandSetPowerSavingState(const uint16_t result);
  Wf121::ErrorCode cb_CommandConfigureTcpIp(const uint16_t result);
  Wf121::ErrorCode cb_CommandStartScanChannels(const uint16_t result);
  Wf121::ErrorCode cb_CommandConnectBssid(const uint16_t result,
                                   const Wf121::HardwareInterface interface,
                                   const Wf121::HardwareAddress address);
  Wf121::ErrorCode cb_CommandSetPassword(const uint8_t status);
  Wf121::ErrorCode cb_CommandGetSignalQuality(const uint16_t result,
                                       const Wf121::HardwareInterface interface); 
  Wf121::ErrorCode cb_CommandDisconnect(const uint16_t result,
                                 const Wf121::HardwareInterface interface);
  Wf121::ErrorCode cb_CommandUdpConnect(const uint16_t result,
                                 const uint8_t endpoint);
  Wf121::ErrorCode cb_CommandUdpBind(const uint16_t result);
  Wf121::ErrorCode cb_CommandStartUdpServer(const uint16_t result,
                                     const uint8_t endpoint);
  Wf121::ErrorCode cb_CommandSendEndpoint(const uint16_t result,
                                   const Wf121::Endpoint endpoint);
  Wf121::ErrorCode cb_CommandSetTransmitSize(const uint16_t result,
                                      const Wf121::Endpoint endpoint);

private:
  Wf121::ErrorCode initializeNetworkManager();
  Wf121::ErrorCode turnOnWifiAdapter();
  Wf121::ErrorCode scanWifiNetwork();
  Wf121::ErrorCode connectToWifiNetwork();
  Wf121::ErrorCode manageSignalStrength();
  Wf121::ErrorCode disconnectFromWifiNetwork();
  Wf121::ErrorCode startUdpServer();
  bool ipAddressesMatch(const Wf121::IpAddress addr1,
                        const Wf121::IpAddress addr2);

  CubeRoverNetworkStateMachine m_state;
  bool m_wifiModuleDetected;
  Wf121::PowerSavingState m_powerSavingState;
  bool m_wifiModuleIdentified;
  bool m_macAddressIdentified;
  Wf121::HardwareAddress m_macAddress;
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

  Wf121::IpAddress m_roverIpAddress = ROVER_IP_ADDRESS;
  Wf121::Netmask m_roverMaskAddress = ROVER_MASK_ADDRESS;
  Wf121::Gateway m_udpGatewayAddress = ROVER_GATEWAY_ADDRESS;
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

}   // End namespace CubeRoverNetworkManager

#endif
