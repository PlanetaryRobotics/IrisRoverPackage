#include <CubeRover/Wf121/Wf121DirectMessage.hpp>

#include <Fw/Time/Time.hpp>
#include <cstring>

// Anonymous namespace for file-scope helper functions:
namespace {
    // Returns the length of the given fixed buffer containing a
    // null-terminated string:
    uint8_t getStrBufferLen(const uint8_t* buf){
        return strlen((const char*) buf);
    }
}

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

    // Processes the given message. Returns whether the message contained a
    // known format (and wasn't just plain text being passed along):
    bool DirectMessageDriver::process(uint8_t msg_len, uint8_t* msg_data){
        // Make sure message isn't too short:
        if(msg_len < FIXED_HEADER_LEN){
            return false;
        }

        // Dispatch to the appropriate handler based on known fixed-headers:
        if(memcmp(msg_data, HEARTBEAT_HEADER, FIXED_HEADER_LEN) == 0){
            return handleHeartbeatMessage(msg_len - HEARTBEAT_HEADER, msg_data + FIXED_HEADER_LEN);
        }
        if(memcmp(msg_data, STATE_HEADER, FIXED_HEADER_LEN) == 0){
            return handle_stateMessage(msg_len - HEARTBEAT_HEADER, msg_data + FIXED_HEADER_LEN);
        }
        if(memcmp(msg_data, DOING_HEADER, FIXED_HEADER_LEN) == 0){
            return handleDoingMessage(msg_len - HEARTBEAT_HEADER, msg_data + FIXED_HEADER_LEN);
        }
        if(memcmp(msg_data, ERROR_HEADER, FIXED_HEADER_LEN) == 0){
            return handleErrorMessage(msg_len - HEARTBEAT_HEADER, msg_data + FIXED_HEADER_LEN);
        }
        if(memcmp(msg_data, FAULT_HEADER, FIXED_HEADER_LEN) == 0){
            return handleFaultMessage(msg_len - HEARTBEAT_HEADER, msg_data + FIXED_HEADER_LEN);
        }

        // Didn't match any known pattern (would have returned above if so):
        return false;
    }

    

    /**
     * @brief Handle Heartbeat Radio-Hercules Direct Message.
     * 
     * Heartbeat message body is special and follows the format:
     *  - [state message]
     *  - '>' separator
     *  - [doing message]
     * 
     * @param body_len Length of body contents (header length excluded)
     * @param body_data Pointer to start of message body (with any header removed)
     * @return Returns the number of bytes used (0 if no valid message was found).
     */
    uint8_t DirectMessageDriver::handleHeartbeatMessage(uint8_t body_len, uint8_t* body_data){
        bool valid = true;
        uint8_t bytes_used = 0;

        // Grab the state message:
        uint8_t state_bytes = DirectMessageDriver::handleStateMessage(body_len, body_data);
        valid &= state_bytes > 0;
        if(!valid){ return 0; }
        bytes_used += state_bytes;

        // State message is followed by a valid separator:
        valid &= (body_data[bytes_used] == ':' || body_data[bytes_used] == '>');
        if(!valid){ return 0; }
        bytes_used += 1;

        // Grab the doing message that follows the separator:
        uint8_t doing_bytes = DirectMessageDriver::handleDoingMessage(body_len, body_data);
        valid &= doing_bytes > 0;
        if(!valid){ return 0; }
        bytes_used += doing_bytes;

        // If valid, update heartbeat time:
        if(valid){
            Fw::Time now = getTime();
            m_timeOfLastHeartbeatMs = static_cast<uint32_t>(now.get_time_ms());
        }

        return bytes_used;
    }



    /**
     * @brief Helper functions that checks if the given `body_data` buffer
     * matches (or starts with) the given state_buffer. If it does, it sets
     * the state to `state` and returns the number of bytes used (length of
     * the null-terminated `state_buf`).
     * 
     * @param state State to assign if there's a match.
     * @param state_buf State buffer to check for. 
     * @param body_len Length of buffer to check.
     * @param body_data Buffer to check.
     * @return uint8_t Number of bytes used (0 if no match).
     */
    uint8_t DirectMessageDriver::checkAndAssignState(RadioSwState state, uint8_t* state_buf, uint8_t body_len, uint8_t* body_data){
        uint8_t state_len = getStrBufferLen(state_buf);
        if(
            body_len >= state_len &&
            memcmp(body_data, state_buf, state_len) == 0
        ){
            this->m_currentRadioState = state;
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
     * @return Returns the number of bytes used (0 if no valid body was found).
     */
    uint8_t DirectMessageDriver::handleStateMessage(uint8_t body_len, uint8_t* body_data){
        // Set state based on state message and return number of bytes used:
        uint8_t bytes_used = 0;

        // BOOT STATE:
        if(bytes_used == 0) { // no match found yet
            bytes_used = checkAndAssignState(RadioSwState::BOOT,             STATE_REF_BUFFER__BOOT, body_len, body_data);
        }
        // INIT STATE:
        if(bytes_used == 0) { // no match found yet
            bytes_used = checkAndAssignState(RadioSwState::INIT,             STATE_REF_BUFFER__INIT, body_len, body_data);
        }
        // WIFI_ON STATE:
        if(bytes_used == 0) { // no match found yet
            bytes_used = checkAndAssignState(RadioSwState::WIFI_ON,          STATE_REF_BUFFER__WIFI_ON, body_len, body_data);
        }
        // CONNECTED STATE:
        if(bytes_used == 0) { // no match found yet
            bytes_used = checkAndAssignState(RadioSwState::CONNECTED,        STATE_REF_BUFFER__CONNECTED, body_len, body_data);
        }
        // UDP_CONNECTED STATE:
        if(bytes_used == 0) { // no match found yet
            bytes_used = checkAndAssignState(RadioSwState::UDP_CONNECTED,    STATE_REF_BUFFER__UDP_CONNECTED, body_len, body_data);
        }

        return bytes_used;
    }



    /**
     * @brief Helper functions that checks if the given `body_data` buffer
     * matches (or starts with) the given `doing_buf`. If it does, it sets
     * the current activity to `doing` and returns the number of bytes used
     * (length of the null-terminated `doing_buf`).
     * 
     * @param doing "Doing" activity to assign if there's a match.
     * @param doing_buf "Doing" activity buffer to check for. 
     * @param body_len Length of buffer to check.
     * @param body_data Buffer to check.
     * @return uint8_t Number of bytes used (0 if no match).
     */
    uint8_t checkAndAssignDoingActivity(RadioSwActivity doing, uint8_t* doing_buf, uint8_t body_len, uint8_t* body_data){
        uint8_t doing_len = getStrBufferLen(doing_buf);
        if(
            body_len >= doing_len &&
            memcmp(body_data, doing_buf, doing_len) == 0
        ){
            this->m_currentRadioActivity = doing;
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
     * @return Returns the number of bytes used (0 if no valid body was found).
     */
    uint8_t DirectMessageDriver::handleDoingMessage(uint8_t body_len, uint8_t* body_data){
        // Set activity based on state message and return number of bytes used:
        uint8_t bytes_used = 0;

        // TURNING_WIFI_ON:
        if(bytes_used == 0) { // no match found yet
            bytes_used = checkAndAssignDoingActivity(RadioSwActivity::TURNING_WIFI_ON,          DOING_REF_BUFFER__TURNING_WIFI_ON, body_len, body_data);
        }
        // CONNECTING_NETWORK:
        if(bytes_used == 0) { // no match found yet
            bytes_used = checkAndAssignDoingActivity(RadioSwActivity::CONNECTING_NETWORK,       DOING_REF_BUFFER__CONNECTING_NETWORK, body_len, body_data);
        }
        // STARTING_UDP:
        if(bytes_used == 0) { // no match found yet
            bytes_used = checkAndAssignDoingActivity(RadioSwActivity::STARTING_UDP,             DOING_REF_BUFFER__STARTING_UDP, body_len, body_data);
        }
        // REMAINING_VIGILENT:
        if(bytes_used == 0) { // no match found yet
            bytes_used = checkAndAssignDoingActivity(RadioSwActivity::REMAINING_VIGILENT,       DOING_REF_BUFFER__REMAINING_VIGILENT, body_len, body_data);
        }
        // WAITING_THEN_CORRECTING:
        if(bytes_used == 0) { // no match found yet
            bytes_used = checkAndAssignDoingActivity(RadioSwActivity::WAITING_THEN_CORRECTING,  DOING_REF_BUFFER__WAITING_THEN_CORRECTING, body_len, body_data);
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
    uint8_t DirectMessageDriver::handleErrorMessage(uint8_t body_len, uint8_t* body_data){
        // TODO: (rn, we don't do anything with this info or strictly need it).
        return body_len; // just say all of it was valid
    }

    /**
     * @brief Handle Fault Radio-Hercules Direct Message.
     * 
     * @param body_len Length of body contents (header length excluded)
     * @param body_data Pointer to start of message body (with any header removed)
     * @return Returns the number of bytes used (0 if no valid body was found).
     */
    uint8_t DirectMessageDriver::handleFaultMessage(uint8_t body_len, uint8_t* body_data){
        // TODO: (rn, we don't do anything with this info or strictly need it).
        return body_len; // just say all of it was valid
    }
}