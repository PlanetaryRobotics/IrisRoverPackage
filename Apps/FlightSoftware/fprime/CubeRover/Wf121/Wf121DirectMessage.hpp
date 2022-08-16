/**
 * @file Wf121DirectMessage.hpp
 * @author Connor Colombo (colombo@cmu.edu)
 * @brief Parsing and handlers for Direct Messages from the Radio FSW.
 *
 * This uses a proprietary format that's separate from the BGAPI protocol used
 * by the Radio's core firmware to communicate.
 * This format's header has no bytes in common with a
 * valid BGAPI header (i.e it fails the BGAPI header test) or ASCII characters:
 *      [0xE6 0xE7 0xE7 0xE6]
 *      [0-255, number of bytes in subsequent message]
 *      [message bytes (0xE6-0xE7 not allowed, suggest ASCII only) . . .]
 * Example (sending 'Hello'):
 *      [0xE6 0xE7 0xE7 0xE6] [0x05] [0x48 0x65 0x6C 0x6C 0x6F]
 * This is to allow direct messaging between the WF121 BGScript and Hercules
 * without going through the BGAPI.
 * NOTE: We can't use a more sophisticated protocol like SLIP for this because
 * BGScript execution is pretty slow on a per-line basis and doesn't have
 * native support for SLIP.
 *
 * If the Radio needs to send a buffer, it will encode it as a hex string with
 * the following format:
 *  " X04\x0B:AD:BE:EF"
 * where "04" is the length of the buffer as a hex-encoded int.
 *
 * STANDARD MESSAGES:
 * Although arbitrary plain text could be sent using this protocol,
 * standardized messages are sent.
 *      - Each standardized message starts with a fixed-length header followed
 *      by a ':' and the message data field. Standard headers are:
 *          - "state:"      Current state of the device. Sent *only* after a
 *                          state transition.
 *          - "doing:"      The primary activity the device is working on. Sent
 *                          *only* when that activity begins.
 *          - "thump:"      Heartbeat message. Contains two fields: the current
 *                          "state:" of the device, the current "doing" activity
 *                          of the device.
 *          - "error:"      Something has gone wrong but the radio can attempt
 *                          to fix it without resetting itself.
 *          - "fatal:"      Something has gone *very* wrong and the radio is
 *                          going to reset itself to try to recover.
 *      -Most messages contain one field of data but those which contain >1
 *      field separate those fields using a ':'.
 * @date 2022-08-11
 */

#ifndef CUBEROVER_WF121_WF121_DIRECT_MESSAGE_HPP_
#define CUBEROVER_WF121_WF121_DIRECT_MESSAGE_HPP_

namespace Wf121::DirectMessage
{
    // State of the Radio's SW (from a "state:" or "thump:" message)
    enum class RadioSwState
    {
        // None (we haven't received any state update yet):
        NONE = 0x00,
        // Booted but hasn't initialized its `state_driver` yet:
        BOOT = 0x10,
        // In the initial state but WiFi radio isn't powered up yet:
        INIT = 0x20,
        // Radio is turned on:
        WIFI_ON = 0x30,
        // Radio is connected to the network (ARP, etc) but UDP client & socket aren't active yet:
        CONNECTED = 0x40,
        // UDP client & socket are active:
        UDP_CONNECTED = 0x50
    };

    // What the Radio is currently working on (from a "doing:" or "thump:" message):
    enum class RadioSwActivity
    {
        // None (we haven't received any activity update yet):
        NONE = 0x00,
        // Trying to turn on the WiFi radio (power it up):
        TURNING_WIFI_ON = 0x10,
        // Connecting to the network (getting the ARP, the hard part):
        CONNECTING_NETWORK = 0x20,
        // Starting UDP client & server:
        STARTING_UDP = 0x30,
        // Everything is up and working. Just looking out for anomalies & correcting them:
        REMAINING_VIGILENT = 0x40,
        // Found a non-fatal anomaly. Waiting a small period of time (ms to seconds) before correcting it:
        WAITING_THEN_CORRECTING = 0x50
    };

    // Error emitted by the Radio SW:
    enum class RadioSwError
    {
        NONE = 0x00,
        // Network disconnected:
        DISCONNECTED = 0x10,
        // UDP client or server went down and will need to be restored:
        UDP_LOST = 0x20
    };

    // Fatal errors emitted from the Radio SW (all of these precede a self-reboot):
    enum class RadioSwFatalError
    {
        // None (we haven't received anything yet):
        NONE = 0x00,
        // Failed to configure the internal TCP/IP settings of the Radio:
        TCPIP_CFG_ERROR = 0x10,
        // Failed to set the internal network password used by the Radio:
        SET_PASSWORD_FAIL = 0x20,
        // Failed to configure which channels to scan when connected
        // (this is a config failure and occurs during setup in the INIT state):
        SET_SCAN_CHAN_FAIL = 0x30,
        // Failed to connect to the network too many times in a row:
        CONNECT_MAX_TRIES = 0x40,
        // WiFi radio went down (wifi_off event) while trying to connect to the
        // network (bad, weird, and should never happen):
        WIFI_OFF_DURING_CONNECT = 0x50,
        // Failed to start UDP services (client & server) too many times in a row:
        START_UDP_MAX_TRIES = 0x60,
        // Failed to turn on the WiFi radio too many times in a row:
        WIFI_ON_MAX_TRIES = 0x70,
        // Radio SW wasn't in "good state" (wifi on & connected & udp client
        // active & udp server active) when the Radio's Self-Watchdog procedure
        // triggered (default is once every 0x7FFFF milliseconds):
        SELF_WD_NOT_GOOD = 0x80,
        // Radio's `anomaly_handling` noticed a serious fault (out of memory,
        // HW fault, etc.) and is going to trigger a system reset:
        CRITICAL_RESET = 0x90
    };

    // Driver to handle processing of Direct Messages from the Radio:
    class DirectMessageDriver
    {
        // Time of last received heartbeat from the Radio in ms since Hercules
        // boot:
        uint32_t m_timeOfLastHeartbeatMs;

        // Latest Radio Metadata:
        RadioSwState m_currentRadioState;
        RadioSwActivity m_currentRadioActivity;

        // TODO: Add radio Error & Fault Queue (We should pass these to WD)

        // Initializer (start everything once outer processes are ready):
        void init();

        // Constructor (just initialize data structures):
        DirectMessageDriver();
        ~DirectMessageDriver();
    };
}

#endif /* CUBEROVER_WF121_WF121_DIRECT_MESSAGE_HPP_ */