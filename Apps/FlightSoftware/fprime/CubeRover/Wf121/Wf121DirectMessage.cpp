#include <CubeRover/Wf121/Wf121DirectMessage.hpp>

#include <stddef.h>
#include <string.h>

// Anonymous namespace for file-scope helper functions:
namespace
{
    // Returns the length of the given fixed buffer containing a
    // null-terminated string:
    size_t getStrBufferLen(const uint8_t *buf)
    {
        return strlen((const char *)buf);
    }
}

namespace Wf121
{
    namespace DirectMessage // Wf121::DirectMessage
    {
        DirectMessageDriver::DirectMessageDriver()
        {
            // nothing to do
        }

        DirectMessageDriver::~DirectMessageDriver()
        {
            // do nothing
        }

        void DirectMessageDriver::init()
        {
            /* nothing to do here right now */
        }

        // Processes the given message. Returns whether the message contained a
        // known format (and wasn't just plain text being passed along):
        bool DirectMessageDriver::processDirectMessage(uint8_t msg_len, uint8_t *msg_data)
        {
            // Make sure message isn't too short:
            if (msg_len < FIXED_HEADER_LEN)
            {
                return false;
            }

            // Dispatch to the appropriate handler based on known fixed-headers:
            if (memcmp(msg_data, HEARTBEAT_HEADER, FIXED_HEADER_LEN) == 0)
            {
                return handleHeartbeatMessage(msg_len - FIXED_HEADER_LEN, msg_data + FIXED_HEADER_LEN);
            }
            if (memcmp(msg_data, STATE_HEADER, FIXED_HEADER_LEN) == 0)
            {
                return handleStateMessage(msg_len - FIXED_HEADER_LEN, msg_data + FIXED_HEADER_LEN);
            }
            if (memcmp(msg_data, DOING_HEADER, FIXED_HEADER_LEN) == 0)
            {
                return handleDoingMessage(msg_len - FIXED_HEADER_LEN, msg_data + FIXED_HEADER_LEN);
            }
            if (memcmp(msg_data, INTERLOCK_HEADER, FIXED_HEADER_LEN) == 0)
            {
                return handleInterlockMessage(msg_len - FIXED_HEADER_LEN, msg_data + FIXED_HEADER_LEN);
            }
            if (memcmp(msg_data, ERROR_HEADER, FIXED_HEADER_LEN) == 0)
            {
                return handleErrorMessage(msg_len - FIXED_HEADER_LEN, msg_data + FIXED_HEADER_LEN);
            }
            if (memcmp(msg_data, FAULT_HEADER, FIXED_HEADER_LEN) == 0)
            {
                return handleFaultMessage(msg_len - FIXED_HEADER_LEN, msg_data + FIXED_HEADER_LEN);
            }

            // Didn't match any known pattern (would have returned above if so):
            return false;
        }

