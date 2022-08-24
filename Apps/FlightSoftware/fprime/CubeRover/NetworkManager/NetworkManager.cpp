// ======================================================================
// \title  NetworkManagerComponentImpl.hpp
// \author Connor Colombo (colombo@cmu.edu)
// \brief  cpp file for NetworkManager component implementation class.
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

#include <CubeRover/NetworkManager/NetworkManager.hpp>
#include <CubeRover/GroundInterface/GroundInterface.hpp>
#include <CubeRover/WatchDogInterface/WatchDogInterface.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <cstring>

extern CubeRover::WatchDogInterfaceComponentImpl watchDogInterface;

namespace CubeRover
{
    // Helper function to convert RadioSwState (used inside RadioDriver) to
    // WIFIState (used by FPrime telem).
    // See `RadioSwState` in `Wf121/Wf121DirectMessage.hpp` for more details on
    // each state:
    // TODO: [CWC] Update FPrime XML so this is 1-to-1 correspondence
    //  - Make sure to get networkmanager_state_from/to
    //  - (and add a RadioSwActivity) telem item while you're at it)
    WIFIState convertRadioState2WifiState(Wf121::DirectMessage::RadioSwState state)
    {
        switch (state)
        {
        case Wf121::DirectMessage::BOOT:
            return WIFIState::UNINITIALIZED;
            break;
        case Wf121::DirectMessage::INIT:
            return WIFIState::INITIALIZED;
            break;
        case Wf121::DirectMessage::WIFI_ON:
            return WIFIState::WIFI_ON;
            break;
        case Wf121::DirectMessage::CONNECTED:
            return WIFIState::CONNECTED;
            break;
        case Wf121::DirectMessage::UDP_CONNECTED:
            return WIFIState::UDP_CONNECTED;
            break;

        // Bad state (we don't know what's really going on inside the Radio rn):
        case Wf121::DirectMessage::NONE:
        case Wf121::DirectMessage::BAD_MESSAGE:
        default:
            return WIFIState::UNINITIALIZED;
        }
    }

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    NetworkManagerComponentImpl ::
#if FW_OBJECT_NAMES == 1
        NetworkManagerComponentImpl(
            const char *const compName) : NetworkManagerComponentBase(compName)
#else
        NetworkManagerComponentImpl(void)
#endif
    {
        m_pRadioDriver = &CORE_RADIO_DRIVER;
        // Initialize process variables:
        m_lastTelemDownlinkTimeMs = 0;
        m_radioConsecutiveResetRequestCounter = 0;
        m_rxPacketCountOnLastReset = 0;
        m_txPacketCountOnLastReset = 0;
        m_lastDownlinkedWifiState = convertRadioState2WifiState(Wf121::DirectMessage::RadioSwState::NONE);
    }

    void NetworkManagerComponentImpl ::
        init(const NATIVE_INT_TYPE instance)
    {
        // Init the base FPrime component:
        NetworkManagerComponentBase::init(instance);

        // Initialize the working buffer with known sentinel bytes:
        m_udpPayloadWorkingBuffer.clear();
        // Initialize the uplink buffer with known sentinel bytes:
        m_uplinkBuffer.clear();

        // Init the RadioDriver (and all its sub-tasks):
        m_pRadioDriver->init();
    }

    NetworkManagerComponentImpl ::
        ~NetworkManagerComponentImpl(void)
    {
        // This should NEVER get called
    }

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    void NetworkManagerComponentImpl ::
        downlink_handler(
            const NATIVE_INT_TYPE portNum,
            Fw::Buffer &fwBuffer)
    {
        // Grab the data from the FW Buffer:
        uint8_t *buffer = reinterpret_cast<uint8_t *>(fwBuffer.getdata());

        // Copy it into the outbound payload:
        m_udpPayloadWorkingBuffer.copyIn(fwBuffer.getsize(), buffer);

        // Queue up the Payload for downlink:
        m_pRadioDriver->m_networkInterface.sendUdpPayload(&m_udpPayloadWorkingBuffer);
    }

    void NetworkManagerComponentImpl ::
        schedIn_handler(
            const NATIVE_INT_TYPE portNum,
            NATIVE_UINT_TYPE context)
    {
        // Perform standard updates:
        update();
        // See if there's any available uplinked data:
        getUplinkDatagram();
    }

