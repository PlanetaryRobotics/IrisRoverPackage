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
 * Ultimately, all updates on this interface are driven by the `Wf121RxTask`
 * or `Wf121UdpTxTask`.
 *
 * Any datagrams received here (data that has to be parsed by other parts of
 * the flight software, i.e. not metadata about the radio) is pushed into
 * `uplinkDatagramQueue`, waiting to be read from by `NetworkManager`.
 *
 * NOTE: The reason this implements `Wf121TxTaskManager` and manages what data
 * the `Wf121UdpTxTask` sends and when is due to the fact that much of the TX
 * control flow is determined by the current `RadioStatus` and callbacks
 * received from `Wf121RxTask`. All of that data coalesces here anyway,
 * so from a single-ownership point of view, this made the most sense.
 * Likewise, the `Wf121TxTask` exists separately because it's best practice to
 * have a single dedicated interface to the hardware peripherals like the
 * serial, so it owns the interaction with `dmaSend`.
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
#include <CubeRover/Wf121/UdpTxCommsStatusManager.hpp>
#include <CubeRover/Wf121/Wf121UdpTxTask.hpp>
#include <CubeRover/Wf121/UdpPayload.hpp>
#include <CubeRover/Wf121/GroundDirectTMTC.hpp>
#include <CubeRover/Wf121/Timestamp.hpp>
#include <Os/Mutex.hpp>

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <cassert>
#include <cstring>
#include <cstdio>

#include "FreeRTOS.h"
#include "os_queue.h"

// - How much the processor should wait before checking back in to see if we
//   meet all criteria for sending data to the Radio to be downlinked.
// - Since this is a high priority task, it's not a good idea for this to be 0
//   (though it *can* be zero) in order to prevent Task starvation.
// - In fact, since it's only likely to happen either A.) At boot when the
//   radio is still connecting or B.) anytime the Radio disconnects (at worst,
//   very briefly once every many minutes), it's okay for this to be quite long
//   to give other Tasks room to work.
// NOTE: This happens when servicing the `Wf121UdpTxTask` (so doesn't block the main Task).
// NOTE: FreeRTOS scheduler ticks are every 1ms.
static const TickType_t WF121_DOWNLINK_READY_TO_SEND_POLLING_CHECK_INTERVAL = 200 / portTICK_PERIOD_MS; // every 200ms (200 ticks)

// Most number of times to try sending a BGAPI command without receiving a response before giving up:
static const uint8_t WF121_BGAPI_COMMAND_MAX_TRIES = 5;

// Max number of FreeRTOS Scheduler ticks to allow the calling task to wait for
// the UDP TX Queue to become available while attempting to put data into it.
// NOTE: this is mostly precautionary since, before attempting to send anything,
// the UDP TX Queue is checked for space and, if there isn't any, the oldest
// item is popped off before writing is attempted.
static const TickType_t WF121_UDP_TX_ENQUEUE_WAIT_TICKS = 5;
// Max number of FreeRTOS Scheduler ticks to allow the Wf121RxTask to wait for
// the UDP RX Queue to become free while attempting to put data into it:
// (if the UDP RX Queue doesn't have space available by that time, the enqueuing
// UdpPayload will be dropped *but* an emergency message will be force-pushed
// to the *front* of the UDP TX Queue so Ground knows that the UDP RX Queue is
// full and not getting serviced fast enough. If Ground thinks this is a
// problem, it can send a `GND_DIRECT_CMD_RESET_ALL_BUFFERS` TC to the RX Task
// to tell it to clear the queue (this way new commands will now be able to get
// through).
static const TickType_t WF121_UDP_RX_ENQUEUE_WAIT_TICKS = 10;

