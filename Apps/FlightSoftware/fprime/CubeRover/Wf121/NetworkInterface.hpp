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

#include <CubeRover/Wf121/Wf121BgApi.hpp>
#include <CubeRover/Wf121/Wf121DirectMessage.hpp>
#include <CubeRover/Wf121/RadioStatus.hpp>
#include <Os/Mutex.hpp>

namespace Wf121
{
    // ! readyToDownlink = state==connected && !BGAPI::awaitingCommandResponse && downlinkEndpoint != DROP_ENDPOINT

    class NetworkInterface : public BgApi::BgApiDriver,
                             public DirectMessage::DirectMessageDriver
    {

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

        // TODO: Look at how WatchDogInterface passes data out to make sure this is sound (queue + mutex? ... or just do it, looks like it just calls uplink_out)

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
        Wf121::ErrorCode cb_EventUdpData(const BgApi::Endpoint endpoint,
                                         const BgApi::IpAddress srcAddress,
                                         const uint16_t srcPort,
                                         uint8_t *data,
                                         const BgApi::DataSize16 dataSize);
    };
}

#endif /* CUBEROVER_WF121_WF121_NETWORK_INTERFACE_HPP_ */
