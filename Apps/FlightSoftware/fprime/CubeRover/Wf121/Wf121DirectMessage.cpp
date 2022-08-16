#include <CubeRover/Wf121/Wf121DirectMessage.hpp>

#include <Fw/Time/Time.hpp>

namespace Wf121::DirectMessage
{
    DirectMessageDriver::DirectMessageDriver() : m_timeOfLastHeartbeatMs(0),
                                                 m_currentRadioState(RadioSwState::NONE),
                                                 m_currentRadioActivity(RadioSwActivity::NONE)
    {
        // nothing else to do
    }

    DirectMessageDriver::~DirectMessageDriver()
    {
        // do nothing
    }

    void DirectMessageDriver::init()
    {
        // Init the time of last heartbeat received to now (we shouldn't expect
        // it to have arrived any earlier than now):
        Fw::Time now = getTime();
        m_timeOfLastHeartbeatMs = static_cast<uint32_t>(now.get_time_ms());
    }
}