#include <CubeRover/Wf121/NetworkInterface.hpp>

#include <CubeRover/WatchDogInterface/WatchDogInterface.hpp>
extern CubeRover::WatchDogInterfaceComponentImpl watchDogInterface;

// Anonymous namespace for file-scope helper functions:
namespace
{
    // Returns the length of the given fixed buffer containing a
    // null-terminated string:
    uint8_t getStrBufferLen(const uint8_t *buf)
    {
        return strlen((const char *)buf);
    }
}

namespace Wf121
{
    // Statically-allocated queue of all UDP payloads received from the Radio.
    // NOTE: This is *NOT* a circular buffer. Data is just dropped if it
    // fills (to ensure command order - i.e. so we don't drop Command A if
    // it was supposed to be received before Command B).
    // This is filled here, by `NetworkInterface`, and is designed to be
    // drained by the `NetworkManager` FPrime component.
    // NOTE: Static allocation here only works if there's only one instance of
    // NetworkInterface (which should be the case) - it belongs to RadioDriver,
    // which belongs to NetworkManager, which there should only be one of.
    // Size of UDP Payloads in `m_xUdpRxPayloadQueue`:
    static const size_t UDP_RX_PAYLOAD_QUEUE_ITEM_SIZE = sizeof(UdpRxPayload);
    // Number of UDP Payloads to keep in the `m_xUdpRxPayloadQueue` (note: each payload is 2+WF121_UDP_MAX_PAYLOAD bytes long).
    static const uint8_t UDP_RX_PAYLOAD_QUEUE_DEPTH = 3;
    // Static queue storage area:
    static uint8_t ucUdpRxPayloadQueueStorageArea[UDP_RX_PAYLOAD_QUEUE_DEPTH * UDP_RX_PAYLOAD_QUEUE_ITEM_SIZE];
    /* The variable used to hold the queue's data structure. */
    static StaticQueue_t xUdpRxPayloadStaticQueue;
    //
    // Same data but for `m_xUdpTxPayloadQueue`:
    static const size_t UDP_TX_PAYLOAD_QUEUE_ITEM_SIZE = sizeof(UdpTxPayload);
    // Number of UDP Payloads to keep in the `m_xUdpTxPayloadQueue` (note: each payload is 2+WF121_UDP_MAX_PAYLOAD bytes long).
    static const uint8_t UDP_TX_PAYLOAD_QUEUE_DEPTH = 5;
    // Static queue storage area:
    static uint8_t ucUdpTxPayloadQueueStorageArea[UDP_TX_PAYLOAD_QUEUE_DEPTH * UDP_TX_PAYLOAD_QUEUE_ITEM_SIZE];
    /* The variable used to hold the queue's data structure. */
    static StaticQueue_t xUdpTxPayloadStaticQueue;

    NetworkInterface::NetworkInterface() : m_protectedRadioStatus(),
                                           m_udpTxCommsStatusManager(),
                                           BgApi::BgApiDriver(),
                                           DirectMessage::DirectMessageDriver(),
                                           m_xUdpRxWorkingData(),
                                           m_xUdpTxWorkingData(),
                                           m_bgApiCommandBuffer(),
                                           m_xUdpTxRecycling()
    {
        // Set up read buffer:
        m_xUdpRxPayloadQueue = NULL; // for now (NULL until initialized)
        m_xUdpRxWorkingData.clear(); // Pre-fill working buffer with sentinel byte

        // Set up write buffer:
        m_xUdpTxPayloadQueue = NULL; // for now (NULL until initialized)
        m_xUdpTxWorkingData.clear(); // Pre-fill working buffer with sentinel byte
        m_xUdpTxRecycling.clear();   // Pre-fill recycling with sentinel byte
    }

    // Dtor to satisfy polymorphism (this class should never actually be deleted):
    NetworkInterface::~NetworkInterface()
    {
        // don't use
    }