        /**
         * @brief Handle Heartbeat Radio-Hercules Direct Message.
         *
         * Heartbeat message body is special and follows the format:
         *  - [udp client endpoint]
         *  - ':' separator
         *  - [udp server endpoint]
         *  - ':' separator
         *  - [state message]
         *  - '>' boundary characters
         *  - [doing message]
         *
         * @param body_len Length of body contents (header length excluded)
         * @param body_data Pointer to start of message body (with any header removed)
         * @return Returns the number of bytes used (0 if no valid message was found).
         */
        uint8_t DirectMessageDriver::handleHeartbeatMessage(uint8_t body_len, uint8_t *body_data)
        {
            // Parser data:
            bool validSoFar = true;
            uint8_t bytes_used = 0;

            // Fetched data (defaults)
            uint8_t downlinkEndpoint = UDP_NULL_ENDPOINT; // where Earthbound data gets routed (Radio's UDP client)
            uint8_t uplinkEndpoint = UDP_NULL_ENDPOINT;   // where data from Earth gets routed (Radio's UDP server)
            RadioSwState state = RadioSwState::BAD_MESSAGE;
            RadioSwActivity doing = RadioSwActivity::BAD_MESSAGE;

            // Grab the endpoints:
            validSoFar &= (body_len >= 4);                                // enough data for endpoint info
            validSoFar &= (body_data[1] == ':') && (body_data[3] == ':'); // proper separators after endpoint data
            if (validSoFar)
            {
                downlinkEndpoint = body_data[0];
                uplinkEndpoint = body_data[2];
                bytes_used += 4;
            }

            // Grab the state message:
            if (validSoFar)
            {
                uint8_t state_bytes = handleStateMessage(body_len - bytes_used, body_data + bytes_used, &state, false);
                validSoFar &= (state_bytes > 0);
                bytes_used += state_bytes;
            }

            // Appropriate boundary character between state and doing fields:
            if (validSoFar)
            {
                validSoFar &= (body_data[bytes_used] == '>');
                bytes_used += 1;
            }

            // Grab the doing message that follows the separator:
            if (validSoFar)
            {
                uint8_t doing_bytes = handleDoingMessage(body_len - bytes_used, body_data + bytes_used, &doing, false);
                validSoFar &= (doing_bytes > 0);
                bytes_used += doing_bytes;
            }

            // Fire the callback:
            cb_dm_Heartbeat(downlinkEndpoint,
                            uplinkEndpoint,
                            state,
                            doing,
                            validSoFar);

            return bytes_used;
        }

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
        uint8_t DirectMessageDriver::checkIfStateMatches(RadioSwState state, const uint8_t *state_buf, uint8_t body_len, uint8_t *body_data, RadioSwState *state_output, bool fire_on_match)
        {
            uint8_t state_len = getStrBufferLen(state_buf);
            if (
                body_len >= state_len &&
                memcmp(body_data, state_buf, state_len) == 0)
            {
                if (fire_on_match)
                {
                    cb_dm_NowInState(state);
                }
                if (state_output)
                {
                    *state_output = state;
                }
                return state_len;
            }
            // Else:
            return 0;
        }

        // Reference buffers for each state:
        static const uint8_t STATE_REF_BUFFER__BOOT[] = "BOOT";
        static const uint8_t STATE_REF_BUFFER__INIT[] = "INIT";
        static const uint8_t STATE_REF_BUFFER__WIFI_ON[] = "WIFI_ON";
        static const uint8_t STATE_REF_BUFFER__CONNECTED[] = "CONNECTED";
        static const uint8_t STATE_REF_BUFFER__UDP_CONNECTED[] = "UDP_CONNECTED";
        /**
         * @brief Handle State Change Radio-Hercules Direct Message.
         *
         * @param body_len Length of body contents (header length excluded)
         * @param body_data Pointer to start of message body (with any header removed)
         * @param state_output Pointer to a state object to set to the resultant value (if not nullptr). Defaults to `nullptr`.
         * @param fire_callback Whether to fire the `cb_dm_NowInState` callback if a valid activity is found (defaults to true).
         *
         * @return Returns the number of bytes used (0 if no valid body was found).
         */
        uint8_t DirectMessageDriver::handleStateMessage(uint8_t body_len, uint8_t *body_data, RadioSwState *state_output, bool fire_callback)
        {
            // Set state based on state message and return number of bytes used:
            uint8_t bytes_used = 0;

            // BOOT STATE:
            if (bytes_used == 0)
            { // no match found yet
                bytes_used = checkIfStateMatches(RadioSwState::BOOT, STATE_REF_BUFFER__BOOT, body_len, body_data, state_output, fire_callback);
            }
            // INIT STATE:
            if (bytes_used == 0)
            { // no match found yet
                bytes_used = checkIfStateMatches(RadioSwState::INIT, STATE_REF_BUFFER__INIT, body_len, body_data, state_output, fire_callback);
            }
            // WIFI_ON STATE:
            if (bytes_used == 0)
            { // no match found yet
                bytes_used = checkIfStateMatches(RadioSwState::WIFI_ON, STATE_REF_BUFFER__WIFI_ON, body_len, body_data, state_output, fire_callback);
            }
            // CONNECTED STATE:
            if (bytes_used == 0)
            { // no match found yet
                bytes_used = checkIfStateMatches(RadioSwState::CONNECTED, STATE_REF_BUFFER__CONNECTED, body_len, body_data, state_output, fire_callback);
            }
            // UDP_CONNECTED STATE:
            if (bytes_used == 0)
            { // no match found yet
                bytes_used = checkIfStateMatches(RadioSwState::UDP_CONNECTED, STATE_REF_BUFFER__UDP_CONNECTED, body_len, body_data, state_output, fire_callback);
            }

            // We couldn't understand the message:
            if (bytes_used == 0)
            {
                // If we miss the state transition, we want to flag that we now
                // don't know what's going on:
                if (fire_callback)
                {
                    cb_dm_NowInState(RadioSwState::BAD_MESSAGE);
                }
                if (state_output)
                {
                    *state_output = RadioSwState::BAD_MESSAGE;
                }
            }

            return bytes_used;
        }

