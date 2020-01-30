#include "CubeRoverNetworkManager.hpp"


/**
 * @brief      Constructs a new instance.
 */
CubeRoverNetworkManager :: CubeRoverNetworkManager(){
  m_powerSavingState = PowerSavingState::NOT_DEFINED;
  m_wifiModuleDetected = false;
  m_wifiModuleIdentified = false;
  m_macAddressIdentified = false;
  m_powerSavingStateSet = false;
  m_ipConfigurationSet = false;
  m_commandSignalQualitySet = false;
  m_connectBssidSet = false;
  m_passwordSet = false;
  m_state = UNINITIALIZED;
  m_scanIndex = 0;
  m_connectIndex = 0;
  m_nbChannelFound = 0;
  m_preScaleCheckSignalCtr = 0;
  m_userCbVeryGoodSignal = NULL;
  m_userCbOkaySignal = NULL;
  m_userCbNotGoodSignal = NULL;
  m_userCbUnusableSignal = NULL;
  m_signalLevel = CubeRoverSignalLevels::NOT_DEFINED;
  m_udpConnectSet = false;
  m_udpServerStarted = false;
  m_logNbOfBytesReceived = 0;
  m_logNbOfBytesSent = 0;
  m_rxUdpFifoBytesCount = 0;
  m_txUdpFifoBytesCount = 0;
  m_rxUdpFifoTailPointer = 0;
  m_rxUdpFifoHeadPointer = 0;
  m_udpSendEndpoint = 0;
  m_udpBindSet = false;
  m_commandSendEndpointSet = false;
  m_commandTransmitSizeSet = false;

  //Initialize lander wifi network 
  memcpy(m_landerWifi.ssid,
         LANDER_SSID,
         sizeof(LANDER_SSID));

  for(uint8_t i=0; i<MAX_NUMBER_CHANNEL_PER_NETWORK; i++){
    m_landerWifi.channel[i].active = false;
  }
}

/**
 * @brief      Update the network.
 *
 * @return     The error code.
 */
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
    case WIFI_TURNING_ON:
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
      errorCode = connectToWifiNetwork();        // SCANNED --> CONNECTED
      break;
    case CONNECTING:
      break;
    case CONNECTED:
    case UDP_CONNECTED:
      errorCode = startUdpServer();
      errorCode = manageSignalStrength();
      break;
    case DISCONNECTING:
      break;
    default:
      break;
  }

  return errorCode;
}


/**
 * @brief      Gets the state.
 *
 * @return     The state.
 */
CubeRoverNetworkStateMachine CubeRoverNetworkManager :: GetState(){
  return m_state;
}


/**
 * @brief      Gets the signal RSSI.
 *
 * @return     The signal quality, the actual signal strengh is connected, 0xff
 *             otherwise.
 */
int8_t CubeRoverNetworkManager :: GetSignalRssi(){
  if(m_state != CONNECTED)
    return 0xFF;

  return m_landerWifi.channel[m_scanIndex].rssi;
}


/**
 * @brief      Gets the signal level.
 *
 * @return     The signal level.
 */
CubeRoverSignalLevels CubeRoverNetworkManager :: GetSignalLevel(){
    return m_signalLevel;
}


/**
 * @brief      Connects a callback.
 *
 * @param[in]  signal  The signal
 * @param[in]  cb      Pointer to the function to connect
 */
void CubeRoverNetworkManager :: ConnectCallback(const CubeRoverSignalLevels signal,
                                                NetworkManagerUserCbFunctionPtr cb){
  switch(signal){
    case VERY_GOOD:
      m_userCbVeryGoodSignal = cb;
      break;
    case OKAY:
      m_userCbOkaySignal = cb;
      break;
    case NOT_GOOD:
      m_userCbNotGoodSignal = cb;
      break;
    case UNUSABLE:
      m_userCbUnusableSignal = cb;
      break;
  }
}