    void NetworkInterface::init()
    {
        // Call parent class initializers (set everything up for them first):
        BgApi::BgApiDriver::init();
        DirectMessage::DirectMessageDriver::init();

        // Create and initialize RX Comms queue (do this before any RX comms
        // tasks start):
        m_xUdpRxPayloadQueue = xQueueCreateStatic(
            /* The number of items the queue can hold. */
            UDP_RX_PAYLOAD_QUEUE_DEPTH,
            UDP_RX_PAYLOAD_QUEUE_ITEM_SIZE,
            ucUdpRxPayloadQueueStorageArea,
            &xUdpRxPayloadStaticQueue);
        /* Queue buffer was not NULL so xQueue should not be NULL. */
        configASSERT(m_xUdpRxPayloadQueue);

        // Init TX comms status manager (its internal queues, semaphores, etc):
        m_udpTxCommsStatusManager.init();

        // Create and initialize TX Comms queue (do this before any RX comms
        // tasks start):
        m_xUdpTxPayloadQueue = xQueueCreateStatic(
            /* The number of items the queue can hold. */
            UDP_TX_PAYLOAD_QUEUE_DEPTH,
            UDP_TX_PAYLOAD_QUEUE_ITEM_SIZE,
            ucUdpTxPayloadQueueStorageArea,
            &xUdpTxPayloadStaticQueue);
        /* Queue buffer was not NULL so xQueue should not be NULL. */
        configASSERT(m_xUdpTxPayloadQueue);

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
     * @param pPayload Pointer to payload to load the data into.
     * @param blockingTicks How many FreeRTOS scheduler ticks to block the
     *      active task for while waiting for data to be available.
     *      Default is 0, which means it returns immediately, whether or not
     *      data was available. Since UDP RX is Queued, this behavior is
     *      desirable b/c it lets us reap all the benefits of using a Queue to
     *      not have to wait for things in the first place.
     *
     * @return Whether a payload was found.
     */
    bool NetworkInterface::getAvailableUdpPayload(UdpRxPayload *pPayload, TickType_t blockingTicks)
    {
        if (m_xUdpRxPayloadQueue != NULL)
        {
            // Grab a value from the Queue. Give up (assume nothing is
            // currently available) if nothing is available within 3 ticks.
            // **DON'T** increase this tick count to some large value,
            // it will halt everything. It should be safe for it to even be 0,
            // we're making it *slightly* non-zero here only as a precaution.
            // NOTE: This send procedure is a **COPY**.
            // NOTE: At FreeRTOS 1000Hz tick rate, each tick is 1ms.
            if (xQueueReceive(m_xUdpRxPayloadQueue, (void *)pPayload, blockingTicks) == pdPASS)
            {
                // Got a payload from the queue!
                // Nothing special to do here, but this is where you'd do it.
                return true;
            }
            else
            {
                // Nothing was available (which is fine), just no new data yet.
                return false;
            }
        }
        return false;
    }

    /**
     * @brief Add the given payload to the UDP TX Queue.
     *
     * NOTE: UART writing happens asynchronously and this Queue will be
     * drained by `Wf121TxTask`.
     *
     * NOTE: This TX queue only has a depth of `UDP_TX_PAYLOAD_QUEUE_DEPTH`
     * payloads. If this queue is full when attempting to add an item, it
     * will drop the oldest payload to make room for a new payload.
     *
     * @param pPayload
     * @return Whether the payload was successfully added to the queue.
     */
    bool NetworkInterface::sendUdpPayload(UdpTxPayload *pPayload)
    {
        bool enqueuedSuccessfully = false;
        if (m_xUdpTxPayloadQueue != NULL) // Make sure the Queue has been init'd
        {
            if (uxQueueSpacesAvailable(m_xUdpTxPayloadQueue) > 0)
            {
                // Push into queue. Drop if queue isn't available in 10 ticks.
                // **DON'T** increase this tick count to some large value,
                // it will halt everything. It should be safe for it to be 0
                // even, we're making it *slightly* non-zero here only as a
                // precaution.
                // NOTE: This send procedure is a **COPY** (so we don't care about
                // `pPayload` after this).
                enqueuedSuccessfully =
                    (xQueueSend(m_xUdpTxPayloadQueue, (void *)pPayload, WF121_UDP_TX_ENQUEUE_WAIT_TICKS) == pdPASS);
            }
            else
            {
                // If no space available in queue, "receive" an item from it to
                // dequeue the oldest item.
                // NOTE: It's possible that between the space available check
                // and this receive operation, some or all of the items could
                // have been processed by the receiving task (i.e. by now the
                // queue could be empty, though unlikely).
                // **So**, to be safe and prevent this receive operation from
                // looping infinitely on an empty queue, only receive an item
                // if it's immediately available (which it should be if the
                // list is still actually full).
                //
                // See notes at `m_xUdpTxRecycling` definition for more thoughts
                // on this method and ways to improve it (or if it really even
                // needs to be improved).
                xQueueReceive(m_xUdpTxPayloadQueue, (void *)&m_xUdpTxRecycling, (TickType_t)0);
                // Now, as above, push the item to the queue, allowing it to
                // wait a small number of ticks if needed just to be safe:
                enqueuedSuccessfully =
                    (xQueueSend(m_xUdpTxPayloadQueue, (void *)pPayload, WF121_UDP_TX_ENQUEUE_WAIT_TICKS) == pdPASS);
            }
        }
        return enqueuedSuccessfully;
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
            m_protectedRadioStatus.setUplinkEndpoint(uplinkEndpoint);
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
            m_protectedRadioStatus.incNumCompleteDirectMessages();
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
        m_protectedRadioStatus.incNumCompleteDirectMessages();
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
        m_protectedRadioStatus.incNumCompleteDirectMessages();
    }

    /**
     * @brief      Callback triggered when we determine we've received
     * valid information about the Radio's UDP interlock
     * RadioUdpInterlockStatus, from an "ilock" Direct Message.
     *
     * @param status Current interlock status
     */
    void NetworkInterface::cb_dm_InterlockUpdate(const DirectMessage::RadioUdpInterlockStatus status)
    {
        // ! TODO: (WORKING-HERE) [CWC]
        // Pass into HERC_HAS_INTERLOCK to a UdpTxCommsStatusManager mailbox (make it awaitable).
        // Pass RADIO_HAS_INTERLOCK into active mailbox as a new ErrorCode.
        // If we're currently awaiting something and we lost the interlock, send a DEBUG message to WD->GND.
        // Finally add set (MAC) and release (RSSI) command encapsulations.
    }

    BgApi::ErrorCode NetworkInterface::cb_CommandSetTransmitSize(const uint16_t result,
                                                                 const BgApi::Endpoint endpoint)
    {
        // NOTE: This callback is triggered when ANYBODY (us or Radio's
        // internal BGScript) commands setTransmitSize for the downlink endpoint,
        // so it's not necessarily in response to us sending a `setTransmitSize`.

        // Let the manager know the result (do this no matter what the endpoint
        // was in case the current `m_protectedRadioStatus.getDownlinkEndpoint`
        // has changed since we sent this command):
        m_udpTxCommsStatusManager.setTransmitSize_Response((BgApi::ErrorCode)result);

        return (BgApi::ErrorCode)result;
    }

    BgApi::ErrorCode NetworkInterface::cb_CommandSendEndpoint(const uint16_t result,
                                                              const BgApi::Endpoint endpoint)
    {
        // NOTE: This callback is triggered when ANYBODY (us or Radio's
        // internal BGScript) commands sendEndpoint for the downlink endpoint,
        // so it's not necessarily in response to us sending a `sendEndpoint`.

        // Let the manager know the result (do this no matter what the endpoint
        // was in case the current `m_protectedRadioStatus.getDownlinkEndpoint`
        // has changed since we sent this command):
        m_udpTxCommsStatusManager.sendEndpointUdp_Response((BgApi::ErrorCode)result);

        return (BgApi::ErrorCode)result;
    }

    /* BGAPI EVENT CALLBACKS */
    BgApi::ErrorCode NetworkInterface::cb_EventSignalQuality(const int8_t rssi,
                                                             const BgApi::HardwareInterface hwInterface)
    {
        if (hwInterface == BgApi::WIFI)
        {
            m_protectedRadioStatus.setRssi(rssi);
        }
        return BgApi::ErrorCode::NO_ERROR;
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
            // Acknowledge that we got the packet and bytes in the counters,
            // even if we couldn't (or didn't need to) push it to the UDP RX
            // Queue:
            this->m_protectedRadioStatus.incUdpRxPacketCount();
            this->m_protectedRadioStatus.incUdpRxByteCount(dataSize);

            // Check if this data is Ground telling us that we need to clear
            // the buffers ASAP:
            // (Ground-Direct command to fix broken comms buffers that are
            // preventing us from getting data through)
            // See comment in `GND_DIRECT_CMD_RESET_ALL_BUFFERS` for more
            // details.
            if (dataSize == getStrBufferLen(GND_DIRECT_CMD_RESET_ALL_BUFFERS) &&
                memcmp(data, GND_DIRECT_CMD_RESET_ALL_BUFFERS, getStrBufferLen(GND_DIRECT_CMD_RESET_ALL_BUFFERS)) == 0)
            {
                // Do the reset(s):
                uint8_t resetCount = 0; // number of resets performed in response to the command
                if (m_xUdpRxPayloadQueue != NULL)
                {
                    xQueueReset(m_xUdpRxPayloadQueue);
                    resetCount++;
                }
                if (m_xUdpTxPayloadQueue != NULL)
                {
                    xQueueReset(m_xUdpTxPayloadQueue);
                    resetCount++;
                }

                // Push a response into the downlink queue:
                // (since we no longer care about `m_xUdpRxWorkingData` at this
                // point - we're done with it- we can just abuse its storage to
                // craft our response message):
                UdpTxPayload *pResponsePayload = static_cast<UdpTxPayload *>(&m_xUdpRxWorkingData);
                memcpy(pResponsePayload->data, GND_DIRECT_RSP_RESET_ALL_BUFFERS, getStrBufferLen(GND_DIRECT_RSP_RESET_ALL_BUFFERS));
                pResponsePayload->dataSize = getStrBufferLen(GND_DIRECT_RSP_RESET_ALL_BUFFERS);
                // Replace the last (placeholder) byte with the reset count:
                pResponsePayload->data[pResponsePayload->dataSize - 1] = resetCount;
                // Push into UDP TX queue:
                sendUdpPayload(pResponsePayload);

                // Return because there's nothing left for us to do here since
                // we wound up in this special case:
                return BgApi::ErrorCode::NO_ERROR;
            }

            // If we're here, we know the data is not an emergency
            // Ground-Command and we can handle it normally...

            // Copy data into working buffer:
            this->m_xUdpRxWorkingData.dataSize = dataSize;
            memcpy(this->m_xUdpRxWorkingData.data, data, dataSize);

            // Push data into inter-process UDP RX Comms Queue:
            // NOTE: This is *NOT* a circular buffer. Data is just dropped if it
            // fills (to ensure command order - i.e. so we don't drop Command A
            // if it was supposed to be received before Command B).
            // Earth will get command ACKs and we'll use these to determine if
            // a command wasn't received and we need to resend.
            if (m_xUdpRxPayloadQueue != NULL)
            {
                // Push into queue. Drop if queue isn't available in 10 ticks.
                // (Queue being available means there's space for another item in it).
                // **DON'T** increase this tick count to some large value,
                // it will halt everything. It should be safe for it to be 0
                // even, we're making it *slightly* non-zero here only as a
                // precaution.
                // NOTE: This send procedure is a **COPY** (so we don't care about `m_xUdpRxWorkingData` after this).
                if (xQueueSend(m_xUdpRxPayloadQueue, (void *)&m_xUdpRxWorkingData, WF121_UDP_RX_ENQUEUE_WAIT_TICKS) == pdPASS)
                {
                    // Send a packet ACK back over the radio:

                    // Generate the response message:
                    HerculesRadioUplinkResponse uplinkResponse(dataSize);
                    // UDP TX queue needs a UdpTxPayload to copy from. Since
                    // we've now sent `m_xUdpRxWorkingData` into the UDP RX
                    // queue, no one cares what's in it anymore. So, we can
                    // save memory by abusing it to buffer our uplinkResponse
                    // data into it and then morph it into a UdpTxPayload.
                    // Note: this will break if UdpTxPayload and UdpRxPayload
                    // become anything but typedefs for UdpPayload (and the
                    // compiler should catch that).
                    UdpTxPayload *pResponsePayload = static_cast<UdpTxPayload *>(&m_xUdpRxWorkingData);
                    memcpy(pResponsePayload->data, uplinkResponse.rawData, sizeof(uplinkResponse.rawData));
                    pResponsePayload->dataSize = sizeof(uplinkResponse.rawData);
                    // DEBUG (TODO: [CWC] REMOVEME): Tell WD->GSW what we got.
                    static const uint8_t debugDownlinkPrefix[] = "RADIO: UPL: ";
                    watchDogInterface.debugPrintfBufferWithPrefix((uint8_t *)debugDownlinkPrefix, getStrBufferLen(debugDownlinkPrefix), pResponsePayload->data, pResponsePayload->dataSize);
                    // Push into UDP TX queue:
                    sendUdpPayload(pResponsePayload);
                }
                else
                {
                    /* Queue was full and wouldn't accept new data.
                       As noted above, this is not great but it's fine, we just
                       drop the packet. Likely the system health is very bad
                       right now, so we need to just let it slide.
                       BUT this might mean we miss a critical command.
                       If ground thinks this is a problem, they can send us an
                       emergency `GND_DIRECT_CMD_RESET_ALL_BUFFERS` command.
                    */

                    // Let ground know this is the case (urgently) by pushing
                    // an alert to the front of the UDP TX queue:

                    // Since we no longer care about `m_xUdpRxWorkingData` at
                    // this point - we're done with it- we can just abuse its
                    // storage to craft our response message):
                    UdpTxPayload *pResponsePayload = static_cast<UdpTxPayload *>(&m_xUdpRxWorkingData);
                    memcpy(pResponsePayload->data, CRITICAL_MSG__HERC_RX_BUFFER_FULL, getStrBufferLen(CRITICAL_MSG__HERC_RX_BUFFER_FULL));
                    pResponsePayload->dataSize = getStrBufferLen(CRITICAL_MSG__HERC_RX_BUFFER_FULL);

                    // This is urgent. Push to the **FRONT** of the UDP TX
                    // Queue, overwriting anything that's there:
                    if (m_xUdpTxPayloadQueue != NULL)
                    {
                        if (xQueueSendToFront(m_xUdpTxPayloadQueue, (void *)pResponsePayload, WF121_UDP_TX_ENQUEUE_WAIT_TICKS) != pdPASS)
                        {
                            // We failed because `errQUEUE_FULL`.
                            // It's **really** important for Ground to get this message.
                            // We already asked nicely, so just nuke the UDP TX Queue and try again:
                            xQueueReset(m_xUdpTxPayloadQueue);
                            xQueueSendToFront(m_xUdpTxPayloadQueue, (void *)pResponsePayload, (TickType_t)0);
                        }
                    }
                } // xQueueSend: m_xUdpRxPayloadQueue

            } // m_xUdpRxPayloadQueue exists?
        }     // correct endpoint?

        return BgApi::ErrorCode::NO_ERROR;
    }

