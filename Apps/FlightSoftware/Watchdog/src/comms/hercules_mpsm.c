
#include "include/comms/hercules_mpsm.h"
#include "include/common.h"
#include "include/utils/ring_buffer.h"


//###########################################################
// Private types
//###########################################################

typedef enum HerculesMpsm__HeaderContentsByteIndices
{
    HEADER_INDEX__MAGIC_ONE = 0,
    HEADER_INDEX__MAGIC_TWO = 1,
    HEADER_INDEX__MAGIC_THREE = 2,
    HEADER_INDEX__PARITY = 3,
    HEADER_INDEX__DATA_LEN_LSB = 4,
    HEADER_INDEX__DATA_LEN_MSB = 5,
    HEADER_INDEX__RESET_VAL_LSB = 6,
    HEADER_INDEX__RESET_VAL_MSB = 7,
    HEADER_INDEX__SEQ_NUM_LSB = 8,
    HEADER_INDEX__SEQ_NUM_MSB = 9,
    HEADER_INDEX__OPCODE_LSB = 10,
    HEADER_INDEX__OPCODE_MSB = 11,
} HerculesMpsm__HeaderContentsByteIndices;

typedef enum HerculesMpsm__State
{
    HERCULES_MPSM_STATE__VALIDATE_HEADER = 0,
    HERCULES_MPSM_STATE__DATA = 1
} HerculesMpsm__State;

typedef struct HerculesMpsm__StateMachine
{
    BOOL initialized;

    HerculesMpsm__State currentState;
    
    // This one doesn't need to be volatile, but it's what RingBuffer takes.
    // This is a bit inefficient but hopefully won't have any real impact.
    // If it does have a performance impact we'll need to make something
    // custom for this use-case.
    volatile uint8_t ringBufferMemory[8];
    RingBuffer* headerRb;

    uint16_t dataLength;
} HerculesMpsm__StateMachine;

//###########################################################
// Private globals and constants
//###########################################################

HerculesMpsm__StateMachine theStateMachine = {
    .initialized = FALSE,
    .currentState = HERCULES_MPSM_STATE__VALIDATE_HEADER,
    .ringBufferMemory = { 0 },
    .headerRb = NULL
    .dataLength = 0
};

//###########################################################
// Private function declarations
//###########################################################

HerculesMpsm__Status HerculesMpsm__checkForValidHeader(HerculesMpsm__Msg* msg, uint8_t newData);
BOOL HerculesMpsm__checkRb(size_t index, uint8_t expected);
uint8_t HerculesMpsm__peekRb(size_t index);
HerculesMpsm__Status HerculesMpsm__appendData(HerculesMpsm__Msg* msg, uint8_t newData);

//###########################################################
// Public function definitions
//###########################################################

HerculesMpsm__Status HerculesMpsm__initMsg(HerculesMpsm__Msg* msg)
{
    if (NULL == msg || NULL == msg->dataBuffer) {
        return HERCULES_MPSM__STATUS__ERROR_NULL;
    }

    if (!(theStateMachine.initialized)) {
        RingBuffer__Status rbStatus = RingBuffer__init(theStateMachine.headerRb,
                                                       theStateMachine.ringBufferMemory,
                                                       SIZE_OF_ARRAY(theStateMachine.ringBufferMemory));
    
        if (RB__STATUS__SUCCESS != rbStatus) {
            return HERCULES_MPSM__STATUS__ERROR_RB_INIT_FAILURE;
        }

        theStateMachine.initialized = TRUE;
    }

    if (0 == msg->dataBufferLen) {
        return HERCULES_MPSM__STATUS__ERROR_ZERO_LENGTH_BUFFER;
    }

    msg->msgStatus = HERCULES_MPSM__MSG_STATUS__IN_PROGRESS;
    msg->msgLen = 0;
    memset(msg->header, 0, SIZE_OF_ARRAY(msg->header));

    return HERCULES_MPSM__STATUS__SUCCESS;
}

