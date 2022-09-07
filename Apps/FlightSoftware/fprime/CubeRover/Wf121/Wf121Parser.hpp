/**
 * @file Wf121Parser.hpp
 * @author Connor Colombo (colombo@cmu.edu)
 * @brief Message Parsing State Machine for Wifi BGAPI messages from the WF121
 * chip.  See `Mpsm.puml` for PlantUML diagram of implemented system.
 *
 * Also allows for reading proprietary direct messages from the Radio that
 * follow the format below. This format's header has no bytes in common with a
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
 * NOTE: On the Hercules side, **ALL DIRECT MESSAGES WILL BE PRECEDED BY AN "evt_endpoint_data"
 * BGAPI header (and the endpoint ID for Hercules UART (1) and 1B BGAPI payload length).**
 *
 * (See: `Wf121DirectMessage.hpp` for more details.)
 *
 * @date 2022-08-11
 */

#ifndef CUBEROVER_WF121_WF121_PARSER_HPP_
#define CUBEROVER_WF121_WF121_PARSER_HPP_

#include <cstring>
#include <cassert>
#include <stdint.h>
#include <stddef.h>

#include "sci.h"
#include "Wf121BgApi.hpp"
#include "SimpleAsyncFifoBuffer.hpp"

namespace Wf121{namespace Wf121Parser // Wf121::Wf121Parser
{
    // *Generic Message Structure:*
    // Message Header Types:
    enum class HeaderType
    {
        BGAPI,
        DIRECT_MESSAGE,
        INVALID
    };
    HeaderType getHeaderType(uint8_t *header);

    // Generic container for a message received from the WF121:
    struct GenericMessage
    {
        // Type of message as indicated by the header:
        HeaderType headerType;
        // Header received:
        uint32_t header;

        // Pointer to a buffer containing the payload:
        uint8_t *payloadBuffer;
        // Maximum capacity of that buffer:
        size_t payloadBufferCapacity;
        // Size of the payload in this message:
        size_t payloadSize;

        explicit GenericMessage(uint8_t *buffer, size_t bufferCapacity);

        /**
         * @brief Resets the buffer to its original state.
         *
         * NOTE: This does zero-out the contents of `dataBuffer`, but this
         * message will continue to use the same buffer location even after
         * being reset.
         */
        void reset();

        /**
         * @brief Returns a pointer to the header, cast as a BGAPI Header.
         */
        inline BgApi::BgApiHeader *bgApiHeader()
        {
            return (BgApi::BgApiHeader *)&this->header;
        }

        /**
         * @brief Updates the `headerType` based on the current `header`
         * data.
         * Call this after reading new data into `header`.
         */
        void updateHeaderType();
    };

    // *Iris-Radio Direct Message Info:*
    // Fixed header used on Radio-Herc Direct Messages:
    const uint8_t DM_HEADER[] = {0xE6, 0xE7, 0xE7, 0xE6};
    const uint8_t DM_HEADER_LEN = 4;
    const uint8_t DM_MAX_PAYLOAD_LEN = 255 - DM_HEADER_LEN - 1;
    // Radio's BGAPI Endpoint for Hercules (so we know if a BGAPI message is for us):
    const uint8_t BGAPI_HERCULES_ENDPOINT = 0x01;

    /**
     * @brief Helper function to compute the BGAPI payload size from a BGAPI header.
     */
    inline uint16_t getBgapiPayloadSize(BgApi::BgApiHeader *bgApiHeader)
    {
        return bgApiHeader->bit.lengthLow + (bgApiHeader->bit.lengthHigh << 8);
    }

// *Message Parsing State Machine:*
#define WF121_MPSM_FULL_PAYLOAD_TIMEOUT_MS 2250 // 2.25 seconds

    // WF121 Message Parsing State Machine:
    class Mpsm
    {
    public:
        // The State of the MPSM:
        enum class State
        {
            // Keep loading 1 Bytes until a valid BGAPI header is found
            WAITING_FOR_VALID_BGAPI_HEADER,
            // Wait for all the WF121 BGAPI Payload bytes to arrive (as indicated by header):
            BGAPI_WAITING_FOR_PAYLOAD,
            // Wait for just the endpoint ID of an "evt_endpoint_data" BGAPI packet (only used when the payload might be a DM and we need to find out):
            BGAPI_WAITING_FOR_ENDPOINT_ID,
            // Wait for just the length information of a BGAPI "uint8array" payload (only used when the payload is a DM):
            BGAPI_WAITING_FOR_DM_PACKET_LEN,
            // We've got a BGAPI packet that suggests we should be getting a DM next. Waiting for the header:
            WAITING_FOR_VALID_DM_HEADER,
            // Wait for the Radio-Hercules Direct Message Length Byte:
            DM_WAITING_FOR_LEN_BYTE,
            // Wait for all the Radio-Hercules Direct Message Payload bytes to arrive (as indicated by length byte):
            DM_WAITING_FOR_PAYLOAD,
        };

        // What happened during this processing step:
        enum class ProcessStatus
        {
            // Possible data corruption was noticed (this should eval to 0x00):
            POSSIBLE_CORRUPTION = 0,
            // Nothing we're waiting for a byte:
            WAITING_FOR_MORE_DATA,
            // BGAPI information was good but it's too early for us to accept it
            // since we don't know we're in sync yet (no DM Heartbeat yet):
            // (before this point we only accept a BGAPI message if it contains a DM,
            // i.e. is a "evt_endpoint_data" message):
            PREMATURE_BGAPI,
            // We had enough bytes to read a header but we didn't recognize it,
            // so we dequeued a byte and will be trying again next call:
            BAD_HEADER,
            // Bad Expected Payload Length (longer than possible):
            BAD_LENGTH,
            // We parsed a valid DIRECT_MESSAGE Header:
            DM_HEADER_PARSED,
            // We parsed a DIRECT_MESSAGE Length Byte:
            DM_LEN_PARSED,
            // We parsed a valid BGAPI Header:
            BGAPI_HEADER_PARSED,
            // We got an endpoint in a "evt_endpoint_data" BGAPI message that suggests the contents are a DM:
            DM_BGAPI_ENDPOINT_PARSED,
            // We got an endpoint in a "evt_endpoint_data" BGAPI message with our endpoint with a length byte that makes sense:
            DM_BGAPI_PACKET_LENGTH_PARSED,
            // We parsed a complete Radio-Hercules Direct Message:
            DM_PARSED,
            // We parsed a complete BGAPI Message:
            BGAPI_PARSED
        };

        Mpsm::State m_currentState;

        // Buffer used to scan for BGAPI and Radio-Herc DM Headers (both are 4B):
        SimpleAsyncFifoBuffer<uint8_t, 4> m_headerBuffer;

        // Number of payload bytes we expect (are going to wait for):
        uint16_t m_numPayloadBytesExpected;

        // Expected size of the DM payload (based on the corresponding BGAPI header):
        // We check this number against the number in the DM packet to make sure both
        // the DM and BGAPI headers agree, so we can be extra sure we've aligned on a message:
        uint8_t m_expectedTotalDmSize;

        // Time, in ms since boot, that we last successfully parsed a header:
        // (We expect the rest of the message to arrive before `WF121_MPSM_FULL_PAYLOAD_TIMEOUT_MS`)
        uint32_t m_completedHeaderTimeMs;

        // Constructor:
        Mpsm();

        // Use the given new byte to advance the state machine:
        Mpsm::ProcessStatus process(GenericMessage &msg, uint8_t newByte);
    };

}} // Wf121::Wf121Parser
#endif /* CUBEROVER_WF121_WF121_PARSER_HPP_ */