    /** Handle the Radio saying we gave it bad data (since we're the only BGAPI endpoint on the
     * Radio, if it's saying it got bad data, it had to have come from us). */
    BgApi::ErrorCode NetworkInterface::cb_EventEndpointSyntaxError(const uint16_t result,
                                                                   const BgApi::Endpoint endpoint)
    {
        if (result != BgApi::ErrorCode::NO_ERROR)
        {
            // BGAPI won't be processing our message, so we should stop waiting
            // for it to do so.
            m_bgApiStatus.setProcessingCmd(false);
        }

        // If we're currently awaiting a particular command response but instead
        // got this, push a `INTERNAL__BAD_SYNTAX` ErrorCode to the appropriate
        // mailbox queue in the TX manager:
        // NOTE: Do this no matter what the `endpoint` is because:
        //     A. The target `m_protectedRadioStatus.getDownlinkEndpoint` could
        //        have changed since we send the command we're waiting on a
        //        response for.
        //     B. This only comes if the packet we sent got garbled (or was
        //        otherwise incomplete in the Radio's eyes); so, it's possible
        //        the endpoint byte we sent could have been one of the bytes
        //        corrupted or lost or misaligned, meaning the endpoint we get
        //        in this callback won't necessarily correspond to the endpoint
        //        in our output.
        m_udpTxCommsStatusManager.setResponseForCurrentlyAwaitedCommand(BgApi::ErrorCode::INTERNAL__BAD_SYNTAX);

        // Let ground know the Radio thinks we sent it gibberish:
        // For debugging. TODO: [CWC] REMOVEME
        watchDogInterface.debugPrintfToWatchdog("RADIO: Bad syntax. Code: %#04x", result);

        return (BgApi::ErrorCode)result;
    }