    void NetworkManagerComponentImpl::update()
    {
        // Flag to make sure all available telem is downlinked on the first call:
        static bool first_update_call = false;

        // Get the current time (right at the start of the update),
        // in ms since Hercules boot:
        uint32_t now = Wf121::Timestamp::getTimeMs();

        // See if we need to emit new telem because of a `RadioSwState` change:
        Wf121::DirectMessage::RadioSwState currentRadioState = ;
        WIFIState currentWifiState = convertRadioState2WifiState(
            m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getRadioState());
        if (first_update_call || m_lastDownlinkedWifiState != currentWifiState)
        {
            log_ACTIVITY_HI_StateChange(static_cast<networkmanager_state_from>(m_lastDownlinkedWifiState), static_cast<networkmanager_state_to>(currentWifiState));
            tlmWrite_WIFIStateStatus(currentWifiState);
            m_lastDownlinkedWifiState = currentWifiState;
        }

        // See if it's time for us to update general WiFi telemetry:
        if ((now - m_lastTelemDownlinkTimeMs) >= NETWORK_MANAGER_GENERAL_TELEM_UPDATE_INTERVAL_MS)
        {
            m_lastTelemDownlinkTimeMs = now;
            tlmWrite_RSSI(m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getRssi());
            // Note: SSR is defunct (never really something we had live access to)
            tlmWrite_PktRecv(m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getUdpRxPacketCount());
            tlmWrite_PktSent(m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getUdpTxPacketCount());

            // Repeat the state now (in case it was lost before):
            tlmWrite_WIFIStateStatus(m_lastDownlinkedWifiState);
        }

        // Check if Radio has gotten back into a good state since the last reset
        // request, meaning we can reset the counter since any failure wouldn't
        // be consecutive with the last.
        // GOOD STATE is defined as:
        //      - UDP_CONNECTED
        //      - RX packet count has increased since reset (we can receive uplink data via Radio)
        //      - TX packet count has increased since reset (we can send downlink data through the Radio)
        if (
            m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getRadioState() == Wf121::DirectMessage::RadioSwState::UDP_CONNECTED &&
            m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getUdpRxPacketCount() > m_rxPacketCountOnLastReset &&
            m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getUdpTxPacketCount() > m_txPacketCountOnLastReset)
        {
            m_radioConsecutiveResetRequestCounter = 0;
        }

        uint32_t lastHeartbeatTimeMs = m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getLastHeartbeatTimeMs();
        // update `now` **AFTER** grabbing the heartbeat time so there's no chance the Heartbeat comes in between
        // getting `now` and checking the delta (i.e. so there's no chance heartbeatTime > now unless there's been an
        // overflow, which is okay):
        now = Wf121::Timestamp::getTimeMs();
        if (m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.needHelpCriticalCommsFailureOccurred() ||
            (now - lastHeartbeatTimeMs) > RADIO_HEARTBEAT_TIMEOUT_MS)
        {
            handleRadioFault();
            // Reset the critical flag (so we don't trigger this again on the next loop unless there's been another fault):
            m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.setCriticalCommsFailure_NeedHelp(false);
        }

        if (first_update_call)
        {
            first_update_call = false;
        }
    }

    // What to do when RadioDriver is says its in a bad state and needs help
    // (i.e. needs NetworkManager to reset something):
    // NOTE: This only really triggers if the Radio:
    //      A.) appears to be dead (isn't giving us heartbeats)
    //      B.) appears to be alive and connected (giving us heartbeats that
    //          say it's connected) BUT isn't responding to us sending it
    //          commands (i.e. Radio isn't receiving inbound UART comms).
    void NetworkManagerComponentImpl::handleRadioFault()
    {
        if (m_radioConsecutiveResetRequestCounter <= RADIO_RESET_CONSECUTIVE_MAX_COUNT__RESET_HERCULES_THRESH)
        {
            // Reset the Radio:
            watchDogInterface.Reset_Specific_Handler(WD_RADIO_RESET_ID);
            // TODO: [CWC] This (^) is how resets were done before but shouldn't we be using `WatchdogResetRequest` for this? Investigate.
            m_radioConsecutiveResetRequestCounter++;

            // Log the current packet counts (so we can see if the Radio gets back into a good state after the reset with bidirectional comms working):
            m_rxPacketCountOnLastReset = m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getUdpRxPacketCount();
            m_txPacketCountOnLastReset = m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getUdpTxPacketCount();

            // Reset the Radio State data (since we expect the Radio is resetting):
            // Set the RadioSwState back to NONE (i.e. we don't know it anymore until the Radio wakes back up and gives us another Heartbeat):
            m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.setRadioState(Wf121::DirectMessage::RadioSwState::NONE);
            // Set last Heartbeat time to right now (so we don't automatically trigger another reset on the next `update` call):
            m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.updateLastHeartbeatTime();
        }
        else
        {
            // We've asked for the Radio to be reset too many times without
            // good data from the Radio. Maybe we're the problem?
            watchDogInterface.Reset_Specific_Handler(WD_HERCULES_RESET_ID);
            // since we aren't asking for anything to happen to the radio, we
            // don't need to update any of the status info like we do above
            // when resetting the Radio.
        }
    }

    void NetworkManagerComponentImpl::getUplinkDatagram()
    {
        if (m_pRadioDriver->m_networkInterface.getAvailableUdpPayload(m_uplinkBuffer))
        {
            // An RX payload was available and we got it:
            Fw::Buffer buffer(0, 0, reinterpret_cast<U64>(m_uplinkBuffer.data), m_uplinkBuffer.dataSize);
            uplink_out(0, buffer);
        }
    }

} // end namespace CubeRover
