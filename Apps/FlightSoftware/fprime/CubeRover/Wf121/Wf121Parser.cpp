#include "Wf121Parser.hpp"
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <CubeRover/Wf121/Wf121.hpp>
#include <CubeRover/Wf121/Timestamp.hpp>
#include <CubeRover/Wf121/Wf121DirectMessage.hpp>
#include "FreeRTOS.h"
#include "os_task.h"

#include <string.h>

#include <CubeRover/WatchDogInterface/WatchDogInterface.hpp>
extern CubeRover::WatchDogInterfaceComponentImpl watchDogInterface;

namespace Wf121
{
    namespace Wf121Parser // Wf121::Wf121Parser
    {
        // Prefix used on
        static const uint8_t bgapiPassthroughPrefix[] = "BGP:";
        static const uint8_t bgapiPassthroughPrefixLen = 4;
        // Maximum size of a BGAPI packet that will be passed through.
        // Above this, it'll be dropped (not passed through to ground. Still
        // will be processed by Hercules):
        // Needs to be << the size of the debug printf buffer in the WD and <
        // m_printBuffer in WatchDogInterface, accounting for all prefixes:
        // (-5 for `DEBUG` prefix on debug packets)
        static const uint16_t BGAPI_PASSTHROUGH_PACKET_MAX_SIZE = 255 - 5 - bgapiPassthroughPrefixLen;

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
            if (memcmp((void *)&(this->header), (void *)DM_HEADER, DM_HEADER_LEN) == 0)
            {
                // This is a valid Radio-Herc Direct Message Header (and, therefore, not a BGAPI Header)
                this->headerType = HeaderType::DIRECT_MESSAGE;
            }
            else if (
                bgApiHeader()->bit.technologyType == BgApi::TechnologyType::TT_WIFI &&
                (bgApiHeader()->bit.msgType == BgApi::MsgType::CMD_RSP_TYPE || bgApiHeader()->bit.msgType == BgApi::MsgType::EVENT_TYPE) &&
                bgApiHeader()->bit.classId <= BgApi::CommandClass::CLASS_WIRED_ETHERNET)
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
        Mpsm::Mpsm() : m_currentState(Mpsm::State::WAITING_FOR_VALID_BGAPI_HEADER),
                       m_headerBuffer(),
                       m_numPayloadBytesExpected(0),
                       m_expectedTotalDmSize(0),
                       m_completedHeaderTimeMs(0) {}

