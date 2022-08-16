#include <CubeRover/Wf121/NetworkInterface.hpp>

namespace Wf121
{
    NetworkInterface::NetworkInterface() : BgApi::BgApiDriver(),
                                           DirectMessage::DirectMessageDriver()
    {
        // nothing else to do
    }

    NetworkInterface::init()
    {
        BgApi::BgApiDriver::init();
        DirectMessage::DirectMessageDriver::init();
    }
}