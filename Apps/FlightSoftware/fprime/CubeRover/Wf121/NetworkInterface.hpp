/**
 * @file NetworkInterface.hpp
 * @author Connor Colombo (colombo@cmu.edu)
 * @brief Since, the WF121 chip is now in charge of connecting to and managing
 * the network by itself, this `NetworkInterface` layer is responsible for
 * sending data to the chip for downlinking and grabbing data from hooks
 * received.
 *
 * In addition to datagrams meant for other parts of the Hercules FSW,
 * this 'data' refers to metadata about the Radio FSW's internal state and
 * activities.
 *
 * Ultimately, all updates on this interface are driven by the `Wf121RxTask`.
 *
 * Any datagrams received here (data that has to be parsed by other parts of
 * the flight software, i.e. not metadata about the radio) is pushed into
 * `uplinkDatagramQueue`, waiting to be read from by `NetworkManager`.
 *
 * @date 2022-08-13
 */

#ifndef CUBEROVER_WF121_WF121_NETWORK_INTERFACE_HPP_
#define CUBEROVER_WF121_WF121_NETWORK_INTERFACE_HPP_

#include <HAL/include/FreeRTOS.h>
#include <HAL/include/os_task.h>
#include <HAL/include/os_portmacro.h>
#include <HAL/include/sys_dma.h>

#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>

#include <CubeRover/Wf121/Wf121BgApi.hpp>
#include <CubeRover/Wf121/Wf121DirectMessage.hpp>
#include <CubeRover/Wf121/RadioStatus.hpp>
#include <CubeRover/Wf121/UdpPayload.hpp>
#include <Os/Mutex.hpp>

#include <cassert>
#include <cstring>
#include <cstdio>

#include "FreeRTOS.h"
#include "os_queue.h"

namespace Wf121
{
    // Struct used for acknowledging that an uplinked UDP packet was
    // successfully received over the Radio (sent back to the radio).
    // Format:
    //      - "HER:" // Fixed 4B header (parallel of "RAD:")
    //      - [length of UDP payload received] // (2B, little endian)
    struct HerculesRadioUplinkResponse
    {
        uint32_t fixedHeader = 0x3A'52'45'48; // 'HER:' (in little-endian)
        uint16_t payloadBytesReceived;        // Number of bytes in the UDP payload
        HerculesRadioUplinkResponse(uint16_t pbr) : payloadBytesReceived(pbr){};
    } __attribute__((packed, aligned(1)));