    // Helper function to handle the `WAIT_FOR_BGAPI_READY` `UdpTxUpdateState`
    // state in `udpTxUpdateHandler`:
    // Sets the yieldData flag if it needs the State Machine to pause and yield
    // data to the `Wf121UdpTxTask`.
    // Returns the next state to transition to.
    NetworkInterface::UdpTxUpdateState NetworkInterface::handleTxState_WAIT_FOR_BGAPI_READY(bool *yieldData)
    {
        // Poll to make sure BGAPI is done processing the last command
        // and it's okay to another BGAPI command:
        //
        // NOTE: We only do this before looking for `WAIT_FOR_NEXT_MESSAGE`
        // because all the other states that trigger BGAPI commands are blocked
        // by checking for a particular command response (which necessarily
        // means that WF121's BGAPI is ready for another command).
        while (m_bgApiStatus.isProcessingCmd())
        {
            // NOTE: `isProcessingCmd` includes a timeout check
            // of `BGAPI_CMD_PROCESSING_TIMEOUT_MS` from the time processingCmd
            // processing was last set to true (so this loop isn't infinite).
            vTaskDelay(WF121_DOWNLINK_READY_TO_SEND_POLLING_CHECK_INTERVAL);
        }

        return UdpTxUpdateState::WAIT_FOR_NEXT_MESSAGE;
    }

