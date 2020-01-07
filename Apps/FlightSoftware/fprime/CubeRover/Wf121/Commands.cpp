#include "Wf121.hpp"

using namespace Wf121;

/**
 * @brief      This command can be used to check whether communication between
 *             the Wi-Fi software and hardware functions.
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: HelloSystem(){
  BgApiHeader txHeader;

  txHeader.bit.msgType = CMD_RSP_TYPE; // command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0); //no payload expected
  txHeader.bit.classId = CLASS_SYSTEM;
  txHeader.bit.cmdId = 0x02;

  return transmitCommand(&txHeader);
}

/**
 * @brief      This command can be used to reset the Wi-Fi module. This command
 *             does not have a response, but it triggers the boot event.
 *
 * @param[in]  bootMode  The boot mode
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ResetSystemWifi(const BootMode bootMode){
  BgApiHeader txHeader;
  uint8_t payload[1] /* size of boot mode */;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
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
 * @brief      This command can be used to set the maximum power saving state
 *             allowed for the Wi-Fi module
 *
 * @param[in]  state  * Mode 0: No power saving is in use. Use this mode for the
 *                    lowest latency and best performance.
 *                    * Mode 1: The Wi-Fi radio is allowed to sleep and it will
 *                      automatically go to sleep after 6000 ms of inactivity.
 *                    * Mode 2: Both MCU and Wi-Fi radio are allowed to go to
 *                      sleep after an inactivity timeout defined in the
 *                      hardware configuration file. The module wakes up
 *                      automatically every eight (8) seconds to check for
 *                      scheduled tasks and it also generates  event to notify
 *                      the host of the scheduled Power Saving Statewake up.
 *                    * If the sleep configuration is not used in the hardware
 *                      configuration file then only states 0 and 1 are
 *                      <sleep>possible.
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetMaxPowerSavingState(const PowerSavingState state){
  BgApiHeader txHeader;
  uint8_t payload[1] /* size of state */;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload)); // payload of size 1
  txHeader.bit.classId = CLASS_SYSTEM;
  txHeader.bit.cmdId = 0x03; // power saving system command

  // Prepare payload
  payload[0] = state;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);
}

/**
 * @brief      This command can be used to synchronize the system state. When
 *             the sync command is sent, multiple events are output representing
 *             the system status. This command can be used to synchronize the
 *             host software's status with the Wi-Fi software's status.
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SyncSystem(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0); // no payload
  txHeader.bit.classId = CLASS_SYSTEM;
  txHeader.bit.cmdId = 0x00; // power saving system command

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);  
}

/**
 * @brief      This command can be used to read the IEEE address of the device.
 *
 * @param[in]  interface  The interface
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: GetMacAddress(const HardwareInterface interface){
  BgApiHeader txHeader;
  uint8_t payload[1] /* size of interface */;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload)); // payload of size 1
  txHeader.bit.classId = CLASS_CONFIGURATION;
  txHeader.bit.cmdId = 0x00; // get MAC address command

  // Prepare payload
  payload[0] = interface;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}

/**
 * @brief      This command can be used to write an IEEE address into the
 *             device.
 *
 * @param[in]  interface  The interface
 * @param[in]  mac        The new value
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetMacAddress(const HardwareInterface interface,
                                       const MacAddress mac){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(HardwareInterface) + sizeof(MacAddress)];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_CONFIGURATION;
  txHeader.bit.cmdId = 0x01; // set MAC address command

  // Prepare payload
  payload[0] = interface;
  memcpy(payload+sizeof(HardwareInterface) , mac, sizeof(MacAddress));

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}

/**
 * @brief      This command can be used to turn on the 802.11 radio.
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: TurnOnWifi(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0); // payload of size 0
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x03; // turn on wifi command

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);   
}

/**
 * @brief      This command can be used to turn off the 802.11 radio.
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: TurnOffWifi(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0); // payload of size 0
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x01; // turn off wifi command

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);   
}

/**
 * @brief      This command can be used to set the default scan channel list for
 *             commands  and .Start ScanConnect Ssid
 *
 * @param[in]  interface        The interface
 * @param[in]  list             The list
 * @param[in]  channelListSize  The channel list size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetScanChannels(const HardwareInterface interface, 
                                         ChannelList *list,
                                         const ChannelListSize channelListSize){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(HardwareInterface) + sizeof(ChannelListSize) + channelListSize];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x09; // set scan channel command

  // Prepare payload
  payload[0] = interface;
  payload[sizeof(HardwareInterface)] = channelListSize;
  memcpy(payload + sizeof(HardwareInterface) + sizeof(ChannelListSize),
         list,
         channelListSize);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}

/**
 * @brief      This command initiates a scan for Access Points. Scanning is not
 *             possible once connected.
 *
 * @param[in]  interface        The interface
 * @param[in]  list             The list
 * @param[in]  channelListSize  The channel size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: StartScanChannels(const HardwareInterface interface, 
                                           ChannelList *list,
                                           const ChannelListSize channelListSize){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(HardwareInterface) + sizeof(ChannelListSize) + channelListSize];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x03; // set scan channel command

  // Prepare payload
  payload[0] = interface;
  payload[sizeof(HardwareInterface)] = channelListSize;
  memcpy(payload + sizeof(HardwareInterface) + sizeof(ChannelListSize),
         list,
         channelListSize);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}

/**
 * @brief      This command can be used to terminate the active scanning procedure.
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: StopScanChannels(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0); // payload of size 0
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x04; // stop scan channel command

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);   
}


/**
 * @brief      This command can be used to try to connect to a specific Access
 *             Point using its unique BSSID. In order to succeed, this command
 *             requires a preceding  command and that the desired wireless
 *             network sme_start_scanwas found during that scan. If the Access
 *             Point is using channel 12 or 13, for the connection to be
 *             successful at least one of the Access Points found within radio
 *             coverage range must advertise the use of channels up to 13.
 *
 * @param[in]  bssid  The bssid
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ConnectBssid(const HardwareAddress bssid){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(HardwareAddress)];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x06; // connect bssid command

  // Prepare payload
  memcpy(payload, bssid, sizeof(HardwareAddress));

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);
}


/**
 * @brief      This command can be used to disconnect from the currently
 *             connected Access Point.
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: Disconnect(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0); // payload of size 0
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x08; // disconnect from AP command

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);   
}

/**
 * @brief      This command can be used to resend scan results of a previous
 *             scan, sorted according to RSSI value. This command can be run
 *             only after the command  has been issued at least once during the
 *             current sme_start_scansession.
 *
 * @param[in]  amount  The amount of wireless network to scans. closest on top
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ScanResultsSortRssi(const uint8_t amount){
  BgApiHeader txHeader;
  uint8_t payload[1];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x0D; // scan resullt rssi

  // prepare the payload
  payload[0] = amount;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);
}


/**
 * @brief      This command can be used to initiate an active scan for Access
 *             Points. Scanning is not possible once connected.
 *
 * @param[in]  ssid      The ssid
 * @param[in]  ssidSize  The ssid size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: StartSsidScan(const Ssid *ssid, 
                                       const SsidSize ssidSize){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(SsidSize) + ssidSize];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x14; // connect SSID command

  // prepare the payload (see datasheet for format)
  payload[0] = ssidSize;

  memcpy(payload + sizeof(SsidSize),
          ssid,
          ssidSize);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);
}


/**
 * @brief      This command can be used to set whether the Access Point is
 *             hidden or visible. The Access Point is set visible by default
 *
 * @param[in]  hidden  The hidden
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetApHidden(const bool hidden){
   BgApiHeader txHeader;
  uint8_t payload[1] /* hidden */;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x14; // connect SSID command

  // prepare the payload (see datasheet for format)
  payload[0] = hidden;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload); 
}


