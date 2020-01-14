#include "CubeRoverNetworkManager.hpp"

CubeRoverNetworkManager :: CubeRoverNetworkManager(){
  m_wifiModuleDetected = false;
  m_wifiModuleTurnOnSet = false;
  m_wifiModuleIdentified = false;
  m_powerSavingState = NOT_DEFINED;
  m_macAddressIdentified = false;
  m_powerSavingStateSet = false;
  m_ipConfigurationSet = false;
  m_state = UNINITIALIZED;
  m_scanIndex = 0;

  //Initialize lander wifi network 
  memcpy(m_landerWifi.ssid,
         LANDER_SSID,
         sizeof(LANDER_SSID));

  for(uint8_t i=0; i<MAX_NUMBER_CHANNEL_PER_NETWORK; i++){
  	m_landerWifi.channel[i].active = false;
  }
}

ErrorCode CubeRoverNetworkManager :: UpdateNetworkManager(){
  ErrorCode errorCode = NO_ERROR;

  if(m_state != UNINITIALIZED){
    errorCode = ExecuteCallbacks();
  }

  // Return on error or if "try again" is returned
  if(errorCode != NO_ERROR && errorCode != TRY_AGAIN) return errorCode;

  switch(m_state){
    case UNINITIALIZED:
      errorCode = initializeNetworkManager();     // UNINITIIALIZED --> INITIALIZED
      if(errorCode != NO_ERROR) return errorCode;
      break;
    case INITIALIZED:
      errorCode = turnOnWifiAdapter();            // INITIALIZED --> WIFI ON
      if(errorCode != NO_ERROR) return errorCode;
      break;
    case WIFI_ON:
      errorCode = scanWifiNetwork();              // WIFI ON --> SCANNING
      if(errorCode != NO_ERROR) return errorCode;
      break;
    case SCANNING:
      break;
    case SCANNED:
      //errorCode = connectToWifiNetwork();
      break;
    default:
      break;
  }

  return errorCode;
}

CubeRoverNetworkStateMachine CubeRoverNetworkManager :: GetState(){
  return m_state;
}

ErrorCode CubeRoverNetworkManager :: initializeNetworkManager(){
  uint16_t tries;
  ErrorCode errorCode;

  // Check if the hardware can be detected
  errorCode = HelloSystem();
  if(errorCode != NO_ERROR) return errorCode;

  // Block until it timeouts
  tries = TRIES_EXECUTE_CALLBACK;
  while(tries > 0 && m_wifiModuleDetected == false){
    errorCode = ExecuteCallbacks();
    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
    tries--;
  }

  // Failed to execute the command because likely module is not present
  if(!tries) return HARDWARE_FAILURE;

  // Reset system
  errorCode = ResetSystemWifi(SYSTEM_BOOT);
  if(errorCode != NO_ERROR) return errorCode;

  tries = TRIES_EXECUTE_CALLBACK;
  while(tries > 0 && m_wifiModuleIdentified == false){
    errorCode = ExecuteCallbacks();
    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
    tries--;
  }

  // Get the mac address
  errorCode = GetMacAddress(WIFI);
  if(errorCode != NO_ERROR) return errorCode;

  tries = TRIES_EXECUTE_CALLBACK;
  while(tries > 0 && m_macAddressIdentified == false){
    errorCode = ExecuteCallbacks();
    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
    tries--;
  }

  // Set the power saving state
  errorCode = SetPowerSavingState(LOW_LATENCY);
  if(errorCode != NO_ERROR) return errorCode;

  // Block until it timeouts or generate an error
  tries = TRIES_EXECUTE_CALLBACK;
  while(tries > 0 && m_wifiModuleTurnOnSet == false){
    errorCode = ExecuteCallbacks();
    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
    tries--;
  }

  // Configure the TCP/IP address
  errorCode = ConfigureTcpIp(m_roverIpAddress,
                             m_roverMaskAddress,
                             m_udpGatewayAddress,
                             false /* do not use dhcp */);

  // Block until it timeouts or generate an error
  tries = TRIES_EXECUTE_CALLBACK;
  while(tries > 0 && m_ipConfigurationSet == false){
    errorCode = ExecuteCallbacks();
    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
    tries--;
  }

  // Failed to execute the command
  if(!tries) return TIMEOUT;

  m_state = INITIALIZED;

  return NO_ERROR;
}

ErrorCode CubeRoverNetworkManager :: turnOnWifiAdapter(){
    uint16_t tries;
    ErrorCode errorCode;

    // Turn on wi-fi command already successfully sent
    if(m_wifiModuleTurnOnSet == true || m_state == WIFI_ON)
      return NO_ERROR;

    // Turn on the wifi module
    errorCode = TurnOnWifi();
    if(errorCode != NO_ERROR) return errorCode;

    // Block until it timeouts or generate an error
    tries = TRIES_EXECUTE_CALLBACK;
    while(tries > 0 && m_wifiModuleTurnOnSet == false){
      errorCode = ExecuteCallbacks();
      if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
      tries--;
    }

    // Failed to execute the command
    if(!tries) return TIMEOUT;

    return NO_ERROR;
}