        Mpsm::ProcessStatus Mpsm::process(GenericMessage &msg, uint8_t newByte)
        {
            // To promote faster syncing, keep scanning until a DM heartbeat
            // (b'\xE6\xE7\xE7\xE6\xXXthump') is seen on full boot.
            // Then, allow other non-DM header types too (just too slow to sync
            // if accepting any valid-looking BGAPI header - not enough entropy).
            static bool have_seen_a_dm_heartbeart_since_boot = false;

            //            printf("s: %d\n", m_currentState);
            // NOTE: This seems to be breaking things *and* isn't strictly necessary. For the time being, leave this out/
            // ^ TODO: [CWC] Figure out why this was broken and fix it.
            //            // If we're currently waiting for post-header data and the time since
            //            // we completed the header is >WF121_MPSM_FULL_PAYLOAD_TIMEOUT_MS, then
            //            // give up and treat this new byte as part of a header search activity:
            //            if (m_currentState != Mpsm::State::WAITING_FOR_VALID_HEADER)
            //            {
            //                uint32_t nowMillis = Timestamp::getTimeMs();
            //                if ((nowMillis - m_completedHeaderTimeMs) >= WF121_MPSM_FULL_PAYLOAD_TIMEOUT_MS)
            //                {
            //                    // Reset state, clear the message, and reset the header buffer:
            //                    // (essentially, start over since we're assuming the previous
            //                    // message was a wash and now we're receiving fresh data):
            //                    m_currentState = Mpsm::State::WAITING_FOR_VALID_HEADER;
            //                    msg.reset();
            //                    m_headerBuffer.reset();
            //                }
            //            }

            Mpsm::ProcessStatus returnStatus = Mpsm::ProcessStatus::WAITING_FOR_MORE_DATA; // default

            if (!have_seen_a_dm_heartbeart_since_boot && Wf121::persistentBgApiPassthroughEnabled())
            {
                // If BGAPI passthrough is enabled, for the state machine to
                // think it's synced (so all BGAPI messages, even pre-sync ones, make it through):
                have_seen_a_dm_heartbeart_since_boot = true;
            }

            // Core State Driver:
            switch (m_currentState)
            {
            // Keep loading 1 Byte until a valid BGAPI header is found:
            case Mpsm::State::WAITING_FOR_VALID_BGAPI_HEADER:
                // Make sure there's room in the header for the new byte:
                if (m_headerBuffer.is_full())
                {
                    m_headerBuffer.dequeue();
                }
                // Add the byte to the buffer:
                m_headerBuffer.enqueue(newByte);

                // Check if the header buffer is now full:
                if (!m_headerBuffer.is_full())
                {
                    returnStatus = Mpsm::ProcessStatus::WAITING_FOR_MORE_DATA;
                }
                else
                {
                    // Read the bytes out in the correct order into the message header:
                    m_headerBuffer.straighten_into((uint8_t *)(&msg.header));
                    // Check if the header is valid:
                    msg.updateHeaderType();

                    if (msg.headerType == HeaderType::BGAPI)
                    {
                        // Set how many bytes we are going to wait for:
                        m_numPayloadBytesExpected = getBgapiPayloadSize(msg.bgApiHeader());
                        // and make sure that's reasonable (final check to see if this is a valid BGAPI header):
                        if (m_numPayloadBytesExpected > BgApi::MAX_PACKET_SIZE || m_numPayloadBytesExpected > msg.payloadBufferCapacity)
                        {
                            // Expecting more data than we can load (than max message size).
                            // This is a problem and suggests corrupted data.
                            // Keep waiting for a valid header and return bad length flag:
                            m_currentState = Mpsm::State::WAITING_FOR_VALID_BGAPI_HEADER;
                            returnStatus = Mpsm::ProcessStatus::BAD_LENGTH;
                        }
                        else
                        {
                            // We got a valid BGAPI header! Proceed to next state:
                            bool headerIsAcceptable = false;
                            // Check if it's a "evt_endpoint_data" message (i.e. if it could contain a DM):
                            if (msg.bgApiHeader()->bit.msgType == BgApi::MsgType::EVENT_TYPE &&
                                msg.bgApiHeader()->bit.classId == BgApi::CommandClass::CLASS_ENDPOINT &&
                                msg.bgApiHeader()->bit.cmdId == 0x01)
                            {
                                // It's an endpoint data event (could be a DM), so we'll check:
                                // (next time we'll be waiting for the first byte of the "evt_endpoint_data" BGAPI payload,
                                // the endpoint ID, to load):
                                m_currentState = Mpsm::State::BGAPI_WAITING_FOR_ENDPOINT_ID;
                                headerIsAcceptable = true;
                            }
                            else if (have_seen_a_dm_heartbeart_since_boot)
                            {
                                // We'll only accept other BGAPI messages if we've seen a DM Heartbeat (so we know we're aligned and in sync):
                                // (next time we'll be waiting for payload bytes to load):
                                m_currentState = Mpsm::State::BGAPI_WAITING_FOR_PAYLOAD;
                                headerIsAcceptable = true;
                            }
                            else
                            {
                                // We can't accept this BGAPI message because we can't be sure we're in sync (haven't seen a heartbeat yet):
                                m_currentState = Mpsm::State::WAITING_FOR_VALID_BGAPI_HEADER;
                                returnStatus = Mpsm::ProcessStatus::PREMATURE_BGAPI;
                                headerIsAcceptable = false;
                            }

                            // Perform standard post-header procedure if the header was accepted:
                            if (headerIsAcceptable)
                            {
                                // Reset the head of the message payload body (i.e. we currently have 0B of payload):
                                msg.payloadSize = 0;
                                // Now that we're done with it, clear the header buffer
                                // (so it's fresh for the next use):
                                m_headerBuffer.reset();
                                // Log the time we got the complete header:
                                m_completedHeaderTimeMs = Timestamp::getTimeMs();
                                returnStatus = Mpsm::ProcessStatus::BGAPI_HEADER_PARSED;
                            }
                        }
                    }
                    else
                    {
                        if (Wf121::persistentBgApiPassthroughEnabled())
                        {
                            // Report the Bad Header if in BGAPI passthrough mode:
                            watchDogInterface.debugPrintfToWatchdog("BGBH: %#04x", msg.header);
                        }
                        //                        watchDogInterface.debugPrintfToWatchdog("RADIO: Bad head: %#04x\n", msg.header);
                        // Got a header's worth of bytes but it wasn't recognized as valid:
                        // (or it's too early to risk accepting it as valid in the case of BGAPI)
                        // So, keep the state as `WAITING_FOR_VALID_HEADER` and:
                        // Dequeue the oldest byte (so we can keep scanning through
                        // the buffer until we get a valid header):
                        m_headerBuffer.dequeue();
                        returnStatus = Mpsm::ProcessStatus::BAD_HEADER;
                    }
                }
                break;

            case Mpsm::State::BGAPI_WAITING_FOR_ENDPOINT_ID:
                if (newByte == BGAPI_HERCULES_ENDPOINT)
                {
                    // Then this is a DM, move on to loading the expected length:
                    m_currentState = Mpsm::State::BGAPI_WAITING_FOR_DM_PACKET_LEN;
                    // Signal what we found
                    returnStatus = Mpsm::ProcessStatus::DM_BGAPI_ENDPOINT_PARSED;
                }
                else if (have_seen_a_dm_heartbeart_since_boot)
                {
                    // This is just a normal evt_endpoint_data BGAPI packet, and
                    // we'll only accept it if we know we're in sync (we've seen a DM HB):

                    // To accept it, tell the SM we're now waiting for a payload:
                    m_currentState = Mpsm::State::BGAPI_WAITING_FOR_PAYLOAD;
                    // Add byte to message (b/c this was the first byte of the BGAPI payload):
                    msg.payloadBuffer[msg.payloadSize] = newByte;
                    msg.payloadSize++;
                    // Signal that we're just looking for more data:
                    returnStatus = Mpsm::ProcessStatus::WAITING_FOR_MORE_DATA;
                }
                else
                {
                    // It's too early for us to consider processing this BGAPI message, return to start:
                    m_currentState = Mpsm::State::WAITING_FOR_VALID_BGAPI_HEADER;
                    // Signal what we found:
                    returnStatus = Mpsm::ProcessStatus::PREMATURE_BGAPI;
                    // Since it's possible we're out of sync and, for all we know, this byte should
                    // have been part of a header so we'll enqueue it before going back:
                    m_headerBuffer.enqueue(newByte);
                    // NOTE: we don't need to reset the message b/c we haven't done anything with it yet.
                }

                break;

            case Mpsm::State::BGAPI_WAITING_FOR_DM_PACKET_LEN:
                // The new byte is the expected TOTAL Direct Message Size (any valid byte is a valid length
                // here since the total can be up to 0xFF):
                m_expectedTotalDmSize = newByte;
                // This should match the total length of the BGAPI payload -1B for the endpoint ID and -1B for this byte:
                if (m_expectedTotalDmSize == (m_numPayloadBytesExpected - 2))
                {
                    // Now we wait for the actual DM header (must be THE NEXT 4 bytes):
                    m_currentState = Mpsm::State::WAITING_FOR_VALID_DM_HEADER;
                    // Signal what happened:
                    returnStatus = Mpsm::ProcessStatus::DM_BGAPI_PACKET_LENGTH_PARSED;
                    // Make sure the header buffer is empty before proceeding (we'll use its space for this):
                    m_headerBuffer.reset();
                }
                else
                {
                    // Something's wrong and we should start over. Likely, we're out of sync.
                    // For all we know, this byte should have been part of a header so we'll enqueue it and go back:
                    m_headerBuffer.enqueue(newByte);
                    m_currentState = Mpsm::State::WAITING_FOR_VALID_BGAPI_HEADER;
                    returnStatus = Mpsm::ProcessStatus::BAD_LENGTH;
                }
                break;

            case Mpsm::State::WAITING_FOR_VALID_DM_HEADER:
                // Add the byte to the buffer:
                m_headerBuffer.enqueue(newByte);

                // As soon as the header buffer's full, we're done.
                // Until then, we just keep waiting for more data:
                if (!m_headerBuffer.is_full())
                {
                    returnStatus = Mpsm::ProcessStatus::WAITING_FOR_MORE_DATA;
                }
                else
                {
                    // Header buffer is full. It has be a valid DM header,
                    // or we've mislead and we're actually out of sync or parsing corrupt data:

                    // Since we just read straight into this m_headerBuffer's memory from empty,
                    // we can just copy its data into the msg header:
                    memcpy((void *)&msg.header, m_headerBuffer.values, (size_t)m_headerBuffer.size);
                    // Check if the header is valid:
                    msg.updateHeaderType();

                    if (msg.headerType == HeaderType::DIRECT_MESSAGE)
                    {
                        // We got a valid DIRECT_MESSAGE header! Proceed to next state:
                        // (next time we'll be waiting for payload bytes to load):
                        m_currentState = Mpsm::State::DM_WAITING_FOR_LEN_BYTE;
                        // Now that we're done with it, clear the header buffer
                        // (so it's fresh for the next use):
                        m_headerBuffer.reset();
                        // Log the time we got the complete header:
                        m_completedHeaderTimeMs = Timestamp::getTimeMs();
                        returnStatus = Mpsm::ProcessStatus::DM_HEADER_PARSED;
                    }
                    else
                    {
                        // This message was invalid. We've been mislead. Restart:
                        m_currentState = Mpsm::State::WAITING_FOR_VALID_BGAPI_HEADER;
                        returnStatus = Mpsm::ProcessStatus::BAD_HEADER;
                        // NOTE: Don't clear the header buffer. Since we're not actually in sync, some of these
                        // bytes might be part of a real header:
                    }
                }
                break;

            // Waiting for the Radio-Hercules Direct Message Length Byte:
            case Mpsm::State::DM_WAITING_FOR_LEN_BYTE:
                // This is the moment of truth, at this point we've gotten three
                // length estimates for the DM payload:
                //      - `m_numPayloadBytesExpected` of the surrounding BGAPI packet
                //      - `m_expectedTotalDmSize` from the BGAPI "uint8array" that contains the DM
                //      - this `newByte` that indicates the DM length.
                // If they all match (and we got two valid headers), we've certainly found a DM and now we
                // know we're in sync.
                // If not, too bad - we're out of sync and we start over.
                // NOTE: Before this point (back in `BGAPI_WAITING_FOR_DM_PACKET_LEN`), we've already made
                // sure `m_expectedTotalDmSize` and `m_numPayloadBytesExpected` line up, so all we have to
                // do here is check `newByte` against `m_expectedTotalDmSize`:
                if (newByte == (m_expectedTotalDmSize - DM_HEADER_LEN - 1))
                { // -1B this length byte
                    // We now have a valid DIRECT_MESSAGE header and length byte! Proceed to next state:
                    // (next time we'll be waiting for payload bytes to load):
                    m_currentState = Mpsm::State::DM_WAITING_FOR_PAYLOAD;
                    // Set how many bytes we are going to wait for:
                    m_numPayloadBytesExpected = newByte;
                    if (m_numPayloadBytesExpected > DM_MAX_PAYLOAD_LEN ||
                        m_numPayloadBytesExpected > msg.payloadBufferCapacity)
                    {
                        // Expecting more data than we can load (than max message size or our buffer).
                        // This is a problem and suggests corrupted data.
                        // Go back to waiting for a valid header and return bad header flag:
                        m_currentState = Mpsm::State::WAITING_FOR_VALID_BGAPI_HEADER;
                        // Since this wasn't a valid data byte, it's possible we're just out
                        // of sync and it should have been part of a header. So, enqueue this
                        // byte into the header buffer:
                        m_headerBuffer.enqueue(newByte);
                        // NOTE: (^) it's okay that we threw away the rest of the old header when we transitioned to
                        // `DM_WAITING_FOR_LEN_BYTE`. We know this `newByte` and those bytes wouldn't have been part of the same header
                        // since none of the DM header bytes can exist in a BGAPI header (the only other alternative) by design.
                        // So, if this `newByte` was actually part of a header and we're out of sync, it would be the first byte of
                        // that header.
                        returnStatus = Mpsm::ProcessStatus::BAD_LENGTH;
                    }
                    else
                    {
                        // Reset the head of the message payload body (i.e. we currently have 0B of payload):
                        msg.payloadSize = 0;
                        returnStatus = Mpsm::ProcessStatus::DM_LEN_PARSED;
                    }
                }
                else
                {
                    // newByte didn't line up with `m_expectedTotalDmSize`. Likely, we're out of sync.
                    // For all we know, this byte should have been part of a header so we'll enqueue it
                    // and then go back to the start:
                    m_headerBuffer.enqueue(newByte);
                    m_currentState = Mpsm::State::WAITING_FOR_VALID_BGAPI_HEADER;
                    returnStatus = Mpsm::ProcessStatus::BAD_LENGTH;
                }
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
                    return Mpsm::ProcessStatus::POSSIBLE_CORRUPTION; // exit early
                }

                // Add byte to message:
                msg.payloadBuffer[msg.payloadSize] = newByte;
                msg.payloadSize++;

                // Check if we've finished reading the data portion, and if so reset:
                if (msg.payloadSize == m_numPayloadBytesExpected)
                {
                    // Reset the state machine:
                    m_currentState = Mpsm::State::WAITING_FOR_VALID_BGAPI_HEADER;

                    // Tell the caller what we got:
                    if (msg.headerType == HeaderType::DIRECT_MESSAGE)
                    {
                        // Check if it's a DM Heartbeat (and we can now safely say we're in sync):
                        if (msg.payloadSize > 5 && memcmp(msg.payloadBuffer, "thump", 5) == 0)
                        {
                            have_seen_a_dm_heartbeart_since_boot = true;
                        }

                        // Pass whatever the Radio sent us to ground (for surface diagnostics on the Lander):
                        // as long as it's not an ILOCK messsage. At least 2 of those come with every
                        // downlink and since this message would be downlinked, that would mean the downlink
                        // buffer would grow by >=2 packets for every packet sent (it would swamp itself).
                        if (msg.payloadSize < 5 || memcmp(msg.payloadBuffer, "ilock", 5) != 0)
                        {
                            watchDogInterface.debugPrintfBufferWithPrefix((uint8_t *)&msg.header, sizeof(msg.header), msg.payloadBuffer, msg.payloadSize);
                        }

                        returnStatus = Mpsm::ProcessStatus::DM_PARSED;
                    }
                    else if (msg.headerType == HeaderType::BGAPI)
                    {
                        returnStatus = Mpsm::ProcessStatus::BGAPI_PARSED;

                        if (Wf121::persistentBgApiPassthroughEnabled())
                        {
                            // If passthrough is enabled, pass the message out
                            // (so long as its not too big):
                            uint8_t forwardingLen = msg.payloadSize + sizeof(msg.header);
                            if (forwardingLen <= BGAPI_PASSTHROUGH_PACKET_MAX_SIZE)
                            {
                                const uint8_t passthroughHeaderLen = bgapiPassthroughPrefixLen + sizeof(msg.header);
                                uint8_t passthroughHeader[passthroughHeaderLen];
                                // Copy in fixed prefix:
                                memcpy(passthroughHeader, bgapiPassthroughPrefix, bgapiPassthroughPrefixLen);
                                // Copy in BGAPI header:
                                memcpy(passthroughHeader + bgapiPassthroughPrefixLen, (void *)&msg.header, sizeof(msg.header));

                                // Forward message:
                                watchDogInterface.debugPrintfBufferWithPrefix(passthroughHeader, passthroughHeaderLen, msg.payloadBuffer, msg.payloadSize);
                            }
                            else
                            {
                                watchDogInterface.debugPrintfToWatchdog("BGAPI Passthrough TOO-BIG. Got: %dB.", forwardingLen);
                            }
                        }

#ifdef WF121_DEBUG_PRINT_ALL_BGAPI_TO_WD // Activate to see any potentially noteworthy (atypical) BGAPI messages in WD prints.
                        if (!(
                                msg.bgApiHeader()->bit.msgType == BgApi::MsgType::CMD_RSP_TYPE && msg.bgApiHeader()->bit.classId == BgApi::CommandClass::CLASS_ENDPOINT && msg.bgApiHeader()->bit.cmdId == 0 || // ignore send data response
                                msg.bgApiHeader()->bit.msgType == BgApi::MsgType::CMD_RSP_TYPE && msg.bgApiHeader()->bit.classId == BgApi::CommandClass::CLASS_ENDPOINT && msg.bgApiHeader()->bit.cmdId == 5 || // ignore set transmit size response
                                msg.bgApiHeader()->bit.msgType == BgApi::MsgType::EVENT_TYPE && msg.bgApiHeader()->bit.classId == BgApi::CommandClass::CLASS_WIFI && msg.bgApiHeader()->bit.cmdId == 22 ||      // ignore RSSI update
                                msg.bgApiHeader()->bit.msgType == BgApi::MsgType::CMD_RSP_TYPE && msg.bgApiHeader()->bit.classId == BgApi::CommandClass::CLASS_WIFI && msg.bgApiHeader()->bit.cmdId == 19 ||    // ignore RSSI update request
                                msg.bgApiHeader()->bit.classId == BgApi::CommandClass::CLASS_HARDWARE && msg.bgApiHeader()->bit.cmdId == 0 ||                                                                   // ignore timer commands and hooks
                                msg.bgApiHeader()->bit.classId == BgApi::CommandClass::CLASS_CONFIGURATION && msg.bgApiHeader()->bit.cmdId == 0                                                                 // ignore get MAC response and event
                                ))
                        {
                            uint8_t headerDecoding[35];
                            uint8_t headerDecodingHead = 0;

                            // Add a fixed prefix:
                            memcpy(headerDecoding + headerDecodingHead, "RADIO: BG: ", 11);
                            headerDecodingHead += 11;

                            // Add cmd rsp or event:
                            switch (msg.bgApiHeader()->bit.msgType)
                            {
                            case BgApi::MsgType::CMD_RSP_TYPE:
                                headerDecoding[headerDecodingHead] = 'R';
                                break;
                            case BgApi::MsgType::EVENT_TYPE:
                                headerDecoding[headerDecodingHead] = 'E';
                                break;
                            default:
                                headerDecoding[headerDecodingHead] = 'O';
                            }
                            headerDecoding[headerDecodingHead + 1] = ':';
                            headerDecoding[headerDecodingHead + 2] = ' ';
                            headerDecodingHead += 3;

                            // Start with class:
                            switch (msg.bgApiHeader()->bit.classId)
                            {
                            case BgApi::CommandClass::CLASS_SYSTEM:
                                memcpy(headerDecoding + headerDecodingHead, "SYSTEM", 6);
                                headerDecodingHead += 6;
                                break;
                            case BgApi::CommandClass::CLASS_CONFIGURATION:
                                memcpy(headerDecoding + headerDecodingHead, "CFG", 3);
                                headerDecodingHead += 3;
                                break;
                            case BgApi::CommandClass::CLASS_WIFI:
                                memcpy(headerDecoding + headerDecodingHead, "WIFI", 4);
                                headerDecodingHead += 4;
                                break;
                            case BgApi::CommandClass::CLASS_TCP_STACK:
                                memcpy(headerDecoding + headerDecodingHead, "TCP", 3);
                                headerDecodingHead += 3;
                                break;
                            case BgApi::CommandClass::CLASS_ENDPOINT:
                                memcpy(headerDecoding + headerDecodingHead, "ENDP", 4);
                                headerDecodingHead += 4;
                                break;
                            case BgApi::CommandClass::CLASS_HARDWARE:
                                memcpy(headerDecoding + headerDecodingHead, "HW", 2);
                                headerDecodingHead += 2;
                                break;
                            case BgApi::CommandClass::CLASS_I2C:
                                memcpy(headerDecoding + headerDecodingHead, "I2C", 3);
                                headerDecodingHead += 3;
                                break;
                            case BgApi::CommandClass::CLASS_WIRED_ETHERNET:
                                memcpy(headerDecoding + headerDecodingHead, "ETH", 3);
                                headerDecodingHead += 3;
                                break;
                            case BgApi::CommandClass::CLASS_HTTP_SERVER:
                                memcpy(headerDecoding + headerDecodingHead, "HTTP", 4);
                                headerDecodingHead += 4;
                                break;
                            case BgApi::CommandClass::CLASS_PERSISTENT_STORE:
                                memcpy(headerDecoding + headerDecodingHead, "PS", 2);
                                headerDecodingHead += 2;
                                break;
                                break;
                            case BgApi::CommandClass::CLASS_DEVICE_FIRMWARE_UPGRADE:
                                memcpy(headerDecoding + headerDecodingHead, "FW", 2);
                                headerDecodingHead += 2;
                                break;
                            default:
                                headerDecoding[headerDecodingHead] = 'O';
                                int clid = msg.bgApiHeader()->bit.classId;
                                headerDecoding[headerDecodingHead + 1] = '0' + (int(clid / 1000) % 10);
                                headerDecoding[headerDecodingHead + 2] = '0' + (int(clid / 100) % 10);
                                headerDecoding[headerDecodingHead + 3] = '0' + (int(clid / 10) % 10);
                                headerDecoding[headerDecodingHead + 4] = '0' + (clid % 10);
                                headerDecodingHead += 5;
                            }

                            // Add in MID:
                            headerDecoding[headerDecodingHead] = ' ';
                            headerDecodingHead++;
                            headerDecoding[headerDecodingHead] = 'm';
                            int mid = msg.bgApiHeader()->bit.cmdId;
                            headerDecoding[headerDecodingHead + 1] = '0' + (int(mid / 1000) % 10);
                            headerDecoding[headerDecodingHead + 2] = '0' + (int(mid / 100) % 10);
                            headerDecoding[headerDecodingHead + 3] = '0' + (int(mid / 10) % 10);
                            headerDecoding[headerDecodingHead + 4] = '0' + (mid % 10);
                            headerDecodingHead += 5;

                            // Signal that data is next:
                            headerDecoding[headerDecodingHead] = ':';
                            headerDecoding[headerDecodingHead + 1] = ' ';
                            headerDecodingHead += 2;

                            watchDogInterface.debugPrintfBufferWithPrefix((uint8_t *)headerDecoding, headerDecodingHead, msg.payloadBuffer, msg.payloadSize);
                        }
#endif // WF121_DEBUG_PRINT_ALL_BGAPI_TO_WD
                    }
                    else
                    {
                        // wtf? shouldn't be here. Possible bitflipping?
                        // Something went wrong. Go back to start and reset:
                        m_currentState = Mpsm::State::WAITING_FOR_VALID_BGAPI_HEADER;
                        msg.reset();
                        // Clear out the header info too just in case since it looks
                        // like bits have been flipped:
                        m_headerBuffer.reset();
                        watchDogInterface.debugPrintfToWatchdog("RADIO: GOT CORRUPT?: %#04x", msg.header);
                        return Mpsm::ProcessStatus::POSSIBLE_CORRUPTION; // exit early
                    }
                }
                else
                {
                    returnStatus = Mpsm::ProcessStatus::WAITING_FOR_MORE_DATA;
                }
                break;

            default:
                // How did we get here? This shouldn't be possible...
                // Halt so WD resets us:
                returnStatus = Mpsm::ProcessStatus::POSSIBLE_CORRUPTION;
                configASSERT(pdFALSE);
            }

            if (Wf121::persistentBgApiPassthroughEnabled() &&
                returnStatus < Mpsm::ProcessStatus::DM_HEADER_PARSED)
            {
                // If we got a byte but it isn't NECESSARILY part of a BGAPI
                // packet, pass it out just in case:
                watchDogInterface.debugPrintfToWatchdog("BGB: 0x%02X.", newByte);
            }

            return returnStatus;
    }
    }
} // Wf121::Wf121Parser
