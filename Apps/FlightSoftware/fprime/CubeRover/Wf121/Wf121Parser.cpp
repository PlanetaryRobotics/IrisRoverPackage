#include "Wf121Parser.hpp"
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Time/Time.hpp>

namespace Wf121::Wf121Parser
{

    /**
     * `GenericMessage` Definitions:
     **/
    GenericMessage::GenericMessage(uint8_t *buffer, size_t bufferCapacity)
        : headerType(HeaderType::INVALID),
          header(0),
          payloadBuffer(buffer),
          payloadBufferCapacity(bufferCapacity),
          payloadSize(0)
    {
        // Shamelessly stolen debugging idea from `WatchDogMspm`:
        // Pre-fill buffer with fixed value.
        // (this isn't necessary but should make debugging easier)
        memset(payloadBuffer, 0xE8, payloadBufferCapacity);
    }

    void GenericMessage::reset()
    {
        header = 0;
        memset(payloadBuffer, 0xE8, payloadBufferCapacity);
        payloadSize = 0;
    }

    void GenericMessage::updateHeaderType()
    {
        if (memcmp(this->header, DM_HEADER, DM_HEADER_LEN) == 0)
        {
            // This is a valid Radio-Herc Direct Message Header (and, therefore, not a BGAPI Header)
            this->headerType = HeaderType::DIRECT_MESSAGE;
        }
        else if (
            bgApiHeader()->bit.technologyType == TT_WIFI &&
            (bgApiHeader()->bit.msgType == CMD_RSP_TYPE || bgApiHeader()->bit.msgType == EVENT_TYPE) &&
            bgApiHeader()->bit.classId <= CLASS_WIRED_ETHERNET)
        {
            // This is a valid BGAPI Header:
            this->headerType = HeaderType::BGAPI;
        }
        else
        {
            // Else, this isn't a valid header for any recognized message type
            // (likely out of sync):
            this->headerType = HeaderType::INVALID;
        }
    }

    /**
     * `Mpsm` Definitions:
     **/
    Mpsm::Mpsm() : m_currentState(Mpsm::State::WAITING_FOR_VALID_HEADER),
                   m_headerBuffer(),
                   m_numPayloadBytesExpected(0),
                   m_completedHeaderTimeMs(0) {}