/**
 * @brief      Sends the UDP data.
 *
 * @param      data     The data
 * @param[in]  size     The size
 * @param[in]  timeout  The timeout
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: SendUdpData(uint8_t * data,
                                                 const uint32_t size,
                                                 const uint32_t timeout){
  ErrorCode errorCode;
  int32_t byteToSend = size;
  uint32_t timer = timeout;

  if(!size) return NO_ERROR;  // nothing to do

  // Check we are in the right connection state
  if(m_state != UDP_CONNECTED){
    return ETHERNET_NOT_CONNECTED;
  }

  m_commandTransmitSizeSet = false;

  errorCode = SetTransmitSize(m_udpSendEndpoint,
                              byteToSend);

  while((timer >0) && (m_commandTransmitSizeSet == false)){
    errorCode = ExecuteCallbacks();
    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR){
      return errorCode;
    }
    timer--;
  }

  timer = timeout;

  // loop until all data is sent
  while(byteToSend > 0){
    // max number of bytes to send at one time is 255 by hardware restriction
    uint8_t byteToSendThisLoop = (byteToSend > 0xff) ? 0xff : byteToSend;
    m_commandSendEndpointSet = false;

    errorCode = SendEndpoint(m_udpSendEndpoint,
                             data,
                             byteToSendThisLoop); 

    while((timer > 0) && (m_commandSendEndpointSet == false)){
      errorCode = ExecuteCallbacks();
      if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
      timer--;
    }

    if(!timer) return TIMEOUT;

    timer = timeout;

    // wait until all data is sent
    if(errorCode == NO_ERROR){
      m_logNbOfBytesSent += byteToSendThisLoop;
      data += byteToSendThisLoop;
      byteToSend -= byteToSendThisLoop;
    }
    else{
      break; // exit loop on error
    }
  }
  
  return errorCode;
}

/**
 * @brief      Receives the UDP data
 *
 * @param      data      The data
 * @param[in]  dataSize  The data size, once read, the variable report the
 *                       number of bytes actually read
 * @param[in]  mode      The mode can be a combination of WAIT_UNTIL_READY,
 *                       NORMAL_READ, PEEK_READ flags
 * @param[in]  timeout   The timeout
 * @param[in]  size  The size
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: ReceiveUdpData(uint8_t * data,
                                                    const uint16_t dataSize,
                                                    uint16_t * dataRead,
                                                    const uint8_t mode,
                                                    const uint32_t timeout){
  ErrorCode errorCode;
  uint32_t timer = timeout;
  uint8_t *ptrData = data;
  uint32_t byteToRead = dataSize;

  if(m_state != UDP_CONNECTED){
    return ETHERNET_NOT_CONNECTED;
  }

  // Loop here until
  if(mode & WAIT_UNTIL_READY){
    while(timer-- && m_rxUdpFifoBytesCount < byteToRead){
      errorCode = ExecuteCallbacks();
      if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
    }

    // timeout! return immediately
    if(!timer || m_rxUdpFifoBytesCount < byteToRead){
        *dataRead = 0;
        return TIMEOUT;
    }
  }
  else{ // don't wait
    errorCode = ExecuteCallbacks();

    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;

    if(m_rxUdpFifoBytesCount < byteToRead){
      return NO_ERROR;
    }
  }

  // dataSize is updated to report the number of byte actually read
  *dataRead = 0;

  // Implementation of a simple ring buffer
  for(uint16_t i=0; i < byteToRead; i++){
    if(m_rxUdpFifoHeadPointer == m_rxUdpFifoTailPointer){
      return TCP_IP_BUFFER_ERROR;
    }
   
    *dataRead = *dataRead + 1; // increment the number of byte read

    // Normal read consumes bytes from the ring buffer
    if(mode & NORMAL_READ){
      m_rxUdpFifoBytesCount--;
      *ptrData = g_rxRingBuffer[m_rxUdpFifoTailPointer];
      m_rxUdpFifoTailPointer = (m_rxUdpFifoTailPointer + 1) % RX_RING_BUFFER_SIZE;
    }

    // Peek read doesn't consume bytes from the ring buffer
    if(mode & PEEK_READ){
      *ptrData = g_rxRingBuffer[(m_rxUdpFifoTailPointer + i) % RX_RING_BUFFER_SIZE];
    }
    ptrData++;
  }

  return NO_ERROR;
}

/**
 * @brief      This function manages the signal strengh of the cuberover and
 *             change state as needed to maintain signal reception
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: manageSignalStrength(){
  ErrorCode errorCode;
  uint16_t tries;

  // prescale execution of signal check to accomodate how often it needs to be
  // checked relatively to UpdateStateMachine calls.
  if(m_preScaleCheckSignalCtr < PRESCALE_CHECK_SIGNAL){
    m_preScaleCheckSignalCtr++;
    return NO_ERROR;
  }

  // reset prescaler counter
  m_preScaleCheckSignalCtr = 0;

  // Manage the signal strength while being connected to either try
  // to find a new channel or re-scan network
  if(m_state == CONNECTED){
    
    // reset flag that keep track if command is already sent
    m_commandSignalQualitySet = false;

    errorCode = GetSignalQuality();
    if(errorCode != NO_ERROR) return errorCode;

    // Block until it timeouts
    tries = TRIES_EXECUTE_CALLBACK;
    while(tries > 0 &&  m_commandSignalQualitySet == false){
      errorCode = ExecuteCallbacks();
      if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
      tries--;
    }

    if(!tries) return TIMEOUT;

    if(errorCode != NO_ERROR) return errorCode;

    // Update signal level to UNUSABLE and execute user application callback if
    // signal level has changed
    if(m_landerWifi.channel[m_scanIndex].rssi <= UNUSABLE){
      if(m_signalLevel != UNUSABLE){
        if(m_userCbUnusableSignal){
          m_userCbUnusableSignal();
        }
      }

      m_signalLevel = UNUSABLE;

      // disconnect on its own to try to find better connection
      errorCode = disconnectFromWifiNetwork();
    }

    // Update signal level to UNUSABLE and execute user application callback if
    // signal level has changed
    if(m_landerWifi.channel[m_scanIndex].rssi <= NOT_GOOD){
      if(m_signalLevel != NOT_GOOD){
        if(m_userCbNotGoodSignal){
          m_userCbNotGoodSignal();    // user application callback
        }
      }

      m_signalLevel = NOT_GOOD;

      // disconnect on its own to try to find better connection
      errorCode = disconnectFromWifiNetwork();
    }

    // Update signal level to UNUSABLE and execute user application callback if
    // signal level has changed
    if(m_landerWifi.channel[m_scanIndex].rssi <= OKAY){
      if(m_signalLevel != OKAY){
        if(m_userCbOkaySignal){
          m_userCbOkaySignal();     // user application callback
        }
      }

      m_signalLevel = OKAY;
    }

    // Update signal level to UNUSABLE and execute user application callback if
    // signal level has changed
    if(m_landerWifi.channel[m_scanIndex].rssi <= VERY_GOOD){
      if(m_signalLevel != VERY_GOOD){
        if(m_userCbVeryGoodSignal){
          m_userCbVeryGoodSignal(); // user application callback
        }
      }      
      
      m_signalLevel = VERY_GOOD;
    }
  }

  return errorCode;
}


/**
 * @brief      Initializes the network manager.
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: initializeNetworkManager(){
  uint16_t tries;
  ErrorCode errorCode;
  Password password[sizeof(LANDER_NETWORK_PASSWORD)];

  memcpy(password,
         LANDER_NETWORK_PASSWORD,
         sizeof(LANDER_NETWORK_PASSWORD));

  // reset flags that monitors progress of initialization
  m_wifiModuleDetected = false;
  m_wifiModuleIdentified = false;
  m_macAddressIdentified = false;
  m_ipConfigurationSet = false;
  m_powerSavingStateSet = false;
  m_passwordSet = false;

  errorCode = Init();
  if(errorCode != NO_ERROR) return errorCode;

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

  // Failed to execute the command
  if(!tries) return TIMEOUT;

  // Get the mac address
  errorCode = GetMacAddress(WIFI);
  if(errorCode != NO_ERROR) return errorCode;

  tries = TRIES_EXECUTE_CALLBACK;
  while(tries > 0 && m_macAddressIdentified == false){
    errorCode = ExecuteCallbacks();
    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
    tries--;
  }

  // Failed to execute the command
  if(!tries) return TIMEOUT;

  // Set the power saving state
  errorCode = SetPowerSavingState(PowerSavingState::LOW_LATENCY);
  if(errorCode != NO_ERROR) return errorCode;

  // Block until it timeouts or generate an error
  tries = TRIES_EXECUTE_CALLBACK;
  while(tries > 0 && m_powerSavingStateSet == false){
    errorCode = ExecuteCallbacks();
    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
    tries--;
  }

  // Failed to execute the command
  if(!tries) return TIMEOUT;

  m_powerSavingState = PowerSavingState::LOW_LATENCY;

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

  // Set the network password
  errorCode = SetPassword(password,
                          sizeof(password)-1 /* remove end character */);

   // Block until it timeouts or generate an error
  tries = TRIES_EXECUTE_CALLBACK;
  while(tries > 0 && m_passwordSet == false){
    errorCode = ExecuteCallbacks();
    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR){
        return errorCode;
    }
    tries--;
  } 

  // Failed to execute the command
  if(!tries) return TIMEOUT;

  m_state = INITIALIZED;

  return NO_ERROR;
}