    // Helper function to handle the `WAIT_FOR_NEXT_MESSAGE` `UdpTxUpdateState`
    // state in `udpTxUpdateHandler`:
    // Sets the yieldData flag if it needs the State Machine to pause and yield
    // data to the `Wf121UdpTxTask`.
    // Returns the next state to transition to.
    NetworkInterface::UdpTxUpdateState NetworkInterface::handleTxState_WAIT_FOR_NEXT_MESSAGE(bool *yieldData)
    {
        static bool first_downlink = false;
        // Grab data to write:
        // (if this is the first call upon connection, send a preformatted
        // "Hello" packet. Otherwise grab from queue.)
        if (first_downlink)
        {
            // if this our first opportunity to downlink data to Earth, send a
            // preformatted "Hello" data packet to let Ground know that Herc and
            // Radio are talking (might help with debugging and let's Earth know
            // instantly once all pieces are up).
            uint8_t msg_len = getStrBufferLen(HELLO_EARTH_MESSAGE);
            memcpy(m_xUdpTxWorkingData.data, HELLO_EARTH_MESSAGE, msg_len);
            m_xUdpTxWorkingData.dataSize = msg_len;
            first_downlink = false;
        }
        else
        {
            // Not our first downlink. Just a normal send. Block Task until new
            // data shows up in the UDP TX Queue. This can be forever since we don't
            // need to do anything until there's available data:
            while (m_xUdpTxPayloadQueue == NULL)
            {
                // Make sure Queue is initialized before trying to receive on it.
                // Should be by this point but, if we're here, clearly something fucked up.
                // We want this to be a tight loop that halts everything so that,
                // if this isn't resolved quickly (i.e. if it wasn't a temporary blip),
                // WD resets us.
            }
            // Wait to be told there's new data to downlink:
            while (xQueueReceive(this->m_xUdpTxPayloadQueue, &m_xUdpTxWorkingData, portMAX_DELAY) != pdPASS)
            {
                // No data was received but awaiting data timed out (after a **really** long time)
                // This shouldn't ever happen unless someone set `INCLUDE_vTaskSuspend` to `0`.
                // If that is the case, just go back to listening.
            }
            // If we're here, m_xUdpTxPayloadQueue now contains new data.
        }

        return UdpTxUpdateState::START_SENDING_MESSAGE;
    }

    // Helper function to handle the `START_SENDING_MESSAGE` `UdpTxUpdateState`
    // state in `udpTxUpdateHandler`:
    // Sets the yieldData flag if it needs the State Machine to pause and yield
    // data to the `Wf121UdpTxTask`.
    // Returns the next state to transition to.
    NetworkInterface::UdpTxUpdateState NetworkInterface::handleTxState_START_SENDING_MESSAGE(bool *yieldData)
    {
        // Perform any setup:

        // We're about to send a new UDP packet so, reset the status manager
        // (clear all the response semaphores/mailboxes):
        m_udpTxCommsStatusManager.reset();

        // Set the target endpoint for the message downlink (grabbed from
        // `m_protectedRadioStatus.getDownlinkEndpoint()` once per downlink so
        /// it doesn't change while we're sending chunks).
        m_downlinkTargetEndpoint = m_protectedRadioStatus.getDownlinkEndpoint();

        // First step in sending will be set up the transmit size:
        return UdpTxUpdateState::SEND_SET_TRANSMIT_SIZE;
    }

    // Helper function to handle the `SEND_SET_TRANSMIT_SIZE` `UdpTxUpdateState`
    // state in `udpTxUpdateHandler`:
    // Sets the yieldData flag if it needs the State Machine to pause and yield
    // data to the `Wf121UdpTxTask`.
    // Returns the next state to transition to.
    NetworkInterface::UdpTxUpdateState NetworkInterface::handleTxState_SEND_SET_TRANSMIT_SIZE(bool *yieldData)
    {
        // Pack the data for setting the transmit size (size of `m_xUdpTxWorkingData`):
        SetTransmitSize(&m_bgApiCommandBuffer,
                        m_downlinkTargetEndpoint,
                        m_xUdpTxWorkingData.dataSize);
        *yieldData = true; // tell State Machine to send this data

        // Next state will be waiting for a response (after sending data):
        return UdpTxUpdateState::WAIT_FOR_SET_TRANSMIT_SIZE_ACK;
    }

