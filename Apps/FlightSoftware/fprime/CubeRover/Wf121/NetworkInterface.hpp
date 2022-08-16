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

namespace Wf121
{
    class NetworkInterface : public BgApi::BgApiDriver, public DirectMessage::DirectMessageDriver
    {
        // Constructor (just initialize data structures):
        NetworkInterface();

        // Initializer (start everything once outer processes are ready):
        void init();

        // TODO: Look at how WatchDogInterface passes data out to make sure this is sound (queue + mutex? ... or just do it, looks like it just calls uplink_out)
    };
}

#endif /* CUBEROVER_WF121_WF121_NETWORK_INTERFACE_HPP_ */