/**
 * @brief      Turn on the wifi adapter.
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: turnOnWifiAdapter(){
  uint16_t tries;
  ErrorCode errorCode;

  // Turn on wi-fi command already successfully sent
  if(m_state == WIFI_TURNING_ON || m_state == WIFI_ON)
    return NO_ERROR;

  // Turn on the wifi module
  errorCode = TurnOnWifi();
  if(errorCode != NO_ERROR) return errorCode;

  // Block until it timeouts or generate an error
  tries = TRIES_EXECUTE_CALLBACK;
  while(tries > 0 && m_state != WIFI_TURNING_ON){
    errorCode = ExecuteCallbacks();
    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
    tries--;
  }

  // Failed to execute the command
  if(!tries) return TIMEOUT;

  return NO_ERROR;
}


/**
 * @brief      Scans a wifi network.
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: scanWifiNetwork(){
  ErrorCode errorCode;
  ChannelList list[] = "";

  // Command scan wifi network already sent
  if(m_state == SCANNING) return NO_ERROR;

  // reset the scan index and number of channels found
  m_scanIndex = 0;
  m_nbChannelFound = 0;
  m_connectIndex = 0;

  // Reset active flag of all channels to keep track of which ones
  // are still relevant
  for(uint8_t i=0; i< MAX_NUMBER_CHANNEL_PER_NETWORK; i++){
    m_landerWifi.channel[i].active = false;
  }

  errorCode = StartScanChannels(WIFI,
                                list,
                                0);

  if(errorCode != NO_ERROR){
      return errorCode;
  }

  // Block until it timeouts or generate an error
  while(m_state != SCANNING){
    errorCode = ExecuteCallbacks();
    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR)
      return errorCode;
  }

  return errorCode;
}


/**
 * @brief      Connects to wifi network.
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: connectToWifiNetwork(){
  ErrorCode errorCode;
  uint16_t tries;

  // If the current is connecting, don't try again
  if(m_state == CONNECTING || m_state == CONNECTED) return NO_ERROR;

  // If no channel have been found just return
  if(m_nbChannelFound == 0){
      m_state = WIFI_ON;            // SCANNED --> WIFI_ON
      return WIFI_NOT_FOUND;
  }

  // Attempt to connect to the best network, after sorting channels,
  // the best option is in the first array
  // Check that there is an active channel scanned
  m_connectBssidSet = false;

  if(m_landerWifi.channel[m_connectIndex].active){
    errorCode = ConnectBssid(m_landerWifi.channel[m_connectIndex].bssid);
  }
  else{
    return INVALID_PARAMETER; // should not happen
  }

  if(errorCode != NO_ERROR) return errorCode;

  m_state = CONNECTING;             // SCANNDED --> CONNECTING

  // Block until it timeouts or generate an error
  tries = TRIES_EXECUTE_CALLBACK;
  while(tries > 0 && m_connectBssidSet == false){
    errorCode = ExecuteCallbacks();
    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR){
        m_state = SCANNED;         // CONNECTING --> SCANNED
        return errorCode;
    }
    tries--;
  }

  // Failed to execute the command
  if(!tries){
      m_state = SCANNED;         // CONNECTING --> SCANNED
      return TIMEOUT;
  }

  // Block until it timeouts or generate an error
  tries = TRIES_EXECUTE_CALLBACK;
  while(tries > 0 && m_state != CONNECTED){
      tries--;
      errorCode = ExecuteCallbacks();
  }

  // Failed to execute the command
  if(!tries){
      m_state = SCANNED;         // CONNECTING --> SCANNED
      return TIMEOUT;
  }

  return errorCode;
}

/**
 * @brief      Starts an udp server.
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: startUdpServer(){
  ErrorCode errorCode;
  uint16_t tries;

  if(m_state == UDP_CONNECTED) return NO_ERROR;

  m_udpConnectSet = false;
  m_udpBindSet = false;
  m_udpServerStarted = false;

  // Connect to UDP to support outcoming data
  errorCode = UdpConnect(&m_udpGatewayAddress,
                         GATEWAY_PORT,
                         -1);

  if(errorCode != NO_ERROR){
    return errorCode;
  }

  // Block until it timeouts or generate an error
  tries = TRIES_EXECUTE_CALLBACK;
  while(tries > 0 && m_udpConnectSet == false){
    errorCode = ExecuteCallbacks();
    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR){
      return errorCode;
    }
    tries--;
  }

  // Failed to execute the command
  if(!tries){
    return TIMEOUT;
  }
  
  errorCode = UdpBind(m_udpSendEndpoint,
                      ROVER_UDP_PORT);

  if(errorCode != NO_ERROR){
    return errorCode;
  }

  // Block until it timeouts or generate an error
  tries = TRIES_EXECUTE_CALLBACK;
  while(tries > 0 && m_udpBindSet == false){
    errorCode = ExecuteCallbacks();
    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR){
      return errorCode;
    }
    tries--;
  }

  // Failed to execute the command
  if(!tries){
    return TIMEOUT;
  }

  // Create a UDP server to support incoming data
  errorCode = StartUdpServer(ROVER_UDP_PORT,
                             -1);

  if(errorCode != NO_ERROR){
    return errorCode;
  }

  // Block until it timeouts or generate an error
  tries = TRIES_EXECUTE_CALLBACK;
  while(tries > 0 && m_udpServerStarted == false){
    errorCode = ExecuteCallbacks();
    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR){
      return errorCode;
    }
    tries--;
  } 

  if(!tries) return TIMEOUT;

  m_state = UDP_CONNECTED;

  return errorCode;
}


/**
 * @brief      Disconnect from the current network
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: disconnectFromWifiNetwork(){
  ErrorCode errorCode;
  uint16_t tries;

  // Don't do anything if not connected
  if(m_state != CONNECTED) return NO_ERROR;

  errorCode = Disconnect();

  if(errorCode != NO_ERROR) return errorCode;

  // Block until it timeouts or generate an error
  tries = TRIES_EXECUTE_CALLBACK;
  while(tries > 0 && m_state != DISCONNECTING){
    errorCode = ExecuteCallbacks();
    if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) 
      return errorCode;
    tries--;
  }

  // DISCONNECTING --> WIFI_ON
  m_state = WIFI_ON;

  return NO_ERROR;
}


/**
 * @brief      This helper verifies that the IP addresses passed are matching
 *
 * @param[in]  addr1  The address 1
 * @param[in]  addr2  The address 2
 *
 * @return     { description_of_the_return_value }
 */
