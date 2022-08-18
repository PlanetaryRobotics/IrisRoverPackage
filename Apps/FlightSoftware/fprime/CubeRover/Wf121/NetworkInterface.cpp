#include <CubeRover/Wf121/NetworkInterface.hpp>

namespace Wf121
{
    NetworkInterface::NetworkInterface() : m_protectedRadioStatus(),
                                           m_txCommsStatus(),
                                           BgApi::BgApiDriver(),
                                           DirectMessage::DirectMessageDriver()

    {
        // nothing else to do
    }

    NetworkInterface::init()
    {
        BgApi::BgApiDriver::init();
        DirectMessage::DirectMessageDriver::init();

        // Init the time of last heartbeat received to now (we shouldn't expect
        // it to have arrived any earlier than now):
        m_protectedRadioStatus.updateLastHeartbeatTime();
    }

    /**
     * @brief      Callback triggered when we receive a Direct Message
     * Heartbeat from the Radio (even if only part of that message was
     * valid and the rest was gibberish).
     *
     * @param downlinkEndpoint BGAPI Endpoint to send data to be downlinked to Earth.
     * @param uplinkEndpoint BGAPI Endpoint to use to receive data from Earth.
     * @param state State the Radio is currently in (or BAD_MESSAGE if the message was garbage by this point).
     * @param doing Activity the Radio is currently doing (or BAD_MESSAGE if the message was garbage by this point).
     * @param fullyValid Whether the entire heartbeat message was intelligible (valid).
     */
    void NetworkInterface::cb_dm_Heartbeat(
        const uint8_t downlinkEndpoint,
        const uint8_t uplinkEndpoint,
        const DirectMessage::RadioSwState state,
        const DirectMessage::RadioSwActivity doing,
        const bool fullyValid)
    {
        // Only set the endpoint if the target endpoints aren't NULL
        // OR the message was fullyValid (we know it actually *wants* us to set
        // the endpoints to NULL for now):
        if (fullyValid || downlinkEndpoint != DirectMessage::UDP_NULL_ENDPOINT)
        {
            m_protectedRadioStatus.setDownlinkEndpoint(downlinkEndpoint);
        }
        if (fullyValid || uplinkEndpoint != DirectMessage::UDP_NULL_ENDPOINT)
        {
            m_protectedRadioStatus.setDownlinkEndpoint(uplinkEndpoint);
        }

        // Set the state if we got a non-corrupted state:
        if (state != DirectMessage::RadioSwState::BAD_MESSAGE)
        {
            m_protectedRadioStatus.setRadioState(state);
        }

        // Set the activity if we got a non-corrupted activity:
        if (doing != DirectMessage::RadioSwActivity::BAD_MESSAGE)
        {
            m_protectedRadioStatus.setRadioActivity(doing);
        }

        // Only update the time if the message was fully valid:
        if (fullyValid)
        {
            m_protectedRadioStatus.updateLastHeartbeatTime();
        }
    }

    /**
     * @brief      Callback triggered when we determine we've received
     * valid information about the Radio's current RadioSwState, from a
     * state change Direct Message.
     *
     * @param state The state we're now in.
     */
    void NetworkInterface::cb_dm_NowInState(const DirectMessage::RadioSwState state)
    {
        // Set the current state (even if's BAD_MESSAGE - we want it to be
        // clear that the state changed and we no longer know what it is):
        m_protectedRadioStatus.setRadioState(state);
    }

    /**
     * @brief      Callback triggered when we determine we've received
     * valid information about the Radio's current RadioSwActivity, from a
     * Direct Message indicating we've just started the activity.
     *
     * @param doing The activity we're now doing.
     */
    void NetworkInterface::cb_dm_NowDoingActivity(const DirectMessage::RadioSwActivity doing)
    {
        // Set the current activity (even if's BAD_MESSAGE - we want it to be
        // clear that the activity changed and we no longer know what it is):
        m_protectedRadioStatus.setRadioActivity(doing);
    }

    BgApi::ErrorCode NetworkInterface::cb_CommandSetTransmitSize(const uint16_t result,
                                                                 const BgApi::Endpoint endpoint)
    {
        BgApi::Endpoint targetEndpoint;
        m_protectedRadioStatus.copyDownlinkEndpointInto(&targetEndpoint);
        if (result == BgApi::NO_ERROR && endpoint == targetEndpoint)
        {
            m_txCommsStatus.gotSetTxSizeAcknowledged();
        }
        return (BgApi::ErrorCode)result;
    }
    BgApi::ErrorCode NetworkInterface::cb_CommandSendEndpoint(const uint16_t result,
                                                              const BgApi::Endpoint endpoint)
    {
        BgApi::Endpoint targetEndpoint;
        m_protectedRadioStatus.copyDownlinkEndpointInto(&targetEndpoint);
        if (result == BgApi::NO_ERROR && endpoint == targetEndpoint)
        {
            m_txCommsStatus.incNumSendEndpointAcknowledgements();
        }
        return (BgApi::ErrorCode)result;
    }

    /* BGAPI EVENT CALLBACKS */
    BgApi::ErrorCode NetworkInterface::cb_EventSignalQuality(const int8_t rssi,
                                                             const BgApi::HardwareInterface hwInterface)
    {
        if (hwInterface == BgApi::WIFI)
        {
            m_protectedRadioStatus.setRssi(rssi)
        }
    }

    // Event for uplinked packets:
    Wf121::ErrorCode NetworkInterface::cb_EventUdpData(const Wf121::Endpoint endpoint,
                                                       const Wf121::IpAddress srcAddress,
                                                       const uint16_t srcPort,
                                                       uint8_t *data,
                                                       const Wf121::DataSize16 dataSize)
    {
        BgApi::Endpoint uplinkEndpoint;
        m_protectedRadioStatus.copyUplinkEndpointInto(&uplinkEndpoint);
        if (endpoint == uplinkEndpoint)
        {
            // ! TODO (WORKING-HERE): [CWC] Make RX buffer queue and use it for data passing:
        }
    }
}