/**
 * @brief      This command can be used to set the network password used when
 *             authenticating with an Access Point.
 *
 * @param[in]  pwd      The new value of the password in ascii format
 * @param[in]  pwdSize  The password size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetPassword(const Password *pwd,
                                     const PasswordSize pwdSize){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(PasswordSize) + pwdSize];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x05; // set password command

  // prepare the payload (see datasheet for format)
  payload[0] = pwdSize;

  memcpy(payload + sizeof(PasswordSize),
         pwd,
         pwdSize);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);
}


/**
 * @brief      This command can be used to start a connection establishment
 *             procedure with an Access Point with the given SSID. This command
 *             supports both visible and hidden SSIDs.Executing this command
 *             will also launch a transparent scan procedure in order to
 *             discover the Access Points in range, but the results of the scan
 *             procedure will not be exposed to the user. The channels used in
 *             the scan procedure can be defined with the  command. If the
 *             command has not been executed all Set Scan Channelschannels (1 to
 *             13) will be scanned. If the Access Point is using channel 12 or
 *             13, for the connection to be successful, at least one of the
 *             Access Points found in range must advertise the use of channels
 *             up to 13
 *
 * @param[in]  ssid      The ssid
 * @param[in]  ssidSize  The ssid size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ConnectSsid(const Ssid *ssid,
                                     const SsidSize ssidSize){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(SsidSize) + ssidSize];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x07; // connect SSID command

  // prepare the payload (see datasheet for format)
  payload[0] = ssidSize;

  memcpy(payload + sizeof(SsidSize),
          ssid,
          ssidSize);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);
}

/**
 * @brief      This command can be used to get a value indicating the signal
 *             quality of the connection.
 *
 * @return     The signal quality.
 */
ErrorCode Wf121Driver :: GetSignalQuality(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0); // payload of size 0
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x13; // get signal quality command

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);   
}

/**
 * @brief      This command can be used to start the Wi-Fi Protected Setup (WPS)
 *             session. Only WPS PUSH mode for the Wi-Fi client side is
 *             available
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: StartWps(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0); // payload of size 0
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x11; // start WPS command

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);   
}


/**
 * @brief      This command can be used to stop the Wi-Fi Protected Setup (WPS)
 *             session.
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: StopWps(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0); // payload of size 0
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x12; // stop WPS command

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);   
}


/**
 * @brief      This command can be used to set the Wi-Fi operating mode either
 *             to Wi-Fi client (STA) or Wi-Fi Access Point (AP). The selected
 *             operating mode will become effective after the next time the
 *             radio is turned on by launching the command .sme_wifi_on
 *
 * @param[in]  mode  The mode
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetOperatingMode(const OperatingMode mode){
  BgApiHeader txHeader;
  uint8_t payload[1] /* mode size */;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x0A; // set operating mode command

  payload[0] = mode;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}


/**
 * @brief      This command can be used to select whether 802.11n mode is
 *             enabled or disabled. The mode is enabled by default.
 *
 * @param[in]  mode  The mode 0: disabled, 1: enabled
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: Set11nMode(const bool mode){
  BgApiHeader txHeader;
  uint8_t payload[1] /* mode size */;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x16; // set operating mode command

  payload[0] = mode;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}


/**
 * @brief      This command can be used to isolate clients from each other in
 *             Access Point mode. Multiple clients can be connected to the AP
 *             and communicate with the AP but not with each other. The
 *             isolation is disabled by default. Note that in this mode no
 *             multicast traffic is re-transmitted by the AP
 *
 * @param[in]  isolation  The isolation 0: disabled, 1: enabled
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetApClientIsolation(const bool isolation){
  BgApiHeader txHeader;
  uint8_t payload[1] /* mode size */;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x16; // set operating mode command

  payload[0] = isolation;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}



/**
 * @brief      This command can be used to set the maximum amount of stations
 *             that can be associated to the Access Point at the same time.
 *
 * @param[in]  maxClients  The maximum number of clients
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetApMaxClient(const uint8_t maxClients){
  BgApiHeader txHeader;
  uint8_t payload[1] /* maxClients size */;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x10; // set AP max clients command

  payload[0] = maxClients;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}


/**
 * @brief      This command can be used to set the Wi-Fi password for an Access
 *             Point.
 *
 * @param[in]  pwd      The new value
 * @param[in]  pwdSize  The password size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetApPassword(const Password *pwd,
                                       const PasswordSize pwdSize){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(PasswordSize) + pwdSize];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x0F; // set access point password command

  // prepare the payload (see datasheet for format)
  payload[0] = pwdSize;

  memcpy(payload + sizeof(PasswordSize),
         pwd,
         pwdSize);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);
}


/**
 * @brief      Starts an ap mode.
 *
 * @param[in]  chan      The channel
 * @param[in]  sm        Security mode
 * @param[in]  ssid      The ssid in ASCII format
 * @param[in]  ssidSize  The ssid size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: StartApMode(const Channel chan, 
                                     const SecurityMode sm,
                                     const Ssid *ssid,
                                     const SsidSize ssidSize){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(Channel) + 1 /*Security Mode */ + sizeof(SsidSize) + ssidSize];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x0B; // start AP mode command 

  // prepare the payload (see datasheet for format)
  payload[0] = chan;
  payload[1] = sm;
  payload[2] = ssidSize;

  memcpy(payload + 3,
          ssid,
          ssidSize);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      Stops the ap mode.
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: StopApMode(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0); // payload of size 0
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x0C; // stop AP mode

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);   
}