bool CubeRoverNetworkManager :: ipAddressesMatch(const IpAddress addr1, const IpAddress addr2){
  for(uint8_t i=0; i< sizeof(IpAddress); i++){
    if(*addr1 != *addr2)  return false;
    addr1++;
    addr2++;
  }

  return true;
}

//--------------------------------------------------------------------------------------
// Implementation of virtual COMMAND callbacks from base class
//--------------------------------------------------------------------------------------


/**
 * @brief      This is the acknowledgment that the command hello system has
 *             succeeded
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_CommandHelloSystem(){
  m_wifiModuleDetected = true;
  return NO_ERROR;
}


/**
 * @brief      This is the acknowledgment that the turn on wifi command has
 *             suceeded
 *
 * @param[in]  result  The result
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_CommandTurnOnWifi(const uint16_t result){
  if(result == NO_ERROR) m_state = WIFI_TURNING_ON;
  return (ErrorCode) result;
}


/**
 * @brief      This is the acknowledgment that the power saving state succeeded.
 *
 * @param[in]  result  The result
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_CommandSetPowerSavingState(const uint16_t result){
  if(result == NO_ERROR) m_powerSavingStateSet = true;
  return (ErrorCode) result;
}


/**
 * @brief      This is the acknoledgement that the command to configure TCP/IP
 *             has succeeded
 *
 * @param[in]  result  The result
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_CommandConfigureTcpIp(const uint16_t result){
  return (ErrorCode) result;
}


/**
 * @brief      This event is reported when the start scan channels command is
 *             sent.
 *
 * @param[in]  result  The result
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_CommandStartScanChannels(const uint16_t result){
  if(result == NO_ERROR){
    m_state = SCANNING;
  }

  return (ErrorCode) result;
}


/**
 * @brief      This event is reported when the command connect is sent
 *
 * @param[in]  result  The result
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_CommandConnectBssid(const uint16_t result,
                                                            const HardwareInterface interface,
                                                            const HardwareAddress address){
  if(result == NO_ERROR){
    m_connectBssidSet = true;
  }

  return (ErrorCode) result;
}


/**
 * @brief      This event is reported after the command Set Password is sent to
 *             wi-fi module.
 *
 * @param[in]  status  The status
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_CommandSetPassword(const uint8_t status){
  if(status == 0){
    m_passwordSet = true;
    return NO_ERROR;
  }
  else{
    return INVALID_PASSWORD;
  }
}


/**
 * @brief      This event is reported after the command Get Signal Quality is sent.
 *
 * @param[in]  result     The result
 * @param[in]  interface  The interface
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_CommandGetSignalQuality(const uint16_t result,
                                                                const HardwareInterface interface){
  m_commandSignalQualitySet = true;
  return (ErrorCode) result;
}


/**
 * @brief      This event is reported in case UDP command is sent.
 *
 * @param[in]  result    The result
 * @param[in]  endpoint  The endpoint
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_CommandUdpConnect(const uint16_t result,
                                                          const uint8_t endpoint){
  return (ErrorCode) result;
}

/**
 * @brief      This event is reported after the command disconnect is sent.
 *
 * @param[in]  result     The result
 * @param[in]  interface  The interface
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_CommandDisconnect(const uint16_t result,
                                                          const HardwareInterface interface){
  if(result == NO_ERROR && m_state == CONNECTED){
    m_state = DISCONNECTING;
  }

  return (ErrorCode) result;
}

/**
 * @brief      This event is reported after the UDP command is sent. 
 *
 * @param[in]  result  The result
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_CommandUdpBind(const uint16_t result){
  if(result == NO_ERROR){
    m_udpBindSet = true;
  }
  return (ErrorCode) result;
}

/**
 * @brief      This event is reported after the UDP start command is sent.
 *
 * @param[in]  result    The result
 * @param[in]  endpoint  The endpoint
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_CommandStartUdpServer(const uint16_t result,
                                                              const uint8_t endpoint){
  if(result == NO_ERROR){
    m_udpServerStarted = true;
  }
  return (ErrorCode) result;
}


/**
 * @brief      This event is reported when some data has been to an endpoint
 *
 * @param[in]  result    The result
 * @param[in]  endpoint  The endpoint
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_CommandSendEndpoint(const uint16_t result,
                                                            const Endpoint endpoint){
  if(result == NO_ERROR){
    m_commandSendEndpointSet = true;
  }

  return (ErrorCode)result;
}


/**
 * @brief      This event is reported when set transmit size command is sent
 *
 * @param[in]  result    The result
 * @param[in]  endpoint  The endpoint
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_CommandSetTransmitSize(const uint16_t result,
                                                               const Endpoint endpoint){
  if(result == NO_ERROR){
      m_commandTransmitSizeSet = true;
  }

  return (ErrorCode)result;
}

//--------------------------------------------------------------------------------------
// Implementation of virtual EVENT callbacks from base class
//--------------------------------------------------------------------------------------

/**
 * @brief      This event is reported when the power saving state 
 *
 * @param[in]  state  The state
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_EventPowerSavingState(const PowerSavingState state){
  m_powerSavingState = state;
  return NO_ERROR;
}


/**
 * @brief      This event is reported when the local MAC address is acquired
 *
 * @param[in]  interface  The interface
 * @param[in]  hwAddr     The hardware address
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_EventMacAddress(const HardwareInterface interface,
                                                        const HardwareAddress hwAddr){
  memcpy(m_macAddress,
         hwAddr,
         sizeof(m_macAddress));
  m_macAddressIdentified = true;
  return NO_ERROR;
}

/**
 * @brief      This event is returned when an boot event is reported
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

/**
 * @brief      This function is executed when the wifi is on. The state machine
 *             may transit to WIFI_ON if operation has succeeded.
 *
 * @param[in]  result  The result
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_EventWifiIsOn(const uint16_t result){
  if(result == NO_ERROR){
    // Successfully turned on the wifi module, go to WIFI_ON state
    m_state = WIFI_ON;
  }
  else{
    // Failed to turn on wifi module, go back to INITIALIZED state
    m_state = INITIALIZED;
  }
  return (ErrorCode) result;
}


/**
 * @brief      This function is executed when network scanning is scanned. The
 *             state machine may return to WIFI_ON state if an error occured
 *             during scanning.
 *
 * @param[in]  scanStatus  The scan status
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_EventScanned(const int8_t scanStatus){
  ErrorCode errorCode = NO_ERROR;

  if(scanStatus == NO_ERROR){
    errorCode = ScanResultsSortRssi(MAX_SORTING_LIST_SIZE);
    // will set to SCANNED state only when results are sorted
  }
  else{
    m_state = WIFI_ON;              // Go back to SCANNING --> WIFI_ON
  }

  return errorCode;
}


/**
 * @brief      Event generated for each result being pushed out by the Wi-fi
 *             module. The channel are sorted by strongest reception first to
 *             lowest. The compatible channels are stored in the m_landerWifi
 *             variable.
 *
 * @param[in]  hwAddr    The hardware address
 * @param[in]  channel   The channel
 * @param[in]  rssi      The rssi
 * @param[in]  snr       The snr
 * @param[in]  secure    The secure
 * @param[in]  ssid      The ssid
 * @param[in]  ssidSize  The ssid size
 *
 * @return     The error code.
 */
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
    m_nbChannelFound = m_scanIndex; 
  }

  return NO_ERROR;
}


