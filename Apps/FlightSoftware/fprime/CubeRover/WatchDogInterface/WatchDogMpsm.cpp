#include <string.h>
#include <cassert>

#include <Fw/Types/Assert.hpp>

#include <CubeRover/WatchDogInterface/WatchDogMpsm.hpp>

namespace
{
    /**
     * The status of parsing the header of a message.
     */
    enum ParseHeaderStatus
    {
        PHS_NEED_MORE_DATA = 0, //!< Need more data to complete the header.
        PHS_PARSED_VALID_HEADER = 1, //!< Parsed a full header that passes its parity check.
        PHS_PARSED_HEADER_BAD_PARITY = 2, //!< Parsed a full header that failed its parity check.

        /**
         * @brief An unexpected internal error occurred.
         *
         * I don't think should actually be possible to occur (it's only returned if we land
         * in the default case of a switch statement, and that should only occur in the event
         * of programmer error).
         */
        PHS_INTERNAL_ERROR = -255
    };

    /**
     * The status of parsing the payload of a message.
     */
    enum ParseDataStatus
    {
        PDS_NEED_MORE_DATA = 0, //!< Need more data to complete the payload.
        PDS_PARSED_ALL_DATA = 1 //!< Successfully got all payload data.
    };

    class HeaderRingArray
    {
        public:
            HeaderRingArray() : m_bufferSize(sizeof(m_buffer))
            {
                clear();
            }

            size_t size() const
            {
                return m_size;
            }

            size_t freeSize() const
            {
                return m_bufferSize - m_size;
            }

            bool full() const
            {
                return (m_size == m_bufferSize);
            }

            bool empty() const
            {
                return (m_size == 0);
            }

            void putOverwrite(uint8_t data)
            {
                bool wasFull = full();

                // Write to the current head index then increment it, wrapping if necessary
                m_buffer[m_head] = data;
                m_head = incrementAndWrap(m_head);

                if (wasFull) {
                    // The buffer was full so we needed to overwrite, therefore we need to
                    // also increment the tail. We don't want to increment the size in this
                    // case.
                    m_tail = incrementAndWrap(m_tail);
                } else {
                    // We added an element to a previously empty slot, so we want to increment
                    // the size.
                    m_size++;
                }
            }

            bool get(uint8_t& outByte)
            {
                if (empty()) {
                    return false;
                }

                outByte = m_buffer[m_tail];
                m_buffer[m_tail] = 0xFA; // Write 0xFA to all "empty" bytes in this array
                m_tail = incrementAndWrap(m_tail);

                m_size -= 1;

                return true;
            }

            uint8_t peek(size_t index)
            {
                if (index >= m_size) {
                    return (uint8_t) -1;
                }

                size_t wrappedIndex = offsetAndWrap(m_tail, index);
                return m_buffer[wrappedIndex];
            }

            void clear()
            {
                // Write 0xFA to all "empty" bytes in this array, this just makes debugging easier
                memset((void*)m_buffer, 0xFA, m_bufferSize);
                m_head = 0;
                m_tail = 0;
                m_size = 0;
            }

        private:
            size_t offsetAndWrap(size_t index, size_t offset)
            {
                size_t newIndex = index + offset;

                while (newIndex >= m_bufferSize) {
                    newIndex -= m_bufferSize;
                }

                return newIndex;
            }

            size_t incrementAndWrap(size_t index)
            {
                return offsetAndWrap(index, 1);
            }

            uint8_t m_buffer[12];
            size_t m_bufferSize;
            size_t m_head;
            size_t m_tail;
            size_t m_size;
    };

    uint16_t combineBytesToShort(uint8_t lsb, uint8_t msb)
    {
        uint16_t out = (uint16_t) (((uint16_t) lsb) & 0x00FFU);
        out |= (uint16_t) ((((uint16_t) msb) << 8) & 0xFF00U);
        return out;
    }
} // end of unnamed namespace


namespace CubeRover
{
    struct WatchDogMpsm::PrivateImplementation
    {
        enum HeaderIndices
        {
            MAGIC_ONE = 0,
            MAGIC_TWO = 1,
            MAGIC_THREE = 2,
            PARITY = 3,
            DATA_LEN_LSB = 4,
            DATA_LEN_MSB = 5,
            RESET_VAL_LSB = 6,
            RESET_VAL_MSB = 7,
            SEQ_NUM_LSB = 8,
            SEQ_NUM_MSB = 9,
            OPCODE_LSB = 10,
            OPCODE_MSB = 11
        };

        static const uint8_t MAGIC_BYTE_ONE_EXPECTED_VALUE = 0x0BU;
        static const uint8_t MAGIC_BYTE_TWO_EXPECTED_VALUE = 0xB0U;
        static const uint8_t MAGIC_BYTE_THREE_EXPECTED_VALUE = 0x21U;
        static const uint8_t MAGIC_BYTE_EXPECTED_VALUE_SUM = 0xDCU;

        HeaderRingArray m_ringArray;