    // Helper function to handle the `WAIT_FOR_SET_TRANSMIT_SIZE_ACK`
    // `UdpTxUpdateState` state in `udpTxUpdateHandler`:
    // Sets the yieldData flag if it needs the State Machine to pause and yield
    // data to the `Wf121UdpTxTask`.
    // Returns the next state to transition to.
    NetworkInterface::UdpTxUpdateState NetworkInterface::handleTxState_WAIT_FOR_SET_TRANSMIT_SIZE_ACK(bool *yieldData)
    {
        UdpTxUpdateState nextState;

        // Wait for a response (or timeout):
        // NOTE: This will clear the mailbox first (see function definition for
        // why). So, if somehow the Radio responded (and we processed the
        // response) basically instantly after us sending the data, then this
        // will block until `UDP_TX_RESPONSE_TIMEOUT_TICKS`.
        BgApi::ErrorCode errorCode = m_udpTxCommsStatusManager.awaitResponse_setTransmitSize();
        switch (errorCode)
        {
        case BgApi::ErrorCode::NO_ERROR:
            // Cool, we can move on.
            nextState = UdpTxUpdateState::SEND_UDP_CHUNK;
            // Reset fail counter:
            m_bgApiCommandFailCount = 0;
            // Since we're necessarily starting a new send, reset the byte counter:
            m_totalUdpMessageBytesDownlinked = 0;
            break;

        case BgApi::ErrorCode::INTERNAL__BAD_SYNTAX:
            // INTERNAL__BAD_SYNTAX isn't a real BGAPI code, but is instead put
            // into the mailbox in response to an `evt_endpoint_syntax_error`
            // being emitted by the Radio while we're awaiting a response to
            // this command (likely means our command got garbled).
            // So, just try sending it again:
        case BgApi::ErrorCode::INTERNAL__TRY_AGAIN:
            // INTERNAL__TRY_AGAIN isn't a real BGAPI code, it's just part of
            // the interpreter and means something didn't work correctly (or
            // wasn't ready). So, try again:
        case BgApi::ErrorCode::TIMEOUT:
            // Didn't get a response in a long time. Maybe Radio didn't get it?
        default:
            // Some other error occurred, try again:
            m_bgApiCommandFailCount++;
            if (m_bgApiCommandFailCount > WF121_BGAPI_COMMAND_MAX_TRIES)
            {
                // We've sent this again too many times. Go to bad case:
                nextState = UdpTxUpdateState::BGAPI_CMD_FAIL;
            }
            else
            {
                // Send again:
                nextState = UdpTxUpdateState::SEND_SET_TRANSMIT_SIZE;
            }
            break;
        }

        return nextState;
    }

    // Helper function to handle the `SEND_UDP_CHUNK` `UdpTxUpdateState`
    // state in `udpTxUpdateHandler`:
    // Sets the yieldData flag if it needs the State Machine to pause and yield
    // data to the `Wf121UdpTxTask`.
    // Returns the next state to transition to.
    NetworkInterface::UdpTxUpdateState NetworkInterface::handleTxState_SEND_UDP_CHUNK(bool *yieldData)
    {
        uint16_t bytesLeftToSend = m_xUdpTxWorkingData.dataSize - m_totalUdpMessageBytesDownlinked;

        // Set number of bytes to send this loop (number that will be
        // pending until we receive a success response):
        // Max number of bytes to send in each chunk is 255
        // (BGAPI limitation, why we need SetTransmitSize):
        m_chunkBytesPending = (bytesLeftToSend > 0xFF) ? 0xFF : bytesLeftToSend;

        // Pack the data for the UDP chunk:
        SendEndpoint(&m_bgApiCommandBuffer,
                     m_downlinkTargetEndpoint,
                     m_xUdpTxWorkingData.data + m_totalUdpMessageBytesDownlinked,
                     m_chunkBytesPending);
        *yieldData = true; // tell State Machine to send this data

        // Next state will be waiting for a response (after sending data):
        return UdpTxUpdateState::WAIT_FOR_UDP_CHUNK_ACK;
    }

    // Helper function to handle the `WAIT_FOR_UDP_CHUNK_ACK`
    // `UdpTxUpdateState` state in `udpTxUpdateHandler`:
    // Sets the yieldData flag if it needs the State Machine to pause and yield
    // data to the `Wf121UdpTxTask`.
    // Returns the next state to transition to.
    NetworkInterface::UdpTxUpdateState NetworkInterface::handleTxState_WAIT_FOR_UDP_CHUNK_ACK(bool *yieldData)
    {
        UdpTxUpdateState nextState;

        // Wait for a response from the downlink endpoint (or timeout):
        // NOTE: This will clear the mailbox first (see function definition for
        // why). So, if somehow the Radio responded (and we processed the
        // response) basically instantly after us sending the data, then this
        // will block until `UDP_TX_RESPONSE_TIMEOUT_TICKS`.
        BgApi::ErrorCode errorCode = m_udpTxCommsStatusManager.awaitResponse_sendEndpointUdp();
        switch (errorCode)
        {
        case BgApi::ErrorCode::NO_ERROR:
            // Cool, we can move on.

            // Update number of bytes downlinked (they're no longer pending):
            m_totalUdpMessageBytesDownlinked += m_chunkBytesPending;
            m_protectedRadioStatus.incUdpTxByteCount(m_chunkBytesPending);

            if (m_totalUdpMessageBytesDownlinked < m_xUdpTxWorkingData.dataSize)
            {
                // We still have more bytes to send. Send another chunk:
                nextState = UdpTxUpdateState::SEND_UDP_CHUNK;
            }
            else
            {
                // We've sent the whole UDP payload:
                nextState = UdpTxUpdateState::DONE_DOWNLINKING;
            }

            // Reset fail counter:
            m_bgApiCommandFailCount = 0;
            break;

        case BgApi::ErrorCode::INTERNAL__BAD_SYNTAX:
            // INTERNAL__BAD_SYNTAX isn't a real BGAPI code, but is instead put
            // into the mailbox in response to an `evt_endpoint_syntax_error`
            // being emitted by the Radio while we're awaiting a response to
            // this command (likely means our command got garbled).
            // So, just try sending it again:
        case BgApi::ErrorCode::INTERNAL__TRY_AGAIN:
            // INTERNAL__TRY_AGAIN isn't a real BGAPI code, it's just part of
            // the interpreter and means something, didn't work correctly (or
            // wasn't ready). So, try again:
        case BgApi::ErrorCode::TIMEOUT:
            // Didn't get a response in a long time. Maybe Radio didn't get it?
            // Try again.
        default:
            // Some other error occurred, try again:
            m_bgApiCommandFailCount++;
            watchDogInterface.debugPrintfToWatchdog("RADIO: WAIT_FOR_UDP_CHUNK_ACK FAIL: %#04x, %u", errorCode, m_bgApiCommandFailCount); // For debugging. TODO: [CWC] REMOVEME
            if (m_bgApiCommandFailCount > WF121_BGAPI_COMMAND_MAX_TRIES)
            {
                // We've sent this again too many times. Go to bad case:
                nextState = UdpTxUpdateState::BGAPI_CMD_FAIL;
            }
            else
            {
                // Send the same chunk again (don't increment anything,
                // just try the send chunk step again):
                nextState = UdpTxUpdateState::SEND_UDP_CHUNK;
            }
            break;
        }

        return nextState;
    }

