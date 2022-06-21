#ifndef WatchDogInterface_WatchDogMpsm_HPP
#define WatchDogInterface_WatchDogMpsm_HPP

#include <stdint.h>
#include <stddef.h>

#define WATCH_DOG_HEADER_PACKED_SIZE 12

namespace CubeRover
{
    /**
     * @brief A Message Parsing State Machine (MPSM) that facilitates receiving and parsing messages from the MSP430
     *        watchdog.
     *
     * This class is specifically tailored to integrate with a DMA-based approach for requesting data from the
     * underlying UART driver. Due to this tailoring, the implementation of this class ends up not actually being
     * much of a state machine. However, since I've consistently named classes with this function MPSM in other
     * places on the Iris project, I've chosen to keep the name here.
     */
    class WatchDogMpsm
    {
        public:
            /**
             * The status of parsing the payload of a message.
             */
            enum ProcessStatus
            {
                PS_DONE_VALID = 0, //!< Successfully got a full message
                PS_DONE_BAD_PARITY_HEADER = 1, //!< Parsed a full header that failed its parity check.
                PS_IN_PROGRESS = 2 //!< Need more data to complete the message.
            };

            /**
             * @brief The unpacked contents of a message header from the MSP430 watchdog.
             */
            class Header
            {
                public:
                    // Magic number is excluded, because this will only be populated if the header had a valid magic number
                    uint8_t parity; //!< The header parity.
                    uint16_t payloadLength; //!< The payload length.
                    uint16_t resetValue; //!< The reset value.
                    uint16_t lowerSeqNum; //!< The lower 16 bits of the sequence number.
                    uint16_t lowerOpCode; //!< The lower 16 bits of the opcode.

                    /**
                     * @brief Constructor.
                     */
                    Header();

                    /**
                     * @brief Clears the contents of this header.
                     */
                    void reset();
            };

            /**
             * @brief An unpacked message received from the MSP430 watchdog.
             */
            class Message
            {
                public:
                    Header parsedHeader; //!< The unpacked header, once successfully received and parsed.

                    uint8_t* dataBuffer; //!< The buffer in which payload data will be stored.
                    size_t dataBufferLen; //!< The max size of `dataBuffer`.

                    /**
                     * The number of bytes in `dataBuffer` that actually contain payload data at this time.
                     */
                    size_t accumulatedDataSize;

                    /**
                     * @brief Constructor.
                     *
                     * @param buffer The buffer used to hold the payload of the message.
                     * @param bufferLen The maximum size of `buffer`.
                     */
                    explicit Message(uint8_t* buffer, size_t bufferLen);

                    /**
                     * @brief Clears the structure. Note that this does not clear `dataBuffer` or `dataBufferLen`.
                     *
                     * This does zero-out the contents of `dataBuffer`, but this message will continue to use
                     * the same buffer location even after being reset.
                     */
                    void reset();
            };

            /**
             * @brief Constructor.
             */
            WatchDogMpsm();

            /**
             * @brief Destructor.
             */
            ~WatchDogMpsm();

            ProcessStatus process(WatchDogMpsm::Message& msg, uint8_t data);

        private:
            struct PrivateImplementation;
            PrivateImplementation* m_impl;
    };
} // end namespace CubeRover

#endif // #ifndef WatchDogInterface_WatchDogMpsm_HPP