/**
 * @brief      Disconnect client from Access point
 *
 * @param[in]  hwAddr  The hardware address
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: DisconnectApClient(const HardwareAddress hwAddr){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(HardwareAddress)];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_WIFI;
  txHeader.bit.cmdId = 0x0E; // disconnect AP client command

  // Prepare payload
  memcpy(payload, hwAddr, sizeof(HardwareAddress));

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);
}


/**
 * @brief      This command can be used to configure TCP/IP settings.When using
 *             static IP addresses, this command can be used to configure the
 *             local IP address, netmask and gateway.When enabling a DHCP Client
 *             the settings for the static IP will be stored, but they will be
 *             overridden as soon as the remote DHCP Server will assign its IP
 *             configuration to the module.
 *
 * @param[in]  ip       The local IP address of the device.
 * @param[in]  mask     The netmask of the device.
 * @param[in]  gateway  The gateway used by the device when in station (client)
 *                      mode.In access point mode this is not relevant, because
 *                      the local DHCP server will automatically use the local
 *                      IP address above as the gateway to pass to remote
 *                      clients.
 * @param[in]  useDhcp  Use DHCP 0 : Use static IP settings 1 : DHCP Client is
 *                      used to obtain the dynamic IP configuration
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ConfigureTcpIp(const IpAddress ip, 
                                        const NetMask mask,
                                        const Gateway gateway,
                                        const bool useDhcp){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(IpAddress) + sizeof(NetMask) + sizeof(Gateway) + sizeof(bool)];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x04; // configure command

  // Prepare payload
  memcpy(payload, ip, sizeof(IpAddress));
  
  memcpy(payload + sizeof(IpAddress),
         mask,
         sizeof(NetMask));
  
  memcpy(payload + sizeof(IpAddress) + sizeof(NetMask),
         gateway,
         sizeof(Gateway));

  memcpy(payload + sizeof(IpAddress) + sizeof(NetMask) + sizeof(Gateway),
         useDhcp,
         sizeof(bool));

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);
}


/**
 * @brief      This command can be used to set the DHCP host name parameter
 *             (option 12) used in client DHCPDISCOVER and DHCPREQUEST messages
 *
 * @param      hostName      The host name
 * @param[in]  hostNameSize  The host name size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetDhcpHostName(DchpHostName  *hostName,
                                         const DhcpHostNameSize hostNameSize){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(DhcpHostNameSize) + hostNameSize];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x08; // set DCHP host name command 

  // prepare the payload (see datasheet for format)
  payload[0] = hostNameSize;

  memcpy(payload + sizeof(DhcpHostNameSize),
         hostName,
         hostNameSize);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);    
}

/**
 * @brief      This command can be used to configure DNS client settings.
 *
 * @param[in]  index  TTwo different DNS servers can be stored. Index indicates
 *                    which of the two this is.0 : primary DNS server 1 :
 *                    secondary DNS server
 * @param[in]  ip     The IP address of the DNS server
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ConfigureDns(const DnsIndex index, 
                                      IpAddress *ip){
  BgApiHeader txHeader;
  uint8_t payload[1 /*DnsIndex */ + sizeof(IpAddress)];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x05; // configure DNS command 

  // prepare the payload (see datasheet for format)
  payload[0] = index;

  memcpy(payload + 1,
         ip,
         sizeof(IpAddress));

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}


/**
 * @brief      This command can be used to start a procedure to resolve the
 *             hostname related to an IP address using the configured DNS
 *             servers
 *
 * @param      name  The name of the server whose IP address should be resolved,
 *                   for example www.bluegiga.com.
 * @param[in]  size  The size
 *
 * @return     The error code
 */
ErrorCode Wf121Driver :: GetDnsHostByName(DchpHostName * name,
                                          const DhcpHostNameSize size){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(DhcpHostNameSize) + size];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x06; // get dhcp host by name command 

  // prepare the payload (see datasheet for format)
  payload[0] = size;

  memcpy(payload + 1,
         name,
         size);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      This command can be used to set the mDNS hostname. mDNS service
 *             cannot be started until the hostname is set. The maximum length
 *             of hostname is 63 bytes.
 *
 * @param      name  The name
 * @param[in]  size  The size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetMdnsHostName(MdnsHostName * name,
                                          const MdnsHostNameSize size){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(MdnsHostNameSize) + size];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x0A; // set mdns hostname command 

  // prepare the payload (see datasheet for format)
  payload[0] = size;

  memcpy(payload + 1,
         name,
         size);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      This command can be used to start the mDNS service.
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: StartMDns(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0); // no payload
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x0B; // start mdsn command 

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);  
}


/**
 * @brief      This command can be used to stop a mDNS service.
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: StopMDns(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0); // no payload
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x0C; // stop mdsn command 

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);  
}


/**
 * @brief      This command can be used to add a new DNS-SD service. The maximum
 *             length of the service name is 15 bytes
 *
 * @param[in]  port             The port
 * @param[in]  protocol         The protocol
 * @param[in]  serviceName      The service name
 * @param[in]  serviceNameSize  The service name size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: DnsSdAddService(const TcpPort port,
                                         const Protocol protocol,
                                         const ServiceName *serviceName,
                                         const ServiceNameSize serviceNameSize){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(TcpPort) + sizeof(Protocol) + sizeof(ServiceNameSize) + serviceNameSize];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x0D; // set dns add service command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         port,
         sizeof(TcpPort));

  memcpy(payload + sizeof(TcpPort),
         protocol,
         sizeof(Protocol));

  memcpy(payload + sizeof(TcpPort) + sizeof(Protocol),
         serviceNameSize,
         sizeof(serviceNameSize));

  memcpy(payload + sizeof(TcpPort) + sizeof(Protocol) + sizeof(ServiceNameSize),
         serviceName,
         serviceNameSize);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}


/**
 * @brief      This command can be used to add a DNS-SD service instance name.
 *             The maximum length of the service instance name is 63 bytes. The
 *             DNS-SD service cannot be started until the instance name is set
 *
 * @param[in]  index            The index
 * @param[in]  serviceName      The service name
 * @param[in]  serviceNameSize  The service name size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: DnsSdAddServiceInstance(const uint8_t index,
                                                 const ServiceName *serviceName,
                                                 const ServiceNameSize serviceNameSize){
  BgApiHeader txHeader;
  uint8_t payload[1 /* index */ + sizeof(ServiceNameSize) + serviceNameSize];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x0E; // set dns add service instance command 

  // prepare the payload (see datasheet for format)
  payload[0] = index;

  memcpy(payload + 1 /* index */,
         serviceNameSize,
         sizeof(ServiceNameSize));

  memcpy(payload + 1 /* index */ + sizeof(ServiceNameSize),
         serviceName,
         serviceNameSize);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}


/**
 * @brief      This command can be used to add a DNS-SD service attribute. The
 *             maximum length of the service attribute is 63 bytes
 *
 * @param[in]  index                 The index
 * @param[in]  serviceAttribute      The service attribute
 * @param[in]  serviceAttributeSize  The service attribute size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: DnsSdAddServiceAttribute(const uint8_t index,
                                                  const ServiceAttribute *serviceAttribute,
                                                  const ServiceAttributeSize serviceAttributeSize){
  BgApiHeader txHeader;
  uint8_t payload[1 /* index */ + sizeof(ServiceAttribute) + serviceAttributeSize];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x0F; // set dns add service attribute command 

  // prepare the payload (see datasheet for format)
  payload[0] = index;

  memcpy(payload + 1 /* index */,
         serviceAttributeSize,
         sizeof(ServiceAttributeSize));

  memcpy(payload + 1 /* index */ + sizeof(ServiceAttributeSize),
         serviceAttribute,
         serviceAttributeSize);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}