ErrorCode CubeRoverNetworkManager :: scanWifiNetwork(){
    ErrorCode errorCode;
    uint16_t tries;
    ChannelList list[] = "";

    // Command scan wifi network already sent
    if(m_state == SCANNING) return NO_ERROR;

    // reset the scan index
    m_scanIndex = 0;

    // TODO clean channels before scanning
    errorCode = StartScanChannels(WIFI,
                                  list,
                                  0);

    // Block until it timeouts or generate an error
    tries = TRIES_EXECUTE_CALLBACK;
    while(tries > 0 && m_state != SCANNING){
      errorCode = ExecuteCallbacks();
      if(errorCode != TRY_AGAIN && errorCode != NO_ERROR)
        return errorCode;
      tries--;
    }

    return errorCode;
}

//--------------------------------------------------------------------------------------
// Implementation of virtual COMMAND callbacks from base class
//--------------------------------------------------------------------------------------
ErrorCode CubeRoverNetworkManager :: cb_CommandHelloSystem(){
  m_wifiModuleDetected = true;
  return NO_ERROR;
}

ErrorCode CubeRoverNetworkManager :: cb_CommandTurnOnWifi(const uint16_t result){
  if(result == NO_ERROR) m_wifiModuleTurnOnSet = true;
  return (ErrorCode) result;
}

ErrorCode CubeRoverNetworkManager :: cb_CommandSetPowerSavingState(const uint16_t result){
  if(result == NO_ERROR) m_powerSavingStateSet = true;
  return (ErrorCode) result;
}

ErrorCode CubeRoverNetworkManager :: cb_CommandConfigureTcpIp(const uint16_t result){
  if(result == NO_ERROR) m_ipConfigurationSet = true;
  return (ErrorCode) result;
}

ErrorCode CubeRoverNetworkManager :: cb_CommandStartScanChannels(const uint16_t result){
  if(result == NO_ERROR){
    m_state = SCANNING;
  }

  return (ErrorCode) result;
}


//--------------------------------------------------------------------------------------
// Implementation of virtual EVENT callbacks from base class
//--------------------------------------------------------------------------------------

ErrorCode CubeRoverNetworkManager :: cb_EventPowerSavingState(const PowerSavingState state){
  m_powerSavingState = state;
  return NO_ERROR;
}

ErrorCode CubeRoverNetworkManager :: cb_EventMacAddress(const HardwareInterface interface,
                                                        const HardwareAddress hwAddr){
  memcpy(m_macAddress,
         hwAddr,
         sizeof(m_macAddress));
  m_macAddressIdentified = true;
  return NO_ERROR;
}

ErrorCode CubeRoverNetworkManager :: cb_EventEndpointSyntaxError(const uint16_t result, const Endpoint endpoint){
  if(result != NO_ERROR){
      return (ErrorCode)result;
  }

  return NO_ERROR;
}

ErrorCode CubeRoverNetworkManager :: cb_EventBoot(const uint16_t major,
                                                   const uint16_t minor,
                                                   const uint16_t patch,
                                                   const uint16_t build,
                                                   const uint16_t bootloaderVersion,
                                                   const uint16_t tcpIpVersion,
                                                   const uint16_t hwVersion){
  // save versions if needed - but so far it seems no needed for final design
  m_wifiModuleIdentified = true;
  return NO_ERROR;
}

ErrorCode CubeRoverNetworkManager :: cb_EventWifiIsOn(const uint16_t result){
  m_state = WIFI_ON;
  return (ErrorCode) result;
}

ErrorCode CubeRoverNetworkManager :: cb_EventScanned(const int8_t scanStatus){
  ErrorCode errorCode = NO_ERROR;

  if(scanStatus == NO_ERROR){
    errorCode = ScanResultsSortRssi(MAX_SORTING_LIST_SIZE);
  }
  else{
    m_state = WIFI_ON;              // SCANNING --> WIFI ON
  }

  return errorCode;
}



ErrorCode CubeRoverNetworkManager :: cb_EventScanSortResult(const HardwareAddress hwAddr,
                                                            const int8_t channel,
                                                            const int16_t rssi,
                                                            const int8_t snr,
                                                            const uint8_t secure,
                                                            const Ssid *ssid,
                                                            const SsidSize ssidSize){

  if(!strncmp((char *)m_landerWifi.ssid,(char *) ssid,(ssidSize < sizeof(m_landerWifi.ssid)) ? ssidSize : sizeof(m_landerWifi.ssid))){
    m_landerWifi.channel[m_scanIndex].rssi = rssi;
    m_landerWifi.channel[m_scanIndex].snr = snr;
    m_landerWifi.channel[m_scanIndex].channel = channel;
    memcpy(m_landerWifi.channel[m_scanIndex].bssid, hwAddr, sizeof(HardwareAddress));
    m_landerWifi.channel[m_scanIndex].active = true;
    m_scanIndex++;
  }

  return NO_ERROR;
}

ErrorCode CubeRoverNetworkManager :: cb_EventScanSortFinished(){
    m_state = SCANNED;
    return NO_ERROR;
}




