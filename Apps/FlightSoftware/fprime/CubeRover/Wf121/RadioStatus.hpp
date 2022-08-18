/**
 * @file RadioStatus.hpp
 * @author Connor Colombo (colombo@cmu.edu)
 * @brief Mutex-protected state struct for managing all information received
 * from the radio.
 *
 * @date 2022-08-17
 */

#ifndef CUBEROVER_WF121_WF121_RADIO_STATUS_HPP_
#define CUBEROVER_WF121_WF121_RADIO_STATUS_HPP_

#include <CubeRover/Wf121/Wf121BgApi.hpp>
#include <CubeRover/Wf121/Wf121DirectMessage.hpp>

#include <Os/Mutex.hpp>
#include <Fw/Time/Time.hpp>

namespace Wf121
{
    // TODO: Add radio Error & Fault Queue (We should pass these to WD)

    /**
     * @brief All important data related to the current radio state.
     */
    struct RadioStatus
    {
        // Time of last received (fully valid) Direct Message Heartbeat from
        // the Radio, in ms since Hercules boot:
        uint32_t timeOfLastHeartbeatMs;
        // Current State (as received from a Direct Message):
        DirectMessage::RadioSwState currentRadioState;
        // Current Activity (as received from a Direct Message):
        DirectMessage::RadioSwActivity currentRadioActivity;
        // BGAPI Endpoint to send data to be downlinked to Earth:
        BgApi::Endpoint downlinkEndpoint;
        // BGAPI Endpoint to use to receive data from Earth:
        BgApi::Endpoint uplinkEndpoint;
        // The latest WiFi connection RSSI (note: value is always negative):
        int8_t rssi;
        // Number of uplinked UDP packets received:
        uint32_t udpRxPacketCount;
        // Number of bytes received in uplinked UDP packets:
        uint32_t udpRxByteCount;
        // Number of UDP packets downlinked:
        uint32_t udpTxPacketCount;
        // Number of bytes sent in downlinked UDP packets:
        uint32_t udpTxByteCount;

        RadioStatus() : timeOfLastHeartbeatMs(0),
                        currentRadioState(DirectMessage::RadioSwState::NONE),
                        currentRadioActivity(DirectMessage::RadioSwActivity::NONE),
                        downlinkEndpoint(DirectMessage::UDP_NULL_ENDPOINT),
                        uplinkEndpoint(DirectMessage::UDP_NULL_ENDPOINT),
                        rssi(0),
                        udpRxPacketCount(0),
                        udpRxByteCount(0),
                        udpTxPacketCount(0),
                        udpTxByteCount(0)
        {
            // Nothing else to do.
        }

        // Copies the entire current status into the given RadioStatus object:
        void copyInto(RadioStatus *target)
        {
            target->timeOfLastHeartbeatMs = this->timeOfLastHeartbeatMs;
            target->currentRadioState = this->currentRadioState;
            target->currentRadioActivity = this->currentRadioActivity;
            target->downlinkEndpoint = this->downlinkEndpoint;
            target->uplinkEndpoint = this->uplinkEndpoint;
            target->rssi = this->rssi;
            target->udpRxPacketCount = this->udpRxPacketCount;
            target->udpRxByteCount = this->udpRxByteCount;
            target->udpTxPacketCount = this->udpTxPacketCount;
            target->udpTxByteCount = this->udpTxByteCount;
        }

        // Sets the `timeOfLastHeartbeatMs` to the current Time in milliseconds
        // since Hercules boot.
        virtual void updateLastHeartbeatTime()
        {
            Fw::Time now = getTime();
            timeOfLastHeartbeatMs = static_cast<uint32_t>(now.get_time_ms());
        }
    };

    /**
     * @brief A wrapper around `RadioStatus` All important data related to the current radio state, protected by a
     * mutex so multiple processes can safely access it.
     *
     * All of the accesses are structured as unidirectional copies so the mutex
     * is locked for as little time as possible.
     *
     * That is, the way to "read" the protected radio status is to use
     * `safeCopy` to copy all of its contents into a local RadioStatus object
     * protected under a mutex lock. Setting data can be safely achieved using
     * any of the setters.
     */
    class ProtectedRadioStatus : private RadioStatus
    {
    private:
        // Mutex that should be locked anytime the status is read or modified:
        ::Os::Mutex mutex;

    public:
        /* GETTERS */
        // Full getter: Obtains a mutex lock, copies the entire current radio
        // status into the given RadioSwState, releases the lock.
        void safeCopyInto(RadioStatus *target)
        {
            this->mutex.lock();
            this->copyInto(target);
            this->mutex.unLock();
        }
        // Obtains a mutex lock, copies the radio state into the given
        // RadioSwState, releases the lock.
        void copyRadioStateInto(DirectMessage::RadioSwState *state)
        {
            this->mutex.lock();
            *state = this->currentRadioState;
            this->mutex.unLock();
        }
        // Obtains a mutex lock, copies the current radio activity into the
        // given RadioSwActivity, releases the lock.
        void copyRadioActivityInto(DirectMessage::RadioSwActivity *act)
        {
            this->mutex.lock();
            *act = this->currentRadioActivity;
            this->mutex.unLock();
        }

        // Obtains a mutex lock, copies the `timeOfLastHeartbeatMs` in
        // milliseconds since Hercules boot into the given int, releases the
        // lock.
        void copyLastHeartbeatTimeMsInto(uint32_t *lastHeartbeatTimeMs)
        {
            this->mutex.lock();
            *lastHeartbeatTimeMs = this->timeOfLastHeartbeatMs;
            this->mutex.unLock();
        }