    // Helper function to handle the `DONE_DOWNLINKING` `UdpTxUpdateState`
    // state in `udpTxUpdateHandler`:
    // Sets the yieldData flag if it needs the State Machine to pause and yield
    // data to the `Wf121UdpTxTask`.
    // Returns the next state to transition to.
    NetworkInterface::UdpTxUpdateState NetworkInterface::handleTxState_DONE_DOWNLINKING(bool *yieldData)
    {
        // Any cleanup we want to do after downlinking a UDP payload / before
        // sending the next:
        m_protectedRadioStatus.incUdpTxPacketCount();

        // Go back to the start:
        return UdpTxUpdateState::WAIT_FOR_BGAPI_READY;
    }

    // Helper function to handle the `BGAPI_CMD_FAIL` `UdpTxUpdateState`
    // state in `udpTxUpdateHandler`:
    // Sets the yieldData flag if it needs the State Machine to pause and yield
    // data to the `Wf121UdpTxTask`.
    // Returns the next state to transition to.
    NetworkInterface::UdpTxUpdateState NetworkInterface::handleTxState_BGAPI_CMD_FAIL(bool *yieldData)
    {
        // What to do after Radio didn't give us a response for a bunch of packets:
        UdpTxUpdateState nextState;

        // Check whether the Radio is still connected (if it's not, we may
        // have failed to get a response just because the Radio is too busy
        // trying to reconnect to respond (we've noticed this in testing - when
        // a BGScript is running tightly, BGAPI endpoints don't respond quickly):
        if (m_protectedRadioStatus.getRadioState() == DirectMessage::RadioSwState::UDP_CONNECTED)
        {
            // Last thing the Radio told us is that it's connected but we're not
            // getting any response from it, tell anyone listening
            // (NetworkManager) that something bad happened:
            // (note: this is a last resort b/c normally the Radio handles fault-handling and resets itself.)
            //
            // NOTE: We **DON'T** want to raise this flag if the Radio's not in
            // the `UDP_CONNECTED` state because that means the Radio's probably
            // in the middle of trying to reconnect and we don't want to
            // interrupt that.
            // If it's instead halted, we'll catch that by other means
            // (`timeOfLastHeartbeatMs` being too long ago).
            m_protectedRadioStatus.setCriticalCommsFailure_NeedHelp(true);

            // Go back to the start (keep trying as if nothing went wrong and
            // hope something listening to the semaphore was able to help us):
            nextState = UdpTxUpdateState::WAIT_FOR_BGAPI_READY;
        }
        else
        {
            // Radio likely lost connection while we were sending all the
            // chunks for this packet, so just go back to the start of trying
            // to send it again:
            nextState = UdpTxUpdateState::START_SENDING_MESSAGE;
        }

        // Manually set `RadioSwState` to `NONE` (the default pre-comms state),
        // even if it was `UDP_CONNECTED`.
        // Since we won't send more data until the state goes back to `UDP_CONNECTED`,
        // this means we won't send more data until we've gotten a HB from the Radio
        // telling us it's in `UDP_CONNECTED` (meaning it's alive and working):
        // That is, this has the effect of pausing the TX task until the Radio
        // tells us it's alive and ready to receive again.
        m_protectedRadioStatus.setRadioState(DirectMessage::RadioSwState::NONE);

        // Radio's BGAPI processor probably didn't get or ignored our message
        // (or *we* didn't get the response saying it was done). Either way,
        // it's probably not actually working on our command now, so let's
        // reset what we think it's doing:
        m_bgApiStatus.setProcessingCmd(false);

        return nextState;
    }

