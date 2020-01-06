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
 * @brief      Transmit data to WF121 module
 *
 * @param      header   The header
 * @param      payload  The payload
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: transmitCommand(BgApiHeader *header,
                                         uint8_t *payload){

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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
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
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x00: // Boot
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x00:  // Get MAC
        break;
      case 0x01:  // Set MAC
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x00: // MAC address
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x00: // Wifi ON
        break;
      case 0x01: // Wifi OFF
        break;
      case 0x09: // Set scan channels
        break;
      case 0x03: // Start scan channels
        break;
      case 0x04: // Stop scan channels
        break;
      case 0x06: // Connect BSSID
        break;
      case 0x08: // Disconnect
        break;
      case 0x0D: // Scan results
        break;
      case 0x05: // Set password
        break;
      case 0x07: // Connect SSID
        break;
      case 0x13: // Get signal quality
        break;
      case 0x11: // Start WPS
        break;
      case 0x12: // Stop WPS
        break;
      case 0x0A: // Set operating mode
        break;
      case 0x10: // Set AP max clients
        break;
      case 0x0F: // Set AP password
        break;
      case 0x0B: // Start AP mode
        break;
      case 0x0C: // Stop AP mode
        break;
      case 0x0E: // AP disconnect client
        break; 
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x00: // Wifi ON
        break;
      case 0x01: // Wifi OFF
        break;
      case 0x02: // Scan result
        break;
      case 0x03: // Scan result drop
        break;
      case 0x04: // Scanned
        break;
      case 0x0F: // Scan sort result
        break;
      case 0x10: // Scan sort finished
        break;
      case 0x05: // Connected
        break;
      case 0x09: // Connect retry
        break;
      case 0x08: // Connect failed
        break;
      case 0x06: // Disconnected
        break;
      case 0x14: // WPS credential SSID
        break;
      case 0x15: // WPS credential password
        break;
      case 0x12: // WPS completed
        break;
      case 0x13: // WPS failed
        break;
      case 0x11: // WPS stopped
        break;
      case 0x16: // signal quality
        break;
      case 0x0A: // AP mode started
        break;
      case 0x0B: // AP mode stopped
        break;
      case 0x0C: // AP mode failed
        break;
      case 0x0D: // AP client joined
        break;
      case 0x0E: // AP client left
        break;
      case 0x07: // interface status
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }    
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x02: // set active
        break;
      case 0x00: // send
        break;
      case 0x05: // set transmit size
        break;
      case 0x01: // set streaming
        break;
      case 0x03: // set streaming destination
        break;
      case 0x04: // close endpoint
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x02: // status
        break;
      case 0x01: // data
        break;
      case 0x03: // closing
        break;
      case 0x04: // error
        break;
      case 0x00: // syntax error
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }    
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x09: // ADC read
        break;
      case 0x02: // Change notification
        break;
      case 0x03: // Change notification pullup
        break;
      case 0x01: // External interrupt
        break;
      case 0x04: // IO port config direction
        break;
      case 0x05: // IO port config drain
        break;
      case 0x07: // IO port read
        break;
      case 0x06: // IO port write
        break;
      case 0x08: // IO port compare
        break;
      case 0x0A: // RTC init
        break;
      case 0x0B: // RTC set time
        break;
      case 0x0C: // RTC get time
        break;
      case 0x0D: // RTC set alarm
        break;
      case 0x00: // set soft timer
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x01: // change notification
        break;
      case 0x02: // external interrupt
        break;
      case 0x03: // RTC alarm
        break;
      case 0x00: // soft timer
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }    
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x04: // configure
        break;
      case 0x08: // DHCP set hostname
        break;
      case 0x05: // DNS configure
        break;
      case 0x06: // DNS get host by name
        break;
      case 0x01: // TCP connect
        break;
      case 0x00: // Start TCP server
        break;
      case 0x03: // UDP connect
        break;
      case 0x07: // UDP bind
        break;
      case 0x02: // start UDP server
        break; 
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x00: // configuration
        break;
      case 0x01: // configuration DNS
        break;
      case 0x03: // Get host by name result
        break;
      case 0x02: // endpoint status
        break;
      case 0x04: //UDP data
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }    
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x02: // connected
        break;
      case 0x00: // set data route
        break;
      case 0x01: // close
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x00: // link status
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }    
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x03: // PS save
        break;
      case 0x04: // PS load
        break;
      case 0x07: // PS dump
        break;
      case 0x00: // PS defrag
        break;
      case 0x05: // PS erase
        break;
      case 0x02: // PS erase all
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x01: // PS keyy changed
        break;
      case 0x00: // PS key
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }    
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x00: // enable
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x01: // button
        break;
      case 0x00: // on req
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }    
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
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x00: // reset
        break;
      case 0x01: // flash set address
        break;
      case 0x02: // flash upload
        break;
      case 0x03: // flash upload finished
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      case 0x00: // boot
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }    
  }

  return NO_ERROR;
}


/**
 * @brief      Execute i2c callback
 *
 * @param      header       The header
 * @param      payload      The payload
 * @param[in]  payloadSize  The payload size
 *
 * @return     The error code.
 */
ErrorCode Wf121Driver :: executeI2cCallback(BgApiHeader *header,
                                            uint8_t *payload,
                                            const uint16_t payloadSize){
  // Process command reply
  if(header->bit.msgType == CMD_RSP_TYPE){
    switch(header->bit.msgId){
      case 0x00: // start read
        break;
      case 0x01: // start write
        break;
      case 0x02: // stop
        break;
      default:
        return COMMAND_NOT_RECOGNIZED;
    }
  }

  // Process event reply
  if(header->bit.msgType == EVENT_TYPE){
    switch(header->bit.msgId){
      default:
        return COMMAND_NOT_RECOGNIZED;
    }    
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
  if(header.bit.msgType == CMD_RSP_TYPE){
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