        // Obtains a mutex lock, copies the `downlinkEndpoint` into the given
        // object, releases the lock.
        void copyDownlinkEndpointInto(BgApi::Endpoint *downlinkEndpoint)
        {
            this->mutex.lock();
            *downlinkEndpoint = this->downlinkEndpoint;
            this->mutex.unLock();
        }
        // Obtains a mutex lock, copies the `uplinkEndpoint` into the given
        // object, releases the lock.
        void copyUplinkEndpointInto(BgApi::Endpoint *uplinkEndpoint)
        {
            this->mutex.lock();
            *uplinkEndpoint = this->uplinkEndpoint;
            this->mutex.unLock();
        }
        // Obtains a mutex lock, copies the `rssi` into the given
        // object, releases the lock.
        void copyRssiInto(int8_t *rssi)
        {
            this->mutex.lock();
            *rssi = this->rssi;
            this->mutex.unLock();
        }

        // Obtains a mutex lock, copies `udpRxPacketCount` into the given
        // object, releases the lock.
        void copyUdpRxPacketCountInto(uint32_t* udpRxPacketCount){
            this->mutex.lock();
            *udpRxPacketCount = this->udpRxPacketCount;
            this->mutex.unLock();
        }
        // Obtains a mutex lock, copies `udpRxByteCount` into the given
        // object, releases the lock.
        void copyUdpRxByteCountInto(uint32_t* udpRxByteCount){
            this->mutex.lock();
            *udpRxByteCount = this->udpRxByteCount;
            this->mutex.unLock();
        }
        // Obtains a mutex lock, copies `udpTxPacketCount` into the given
        // object, releases the lock.
        void copyUdpTxPacketCountInto(uint32_t* udpTxPacketCount){
            this->mutex.lock();
            *udpTxPacketCount = this->udpTxPacketCount;
            this->mutex.unLock();
        }
        // Obtains a mutex lock, copies `udpTxByteCount` into the given
        // object, releases the lock.
        void copyUdpTxByteCountInto(uint32_t* udpTxByteCount){
            this->mutex.lock();
            *udpTxByteCount = this->udpTxByteCount;
            this->mutex.unLock();
        }

        /* SETTERS: */
        // Obtains a mutex lock, sets the radio state to the given
        // RadioSwState, releases the lock.
        void setRadioState(DirectMessage::RadioSwState state)
        {
            this->mutex.lock();
            this->currentRadioState = state;
            this->mutex.unLock();
        }
        // Obtains a mutex lock, sets the current radio activity to the given
        // RadioSwActivity, releases the lock.
        void setRadioActivity(DirectMessage::RadioSwActivity act)
        {
            this->mutex.lock();
            this->currentRadioActivity = act;
            this->mutex.unLock();
        }
        // Obtains a mutex lock, sets the `timeOfLastHeartbeatMs` to the current
        // Time in milliseconds since Hercules boot, releases the lock.
        void updateLastHeartbeatTime()
        {
            // Do all the computation to get the time first...
            uint32_t now = static_cast<uint32_t>(getTime().get_time_ms());
            // ... and only lock the mutex when absolutely needed:
            this->mutex.lock();
            this->timeOfLastHeartbeatMs = now;
            this->mutex.unLock();
        }

        // Obtains a mutex lock, sets the `downlinkEndpoint` to the given value,
        // releases the lock.
        void setDownlinkEndpoint(BgApi::Endpoint downlinkEndpoint)
        {
            this->mutex.lock();
            this->downlinkEndpoint = downlinkEndpoint;
            this->mutex.unLock();
        }
        // Obtains a mutex lock, sets the `uplinkEndpoint` to the given value,
        // releases the lock.
        void setUplinkEndpoint(BgApi::Endpoint uplinkEndpoint)
        {
            this->mutex.lock();
            this->uplinkEndpoint = uplinkEndpoint;
            this->mutex.unLock();
        }
        // Obtains a mutex lock, sets the `rssi` to the given value,
        // releases the lock.
        void setRssi(int8_t rssi)
        {
            this->mutex.lock();
            this->rssi = rssi;
            this->mutex.unLock();
        }
        // Obtains a mutex lock, increments `udpRxPacketCount` by `x`, releases
        // the lock:
        void incUdpRxPacketCount(uint32_t x = 1UL){
            this->mutex.lock();
            this->udpRxPacketCount += x;
            this->mutex.unLock();
        }
        // Obtains a mutex lock, increments `udpRxByteCount` by `x`, releases
        // the lock:
        void incUdpRxByteCount(uint32_t x = 1UL){
            this->mutex.lock();
            this->udpRxByteCount += x;
            this->mutex.unLock();
        }
        // Obtains a mutex lock, increments `udpTxPacketCount` by `x`, releases
        // the lock:
        void incUdpTxPacketCount(uint32_t x = 1UL){
            this->mutex.lock();
            this->udpTxPacketCount += x;
            this->mutex.unLock();
        }
        // Obtains a mutex lock, increments `udpTxByteCount` by `x`, releases
        // the lock:
        void incUdpTxByteCount(uint32_t x = 1UL){
            this->mutex.lock();
            this->udpTxByteCount += x;
            this->mutex.unLock();
        }
    };
}

#endif /* CUBEROVER_WF121_WF121_RADIO_STATUS_HPP_ */