/**
 * @brief      Event generated when the scan sorting is completed.
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_EventScanSortFinished(){
  m_state = SCANNED; // SCANNING --> SCANNED
  m_scanIndex = 0;  // reset scan index to 0. At that point we know that best reception is at index 0
  return NO_ERROR;
}


/**
 * @brief      This event is generated upon successful connection
 *
 * @param[in]  status       The status
 * @param[in]  hwInterface  The hardware interface
 * @param[in]  bssid        The bssid
 * @param[in]  bssidSize    The bssid size
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_EventConnected(const int8_t status,
                                                        const HardwareInterface hwInterface,
                                                        const Ssid *bssid,
                                                        const SsidSize bssidSize){ 
  m_state = CONNECTED;
  return NO_ERROR;
}

/**
 * @brief      This event is generated when the network is disconnected
 *
 * @param[in]  reason       The reason
 * @param[in]  hwInterface  The hardware interface
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_EventDisconnected(const uint16_t reason,
                                                          const HardwareInterface hwInterface){
  // If we got disconnected for some reason, try to connect again to the same
  // channel
  m_state = SCANNED;        // DISCONNECTING --> SCANNED
  return (ErrorCode) reason;
}

/**
 * @brief      This event is generated when the connection failed
 *
 * @param[in]  reason       The reason
 * @param[in]  hwInterface  The hardware interface
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_EventConnectFailed(const uint16_t reason,
                                                           const HardwareInterface hwInterface){
  // Go back to SCANNED state regardless of the connection status
  m_state = SCANNED;        // CONNECTING --> SCANNED
  m_connectIndex++;         // Failed to connect to a given channel try next one 

  // If we attempt to connect to all channel, re-do a scan of the network
  if(m_connectIndex == m_nbChannelFound) {
    m_state = WIFI_ON;
  }
  return (ErrorCode) reason;
}


/**
 * @brief      Update the signal quality
 *
 * @param[in]  rssi         The rssi
 * @param[in]  hwInterface  The hardware interface
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_EventSignalQuality(const int8_t rssi,
                                                           const HardwareInterface hwInterface){
  if(m_state == CONNECTED){
    m_landerWifi.channel[m_scanIndex].rssi = rssi;
  }

  return NO_ERROR;
}

/**
 * @brief      The module received some data in the UDP buffer.
 *
 * @param[in]  endpoint    The endpoint
 * @param[in]  srcAddress  The source address
 * @param[in]  srcPort     The source port
 * @param[in]  data        The data
 * @param[in]  dataSize    The data size
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_EventUdpData(const Endpoint endpoint,
                                                     const IpAddress srcAddress,
                                                     const uint16_t srcPort,
                                                     uint8_t * data,
                                                     const DataSize16 dataSize){
  uint8_t *ptrData = data;

  if(ipAddressesMatch(srcAddress, m_udpGatewayAddress)){

    // Log the number of bytes received
    m_logNbOfBytesReceived += dataSize;

    // Implementation of a simple ring buffer
    for(uint16_t i=0; i<dataSize; i++){
      // Check if we can write new data to the ring buffer by looking
      // where the read pointer is located
      if((m_rxUdpFifoHeadPointer + 1 % RX_RING_BUFFER_SIZE) == m_rxUdpFifoTailPointer){
        return TCP_IP_BUFFER_ERROR;
      }

      // Increment number of byte available for a read
      m_rxUdpFifoBytesCount++;
      g_rxRingBuffer[m_rxUdpFifoHeadPointer] = *ptrData;
      m_rxUdpFifoHeadPointer = (m_rxUdpFifoHeadPointer + 1) % RX_RING_BUFFER_SIZE;
      ptrData++;
    }
  }

  return NO_ERROR;
}


/**
 * @brief      The TCP/IP status changed
 *
 * @param[in]  endpoint    The endpoint
 * @param[in]  localIp     The local ip
 * @param[in]  localPort   The local port
 * @param[in]  remoteIp    The remote ip
 * @param[in]  remotePort  The remote port
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_EventTcpIpEndpointStatus( const uint8_t endpoint,
                                                                  const IpAddress localIp,
                                                                  const uint16_t localPort,
                                                                  const IpAddress remoteIp,
                                                                  const uint16_t remotePort){
  ErrorCode errorCode = NO_ERROR;

  if(localPort != ROVER_UDP_PORT){
      m_udpConnectSet = true;
      m_udpSendEndpoint = endpoint;
  }
  return errorCode;
}


/**
 * @brief      Event returned when ip address is set.
 *
 * @param[in]  address  The address
 * @param[in]  netmask  The netmask
 * @param[in]  gateway  The gateway
 * @param[in]  useDhcp  The use dhcp
 *
 * @return     The error code.
 */
ErrorCode CubeRoverNetworkManager :: cb_EventConfigureTcpIp( const IpAddress address,
                                                             const Netmask netmask,
                                                             const Gateway gateway,
                                                             const uint8_t useDhcp){
  m_ipConfigurationSet = true;
  return NO_ERROR;
}