    class NetworkInterface : public BgApi::BgApiDriver,
                             public DirectMessage::DirectMessageDriver
    {
    public:
        friend class RadioDriver; // let Wf121::RadioDriver access private data in this class

        // Simple struct for all data relevant to transmitting UDP messages to
        // the Radio:
        struct UdpTxCommsStatus
        {
            // Whether the Radio acknowledged us setting the UDP TX size:
            bool setTxSizeAcknowledged;
            // How many acknowledgements we've gotten for sendEndpoint chunks:
            uint16_t numSendEndpointAcknowledgements;
            ::Os::Mutex mutex;

            UdpTxCommsStatus() : setTxSizeAcknowledged(false),
                                 numSendEndpointAcknowledgements(0)
            {
            }

            // Reset back to the pre-messaging state:
            void reset()
            {
                this->mutex.lock();
                setTxSizeAcknowledged = false;
                numSendEndpointAcknowledgements = 0;
                this->mutex.unLock();
            }

            // *GETTERS:*
            // Returns whether setTxSize has been acknowledged:
            bool hasSetTxSizeBeenAcknowledged()
            {
                bool acked;
                this->mutex.lock();
                acked = setTxSizeAcknowledged;
                this->mutex.unLock();
                return acked;
            }

            // Returns the number of sendEndpoint acknowledgements we've
            // received:
            uint16_t getNumSendEndpointAcknowledgements()
            {
                uint16_t numAcks = 0;
                this->mutex.lock();
                numAcks = numSendEndpointAcknowledgements;
                this->mutex.unLock();
                return numAcks;
            }

            // *SETTERS:*
            // Call this when we get an ack for setTxSize:
            void gotSetTxSizeAcknowledgement()
            {
                this->mutex.lock();
                setTxSizeAcknowledged = true;
                this->mutex.unLock();
            }

            // Increment the number of sendEndpoint acknowledgements we've
            // received:
            void incNumSendEndpointAcknowledgements()
            {
                this->mutex.lock();
                numSendEndpointAcknowledgements++;
                this->mutex.unLock();
            }
        };

        // Current status of the radio:
        ProtectedRadioStatus m_protectedRadioStatus;

        // Current status of the outbound comms to the radio:
        UdpTxCommsStatus m_udpTxCommsStatus;

        // Constructor (just initialize data structures):
        NetworkInterface();

        // Initializer (start everything once outer processes are ready):
        void init();

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
        bool getAvailableUdpPayload(UdpRxPayload *pPayload);

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
         * @return Whether the payload was successfully added to the queue
         * (**NOT** whether it was successfully sent, since that can't be
         * determined yet since writing is handled asynchronously by another
         * Task.)
         */
        bool sendUdpPayload(UdpTxPayload *pPayload);

    private:
        // Data struct for working with RXed UDP data internally:
        UdpRxPayload xUdpRxWorkingData;
        // Buffer to read TX payloads into to remove them.
        // We have to do this b/c the only way to dequeue an item from the
        // Queue is to read it into something (and just passing nullptr gives a
        // segfault).
        //
        // NOTE: We can't just use xUdpTxWorkingData for this since 1. we'd
        // then be accessing it from multiple tasks, and 2. it might be being
        // used for writing at the very moment we need to recycle something.
        //
        // NOTE: There are better (but more complicated) ways to implement
        // this multithread safe circular buffering using a double layered
        // queue (a ring buffer of buffers with mutex protection and a RTOS
        // queue of indexes to buffers in the ring buffer) but we don't have
        // the dev time to build and test that impl. so unless this system
        // leads to long runtime issues, we're going to have to stick with this.
        // NOTE: Also, since reading is a copy operation (of a large buffer),
        // this can be expensive. It's best to make sure that
        // `UDP_TX_PAYLOAD_QUEUE_DEPTH` is sufficiently large that recycling
        // is rare.
        //
        // NOTE: Performance is possibly important here because if, for some
        // reason, all of our writes are failing (e.g. not connected), we could
        // just be continually eating throw and clearing out the buffer.
        // That said, this can be ameliorated by just preventing data from
        // being added to the buffer if it's full and we're not connected
        // (handle this in NetworkManager) or just changing the downlink
        // destination if not connected.
        // Worst-case copy performance would be ~4clk cycles per word. So, for
        // structs sized (1006-20-8)+2=978B=490 words, that's 1960 cycles per
        // copy which is 245us per copy assuming an 8MHz clock (not too bad
        // actually considering how infrequently (on the order of seconds)
        // we're downlinking. Even if copy performance is 4cycles/Byte (due to
        // bad byte alignment somewhere?), that still only 0.980ms. A
        // worst-case 0.25-1ms penalty every several thousand ms isn't too bad
        // and acceptable given our current development state.
        //
        // NOTE: An easy-to-implement, simpler, more efficient, but much more
        // drastic method would be to just `xQueueReset` if the buffer fills,
        // nuking everything in it and starting over with a fresh Queue.
        // In terms of data preservation, for small queues where all the
        // data would soon be overwritten anyway, this isn't much worse than
        // just dequeuing each item but the advantage is that it will be much
        // faster (no expensive copy-to-recycling is required) and require less
        // memory (don't need a recycling buffer).
        UdpTxPayload xUdpTxRecycling;

        // Handle to statically-allocated queue of all UDP payloads received
        // from = the Radio.
        // NOTE: This is *NOT* a circular buffer. Data is just dropped if it
        // fills (to ensure command order - i.e. so we don't drop Command A if
        // it was supposed to be received before Command B).
        // This is filled here, by `NetworkInterface`, and is designed to be
        // drained by the `NetworkManager` FPrime component.
        QueueHandle_t xUdpRxPayloadQueue;
        // Handle to statically-allocated queue of all UDP payloads to be sent
        // to the Radio.
        // NOTE: Unlike `xUdpRxPayloadQueue`, this queue will be handled in such
        // a way that if you try to add an item to it when it's full, it will
        // dequeue the oldest item and load a new one (prioritizing downlinking
        // new telemetry, etc. over older data). In this way, it's effectively
        // a circular buffer (the closest you can get in native FreeRTOS while
        // still supporting multiple writers).
        // ^ *but* this behavior only works if you write to the Queue using
        // `sendUdpPayload`.
        QueueHandle_t xUdpTxPayloadQueue;

        ////
        // CALLBACK HOOKS:
        ////
        /* DIRECT MESSAGE CALLBACKS */
        void cb_dm_Heartbeat(const uint8_t downlinkEndpoint,
                             const uint8_t uplinkEndpoint,
                             const DirectMessage::RadioSwState state,
                             const DirectMessage::RadioSwActivity doing,
                             const bool fullyValid);
        void cb_dm_NowInState(const DirectMessage::RadioSwState state);
        void cb_dm_NowDoingActivity(const DirectMessage::RadioSwActivity doing);

        /* BGAPI COMMAND CALLBACKS */
        BgApi::ErrorCode cb_CommandSetTransmitSize(const uint16_t result,
                                                   const BgApi::Endpoint endpoint);
        BgApi::ErrorCode cb_CommandSendEndpoint(const uint16_t result,
                                                const BgApi::Endpoint endpoint);

        /* BGAPI EVENT CALLBACKS */
        BgApi::ErrorCode cb_EventSignalQuality(const int8_t rssi,
                                               const BgApi::HardwareInterface hwInterface);
        // Event for uplinked packets:
        BgApi::ErrorCode cb_EventUdpData(const BgApi::Endpoint endpoint,
                                         const BgApi::IpAddress srcAddress,
                                         const uint16_t srcPort,
                                         uint8_t *data,
                                         const BgApi::DataSize16 dataSize);
    };
}

#endif /* CUBEROVER_WF121_WF121_NETWORK_INTERFACE_HPP_ */