/**
 * @brief      This command can be used to remove a DNS-SD service
 *
 * @param[in]  index  The index
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: DnsSdRemoveService(const uint8_t index){
  BgApiHeader txHeader;
  uint8_t payload[1];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x10; // remove service command 

  // prepare the payload (see datasheet for format)
  payload[0] = index;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}


/**
 * @brief      This command can be used to start a DNS-SD service
 *
 * @param[in]  index  The index
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: DnsSdStartService(const uint8_t index){
  BgApiHeader txHeader;
  uint8_t payload[1];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x11; // start service command 

  // prepare the payload (see datasheet for format)
  payload[0] = index;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}


/**
 * @brief      This command can be used to stop a DNS-SD service.
 *
 * @param[in]  index  The index
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: DnsSdStopService(const uint8_t index){
  BgApiHeader txHeader;
  uint8_t payload[1];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x12; // stop service command 

  // prepare the payload (see datasheet for format)
  payload[0] = index;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}


/**
 * @brief      This command is used to join a multicast group.Maximum number of
 *             multicast groups that can be joined is 4. Use 224.0.0.2 -
 *             224.0.0.254 as address range. Note that 224.0.0.1 is
 *             automatically joined.
 *
 * @param      ip    IP address of the multicast group
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: MulticastJoin(IpAddress *ip){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(IpAddress)];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x13; // multicast join command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         ip,
         sizeof(IpAddress));

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}


/**
 * @brief      This command is used to leave a multicast group.
 *
 * @param      ip    IP address of the multicast group
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: MulticastLeave(IpAddress *ip){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(IpAddress)];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x14; // multicast leave command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         ip,
         sizeof(IpAddress));

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}


/**
 * @brief      This command can be used to configure DHCP Server subnetwork mask
 *             and address lease time.Values are saved in PS-keys called
 *             FLASH_PS_KEY_DHCPS_SPACE, FLASH_PS_KEY_DHCPS_MASK and
 *             FLASH_PS_KEY_DHCPS_LEASETIME and default values are 192.168.1.2,
 *             255.255.255.0 and 86400 seconds. Parameters are taken in use on
 *             DHCP server startup.
 *
 * @param      ip         First IPv4 address of address leasing pool
 * @param      netmask    Subnetwork mask
 * @param[in]  leaseTime  DHCP address lease timeout in seconds
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: DhcpConfigure(IpAddress *ip,
                                       Netmask *netmask,
                                       const uint32_t leaseTime){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(IpAddress)];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x15; // dhcp configure leave command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         ip,
         sizeof(IpAddress));

  memcpy(payload + sizeof(IpAddress),
         netmask,
         sizeof(Netmask));

  memcpy(payload + sizeof(IpAddress) + sizeof(Netmask),
         leaseTime, /* lease time */
         4);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}


/**
 * @brief      This command can be used to get IPv4 address and MAC address of
 *             each client connected to WF121 access point.
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: DhcpClients(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0);
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x16; // dhcp clients command 

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);   
}

/**
 * @brief      This command can be used to attempt the creation of a new TCP
 *             socket to a TCP server
 *
 * @param      ip       The IP address of the remote server to which the module
 *                      should connect.
 * @param[in]  port     The TCP port on the remote server
 * @param[in]  routing  The endpoint where the incoming data from the TCP server
 *                      should be routed to. : data received is not
 *                      automatically routed to other endpoint, but received as
 *                      -1 endpoint event.
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: TcpConnect(IpAddress *ip, 
                                    const TcpPort port,
                                    const int8_t routing){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(IpAddress) + sizeof(TcpPort) + 1 /* routing endpoint*/];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x01; // TCP connect command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         ip,
         sizeof(IpAddress));

  memcpy(payload + sizeof(IpAddress),
         &port,
         sizeof(TcpPort));

  payload[sizeof(IpAddress) + sizeof(TcpPort)] = routing;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload); 
}


/**
 * @brief      This command can be used to start a TCP server.Once the TCP
 *             server is started, and a remote client establishes a new
 *             connection, the data coming from this client will be routed by
 *             default to the endpoint specified in the  parameter. If such
 *             endpoint, default_destinationsay the UART interface, is
 *             configured to communicate with host via the BGAPI, then data will
 *             be carried via the event, otherwise raw data is sent out of the
 *             specified interface. When -1 is used, data received endpoint_data
 *             -1from the client is passed to BGScript via event, and/or event
 *             containing the data endpoint_data endpoint_data is sent out of
 *             the interfaces over which BGAPI is enable
 *
 * @param[in]  port                The port
 * @param[in]  defaultDestination  Endpoint where data from connected client
 *                                 will be routed to. Use -1 to generate events
 *                                 instead
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: StartTcpServer(const TcpPort port,
                                        const int8_t defaultDestination){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(TcpPort) + 1 /* default destination*/];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x00; // TCP connect command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &port,
         sizeof(TcpPort));

  payload[sizeof(TcpPort)] = defaultDestination;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);
}


/**
 * @brief      Connect to UDP
 *
 * @param      ip       The IP address
 * @param[in]  port     The port
 * @param[in]  routing  The endpoint index where the data from this connection
 *                      should be routed to. Notice that in the current
 *                      firmwares there cannot be data coming from the endpoint
 *                      assigned to this UDP connection, due to the
 *                      connectionless nature of the UDP protocol. So, any index
 *                      can be used here and no practical effect should be
 *                      expected.
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: UdpConnect(IpAddress *ip, 
                                    const UdpPort port,
                                    const int8_t routing){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(IpAddress) + sizeof(UdpPort) + 1 /* routing endpoint*/];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x03; // UDP connect command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         ip,
         sizeof(IpAddress));

  memcpy(payload + sizeof(IpAddress),
         &port,
         sizeof(UdpPort));

  payload[sizeof(IpAddress) + sizeof(UdpPort)] = routing;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload); 
}


/**
 * @brief      In case an UDP endpoint exists, this command can be used to
 *             change the currently used local source port (which is otherwise
 *             pseudo-randomly generated by the firmware) to a desired specific
 *             source port. Use this command after the command tcpip_udp_connect
 *             is issued and the UDP endpoint assigned
 *
 * @param[in]  endpoint  The endpoint which source port to change
 * @param[in]  port      The UDP port of source
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: UdpBind(const Endpoint endpoint, 
                                 const UdpPort port){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(Endpoint) + sizeof(UdpPort)];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x07; // UDP bind command 

  // prepare the payload (see datasheet for format)
  payload[0] = endpoint;

  memcpy(payload + sizeof(Endpoint),
         port,
         sizeof(UdpPort));

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload); 
}


/**
 * @brief      This command can be used to start an UDP server
 *
 * @param[in]  port                the local UDP port that the server listens on
 * @param[in]  defaultDestination  The endpoint to which incoming UDP packets
 *                                 should be written.-1 for destination means
 *                                 incoming data is notified with Udp data event Udp
 *                                 Dataw hich is the event carrying in addition
 *                                 the source IP address and port.
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: StartUdpServer(const UdpPort port,
                                        const uint8_t defaultDestination){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(UdpPort) + 1 /* default destination*/];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x02; // start udp server command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &port,
         sizeof(UdpPort));

  payload[sizeof(UdpPort)] = defaultDestination;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);
}


/**
 * @brief      This command can be used to enable or disable gateway and DNS
 *             router options in DHCP server offer and ack. Options are enabled
 *             by default.
 *
 * @param[in]  enable  The enable
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: DhcpEnableRouting(const bool enable){
  BgApiHeader txHeader;
  uint8_t payload[1 /* enable */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_TCP_STACK;
  txHeader.bit.cmdId = 0x09; // start udp server command 

  // prepare the payload (see datasheet for format)
  payload[0] = enable;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);
}


/**
 * @brief      This command can be used to activate or deactivate endpoints. By
 *             default endpoints are active, i.e. you can send data to them, and
 *             data can be received from them. This command allows you to
 *             temporarily halt the outgoing data from an endpoint by
 *             deactivating it. For example, deactivating a UART endpoint over
 *             which BGAPI is carried, will prevent BGAPI events and responses
 *             to go out of that UART interface (but host can still send BGAPI
 *             commands to it). Similarly, deactivating the BGScript endpoint
 *             will prevent events to be passed to the script, thus preventing
 *             the calls in it to be executed. Server endpoints however are
 *             never active, as they can neither send nor receive data
 *
 * @param[in]  endpoint        The endpoint
 * @param[in]  endpointStatus  Endpoint status 0 : inactive 1 : active
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetActiveEndpoint(const Endpoint endpoint,
                                           const bool endpointStatus){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(Endpoint) + 1 /* endpointStatus */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_ENDPOINT;
  txHeader.bit.cmdId = 0x02; // set active endpoint command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &endpoint,
         sizeof(Endpoint));

  payload[sizeof(Endpoint)] = endpointStatus;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);
}