        /**
         * @brief Helper function that checks if the given `body_data` buffer
         * matches (or starts with) the given `doing_buf`. If it does, it returns
         * the number of bytes used (length of the null-terminated `doing_buf`).
         * If it matches and `fire_on_match` is true, it will also fire off the
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
        uint8_t DirectMessageDriver::checkIfDoingActivityMatches(RadioSwActivity doing, const uint8_t *doing_buf, uint8_t body_len, uint8_t *body_data, RadioSwActivity *doing_output, bool fire_on_match)
        {
            uint8_t doing_len = getStrBufferLen(doing_buf);
            if (
                body_len >= doing_len &&
                memcmp(body_data, doing_buf, doing_len) == 0)
            {
                if (fire_on_match)
                {
                    cb_dm_NowDoingActivity(doing);
                }
                if (doing_output)
                {
                    *doing_output = doing;
                }
                return doing_len;
            }
            // Else:
            return 0;
        }

        // Reference buffers for each action:
        static const uint8_t DOING_REF_BUFFER__TURNING_WIFI_ON[] = "TURNING_WIFI_ON";
        static const uint8_t DOING_REF_BUFFER__CONNECTING_NETWORK[] = "CONNECTING_NETWORK";
        static const uint8_t DOING_REF_BUFFER__STARTING_UDP[] = "STARTING_UDP";
        static const uint8_t DOING_REF_BUFFER__REMAINING_VIGILENT[] = "REMAINING_VIGILENT";
        static const uint8_t DOING_REF_BUFFER__WAITING_THEN_CORRECTING[] = "WAITING_THEN_CORRECTING";
        /**
         * @brief Handle "doing" (Activity Start) Radio-Hercules Direct Message.
         *
         * @param body_len Length of body contents (header length excluded)
         * @param body_data Pointer to start of message body (with any header removed)
         * @param doing_output Pointer to a state object to set to the resultant value (if not nullptr). Defaults to nullptr.
         * @param fire_callback Whether to fire the `cb_dm_NowDoingActivity` callback if a valid activity is found (defaults to true).
         * @return Returns the number of bytes used (0 if no valid body was found).
         */
        uint8_t DirectMessageDriver::handleDoingMessage(uint8_t body_len, uint8_t *body_data, RadioSwActivity *doing_output, bool fire_callback)
        {
            // Set activity based on state message and return number of bytes used:
            uint8_t bytes_used = 0;

            // TURNING_WIFI_ON:
            if (bytes_used == 0)
            { // no match found yet
                bytes_used = checkIfDoingActivityMatches(RadioSwActivity::TURNING_WIFI_ON, DOING_REF_BUFFER__TURNING_WIFI_ON, body_len, body_data, doing_output, fire_callback);
            }
            // CONNECTING_NETWORK:
            if (bytes_used == 0)
            { // no match found yet
                bytes_used = checkIfDoingActivityMatches(RadioSwActivity::CONNECTING_NETWORK, DOING_REF_BUFFER__CONNECTING_NETWORK, body_len, body_data, doing_output, fire_callback);
            }
            // STARTING_UDP:
            if (bytes_used == 0)
            { // no match found yet
                bytes_used = checkIfDoingActivityMatches(RadioSwActivity::STARTING_UDP, DOING_REF_BUFFER__STARTING_UDP, body_len, body_data, doing_output, fire_callback);
            }
            // REMAINING_VIGILENT:
            if (bytes_used == 0)
            { // no match found yet
                bytes_used = checkIfDoingActivityMatches(RadioSwActivity::REMAINING_VIGILENT, DOING_REF_BUFFER__REMAINING_VIGILENT, body_len, body_data, doing_output, fire_callback);
            }
            // WAITING_THEN_CORRECTING:
            if (bytes_used == 0)
            { // no match found yet
                bytes_used = checkIfDoingActivityMatches(RadioSwActivity::WAITING_THEN_CORRECTING, DOING_REF_BUFFER__WAITING_THEN_CORRECTING, body_len, body_data, doing_output, fire_callback);
            }

            // We couldn't understand the message:
            if (bytes_used == 0)
            {
                if (fire_callback)
                {
                    cb_dm_NowDoingActivity(RadioSwActivity::BAD_MESSAGE);
                }
                if (doing_output)
                {
                    *doing_output = RadioSwActivity::BAD_MESSAGE;
                }
            }

            return bytes_used;
        }

