// ======================================================================
// \title  NetworkManagerComponentImpl.hpp
// \author Connor Colombo (colombo@cmu.edu)
// \brief  hpp file for NetworkManager component implementation class.
//
// NOTE: This is pretty much just a pass-through class to `RadioDriver` now
// since actual network management activities are handled by the Radio's
// internal MCU itself. All this does is pass data into and pull data out of
// the `RadioDriver` and handle unresolvable Radio faults (mainly by resetting
// the Radio).
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef NetworkManager_HPP
#define NetworkManager_HPP

#include "CubeRover/NetworkManager/NetworkManagerComponentAc.hpp"
#include "CubeRover/Wf121/Wf121.hpp"

namespace CubeRover
{

  // STATICALLY allocate a SINGLE copy of the RadioDriver (both of these are
  // very important for proper set up of the internal Tasks):
  static const Wf121::RadioDriver CORE_RADIO_DRIVER;

  // Target (minimum) time between NetworkManager general telemetry updates,
  // in milliseconds:
  // (note, telemetry triggered by things like a state change may occur more frequently)
  static const uint32_t NETWORK_MANAGER_GENERAL_TELEM_UPDATE_INTERVAL_MS = 5000; // 5s

  // How long (in ms) we'll wait for a Radio heartbeat before assuming it needs
  // a reset (NOTE: Standard heartbeat interval is once every 10s):
  static const uint32_t RADIO_HEARTBEAT_TIMEOUT_MS = 31000; // 31s, a little over 3 HB periods

  // How many times we'll request the Radio to be reset before we realize the
  // problem might actually be us (Hercules) and ask WD to reset us:
  // NOTE: This is number of consecutive resets without the radio going back
  // into good state (connected).
  static const uint32_t RADIO_RESET_CONSECUTIVE_MAX_COUNT__RESET_HERCULES_THRESH = 5;

  // ID to pass to WatchDog ResetSpecific to reset the radio:
  static const uint8_t WD_RADIO_RESET_ID = Reset_Radio; // 0x04;
  // ID to pass to WatchDog ResetSpecific to reset the Hercules:
  static const uint8_t WD_HERCULES_RESET_ID = Reset_Hercules; // 0x01;
  // (^should be able to pull these straight from `reset_values_possible` enum)

  class NetworkManagerComponentImpl : public NetworkManagerComponentBase
  {

  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object NetworkManager
    //!
    NetworkManagerComponentImpl(
#if FW_OBJECT_NAMES == 1
        const char *const compName /*!< The component name*/
#else
        void
#endif
    );

    //! Initialize object NetworkManager
    //!
    void init(
        const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy object NetworkManager
    //!
    ~NetworkManagerComponentImpl(void);

    PRIVATE :

        // ----------------------------------------------------------------------
        // Handler implementations for user-defined typed input ports
        // ----------------------------------------------------------------------

        //! Handler implementation for fileDownlinkBufferSendIn
        //!
        void
        downlink_handler(
            const NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::Buffer &fwBuffer);

    //! Handler implementation for schedIn
    //!
    void schedIn_handler(
        const NATIVE_INT_TYPE portNum, /*!< The port number*/
        NATIVE_UINT_TYPE context       /*!< The call order*/
    );

    // User defined methods, members, and structs

    // Pointer to the RadioDriver being used (we add this level of indirection
    // and don't just use `CORE_RADIO_DRIVER` in case we want to change what's
    // used in the future):
    Wf121::RadioDriver *m_pRadioDriver;
    // Single common working buffer used for handling RX'd or TX'ing UDP Payloads:
    // (NOT for long term storage of data / passing data out of NetworkManager):
    Wf121::UdpPayload m_udpPayloadWorkingBuffer;

    // Latest WiFi state downlinked by NetworkManager:
    WIFIState m_lastDownlinkedWifiState;

    // Time of last general telemetry downlink, in ms since Hercules boot:
    uint32_t m_lastTelemDownlinkTimeMs;

    // How many times we've asked for the Radio to be reset:
    uint8_t m_radioConsecutiveResetRequestCounter;
    // State of packet counters the last time we ordered a reset
    // (used to determine if Radio is back in good state and we can reset
    // `m_radioConsecutiveResetRequestCounter`):
    uint32_t m_rxPacketCountOnLastReset;
    uint32_t m_txPacketCountOnLastReset;

    // Buffer for data uplinked through the Radio via UDP:
    Wf121::UdpRxPayload m_uplinkBuffer;

    void update(); // Behavior of periodic status update
    void getUplinkDatagram();

    // What to do when RadioDriver is says its in a bad state and needs help
    // (i.e. needs NetworkManager to reset something):
    void handleRadioFault();
  };

} // end namespace CubeRover

#endif
