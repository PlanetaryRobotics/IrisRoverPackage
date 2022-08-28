/**
 * @file Wf121DirectMessage.hpp
 * @author Connor Colombo (colombo@cmu.edu)
 * @brief Parsing and handlers for Direct Messages from the Radio FSW.
 *
 * This uses a proprietary format that's separate from the BGAPI protocol used
 * by the Radio's core firmware to communicate and is designed to be
 * human-readable.
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
 *
 * NOTE: On the Hercules side, **ALL DIRECT MESSAGES WILL BE PRECEDED BY AN "evt_endpoint_data"
 * BGAPI header (and the endpoint ID for Hercules UART (1) and 1B BGAPI payload length).**
 * @date 2022-08-11
 */

#ifndef CUBEROVER_WF121_WF121_DIRECT_MESSAGE_HPP_
#define CUBEROVER_WF121_WF121_DIRECT_MESSAGE_HPP_

#include <Fw/Time/Time.hpp>

namespace Wf121{namespace DirectMessage // Wf121::DirectMessage
{
    // Radio's built in drop (effectively /dev/null) endpoint (used when UDP isn't set up):
    const uint8_t UDP_NULL_ENDPOINT = 31;

    // State of the Radio's SW (from a "state:" or "thump:" message)
    enum class RadioSwState
    {
        // None (we haven't received any state update yet):
        NONE = 0x00,
        // The last update contained a valid message header but the body was corrupted (didn't match any known message):
        BAD_MESSAGE = 0x0F,
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
        // The last update contained a valid message header but the body was corrupted (didn't match any known message):
        BAD_MESSAGE = 0x0F,
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
        // None (we haven't received anything yet):
        NONE = 0x00,
        // We got a valid message header but the body was corrupted (didn't match any known message):
        BAD_MESSAGE = 0x0F,
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
        // We got a valid message header but the body was corrupted (didn't match any known message):
        BAD_MESSAGE = 0x0F,
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

    // Known message headers:
    const uint8_t FIXED_HEADER_LEN = 6; // Length of all fixed headers
    const uint8_t HEARTBEAT_HEADER[] = "thump:";
    const uint8_t STATE_HEADER[] = "state:";
    const uint8_t DOING_HEADER[] = "doing:";
    const uint8_t ERROR_HEADER[] = "error:";
    const uint8_t FAULT_HEADER[] = "fault:";

    // Driver to handle processing of Direct Messages from the Radio:
    class DirectMessageDriver
    {
    public:
        // Initializer (start everything once outer processes are ready):
        void init();

        // Constructor (just initialize data structures):
        DirectMessageDriver();
        virtual ~DirectMessageDriver();

        // Processes the given message. Returns whether the message contained a
        // known format (and wasn't just plain text being passed along):
        bool processDirectMessage(uint8_t msg_len, uint8_t *msg_data);