    /**
     * @brief The callback invoked by the `Wf121UdpTxTask` when it's running
     * (each call of this function is one "writing loop"). Whenever the
     * `Wf121TxTaskManager` determines it's time to send data, it returns a
     * pointer to a `UdpTxPayload` and lets the `UdpTxTask` perform a write.
     *
     * @param pTask Pointer to Task triggering this handle
     *
     * @returns Pointer to data to be written to the Radio-Hercules UART.
     */
    BgApi::BgApiCommBuffer *NetworkInterface::udpTxUpdateHandler(Wf121UdpTxTask *pTask)
    {
        // State of internal state machine (kick it off at the beginning):
        static UdpTxUpdateState inner_state = UdpTxUpdateState::WAIT_FOR_BGAPI_READY;

        // Flag to tell us to exit the state machine loop and yield data:
        bool yieldData = false;

        // Keep pushing through the state machine until someone wants to yieldData:
        while (!yieldData)
        {
            // If we're in the middle of sending chunks for a message and are no
            // longer connected, reset the state to the beginning of trying to send
            // the current message. That is, wait for reconnect and try sending the
            // entire message again.
            if (inner_state > UdpTxUpdateState::START_SENDING_MESSAGE &&
                m_protectedRadioStatus.getRadioState() != DirectMessage::RadioSwState::UDP_CONNECTED)
            {
                inner_state = UdpTxUpdateState::START_SENDING_MESSAGE;
            }

            // Poll until we meet all downlinking criteria (check every X ms):
            // (make sure we can downlink before yielding **any** data or
            // even determining what data to yield):
            while (
                m_protectedRadioStatus.getRadioState() != DirectMessage::RadioSwState::UDP_CONNECTED || // Radio is connected and able to send UDP data
                m_protectedRadioStatus.getDownlinkEndpoint() == DirectMessage::UDP_NULL_ENDPOINT        // Target for that UDP data isn't /dev/null
            )
            {
                vTaskDelay(WF121_DOWNLINK_READY_TO_SEND_POLLING_CHECK_INTERVAL);
            }

            // Now advance the state machine:
            switch (inner_state)
            {
            case UdpTxUpdateState::/******/ WAIT_FOR_BGAPI_READY:
                inner_state = handleTxState_WAIT_FOR_BGAPI_READY(&yieldData);
                watchDogInterface.debugPrintfToWatchdog("RADIO: TX in WAIT_FOR_BGAPI_READY"); // For debugging. TODO: [CWC] REMOVEME
                break;

            case UdpTxUpdateState::/******/ WAIT_FOR_NEXT_MESSAGE:
                inner_state = handleTxState_WAIT_FOR_NEXT_MESSAGE(&yieldData);
                watchDogInterface.debugPrintfToWatchdog("RADIO: TX in WAIT_FOR_NEXT_MESSAGE"); // For debugging. TODO: [CWC] REMOVEME
                break;

            case UdpTxUpdateState::/******/ START_SENDING_MESSAGE:
                inner_state = handleTxState_START_SENDING_MESSAGE(&yieldData);
                watchDogInterface.debugPrintfToWatchdog("RADIO: TX in START_SENDING_MESSAGE"); // For debugging. TODO: [CWC] REMOVEME
                break;

            case UdpTxUpdateState::/******/ WAIT_FOR_UDP_INTERLOCK:
                inner_state = handleTxState_WAIT_FOR_UDP_INTERLOCK(&yieldData);
                watchDogInterface.debugPrintfToWatchdog("RADIO: TX in WAIT_FOR_UDP_INTERLOCK"); // For debugging. TODO: [CWC] REMOVEME
                break;

            case UdpTxUpdateState::/******/ SEND_SET_TRANSMIT_SIZE:
                inner_state = handleTxState_SEND_SET_TRANSMIT_SIZE(&yieldData);
                watchDogInterface.debugPrintfToWatchdog("RADIO: TX in SEND_SET_TRANSMIT_SIZE"); // For debugging. TODO: [CWC] REMOVEME
                break;

            case UdpTxUpdateState::/******/ WAIT_FOR_SET_TRANSMIT_SIZE_ACK:
                inner_state = handleTxState_WAIT_FOR_SET_TRANSMIT_SIZE_ACK(&yieldData);
                watchDogInterface.debugPrintfToWatchdog("RADIO: TX in WAIT_FOR_SET_TRANSMIT_SIZE_ACK"); // For debugging. TODO: [CWC] REMOVEME
                break;

            case UdpTxUpdateState::/******/ SEND_UDP_CHUNK:
                inner_state = handleTxState_SEND_UDP_CHUNK(&yieldData);
                watchDogInterface.debugPrintfToWatchdog("RADIO: TX in SEND_UDP_CHUNK"); // For debugging. TODO: [CWC] REMOVEME
                break;

            case UdpTxUpdateState::/******/ WAIT_FOR_UDP_CHUNK_ACK:
                inner_state = handleTxState_WAIT_FOR_UDP_CHUNK_ACK(&yieldData);
                watchDogInterface.debugPrintfToWatchdog("RADIO: TX in WAIT_FOR_UDP_CHUNK_ACK"); // For debugging. TODO: [CWC] REMOVEME
                break;

            case UdpTxUpdateState::/******/ DONE_DOWNLINKING:
                inner_state = handleTxState_DONE_DOWNLINKING(&yieldData);
                watchDogInterface.debugPrintfToWatchdog("RADIO: TX in DONE_DOWNLINKING"); // For debugging. TODO: [CWC] REMOVEME
                break;

            case UdpTxUpdateState::/******/ BGAPI_CMD_FAIL:
                inner_state = handleTxState_BGAPI_CMD_FAIL(&yieldData);
                watchDogInterface.debugPrintfToWatchdog("RADIO: TX in BGAPI_CMD_FAIL"); // For debugging. TODO: [CWC] REMOVEME
                break;
            default:
                // There must have been a bit-flip or something because we've
                // enumerated all the states above.
                // Attempt to recover by going back to the start:
                inner_state = UdpTxUpdateState::WAIT_FOR_BGAPI_READY;
            }

            if (inner_state == UdpTxUpdateState::BGAPI_CMD_FAIL)
            {
                // BGAPI_CMD_FAIL only lasts for one transition,
                // handle it now before looping again (in case we failed because
                // of a disconnect and end up looping until connection on the
                // next state machine driver run - we want to handle this right
                // away):
                inner_state = handleTxState_BGAPI_CMD_FAIL(&yieldData);
                watchDogInterface.debugPrintfToWatchdog("RADIO: TX did early BGAPI_CMD_FAIL"); // For debugging. TODO: [CWC] REMOVEME
            }
        }

        // If we're out here, we have BGAPI data to send to the radio:
        // Yield (pass) BGAPI Comm Buffer data to WF121:
        m_bgApiStatus.setProcessingCmd(true); // flag that WF121 is about to be processing a command
        return &m_bgApiCommandBuffer;
    }
}
