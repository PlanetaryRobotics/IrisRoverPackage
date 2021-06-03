#ifndef WatchDogInterface_WatchDogMpsm_HPP
#define WatchDogInterface_WatchDogMpsm_HPP

#include <stdint.h>

#define WATCH_DOG_HEADER_PACKED_SIZE 12;

namespace CubeRover
{
    // Note: "MPSM" stands for "message parsing state machine"
    // This one isn't actually much of a state machine, but the approach is similar so I kept the name.
    class WatchDogMpsm
    {
        public:
            enum ParseHeaderStatus 
            {
                NEED_MORE_DATA = 0,
                PARSED_VALID_HEADER = 1,
                PARSED_HEADER_BAD_PARITY = 2,

                // An unexpected internal error occurred. I don't think this should be possible
                // (it's only returned if we land in the default case of a switch)
                INTERNAL_ERROR = -255
            };

            enum ParseDataStatus 
            {
                NEED_MORE_DATA = 0,
                PARSED_ALL_DATA = 1
            };

            class Header
            {
                public:
                    // Magic number is excluded, because this will only be populated if the header had a valid magic number
                    uint8_t parity;
                    uint16_t payloadLength;
                    uint16_t resetValue;
                    uint16_t lowerSeqNum;
                    uint16_t lowerOpCode;

                    Header();

                    void reset();
            };

            class Message
            {
                public:
                    Header parsedHeader;

                    uint8_t* dataBuffer;
                    size_t dataBufferLen;

                    size_t accumulatedDataSize;

                    explicit Message(uint8_t* buffer, size_t bufferLen);

                    void reset();
            };

            WatchDogMpsm();
            ~WatchDogMpsm();

            ParseHeaderStatus getHeaderDmaDetails(WatchDogMpsm::Message& msg,
                                                  uint8_t** nextTransferDestination,
                                                  unsigned& nextTranferSize);

            void notifyHeaderDmaComplete(WatchDogMpsm::Message& msg,
                                         const uint8_t* destination,
                                         unsigned size);

            ParseDataStatus getDataDmaDetails(WatchDogMpsm::Message& msg,
                                              uint8_t** destination,
                                              unsigned& size);

            void notifyDataDmaComplete(WatchDogMpsm::Message& msg, unsigned size);

        private:
            struct PrivateImplementation;
            PrivateImplementation* m_impl;
    };
} // end namespace CubeRover

#endif // #ifndef WatchDogInterface_WatchDogMpsm_HPP