        /**
         * @brief Handle Heartbeat Radio-Hercules Direct Message.
         *
         * @param body_len Length of body contents (header length excluded)
         * @param body_data Pointer to start of message body (with any header removed)
         * @return Returns the number of bytes used (0 if no valid body was found).
         */
        uint8_t handleHeartbeatMessage(uint8_t body_len, uint8_t *body_data);
        /**
         * @brief Handle State Change Radio-Hercules Direct Message.
         *
         * @param body_len Length of body contents (header length excluded)
         * @param body_data Pointer to start of message body (with any header removed)
         * @param state_output Pointer to a state object to set to the resultant value (if not nullptr). Defaults to `nullptr`.
         * @param fire_callback Whether to fire the `cb_dm_NowInState` callback if a valid activity is found (defaults to true).
         * @return Returns the number of bytes used (0 if no valid body was found).
         */
        uint8_t handleStateMessage(uint8_t body_len, uint8_t *body_data, RadioSwState *state_output = nullptr, bool fire_callback = true);
        /**
         * @brief Handle "doing" (Activity Start) Radio-Hercules Direct Message.
         *
         * @param body_len Length of body contents (header length excluded)
         * @param body_data Pointer to start of message body (with any header removed)
         * @param doingOutput Pointer to an activity object to set to the resultant value (if not nullptr). Defaults to `nullptr`.
         * @param doing_output Pointer to a state object to set to the resultant value (if not nullptr). Defaults to nullptr.
         * @param fire_callback Whether to fire the `cb_dm_NowDoingActivity` callback if a valid activity is found (defaults to true).
         * @return Returns the number of bytes used (0 if no valid body was found).
         */
        uint8_t handleDoingMessage(uint8_t body_len, uint8_t *body_data, RadioSwActivity *doing_output = nullptr, bool fire_callback = true);
        /**
         * @brief Handle Error Radio-Hercules Direct Message.
         *
         * @param body_len Length of body contents (header length excluded)
         * @param body_data Pointer to start of message body (with any header removed)
         * @return Returns the number of bytes used (0 if no valid body was found).
         */
        uint8_t handleErrorMessage(uint8_t body_len, uint8_t *body_data);
        /**
         * @brief Handle Fault Radio-Hercules Direct Message.
         *
         * @param body_len Length of body contents (header length excluded)
         * @param body_data Pointer to start of message body (with any header removed)
         * @return Returns the number of bytes used (0 if no valid body was found).
         */
        uint8_t handleFaultMessage(uint8_t body_len, uint8_t *body_data);

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
        virtual void cb_dm_Heartbeat(
            const uint8_t downlinkEndpoint,
            const uint8_t uplinkEndpoint,
            const RadioSwState state,
            const RadioSwActivity doing,
            const bool fullyValid)
        {
            /* do nothing by default */
        }

        /**
         * @brief      Callback triggered when we determine we've received
         * valid information about the Radio's current RadioSwState, from a
         * state change Direct Message.
         *
         * @param state The state we're now in.
         */
        virtual void cb_dm_NowInState(const RadioSwState state)
        {
            /* do nothing by default */
        }

        /**
         * @brief      Callback triggered when we determine we've received
         * valid information about the Radio's current RadioSwActivity, from a
         * Direct Message indicating we've just started the activity.
         *
         * @param doing The activity we're now doing.
         */
        virtual void cb_dm_NowDoingActivity(const RadioSwActivity doing)
        {
            /* do nothing by default */
        }

    protected:
        /**
         * @brief Helper function that checks if the given `body_data` buffer
         * matches (or starts with) the given `state_buf`. If it does, it returns
         * the number of bytes used (length of the null-terminated `state_buf`).
         * If it matches and `fire_on_match` is true, it will also fire off the
         * `cb_dm_NowInState` callback with `state` before returning.
         *
         * @param state State to assign if there's a match.
         * @param state_buf State buffer to check for.
         * @param body_len Length of buffer to check.
         * @param body_data Buffer to check.
         * @param state_output Pointer to a state object to set to the resultant value (if not nullptr).
         * @param fire_on_match Whether to fire `cb_dm_NowInState` if there's a match.
         * @return uint8_t Number of bytes used (0 if no match).
         */
        uint8_t checkIfStateMatches(RadioSwState state, const uint8_t *state_buf, uint8_t body_len, uint8_t *body_data, RadioSwState *state_output, bool fire_on_match);

        /**
         * @brief Helper function that checks if the given `body_data` buffer
         * matches (or starts with) the given `doing_buf`. If it does, it returns
         * the number of bytes used (length of the null-terminated `doing_buf`).
         * If it matches and `fire_on_match` is true, it will also fires off the
         * `cb_dm_NowDoingActivity` callback with `doing` before returning.
         *
         * @param doing "Doing" activity to assign if there's a match.
         * @param doing_buf "Doing" activity buffer to check for.
         * @param body_len Length of buffer to check.
         * @param body_data Buffer to check.
         * @param doing_output Pointer to a state object to set to the resultant value (if not nullptr).
         * @param fire_on_match Whether to fire `cb_dm_NowDoingActivity` if there's a match.
         * @return uint8_t Number of bytes used (0 if no match).
         */
        uint8_t checkIfDoingActivityMatches(RadioSwActivity doing, const uint8_t *doing_buf, uint8_t body_len, uint8_t *body_data, RadioSwActivity *doing_output, bool fire_on_match);
    };
}}

#endif /* CUBEROVER_WF121_WF121_DIRECT_MESSAGE_HPP_ */