HerculesMpsm__Status HerculesMpsm__process(HerculesMpsm__Msg* msg, uint8_t newData)
{
    if (NULL == msg || NULL == msg->dataBuffer) {
        return HERCULES_MPSM__STATUS__ERROR_NULL;
    }

    if (HERCULES_MPSM__MSG_STATUS__IN_PROGRESS != msg->msgStatus) {
        return HERCULES_MPSM__STATUS__ERROR_WRONG_STATE;
    }

    switch (theStateMachine.currentState) {
        case HERCULES_MPSM_STATE__VALIDATE_HEADER:
            return HerculesMpsm__checkForValidHeader(msg, newData);

        case HERCULES_MPSM_STATE__DATA:
            return HerculesMpsm__appendData(msg, newData);

        default:
            // This is bad and shouldn't ever happen
            return HERCULES_MPSM__STATUS__ERROR_INTERNAL_ERROR;
    }
}

HerculesMpsm__Status HerculesMpsm__reset(HerculesMpsm__Msg* msg)
{
    if (NULL == msg || NULL == msg->dataBuffer) {
        return HERCULES_MPSM__STATUS__ERROR_NULL;
    }

    if (!(theStateMachine.initialized)) {
        return HERCULES_MPSM__STATUS__ERROR_NOT_INITIALIZED;
    }

    // Clear the header ring buffer
    RingBuffer__Status rbStatus = RingBuffer__clear(theStateMachine.headerRb);

    if (RB__STATUS__SUCCESS != rbStatus) {
        return HERCULES_MPSM__STATUS__ERROR_RB_CLEAR_FAILURE;
    }

    // Reset the state machine
    theStateMachine.currentState = HERCULES_MPSM_STATE__VALIDATE_HEADER;
    theStateMachine.dataLength = 0;

    // Re-initialize the message
    return HerculesMpsm__initMsg(msg);
}

//###########################################################
// Private function definitions
//###########################################################