    Mpsm::ProcessStatus process(GenericMessage &msg, uint8_t newByte)
    {
        // If we're currently waiting for post-header data and the time since
        // we completed the header is >WF121_MPSM_FULL_PAYLOAD_TIMEOUT_MS, then
        // give up and treat this new byte as part of a header search activity:
        if (m_currentState != Mpsm::State::WAITING_FOR_VALID_HEADER)
        {
            Fw::Time now = getTime();
            uint32_t nowMillis = static_cast<uint32_t>(now.get_time_ms());
            if (nowMillis - m_completedHeaderTimeMs >= WF121_MPSM_FULL_PAYLOAD_TIMEOUT_MS)
            {
                // Reset state, clear the message, and reset the header buffer:
                // (essentially, start over since we're assuming the previous
                // message was a wash and now we're receiving fresh data):
                m_currentState = Mpsm::State::WAITING_FOR_VALID_HEADER;
                msg.reset();
                m_headerBuffer.reset();
            }
        }

        // Core State Driver:
        switch (m_currentState)
        {
        // Keep loading 1 Bytes until a valid header is found:
        case Mpsm::State::WAITING_FOR_VALID_HEADER:
            // Add the byte to the buffer:
            m_headerBuffer.enqueue(newByte);

            // Check if the header buffer is now full:
            if (m_headerBuffer.is_full())
            {
                // Read the bytes out in the correct order into the message header:
                m_headerBuffer.straighten_into((uint8_t *)(&msg.header));
                // Check if the header is valid:
                msg.updateHeaderType();
                if (msg.headerType = HeaderType::BGAPI)
                {
                    // Set how many bytes we are going to wait for:
                    m_numPayloadBytesExpected = getBgapiPayloadSize(msg.bgApiHeader());
                    // and make sure that's reasonable (final check to see if this is a valid BGAPI header):
                    if (m_numPayloadBytesExpected > WF121_BGAPI_MAX_PACKET_SIZE || m_numPayloadBytesExpected > msg.payloadBufferCapacity)
                    {
                        // Expecting more data than we can load (than max message size).
                        // This is a problem and suggests corrupted data.
                        // Keep waiting for a valid header and return bad header flag:
                        m_currentState = Mpsm::State::WAITING_FOR_VALID_HEADER;
                        return Mpsm::ProcessStatus::BAD_LENGTH;
                    }
                    // We got a valid BGAPI header! Proceed to next state:
                    // (next time we'll be waiting for payload bytes to load):
                    m_currentState = Mpsm::State::BGAPI_WAITING_FOR_PAYLOAD;
                    // Now that we're done with it, clear the header buffer
                    // (so it's fresh for the next use):
                    m_headerBuffer.reset();
                    // Log the time we got the complete header:
                    m_completedHeaderTimeMs = static_cast<uint32_t>(getTime().get_time_ms());
                    return Mpsm::ProcessStatus::BGAPI_HEADER_PARSED;
                }
                else if (msg.headerType = HeaderType::DIRECT_MESSAGE)
                {
                    // We got a valid DIRECT_MESSAGE header! Proceed to next state:
                    // (next time we'll be waiting for payload bytes to load):
                    m_currentState = Mpsm::State::DM_WAITING_FOR_LEN_BYTE;
                    // Now that we're done with it, clear the header buffer
                    // (so it's fresh for the next use):
                    m_headerBuffer.reset();
                    // Log the time we got the complete header:
                    m_completedHeaderTimeMs = static_cast<uint32_t>(getTime().get_time_ms());
                    return Mpsm::ProcessStatus::DM_HEADER_PARSED;
                }
                else
                {
                    // Got a header's worth of bytes but it wasn't recognized as valid:
                    // So, keep the state as `WAITING_FOR_VALID_HEADER` and:
                    // Dequeue the oldest byte (so we can keep scanning through
                    // the buffer until we get a valid header):
                    m_headerBuffer.dequeue();
                    return Mpsm::ProcessStatus::BAD_HEADER;
                }
            }
            break;

        // Waiting for the Radio-Hercules Direct Message Length Byte:
        case Mpsm::State::DM_WAITING_FOR_LEN_BYTE:
            // We now have a valid DIRECT_MESSAGE header and length byte! Proceed to next state:
            // (next time we'll be waiting for payload bytes to load):
            m_currentState = Mpsm::State::BGAPI_WAITING_FOR_PAYLOAD;
            // Set how many bytes we are going to wait for:
            m_numPayloadBytesExpected = newByte;
            if (m_numPayloadBytesExpected > DM_MAX_PAYLOAD_LEN || m_numPayloadBytesExpected > msg.payloadBufferCapacity)
            {
                // Expecting more data than we can load (than max message size).
                // This is a problem and suggests corrupted data.
                // Go back to waiting for a valid header and return bad header flag:
                m_currentState = Mpsm::State::WAITING_FOR_VALID_HEADER;
                return Mpsm::ProcessStatus::BAD_LENGTH;
            }
            return Mpsm::ProcessStatus::DM_LEN_PARSED;
            break;

        // Waiting for all the Radio-Hercules Direct Message Payload bytes to arrive (as indicated by length byte):
        case Mpsm::State::DM_WAITING_FOR_PAYLOAD:
        // Waiting for all the WF121 BGAPI Payload bytes to arrive (as indicated by header):
        case Mpsm::State::BGAPI_WAITING_FOR_PAYLOAD:
            // Handle both of these (^) states the same way:

            // Make sure we're not about to overflow a buffer:
            if ((msg.payloadSize + 1) > msg.payloadBufferCapacity)
            {
                // We have a problem and are trying to overfill the buffer
                // (and we already checked that the length was
                // <=payloadBufferCapacity, so likely there's been some
                // bitflipping):
                return Mpsm::ProcessStatus::POSSIBLE_CORRUPTION;
            }

            // Add byte to message:
            msg.payloadBuffer[msg.payloadSize] = newData;
            msg.payloadSize++;

            // Check if we've finished reading the data portion, and if so reset:
            if (m_numPayloadBytesExpected == msg.payloadSize)
            {
                // Reset the state machine:
                m_currentState = Mpsm::State::WAITING_FOR_VALID_HEADER;

                // Tell the caller what we got:
                if (msg.headerType == HeaderType::DIRECT_MESSAGE)
                {
                    return Mpsm::ProcessStatus::DM_PARSED;
                }
                else if (msg.headerType == HeaderType::BGAPI)
                {
                    return Mpsm::ProcessStatus::BGAPI_PARSED;
                }
                else
                {
                    // wtf? shouldn't be here.
                    // Something went wrong. Go back to start and reset:
                    m_currentState = Mpsm::State::WAITING_FOR_VALID_HEADER;
                    msg.reset();
                    m_headerBuffer.reset();
                    return Mpsm::ProcessStatus::POSSIBLE_CORRUPTION;
                }
            }
            else
            {
                return Mpsm::ProcessStatus::WAITING_FOR_MORE_DATA;
            }
            break;

        default:
            // How did we get here? This shouldn't be possible...
            assert(false);
            return Mpsm::ProcessStatus::POSSIBLE_CORRUPTION;
        }
    }
}