/**
 * @brief      Sends data to an endpoint.
 *
 * @param[in]  endpoint  The endpoint
 * @param      data      The data
 * @param[in]  dataSize  The data size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SendEndpoint(const Endpoint endpoint,
                                      uint8_t *data,
                                      const DataSize dataSize){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(Endpoint) + sizeof(DataSize) + dataSize];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_ENDPOINT;
  txHeader.bit.cmdId = 0x00; // send data to endpoint command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &endpoint,
         sizeof(Endpoint));

  memcpy(payload + sizeof(Endpoint),
         dataSize,
         sizeof(DataSize));

  memcpy(payload + sizeof(Endpoint) + sizeof(DataSize),
         data,
         dataSize);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);    
}


/**
 * @brief      This command can be used to set the desired transmit packet size:
 *             endpoint will buffer outgoing data until packet size is reached
 *             and then transmit it to remote end. This only applies to UDP
 *             endpoints, and should not be used with any other type of
 *             endpoint, including TCP. When using packet size 0, the data will
 *             be sent not be used with any other type of endpoint, including
 *             TCP.If the transmit packet size is set to a higher value than
 *             255, then multiple endpoint_send command need to be issued to
 *             fill the transmit buffer and to effectively send the data to the
 *             remote end, due to the fact that the endpoint_send command can
 *             carry at most 255 payload bytes.
 *
 * @param[in]  endpoint      The endpoint
 * @param[in]  transmitSize  The transmit size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetTransmitSize(const Endpoint endpoint,
                                         const uint16_t transmitSize){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(Endpoint) + sizeof(uint16_t) /* transmitSize */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_ENDPOINT;
  txHeader.bit.cmdId = 0x05; // set transmit size command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &endpoint,
         sizeof(Endpoint));

  memcpy(payload + sizeof(Endpoint),
         transmitSize,
         sizeof(transmitSize));

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      This command can be used to configure an UART into a streaming or
 *             BGAPI mode. When an UART endpoint is in a streaming mode, the
 *             data gets transparently routed to another endpoint like TCP. In
 *             BGAPI mode the data is exposed via BGAPI.This setting currently
 *             only operates on UART endpoints
 *
 * @param[in]  endpoint   The endpoint
 * @param[in]  streaming  Endpoint mode 0 : Use as BGAPI interface 1 : Streaming
 *                        to another endpoint
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetStreaming(const Endpoint endpoint,
                                      const Streaming streaming){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(Endpoint) + sizeof(uint8_t) /* streaming */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_ENDPOINT;
  txHeader.bit.cmdId = 0x01; // set streaming command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &endpoint,
         sizeof(Endpoint));

  payload[sizeof(Endpoint)] = streaming;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      This command can be used to set the destination where data from an endpoint will be routed to.
 *
 * @param[in]  endpoint  The endpoint
 * @param[in]  dest      The destination
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetStreamingDestination(const Endpoint endpoint,
                                                 const StreamingDestination dest){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(Endpoint) + sizeof(StreamingDestination)];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_ENDPOINT;
  txHeader.bit.cmdId = 0x03; // set streaming destination command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &endpoint,
         sizeof(Endpoint));

  memcpy(payload + sizeof(Endpoint),
         dest,
         sizeof(StreamingDestination));

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      This command can be used to close an endpoint. This command is
 *             valid only for UDP or TCP endpoints
 *
 * @param[in]  endpoint  The endpoint
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: CloseEndpoint(const Endpoint endpoint){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(Endpoint)];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_ENDPOINT;
  txHeader.bit.cmdId = 0x04; // close endpoint command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &endpoint,
         sizeof(Endpoint));

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}

/**
 * @brief      This command can be used to disable an UART type endpoint. This
 *             command effectively turns down an UART interface until the module
 *             is reset or power-cycled. When an UART interface is disabled its
 *             pins go to high-impedance state.
 *
 * @param[in]  endpoint  Index of the endpoint to disable 0: UART0 1: UART1
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: DisableEndpoint(const Endpoint endpoint){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(Endpoint)];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_ENDPOINT;
  txHeader.bit.cmdId = 0x06; // disable endpoint command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &endpoint,
         sizeof(Endpoint));

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}

/**
 * @brief      This command can be used to enable the software timer. Multiple
 *             concurrent timers can be running at the same time
 *
 * @param[in]  timeMs      Interval between how often to send events, in
 *                         milliseconds.If time is 0, removes the scheduled
 *                         timer
 * @param[in]  handleTimer The handle
 * @param[in]  singleShot  The single shot
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetSoftTimer(const TimeMs timeMs,
                                      const HandleTimer handle,
                                      const bool singleShot){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(TimeMs) + sizeof(HandleTimer) + 1 /* singleshot */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HARDWARE;
  txHeader.bit.cmdId = 0x00; // set soft timer command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &timeMs,
         sizeof(TimeMs));

  memcpy(payload + sizeof(TimeMs),
         &handle,
         sizeof(HandleTimer));

  payload[sizeof(TimeMs) + sizeof(HandleTimer)] = singleshot;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      This command can be used to configure pins which will generate
 *             interrupts.In the WF121 Wi-Fi module there are four pins which
 *             support interrupts: RD0/INT0, RD9/INT2, RD10/INT3, RD11/INT4.
 *             INT1 is reserved for WF121's internal use and cannot be used for
 *             other purposes. Interrupts can be triggered either on the rising
 *             edge or the falling edge
 *
 * @param[in]  enable    External interrupt bits to enable 
 *                       INT0 : 0x01
 *                       INT2 : 0x04
 *                       INT3 : 0x08
 *                       INT4 : 0x10
 *                       Example: interrupts INT0 and INT4 are enabled 
 *                       with value of 0x5
 * @param[in]  polarity  External interrupt polarity bits, rising edge if set, 
 *                       falling edge otherwise
 *                       INT0 : 0x01
 *                       INT2 : 0x04
 *                       INT3 : 0x08
 *                       INT4 : 0x10
 *                       Example: INT0 as falling and INT2 as rising 
 *                       are set with value of 0x4
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ConfigureExternalInterrupt(const InterruptMask enable,
                                                    const InterruptMask polarity){
  BgApiHeader txHeader;
  uint8_t payload[sizeof(InterruptMask) + sizeof(InterruptMask)];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HARDWARE;
  txHeader.bit.cmdId = 0x01; // configure external command 

  // prepare the payload (see datasheet for format)
  payload[0] = enable;
  payload[1] = polarity;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload); 
}