        // Can't normally align member variable, but uint64_t will be naturally aligned on an 8-byte boundary
        // thus forcing the m_headerBuffer to also be aligned at such a boundary. In other words, this union
        // is equivalent to:
        // uint8_t m_headerBuffer[WATCH_DOG_HEADER_PACKED_SIZE] __attribute__((aligned(8)));
        union {
            uint8_t m_headerBuffer[WATCH_DOG_HEADER_PACKED_SIZE];
            uint64_t not_used;
        };

        enum ParsingState
        {
            PARSING_STATE__VALIDATE_HEADER = 0,
            PARSING_STATE__DATA = 1,
        };

        ParsingState m_state;

        PrivateImplementation()
            : m_ringArray(),
              m_state(PARSING_STATE__VALIDATE_HEADER)
        {
            memset((void*)m_headerBuffer, 0xFA, sizeof(m_headerBuffer));
        }

        ParseHeaderStatus checkForValidHeader(WatchDogMpsm::Message& msg)
        {
            // The header is a full 12 bytes, so the ring array (size of 8) needs to be full for the header to be valid
            if (!m_ringArray.full()) {
                return PHS_NEED_MORE_DATA;
            }

            bool spinForNewMagic = false;
            ParseHeaderStatus retStatus = PHS_NEED_MORE_DATA;

            // Start checking the header bytes.
            if (m_ringArray.peek((uint8_t) HeaderIndices::MAGIC_ONE) == MAGIC_BYTE_ONE_EXPECTED_VALUE
                && m_ringArray.peek((uint8_t) HeaderIndices::MAGIC_TWO) == MAGIC_BYTE_TWO_EXPECTED_VALUE
                && m_ringArray.peek((uint8_t) HeaderIndices::MAGIC_THREE) == MAGIC_BYTE_THREE_EXPECTED_VALUE) {
                // The magic numbers are correct, now check parity
                uint8_t expectedParity = m_ringArray.peek((uint8_t) HeaderIndices::PARITY);
                uint8_t dataLenLsb = m_ringArray.peek((uint8_t) HeaderIndices::DATA_LEN_LSB);
                uint8_t dataLenMsb = m_ringArray.peek((uint8_t) HeaderIndices::DATA_LEN_MSB);
                uint8_t resetValLsb = m_ringArray.peek((uint8_t) HeaderIndices::RESET_VAL_LSB);
                uint8_t resetValMsb = m_ringArray.peek((uint8_t) HeaderIndices::RESET_VAL_MSB);
                uint8_t seqNumLsb = m_ringArray.peek((uint8_t) HeaderIndices::SEQ_NUM_LSB);
                uint8_t seqNumMsb = m_ringArray.peek((uint8_t) HeaderIndices::SEQ_NUM_MSB);
                uint8_t opcodeLsb = m_ringArray.peek((uint8_t) HeaderIndices::OPCODE_LSB);
                uint8_t opcodeMsb = m_ringArray.peek((uint8_t) HeaderIndices::OPCODE_MSB);

                uint8_t computedParity = MAGIC_BYTE_EXPECTED_VALUE_SUM;
                computedParity += dataLenLsb + dataLenMsb;
                computedParity += resetValLsb + resetValMsb;
                computedParity += seqNumLsb + seqNumMsb;
                computedParity += opcodeLsb + opcodeMsb;
                computedParity = ~computedParity;

                // Copy the header details into the parsed header structure, even if this
                // fails the parity check.
                msg.parsedHeader.parity = expectedParity;
                msg.parsedHeader.payloadLength = combineBytesToShort(dataLenLsb, dataLenMsb);
                msg.parsedHeader.resetValue = combineBytesToShort(resetValLsb, resetValMsb);
                msg.parsedHeader.lowerSeqNum = combineBytesToShort(seqNumLsb, seqNumMsb);
                msg.parsedHeader.lowerOpCode = combineBytesToShort(opcodeLsb, opcodeMsb);

                if (expectedParity == computedParity) {
                    // Passed parity check, so this is a valid header. We can empty the ring array since we've already
                    // copied the data into the parsed header structure.
                    m_ringArray.clear();
                    memset((void*)m_headerBuffer, 0xFA, sizeof(m_headerBuffer));
                    return PHS_PARSED_VALID_HEADER;
                } else {
                    // Our computed parity doesn't match the expected parity. We want to return an error indicating as
                    // much, but we also can pump the ring array until we find new magic numbers.
                    retStatus = PHS_PARSED_HEADER_BAD_PARITY;
                    spinForNewMagic = true;
                }
            } else {
                // One or more magic numbers were invalid, so pump data from the ring array
                // until we encounter new, proper magic numbers or we run out of data to
                // check the magic numbers.
                spinForNewMagic = true;
            }

            if (spinForNewMagic) {
                // This lets us maximize our dma request size, as we will know the minimum amount of data necessary to
                // find a valid header (i.e. the amount of data to fill the header buffer after we shorten it here).
                bool foundNewMagic = false;
                uint8_t unused;

                while (!foundNewMagic && !m_ringArray.empty()) {
                    m_ringArray.get(unused);

                    // If there is only one element, then we only look for the first magic number.
                    // If there are two elements then we only look for the first two magic numbers.
                    // If there are three or more elements then we look for all three magic numbers.
                    bool foundMagicThisIter = true;
                    size_t ringSize = m_ringArray.size();

                    if (ringSize >= 3) {
                        bool m3 =
                            (m_ringArray.peek((uint8_t) HeaderIndices::MAGIC_THREE) == MAGIC_BYTE_THREE_EXPECTED_VALUE);
                        foundMagicThisIter = (foundMagicThisIter && m3);
                    }

                    if (ringSize >= 2) {
                        bool m2 =
                            (m_ringArray.peek((uint8_t) HeaderIndices::MAGIC_TWO) == MAGIC_BYTE_TWO_EXPECTED_VALUE);
                        foundMagicThisIter = (foundMagicThisIter && m2);
                    }

                    if (ringSize >= 1) {
                        bool m1 =
                            (m_ringArray.peek((uint8_t) HeaderIndices::MAGIC_ONE) == MAGIC_BYTE_ONE_EXPECTED_VALUE);
                        foundMagicThisIter = (foundMagicThisIter && m1);
                    }

                    // If ring size is zero then foundMagicThisIter will stay true, but that's fine because we'll
                    // exit this loop at the end of this iteration anyway since the ring is empty
                    foundNewMagic = foundMagicThisIter;
                }
            }

            // This header is invalid for some reason
            return retStatus;
        }
    };


