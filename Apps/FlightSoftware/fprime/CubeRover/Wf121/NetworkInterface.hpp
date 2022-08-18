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
#include <Os/Mutex.hpp>

#include <cassert>
#include <cstdio>

#include "FreeRTOS.h"
#include "os_queue.h"

namespace Wf121
{
    // ! readyToDownlink = state==connected && !BGAPI::awaitingCommandResponse && downlinkEndpoint != DROP_ENDPOINT

    class NetworkInterface : public BgApi::BgApiDriver,
                             public DirectMessage::DirectMessageDriver
    {
    public:
        // Simple container struct for data received over UDP,
        // (no mutex protection here b/c this goes inside a FreeRTOS IPC Queue).
        // The idea is that every time a valid UDP payload is received from
        // the Radio via a BGAPI event, we pass one of these into the
        // `xUdpRxPayloadQueue`. Uses a fixed size buffer since FreeRTOS Queues
        // only pass fixed size data (and it's risky to pass pointers through
        // it if they might go out of scope).
        struct UdpRxPayload{
            uint16_t dataSize;
            uint8_t data[WF121_UDP_MAX_PAYLOAD];
            
            UdpRxPayload() : dataSize(0){
                /* nothing else to do here */
            }

            // Clear all data (overwrite with fixed sentinel byte):
            // (NOTE: don't want to just blindly do this on every ctor call
            // since, for a buffer this size, it's slightly expensive)
            void clear(){
                dataSize = 0;
                // Write 0xFA to all "empty" bytes in this array, this just makes debugging easier
                memset(this->data, 0xAB, WF121_UDP_MAX_PAYLOAD);
            }
        };

        // Handle to statically-allocated queue of all UDP payloads received from =
        // the Radio.
        // NOTE: This is *NOT* a circular buffer. Data is just dropped if it
        // fills (to ensure command order - i.e. so we don't drop Command A if
        // it was supposed to be recieved before Command B).
        // This is filled here, by `NetworkInterface`, and is designed to be
        // drained by the `NetworkManager` FPrime component.
        QueueHandle_t xUdpRxPayloadQueue;

        // Simple struct for all data relevant to transmitting messages to the
        // Radio:
        struct TxCommsStatus
        {
            // Whether the Radio acknowledged us setting the UDP TX size:
            bool setTxSizeAcknowledged;
            // How many acknowledgements we've gotten for sendEndpoint chunks:
            uint16_t numSendEndpointAcknowledgements;
            ::Os::Mutex mutex;

            TxCommsStatus() : setTxSizeAcknowledged(false),
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

            // Call this when we get an ack for setTxSize:
            void gotSetTxSizeAcknowledged()
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
        TxCommsStatus m_txCommsStatus;

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
        bool getAvailableUdpPayload(UdpRxPayload* pPayload);        

    private:
        // Data struct for working with RXed UDP data internally: 
        UdpRxPayload xUdpRxWorkingData;

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
