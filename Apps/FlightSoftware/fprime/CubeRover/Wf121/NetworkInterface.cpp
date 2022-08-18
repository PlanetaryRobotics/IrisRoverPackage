#include <CubeRover/Wf121/NetworkInterface.hpp>

namespace Wf121
{

    // Statically-allocated queue of all UDP payloads received from the Radio.
    // NOTE: This is *NOT* a circular buffer. Data is just dropped if it
    // fills (to ensure command order - i.e. so we don't drop Command A if
    // it was supposed to be recieved before Command B).
    // This is filled here, by `NetworkInterface`, and is designed to be
    // drained by the `NetworkManager` FPrime component.
    // NOTE: Static allocation here only works if there's only one instance of
    // NetworkInterface (which should be the case) - it belongs to RadioDriver,
    // which belongs to NetworkManager, which there should only be one of.
    // Size of UDP Payloads in `xUdpRxPayloadQueue`:
    static const size_t UDP_RX_PAYLOAD_QUEUE_ITEM_SIZE = sizeof( NetworkInterface::UdpRxPayload );
    // Number of UDP Payloads to keep in the `xUdpRxPayloadQueue` (note: each payload is 2+WF121_UDP_MAX_PAYLOAD bytes long).
    static const uint8_t UDP_RX_PAYLOAD_QUEUE_DEPTH = 3;
    // Static queue storage area:
    static uint8_t ucUdpRxPayloadQueueStorageArea[ UDP_RX_PAYLOAD_QUEUE_DEPTH * UDP_RX_PAYLOAD_QUEUE_ITEM_SIZE ];
    /* The variable used to hold the queue's data structure. */
    static StaticQueue_t xUdpRxPayloadStaticQueue;

    NetworkInterface::NetworkInterface() : m_protectedRadioStatus(),
                                           m_txCommsStatus(),
                                           BgApi::BgApiDriver(),
                                           DirectMessage::DirectMessageDriver(),
                                           xUdpRxWorkingData()

    {
        xUdpRxPayloadQueue = NULL; // for now (NULL until initialized)
        xUdpRxWorkingData.clear(); // Pre-fill working buffer with sentinel byte
    }

    NetworkInterface::init()
    {
        BgApi::BgApiDriver::init();
        DirectMessage::DirectMessageDriver::init();

        // Create and initialize RX Comms queue (do this before any RX comms
        // tasks start):
        xUdpRxPayloadQueue = xQueueCreateStatic(
                                /* The number of items the queue can hold. */
                                NetworkInterface::UDP_RX_PAYLOAD_QUEUE_DEPTH,
                                UDP_RX_PAYLOAD_QUEUE_ITEM_SIZE,
                                ucUdpRxPayloadQueueStorageArea,
                                &xUdpRxPayloadStaticQueue );
        /* Queue buffer was not NULL so xQueue should not be NULL. */
        configASSERT( xUdpRxPayloadQueue );

        // Init the time of last heartbeat received to now (we shouldn't expect
        // it to have arrived any earlier than now):
        m_protectedRadioStatus.updateLastHeartbeatTime();
    }


    /**
     * @brief Checks the UDP RX queue to see if there are any available UDP
     * payloads. If so, it copies that data into `pPayload`.
     * 
     * NOTE: UART reading happens asynchronously and places parsed payloads
     * into the UDP RX Queue. All this does is see if there are any unread
     * payloads.
     * 
     * NOTE: This RX queue only has a depth of `UDP_RX_PAYLOAD_QUEUE_DEPTH`
     * payloads. By design, `NetworkInterface` will drop payloads if the
     * Queue is full, so make sure this function is being called enough to
     * keep the Queue below `UDP_RX_PAYLOAD_QUEUE_DEPTH`.
     * 
     * @param pPayload 
     * @return Whether a payload was found.
     */
    bool NetworkInterface::getAvailableUdpPayload(NetworkInterface::UdpRxPayload* pPayload){
        if(xUdpRxPayloadQueue != NULL){
            // Grab a value from the Queue. Give up (assume nothing is
            // currently available) if nothing is available within 10 ticks.
            // **DON'T** increase this tick count to some large value,
            // it will halt everything. It should be safe for it to even be 0,
            // we're making it *slightly* non-zero here only as a precaution.
            // NOTE: This send procedure is a **COPY**.
            if(xQueueRecieve(xUdpRxPayloadQueue, (void *)pPayload, ( TickType_t ) 10) == pdPASS){
                // Got a payload from the queue!
                // Nothing special to do here, but this is where you'd do it.
                return true;
            } else {
                // Nothing was available (which is fine), just no new data yet.
            }
        }
        return false;
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
    BgApi::ErrorCode NetworkInterface::cb_EventUdpData(const BgApi::Endpoint endpoint,
                                                       const BgApi::IpAddress srcAddress,
                                                       const uint16_t srcPort,
                                                       uint8_t *data,
                                                       const BgApi::DataSize16 dataSize)
    {
        BgApi::Endpoint uplinkEndpoint;
        m_protectedRadioStatus.copyUplinkEndpointInto(&uplinkEndpoint);
        if (endpoint == uplinkEndpoint)
        {
            // Copy data into working buffer:
            this->xUdpRxWorkingData.dataSize = dataSize;
            memcpy(this->xUdpRxWorkingData.data, data, dataSize);
            
            // Acknowledge that we got the packet and bytes in the counters,
            // even if we couldn't push it to the queue:
            this->m_protectedRadioStatus.incUdpRxPacketCount();
            this->m_protectedRadioStatus.incUdpRxByteCount(dataSize);

            // Push data into inter-process UDP RX Comms Queue:
            // NOTE: This is *NOT* a circular buffer. Data is just dropped if it
            // fills (to ensure command order - i.e. so we don't drop Command A
            //if it was supposed to be recieved before Command B).
            // Earth will get command ACKs and we'll use these to determine if
            // a command wasn't recieved and we need to resend.
            if(xUdpRxPayloadQueue != NULL){
                // Push into queue. Drop if queue isn't available in 10 ticks.
                // (Queue being available means there's space for another item in it).
                // **DON'T** increase this tick count to some large value,
                // it will halt everything. It should be safe for it to be 0
                // even, we're making it *slightly* non-zero here only as a
                // precaution.
                // NOTE: This send procedure is a **COPY** (so we don't care about `xUdpRxWorkingData` after this).
                if(xQueueSend(xUdpRxPayloadQueue, (void *)&xUdpRxWorkingData, ( TickType_t ) 10) == pdPASS){
                    // ! TODO: send a packet ACK back over the radio.
                } else {
                    /* Queue was full and wouldn't accept new data.
                       As noted above, this is not great but it's fine. */
                };
            }
        }
    }
}