        // Reference buffers for each interlock status:
        static const uint8_t ILOCK_REF_BUFFER__RADIO_HAS_INTERLOCK[] = "OFF";
        static const uint8_t ILOCK_REF_BUFFER__HERC_HAS_INTERLOCK[] = "ON";
        /**
         * @brief Helper function that checks if the given `body_data` buffer
         * exactly matches the given `ilock_buf`. If it does, it returns
         * the number of bytes used (length of the null-terminated `ilock_buf`).
         *
         * @param status Interlock status to assign if there's a match.
         * @param ilock_buf Interlock buffer to check for.
         * @param body_len Length of buffer to check.
         * @param body_data Buffer to check.
         * @return uint8_t Number of bytes used (0 if no match).
         */
        uint8_t DirectMessageDriver::checkIfInterlockMatches(RadioUdpInterlockStatus status, const uint8_t *ilock_buf, uint8_t body_len, uint8_t *body_data);
        {
            uint8_t ilock_len = getStrBufferLen(ilock_buf);
            if (
                body_len == ilock_len && // only counting *exact* matches
                memcmp(body_data, ilock_buf, ilock_len) == 0)
            {
                cb_dm_InterlockUpdate(status);
                return ilock_len;
            }
            // Else:
            return 0;
        }

        /**
         * @brief Handle Interlock Radio-Hercules Direct Message.
         *
         * @param body_len Length of body contents (header length excluded)
         * @param body_data Pointer to start of message body (with any header removed)
         * @return Returns the number of bytes used (0 if no valid body was found).
         */
        uint8_t DirectMessageDriver::handleInterlockMessage(uint8_t body_len, uint8_t *body_data)
        {
            // Set activity based on state message and return number of bytes used:
            uint8_t bytes_used = 0;

            // RADIO_HAS_INTERLOCK:
            if (bytes_used == 0)
            { // no match found yet
                bytes_used = checkIfInterlockMatches(RadioUdpInterlockStatus::RADIO_HAS_INTERLOCK, ILOCK_REF_BUFFER__RADIO_HAS_INTERLOCK, body_len, body_data);
            }
            // HERC_HAS_INTERLOCK:
            if (bytes_used == 0)
            { // no match found yet
                bytes_used = checkIfInterlockMatches(RadioUdpInterlockStatus::HERC_HAS_INTERLOCK, ILOCK_REF_BUFFER__HERC_HAS_INTERLOCK, body_len, body_data);
            }

            // We couldn't understand the message:
            if (bytes_used == 0)
            {
                cb_dm_InterlockUpdate(RadioUdpInterlockStatus::BAD_MESSAGE);
            }

            return bytes_used;
        }

        /**
         * @brief Handle Error Radio-Hercules Direct Message.
         *
         * @param body_len Length of body contents (header length excluded)
         * @param body_data Pointer to start of message body (with any header removed)
         * @return Returns the number of bytes used (0 if no valid body was found).
         */
        uint8_t DirectMessageDriver::handleErrorMessage(uint8_t body_len, uint8_t *body_data)
        {
            // TODO: (rn, we don't do anything with this info or strictly need it - right now we just pass it to WD to be passed to Ground if we're on the lander).
            return body_len; // just say all of it was valid
        }

        /**
         * @brief Handle Fault Radio-Hercules Direct Message.
         *
         * @param body_len Length of body contents (header length excluded)
         * @param body_data Pointer to start of message body (with any header removed)
         * @return Returns the number of bytes used (0 if no valid body was found).
         */
        uint8_t DirectMessageDriver::handleFaultMessage(uint8_t body_len, uint8_t *body_data)
        {
            // TODO: (rn, we don't do anything with this info or strictly need it - right now we just pass it to WD to be passed to Ground if we're on the lander).
            return body_len; // just say all of it was valid
        }
    }
}