    WatchDogMpsm::Header::Header()
    {
        reset();
    }

    void WatchDogMpsm::Header::reset()
    {
        parity = 0;
        payloadLength = 0;
        resetValue = 0;
        lowerSeqNum = 0;
        lowerOpCode = 0;
    }

    WatchDogMpsm::Message::Message(uint8_t* buffer, size_t bufferLen)
        : parsedHeader(),
          dataBuffer(buffer),
          dataBufferLen(bufferLen),
          accumulatedDataSize(0)

    {
        memset((void*)dataBuffer, 0xFA, dataBufferLen); // This isn't necessary but should make debugging easier
    }    

    void WatchDogMpsm::Message::reset()
    {
        parsedHeader.reset();
        memset((void*)dataBuffer, 0xFA, accumulatedDataSize); // This isn't necessary but should make debugging easier memset(
        accumulatedDataSize = 0;
    }

    WatchDogMpsm::WatchDogMpsm()
        : m_impl(new PrivateImplementation())
    {
    }

    WatchDogMpsm::~WatchDogMpsm()
    {
        delete m_impl;
    }

    WatchDogMpsm::ProcessStatus WatchDogMpsm::process(WatchDogMpsm::Message& msg, uint8_t newData)
    {
        switch (m_impl->m_state) {
            case PrivateImplementation::PARSING_STATE__VALIDATE_HEADER:
                {
                    m_impl->m_ringArray.putOverwrite(newData);
                    ParseHeaderStatus status = m_impl->checkForValidHeader(msg);

                    switch (status) {
                        case PHS_NEED_MORE_DATA:
                            return PS_IN_PROGRESS;

                        case PHS_PARSED_VALID_HEADER:
                            if (msg.parsedHeader.payloadLength == 0) {
                                // We parsed a valid header and it's header only, so we want another
                                // header next
                                m_impl->m_state = PrivateImplementation::PARSING_STATE__VALIDATE_HEADER;

                                // That being said, we did get a valid message
                                return PS_DONE_VALID;
                            } else {
                                // We parsed a valid header but we also need a payload, so we want data
                                // next
                                m_impl->m_state = PrivateImplementation::PARSING_STATE__DATA;

                                // We're not done with the current message
                                return PS_IN_PROGRESS;
                            }

                        case PHS_PARSED_HEADER_BAD_PARITY:
                            // We parsed a header with invalid parity, so we'll continue looking for
                            // a good header
                            m_impl->m_state = PrivateImplementation::PARSING_STATE__VALIDATE_HEADER;

                            // However, we'll notify that we parsed a header that was proper other than
                            // parity
                            return PS_DONE_BAD_PARITY_HEADER;

                        case PHS_INTERNAL_ERROR:
                        default:
                            assert(false);
                    }
                }
                break;


            case PrivateImplementation::PARSING_STATE__DATA:
                msg.dataBuffer[msg.accumulatedDataSize] = newData;
                msg.accumulatedDataSize++;

                // Check if we've finished reading the data portion, and if so reset
                if (msg.parsedHeader.payloadLength == msg.accumulatedDataSize) {
                    // Reset the state machine
                    m_impl->m_state = PrivateImplementation::PARSING_STATE__VALIDATE_HEADER;
                    return PS_DONE_VALID;
                } else {
                    return PS_IN_PROGRESS;
                }

            default:
                assert(false);
        }

        // Should be unreachable
        assert(false);
        return PS_IN_PROGRESS;
    }
} // end namespace CubeRover