HerculesMpsm__Status HerculesMpsm__checkForValidHeader(HerculesMpsm__Msg* msg, uint8_t newData)
{
    RingBuffer__Status rbStatus = RingBuffer__putOverwrite(theStateMachine.headerRb, newData);

    if (RB__STATUS__SUCCESS != rbStatus) {
        msg->msgStatus = HERCULES_MPSM__MSG_STATUS__ERROR_RB_FAILURE;
        return HERCULES_MPSM__STATUS__ERROR_RB_PUT_FAILURE;
    }

    // The header is 8 bytes, so the ring buffer (which has a size of 8) needs to be full for the
    // header to be valid.
    if (!RingBuffer__full(theStateMachine.headerRb)) {
        return HERCULES_MPSM__STATUS__NEED_MORE_DATA;
    }

    // Start checking the header bytes
    if (HerculesMpsm__checkRb((uint8_t) HEADER_INDEX__MAGIC_ONE, HERC_MSGS__CONSTANTS__HEADER_MAGIC_ONE)
        && HerculesMpsm__checkRb((uint8_t) HEADER_INDEX__MAGIC_TWO, HERC_MSGS__CONSTANTS__HEADER_MAGIC_TWO)
        && HerculesMpsm__checkRb((uint8_t) HEADER_INDEX__MAGIC_THREE, HERC_MSGS__CONSTANTS__HEADER_MAGIC_THREE)) {
        // The magic numbers are correct, now check parity
        uint8_t expectedParity = HerculesMpsm__peekRb(HEADER_INDEX__PARITY);

        uint8_t dataLenLsb = HerculesMpsm__peekRb(HEADER_INDEX__DATA_LEN_LSB);
        uint8_t dataLenMsb = HerculesMpsm__peekRb(HEADER_INDEX__DATA_LEN_MSB);
        uint8_t resetValLsb = HerculesMpsm__peekRb(HEADER_INDEX__RESET_VAL_LSB);
        uint8_t resetValMsb = HerculesMpsm__peekRb(HEADER_INDEX__RESET_VAL_MSB);
        uint8_t seqNumLsb = HerculesMpsm__peekRb(HEADER_INDEX__SEQ_NUM_LSB);
        uint8_t seqNumMsb = HerculesMpsm__peekRb(HEADER_INDEX__SEQ_NUM_MSB);
        uint8_t opcodeLsb = HerculesMpsm__peekRb(HEADER_INDEX__OPCODE_LSB);
        uint8_t opcodeMsb = HerculesMpsm__peekRb(HEADER_INDEX__OPCODE_MSB);

        uint8_t computedParity = HERC_MSGS__CONSTANTS__HEADER_MAGIC_SUM;
        computedParity += dataLenLsb + dataLenMsb;
        computedParity += resetValLsb + resetValMsb;
        computedParity += seqNumLsb + seqNumMsb;
        computedParity += opcodeLsb + opcodeMsb;
        computedParity = ~computedParity;

        if (expectedParity == computedParity) {
            // The parity is correct, so this is a valid header. 
            // Copy the header contents into the output structure
            msg->header.payloadLength = (uint16_t) (((uint16_t) dataLenLsb) & 0x00FFU);
            msg->header.payloadLength |= (uint16_t) ((((uint16_t) dataLenMsb) << 8) & 0xFF00U);
            msg->header.resetValue = (uint16_t) (((uint16_t) resetValLsb) & 0x00FFU);
            msg->header.resetValue |= (uint16_t) ((((uint16_t) resetValMsb) << 8) & 0xFF00U);
            msg->header.lowerSeqNum = (uint16_t) (((uint16_t) seqNumLsb) & 0x00FFU);
            msg->header.lowerSeqNum |= (uint16_t) ((((uint16_t) seqNumMsb) << 8) & 0xFF00U);
            msg->header.lowerOpCode = (uint16_t) (((uint16_t) opcodeLsb) & 0x00FFU);
            msg->header.lowerOpCode |= (uint16_t) ((((uint16_t) opcodeMsb) << 8) & 0xFF00U);

            // Clear the ring buffer, as we're now done with the header.
            rbstatus = RingBuffer__clear(theStateMachine.headerRb);
        
            // Clear should only throw is headerRb is NULL, and it shouldn't be NULL
            assert(RB__STATUS__SUCCESS == rbstatus);

            // Record the data length
            theStateMachine.dataLength = msg->header.payloadLength;

            // If this is a header-only message then we're done parsing
            if (theStateMachine.dataLength == 0) {
                // Reset the state machine
                theStateMachine.currentState = HERCULES_MPSM_STATE__VALIDATE_HEADER;

                msg->msgStatus = HERCULES_MPSM__MSG_STATUS__DONE_VALID;
                return HERCULES_MPSM__STATUS__PARSED_MESSAGE;
            } else {
                // We expect data after this header, so advance the state machine
                theStateMachine.currentState = HERCULES_MPSM_STATE__DATA;

                // We can check now if the buffer given by the user is big enough for our data
                if (theStateMachine.dataLength > msg->dataBufferLen) {
                    msg->msgStatus = HERCULES_MPSM__MSG_STATUS__ERROR_BUFFER_TOO_SMALL;
                    return HERCULES_MPSM__STATUS__ERROR_BUFFER_TOO_SMALL;
                }
            }
        }
    }

    return HERCULES_MPSM__STATUS__NEED_MORE_DATA;
}

BOOL HerculesMpsm__checkRb(size_t index, uint8_t expected)
{
    uint8_t value = 0;

    RingBuffer__Status rbStatus = RingBuffer__peekAt(theStateMachine.headerRb, index, &value);

    if (RB__STATUS__SUCCESS != rbStatus) {
        return FALSE;
    } else {
        return (value == expected);
    }
}

uint8_t HerculesMpsm__peekRb(size_t index)
{
    uint8_t value = 0;

    RingBuffer__Status rbStatus = RingBuffer__peekAt(theStateMachine.headerRb, index, &value);

    if (RB__STATUS__SUCCESS != rbStatus) {
        return 0;
    } else {
        return value;
    }
}


HerculesMpsm__Status HerculesMpsm__appendData(HerculesMpsm__Msg* msg, uint8_t newData);
{
    msg->dataBuffer[msg->msgLen] = newData;
    msg->msgLen++;

    // Check if we've finished reading the data portion, and if so reset
    if (msg->msgLen == theStateMachine.dataLength) {
        // Reset the state machine
        theStateMachine.currentState = HERCULES_MPSM_STATE__VALIDATE_HEADER;
        theStateMachine.dataLength = 0;

        msg->msgStatus = HERCULES_MPSM__MSG_STATUS__DONE_VALID;
        return HERCULES_MPSM__STATUS__PARSED_MESSAGE;
    } else {
        return HERCULES_MPSM__STATUS__NEED_MORE_DATA;
    }
}