/**
 * @brief      This command can be used to configure change notifications (CN).
 *             The PIC32 micro controller has a limited number of standard GPIO
 *             interrupts. Change notifications can be used in a similar way to
 *             GPIO interrupts in most cases but they are not identical with
 *             each other and operate on different pins. This command can be
 *             used to configure for which pins the change notification
 *             interrupts are enabled. A list of pins and corresponding change
 *             notification source see WF121 Datasheet  More detailed page 9,
 *             Table 2: Multifunction pad descriptions
 *
 * @param[in]  enable  The enable
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ConfigureChangeNotification(const uint32_t enable){
  BgApiHeader txHeader;
  uint8_t payload[4 /* enable */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HARDWARE;
  txHeader.bit.cmdId = 0x02; // config change notificiation command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &enable,
         4 /*size of enable */);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      This command can be used to configure change notification pull-up
 *             settings. For a detailed discussion concerning change
 *             notifications, see the Change Notification Notice command.
 *
 * @param[in]  pullup  Bitmask for which of the change notification pins have
 *                     pull-ups enabled
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ChangeNotificationPullup(const uint32_t pullup){
  BgApiHeader txHeader;
  uint8_t payload[4 /* pullup */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HARDWARE;
  txHeader.bit.cmdId = 0x03; // pullup command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &pullup,
         4 /*size of enable */);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      This command can be used to configure the data flow direction of
 *             I/O-port(s).
 *
 * @param[in]  port          The port
 * @param[in]  bitMask       Bit mask of which pins on the port this command
 *                           affects
 * @param[in]  bitDirection  The bit mask describing which are inputs and which
 *                           are outputs. 0 : Output. 1 : Input
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ConfigureIoPort(const Wf121IoPort port,
                                         const uint16_t bitMask,
                                         const uint16_t bitDirection){
  BgApiHeader txHeader;
  uint8_t payload[1 /* port */ + 2 /* bitmask */ + 2 /* bit direction */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HARDWARE;
  txHeader.bit.cmdId = 0x04; // io port configuration command 

  // prepare the payload (see datasheet for format)
  payload[0] = port;

  memcpy(payload + 1,
         &bitMask,
         2 /* size of bit mask */);

  memcpy(payload + 3, /* port + bit mask */
         &bitDirection,
         2 /* size of bit direction */);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      This command can be used to configure I/O-port open drain
 *             functionality. Open drain means that when the pin is in high
 *             state, it is in high impedance state and when low it is able to
 *             sink current. Open drain is sometimes also called .Open Collector
 *
 * @param[in]  port       The port
 * @param[in]  bitMask    Bitmask of which pins on the port this command affects
 * @param[in]  openDrain  Bitmask of which pins are configured to be open
 *                        drain.For each bit this means: 0 : Open drain disabled
 *                        1: Open drain enabled
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ConfigureIoOpenDrain(const Wf121IoPort port,
                                              const uint16_t bitMask,
                                              const uint16_t openDrain){
  BgApiHeader txHeader;
  uint8_t payload[1 /* port */ + 2 /* bitmask */ + 2 /* open drain */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HARDWARE;
  txHeader.bit.cmdId = 0x05; // io port open drain command 

  // prepare the payload (see datasheet for format)
  payload[0] = port;

  memcpy(payload + 1,
         &bitMask,
         2 /* size of bit mask */);

  memcpy(payload + 3, /* port + bit mask */
         &openDrain,
         2 /* size of open drain */);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      This command can be used to write the pins of an I/O-port.
 *
 * @param[in]  port     The port
 * @param[in]  bitMask  Bit mask of which pins on the port this command affects.
 *                      For each bit in the bit mask: 0 = Don't modify/write, 1
 *                      = modify/write
 * @param[in]  val      Bit mask of which pins to set. For each bit in the bit
 *                      mask: 0 : low 1 : high
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: WriteIoPort(const Wf121IoPort port,
                                     const uint16_t bitMask,
                                     const uint16_t val){
  BgApiHeader txHeader;
  uint8_t payload[1 /* port */ + 2 /* bitmask */ + 2 /* val*/];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HARDWARE;
  txHeader.bit.cmdId = 0x06; // io port write command 

  // prepare the payload (see datasheet for format)
  payload[0] = port;

  memcpy(payload + 1,
         &bitMask,
         2 /* size of bit mask */);

  memcpy(payload + 3, /* port + bit mask */
         &val,
         2 /* size of open drain */);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}

/**
 * @brief      This command can be used to read the status of pins of an
 *             I/O-port.
 *
 * @param[in]  port     The port
 * @param[in]  bitMask  Bitmask of which pins on the port should be read. For
 *                      each bit in the bitmask:0 : Don't read1 : Read
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ReadIoPort(const Wf121IoPort port,
                                     const uint16_t bitMask){
  BgApiHeader txHeader;
  uint8_t payload[1 /* port */ + 2 /* bitmask */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HARDWARE;
  txHeader.bit.cmdId = 0x07; // io port read command 

  // prepare the payload (see datasheet for format)
  payload[0] = port;

  memcpy(payload + 1,
         &bitMask,
         2 /* size of bit mask */);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      This command can be used to define compare settings, e.g., for
 *             PWM purposes. Output compare output is disabled when the module
 *             enters sleep mode. The <timer> tag in the  file must be
 *             configured hardware.xmlproperly if using this command
 *
 * @param[in]  index         The index
 * @param[in]  bit32         Is 32-bit mode selected. 
 *                           0: 16-bit
 *                           1: 32-bit, Requires timer to be configured for 32.
 * @param[in]  timer         The timer
 * @param[in]  mode          comparison mode 
 *                           0: Output compare peripheral is disabled but continues 
 *                           to draw current
 *                           1: Initialize OCx pin low; compare event forces OCx 
 *                           pin high
 *                           2: Initialize OCx pin high; compare event forces OCx
 *                           pin low
 *                           3: Compare event toggles OCx pin
 *                           4: Initialize OCx pin low; generate single output
 *                           pulse on OCx pin
 *                           5: Initialize OCx pin low; generate continuous output 
 *                           pulses on OCx pin
 *                           6: PWM mode on OCx; Fault pin disabled
 *                           7: PWM mode on OCx; Fault pin enabled
 *                           
 * @param[in]  compareValue  0-0xFFFF for 16-bit0-0xFFFFFFFF for 32-bit
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: OutputCompare(const CompareModuleIndex index,
                                       const bool bit32,
                                       const CompareModuleTimer timer,
                                       const CompareModuleMode mode,
                                       const uint32_t compareValue){
  BgApiHeader txHeader;
  uint8_t payload[1 /* index */ + 1 /* bit32 */ + 1 /* timer */ + 1 /* mode */ + 4 /* compareValue */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HARDWARE;
  txHeader.bit.cmdId = 0x08; // io port read command 

  // prepare the payload (see datasheet for format)
  payload[0] = index;
  payload[1] = bit32;
  payload[2] = timer;
  payload[3] = mode;

  memcpy(payload + 4,
         &compareValue,
         4 /* size of compare value */);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      This command is used to read the module's A/D converter. The
 *             hardware configuration file (normally hardware.xml) should also
 *             contain the <adc ... /> tag, which is meant to enable the pins to
 *             use for the ADC readings.
 *
 * @param[in]  adcInput  The adc input
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: AdcRead(const uint8_t adcInput){
  BgApiHeader txHeader;
  uint8_t payload[1 /* adc input */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HARDWARE;
  txHeader.bit.cmdId = 0x09; // adc input command 

  // prepare the payload (see datasheet for format)
  payload[0] = adcInput;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      This command can be used to initialize the internal Real Time Clock (RTC).
 *
 * @param[in]  enable  Enable/Disable RTC
 * @param[in]  drift   Drift of clock. Added to 32.768kHz SOSC every minute.
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: RtcInit(const bool enable,
                                 const int16_t drift){
  BgApiHeader txHeader;
  uint8_t payload[1 /*enable */ + 2 /* drift */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HARDWARE;
  txHeader.bit.cmdId = 0x0A; // RTC init command 

  // prepare the payload (see datasheet for format)
  payload[0] = enable;

  memcpy(payload + 1,
         &drift,
         2 /* size of drift */);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}

/**
 * @brief      This command can be used to set the internal Real Time Clock
 *             (RTC) time
 *
 * @param[in]  year     The year
 * @param[in]  month    The month
 * @param[in]  day      The day
 * @param[in]  weekday  The weekday
 * @param[in]  hour     The hour
 * @param[in]  minute   The minute
 * @param[in]  second   The second
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: RtcSetTime(const int16_t year,
                                    const int8_t month,
                                    const int8_t day,
                                    const int8_t weekday,
                                    const int8_t hour,
                                    const int8_t minute,
                                    const int8_t second){
  BgApiHeader txHeader;
  uint8_t payload[2 /*year */ + 6 /* month..seconds */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HARDWARE;
  txHeader.bit.cmdId = 0x0B; // RTC set time command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &year,
         2 /* size of year */);

  payload[2] = month;
  payload[3] = day;
  payload[4] = weekday;
  payload[5] = hour;
  payload[6] = minute;
  payload[7] = second;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      This commands can be used to read the internal Real Time Clock (RTC) value
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: RtcGetTime(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0);
  txHeader.bit.classId = CLASS_HARDWARE;
  txHeader.bit.cmdId = 0x0C; // RTC get time command 

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      This command can be used to set an alarm for the internal Real
 *             Time Clock (RTC)
 *
 * @param[in]  year         The year
 * @param[in]  month        The month
 * @param[in]  day          The day
 * @param[in]  weekday      The weekday
 * @param[in]  hour         The hour
 * @param[in]  minute       The minute
 * @param[in]  second       The second
 * @param[in]  repeatMask   The repeat mask
 * @param[in]  repeatCount  The repeat count
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: RtcSetAlarm( const int16_t year,
                                      const int8_t month,
                                      const int8_t day,
                                      const int8_t weekday,
                                      const int8_t hour,
                                      const int8_t minute,
                                      const int8_t second,
                                      const uint8_t repeatMask,
                                      const uint16_t repeatCount){
  BgApiHeader txHeader;
  uint8_t payload[2 /*year */ + 6 /* month..seconds */ + 3 /* repeat mask & count */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HARDWARE;
  txHeader.bit.cmdId = 0x0D; // RTC set alarm command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &year,
         2 /* size of year */);

  payload[2] = month;
  payload[3] = day;
  payload[4] = weekday;
  payload[5] = hour;
  payload[6] = minute;
  payload[7] = second;
  payload[8] = repeatMask;

  memcpy(payload + 9,
         &repeatCount,
         2 /* size of repeat count */);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);  
}


/**
 * @brief      This command can be used to re-configure an UART interface
 *
 * @param[in]  uartId    The uart identifier
 * @param[in]  baudrate  The baudrate
 * @param[in]  format    The format
 * @param[in]  stop      The stop
 * @param[in]  parity    The parity
 * @param[in]  flowCtl   The flow control
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ConfigureUart(const uint8_t uartId,
                                       const uint32_t baudrate,
                                       const UartDataBit format,
                                       const UartStopBit stop,
                                       const UartParity parity,
                                       const UartFlowCtl flowCtl){
  BgApiHeader txHeader;
  uint8_t payload[1 /* uartId */ + 4 /* baudrate */ + 4 /* uart data bit .. flow ctl */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HARDWARE;
  txHeader.bit.cmdId = 0x0E; // Uart config command 

  // prepare the payload (see datasheet for format)
  payload[0] = uartId;

  memcpy(payload + 1,
         &baudrate,
         4 /* size of baudrate */);
  
  payload[5] = format;
  payload[6] = stop;
  payload[7] = parity;
  payload[8] = flowCtl;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}


/**
 * @brief      his command can be used to read the current configuration of an
 *             UART interface
 *
 * @param[in]  uartId  The uart identifier
 *
 * @return     The hardware configuration.
 */
ErrorCode Wf121Driver :: GetHardwareConfiguration(const uint8_t uartId){
  BgApiHeader txHeader;
  uint8_t payload[1];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HARDWARE;
  txHeader.bit.cmdId = 0x0F; // Uart config command 

  // prepare the payload (see datasheet for format)
  payload[0] = uartId;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);   
}


/**
 * @brief      This command can be used to start I2C transmission for reading
 *             data. The data is transferred via Endpoint Dataevents.
 *
 * @param[in]  endpoint      I2C endpoint to use.
 * @param[in]  slaveAddress  Slave address to use
 * @param[in]  length        Amount of data to move
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: I2cStartRead(const uint8_t endpoint,
                                      const uint16_t slaveAddress,
                                      const uint8_t length){
  BgApiHeader txHeader;
  uint8_t payload[1 /* endpoint */ + 2 /*slave address */ + 1 /* length */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_I2C;
  txHeader.bit.cmdId = 0x00; // i2c start read command 

  // prepare the payload (see datasheet for format)
  payload[0] = endpoint;

  memcpy(payload + 1,
         &slaveAddress,
         2 /* size of slave address */);

  payload[3] = length;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);    
}


/**
 * @brief      This command can be used to prepare an I2C endpoint for data
 *             transmission. The data is sent using the endpoint send command
 *
 * @param[in]  endpoint      I2C endpoint to use.
 * @param[in]  slaveAddress  The slave address
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: I2cStartwrite(const uint8_t endpoint,
                                       const uint16_t slaveAddress){
  BgApiHeader txHeader;
  uint8_t payload[1 /* endpoint */ + 2 /*slave address */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_I2C;
  txHeader.bit.cmdId = 0x01; // i2c start write command 

  // prepare the payload (see datasheet for format)
  payload[0] = endpoint;

  memcpy(payload + 1,
         &slaveAddress,
         2 /* size of slave address */);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);    
}


/**
 * @brief      This command can be used to stop the I2C transmission
 *
 * @param[in]  endpoint  The endpoint
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: I2cStop(const uint8_t endpoint){
  BgApiHeader txHeader;
  uint8_t payload[1 /* endpoint */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_I2C;
  txHeader.bit.cmdId = 0x02; // i2c stop command 

  // prepare the payload (see datasheet for format)
  payload[0] = endpoint;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);    
}