namespace Wf121
{
    class NetworkInterface : public BgApi::BgApiDriver,
                             public DirectMessage::DirectMessageDriver,
                             public virtual Wf121TxTaskManager
    {
    public:
        friend class RadioDriver; // let Wf121::RadioDriver access private data in this class

        // Current status of the radio:
        ProtectedRadioStatus m_protectedRadioStatus;

        // Current status of the outbound comms to the radio:
        UdpTxCommsStatusManager m_udpTxCommsStatusManager;

        // Constructor (just initialize data structures):
        NetworkInterface();

        // Dtor to satisfy polymorphism (this class should never actually be deleted):
        ~NetworkInterface();

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
        bool getAvailableUdpPayload(UdpRxPayload *pPayload, TickType_t blockingTicks = 0);

        /**
         * @brief Add the given payload to the UDP TX Queue.
         *
         * NOTE: UART writing happens asynchronously and this Queue will be
         * drained by `Wf121UdpTxTask`.
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
        virtual BgApi::BgApiCommBuffer *udpTxUpdateHandler(Wf121UdpTxTask *pTask);

    private:
        // Data struct for working with RXed UDP data internally:
        UdpRxPayload m_xUdpRxWorkingData;
        // Data struct for working with TXing UDP data internally (receiving data from the Queue)
        UdpTxPayload m_xUdpTxWorkingData;
        // Buffer used to store data for BGAPI commands to be sent to the Radio:
        BgApi::BgApiCommBuffer m_bgApiCommandBuffer;
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
        UdpTxPayload m_xUdpTxRecycling;

        // Handle to statically-allocated queue of all UDP payloads received
        // from = the Radio.
        // NOTE: This is *NOT* a circular buffer. Data is just dropped if it
        // fills (to ensure command order - i.e. so we don't drop Command A if
        // it was supposed to be received before Command B).
        // This is filled here, by `NetworkInterface`, and is designed to be
        // drained by the `NetworkManager` FPrime component.
        QueueHandle_t m_xUdpRxPayloadQueue;
        // Handle to statically-allocated queue of all UDP payloads to be sent
        // to the Radio.
        // NOTE: Unlike `m_xUdpRxPayloadQueue`, this queue will be handled in such
        // a way that if you try to add an item to it when it's full, it will
        // dequeue the oldest item and load a new one (prioritizing downlinking
        // new telemetry, etc. over older data). In this way, it's effectively
        // a circular buffer (the closest you can get in native FreeRTOS while
        // still supporting multiple writers).
        // ^ *but* this behavior only works if you write to the Queue using
        // `sendUdpPayload`.
        QueueHandle_t m_xUdpTxPayloadQueue;

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

        // Number of consecutive times we've failed to send an individual BGAPI
        // command in a row:
        uint8_t m_bgApiCommandFailCount = 0;
        // Number of message chunk bytes pending (that we sent with
        // `sendEndpoint` and are awaiting a response for):
        uint8_t m_chunkBytesPending = 0;
        // Total number of UDP bytes (successfully) downlinked (in the current
        // message / payload):
        uint16_t m_totalUdpMessageBytesDownlinked = 0;
        // Target endpoint for a downlink (grabbed from
        // `m_protectedRadioStatus.getDownlinkEndpoint()` once per downlink so
        // it doesn't change while we're sending chunks).
        BgApi::Endpoint m_downlinkTargetEndpoint;

        // States used by the State Machine inside udpTxUpdateHandler.
        enum class UdpTxUpdateState
        {
            // Waiting for BGAPI to not be busy:
            WAIT_FOR_BGAPI_READY = 0x10,
            // Not in the middle of sending data. Wait for more data to send:
            WAIT_FOR_NEXT_MESSAGE = 0x11,
            // Start sending the message (perform any setup):
            START_SENDING_MESSAGE = 0x12,
            // We have a message to send and now need to send `SetTransmitSize`:
            SEND_SET_TRANSMIT_SIZE = 0x13,
            // Wait for acknowledgement of `SetTransmitSize`:
            WAIT_FOR_SET_TRANSMIT_SIZE_ACK = 0x20,
            // Send a UDP chunk:
            SEND_UDP_CHUNK = 0x21,
            // Wait for acknowledgement of last UDP chunk's `SendEndpoint`:
            WAIT_FOR_UDP_CHUNK_ACK = 0x22,
            // Done downlinking data:
            DONE_DOWNLINKING = 0xE0,
            // Handle a failure to send a BGAPI command (after surpassing WF121_BGAPI_COMMAND_MAX_TRIES):
            BGAPI_CMD_FAIL = 0xF0
        };
        // State handlers:
        UdpTxUpdateState handleTxState_WAIT_FOR_BGAPI_READY(bool *yieldData);
        UdpTxUpdateState handleTxState_WAIT_FOR_NEXT_MESSAGE(bool *yieldData);
        UdpTxUpdateState handleTxState_START_SENDING_MESSAGE(bool *yieldData);
        UdpTxUpdateState handleTxState_SEND_SET_TRANSMIT_SIZE(bool *yieldData);
        UdpTxUpdateState handleTxState_WAIT_FOR_SET_TRANSMIT_SIZE_ACK(bool *yieldData);
        UdpTxUpdateState handleTxState_SEND_UDP_CHUNK(bool *yieldData);
        UdpTxUpdateState handleTxState_WAIT_FOR_UDP_CHUNK_ACK(bool *yieldData);
        UdpTxUpdateState handleTxState_DONE_DOWNLINKING(bool *yieldData);
        UdpTxUpdateState handleTxState_BGAPI_CMD_FAIL(bool *yieldData);
    };
}

#endif /* CUBEROVER_WF121_WF121_NETWORK_INTERFACE_HPP_ */