/**
 * @brief      This command can be used to configure the Ethernet interface's
 *             data route and functionality mode
 *
 * @param[in]  route  0 : Off Ethernet interface is off and the link is down.
 *                    1 : BridgeModule is transparent Ethernet - Wi-Fi bridge mode
 *                    and all data is routed from Ethernet to Wi-Fi and vice
 *                    versa and it will bypass the built in IP stack.Before
 *                    enabling this setting you must first connect a Wi-Fi
 *                    network in client (STA) mode.
 *                    2 : Ethernet server Ethernet is connected to the module's 
 *                    built-in IP stack. And the built-in DHCP and HTTP servers, 
 *                    as well as TCP and UDP endpoints accessible via Ethernet.
 *                    Before enabling this setting, module must be configured 
 *                    into a Wi-Fi access point (AP) mode. However when this 
 *                    command is entered Wi-Fi is disabled and Ethernet used instead. 
 *                    3 : Ethernet device In this mode the Ethernet can be used as a
 *                    client to connect to a network instead of Wi-Fi. When this
 *                    command is entered the Wi-Fi radio is disabled
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SetDataRoute(const WiredEthernetRoute route){
  BgApiHeader txHeader;
  uint8_t payload[1 /* route */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_WIRED_ETHERNET;
  txHeader.bit.cmdId = 0x00; // set data route command 

  // prepare the payload (see datasheet for format)
  payload[0] = route;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);    
}


/**
 * @brief      This command can be used to close the wired Ethernet connection
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: CloseRoute(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0);
  txHeader.bit.classId = CLASS_WIRED_ETHERNET;
  txHeader.bit.cmdId = 0x01; // close route command 

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);    
}

/**
 * @brief      This command can be used to test wired Ethernet connection
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: Connected(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0);
  txHeader.bit.classId = CLASS_WIRED_ETHERNET;
  txHeader.bit.cmdId = 0x02; // connected command 

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);    
}


/**
 * @brief      This command can be used to enable or disable built-in HTTP, DHCP
 *             or DNS servers.When the DHCP server is started, the IP address
 *             pool for the clients will start with the IP address set with DHCP
 *             Configure--TCP/IP--Wi-Fi command.
 *
 * @param[in]  https  Enable/disable HTTP server 0: Disable 1: Enable
 * @param[in]  dhcps  Enable/disable DHCPS server 0: Disable 1: Enable
 * @param[in]  dnss   Enable/disable DNSS server 0: Disable 1: Enable
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: EnableServers(const bool https,
                                       const bool dhcps,
                                       const bool dnss){
  BgApiHeader txHeader;
  uint8_t payload[3 /* https + dhcps + dnss */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HTTP_SERVER;
  txHeader.bit.cmdId = 0x00; // enable servers command 

  payload[0] = https;
  payload[1] = dhcps;
  payload[2] = dnss;

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);    
}


/**
 * @brief      This command can be used to add a mapping between an HTTP server
 *             URL and a storage device from where a resource will be served
 *
 * @param[in]  device    Storage device type 0 : Built-in flash 
 *                                           1 : BGAPI / BGScript 
 *                                           2 : SD card
 * @param      path      The path
 * @param[in]  pathSize  The path size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: AddServerPath(const uint8_t device,
                                 ServerPath *path,
                                 const ServerPathSize pathSize){
  BgApiHeader txHeader;
  uint8_t payload[1 /* device */ + pathSize + sizeof(ServerPathSize)];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HTTP_SERVER;
  txHeader.bit.cmdId = 0x01; // add path command 

  // prepare the payload (see datasheet for format)
  payload[0] = device;
  payload[1] = pathSize;

  memcpy(payload + 2,
         path,
         pathSize);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);     
}


/**
 * @brief      This command can be used to send HTTP response data to a pending
 *             HTTP request
 *
 * @param[in]  request   The request
 * @param      data      The data
 * @param[in]  dataSize  The data size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ApiResponse(const uint32_t request,
                                     HttpResponseData *data,
                                     const HttpResponseDataSize dataSize){
  BgApiHeader txHeader;
  uint8_t payload[4 /* request */ + dataSize + sizeof(HttpResponseDataSize)];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HTTP_SERVER;
  txHeader.bit.cmdId = 0x02; // api response command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &request,
         4 /* request */);

  memcpy(payload + 4,
         &dataSize,
         sizeof(HttpResponseDataSize));

  memcpy(payload + 4 + sizeof(HttpResponseDataSize),
         data,
         dataSize);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);     
}


/**
 * @brief      This command can be used to signal that all HTTP response data
 *             has been sent and that the pending HTTP request can be closed.
 *
 * @param[in]  request  The request
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ApiResponseFinish(const uint32_t request){
  BgApiHeader txHeader;
  uint8_t payload[4 /* request */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_HTTP_SERVER;
  txHeader.bit.cmdId = 0x03; // api response finish command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &request,
         4 /* request */);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);     
}

/**
 * @brief      This command can be used to manually initiate the defragmentation
 *             of the Persistent Store. Persistent store is also automatically
 *             defragmented if there is not enough space
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: DefragPersistentStore(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0);
  txHeader.bit.classId = CLASS_PERSISTENT_STORE;
  txHeader.bit.cmdId = 0x01; // ps defrag command 

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);    
}

/**
 * @brief      This command can be used to erase all PS keys from the Persistent
 *             Store
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: EraseAllPersistentStore(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0);
  txHeader.bit.classId = CLASS_PERSISTENT_STORE;
  txHeader.bit.cmdId = 0x02; // erase all command 

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);    
}


/**
 * @brief      This command can be used to store a value into the given PS
 *             (Persistent Store) key. This command can be used to store user
 *             data into the Wi-Fi module flash memory, so that the data remains
 *             available across resets and power cycles.The maximum size of a
 *             single PS-key is 255 bytes and a total of 128 keys are available.
 *             There is 4KB of reserved space in total for all PS-Keys
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: SavePersistentStore(const uint16_t key,
                                             KeyValue *keyVal,
                                             const KeyValueSize keyValSize){
  BgApiHeader txHeader;
  uint8_t payload[2 /* key */ + sizeof(KeyValueSize) + keyValSize];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_PERSISTENT_STORE;
  txHeader.bit.cmdId = 0x03; // save key command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &key,
         2 /* sizeof key */);

  memcpy(payload + 2,
         &keyValSize,
         sizeof(KeyValueSize));

  memcpy(payload + 2 + sizeof(KeyValueSize),
         keyVal,
         KeyValueSize);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);    
}


/**
 * @brief      This command can be used to retrieve the value of the given PS
 *             key from the Persistent Store
 *
 * @param[in]  key   The key
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: LoadPersistentStore(const uint16_t key){
  BgApiHeader txHeader;
  uint8_t payload[2 /* key */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_PERSISTENT_STORE;
  txHeader.bit.cmdId = 0x04; // load key command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &key,
         2 /* sizeof key */);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);    
}


/**
 * @brief      This command can be used to dump all the PS keys from the
 *             Persistent Store. The command will generate a series of PS key
 *             events. The last PS key event is identified by the key index
 *             value 65535, indicating that the dump has finished listing all PS
 *             keys
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: DumpPersistentStore(){
  BgApiHeader txHeader;

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, 0);
  txHeader.bit.classId = CLASS_PERSISTENT_STORE;
  txHeader.bit.cmdId = 0x01; // ps dump command 

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, NULL);    
}

/**
 * @brief      This command can be used to erase a single PS key and its value
 *             from the Persistent Store
 *
 * @param[in]  key   The key
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: ErasePersistentStore(const uint16_t key){
  BgApiHeader txHeader;
  uint8_t payload[2 /* key */];

  //Prepare command header
  txHeader.bit.msgType = CMD_RSP_TYPE; //command
  txHeader.bit.technologyType = 1; // wifi
  setHeaderPayloadSize(&txHeader, sizeof(payload));
  txHeader.bit.classId = CLASS_PERSISTENT_STORE;
  txHeader.bit.cmdId = 0x05; // load key command 

  // prepare the payload (see datasheet for format)
  memcpy(payload,
         &key,
         2 /* sizeof key */);

  // transmit a command, an event is expected in  return
  return transmitCommand(&txHeader, payload);      
}