#include <assert.h>
#include <string.h>

#include "comms/debug_comms.h"
#include "comms/hercules_comms.h"
#include "common.h"
#include "comms/hercules_mpsm.h"
#include "comms/ip_udp.h"

//###########################################################
// Private types
//###########################################################

struct HerculesComms__State
{
    BOOL initialized;
    UART__State* uartState;

    uint8_t rxMsgBuffer[HERC_MSGS__CONSTANTS__MAX_PAYLOAD_SIZE];
    HerculesMpsm__Msg herculesMsg;

    uint16_t uplinkSequenceNumber;
    HercMsgs__Header headerStruct;
    uint8_t headerSerializationBuffer[HERC_MSGS__PACKED_SIZE__HEADER];
};

//###########################################################
// Private globals and constants
//###########################################################

static HerculesComms__State theState = {
    .initialized = FALSE,
    .uartState = NULL,
    .rxMsgBuffer = { 0 },
    .herculesMsg = {
        .msgStatus = HERCULES_MPSM__MSG_STATUS__NOT_INITIALIZED,
        .header = { 0 },
        .dataBuffer = NULL,
        .dataBufferLen = 0,
        .msgLen = 0
    },
    .uplinkSequenceNumber = 0,
    .headerStruct = { 0 },
    .headerSerializationBuffer = { 0 }
};

//###########################################################
// Private function declarations
//###########################################################

/**
 * @private
 *
 * @brief Transmits a message to the Hercules with the given parameters.
 *
 * @param hState The comms instance.
 * @param resetValue The reset value to send as part of the header.
 * @param lowerSeqNum The sequence number to send as part of the header.
 * @param lowerOpCode The opcode to send as part of the header.
 * @param data The payload data to send. Must be non-NULL if `dataLen` is non-zero.
 * @param dataLen The length of `data`.
 *
 * @return One of the following:
 *   - HERCULES_COMMS__STATUS__SUCCESS: The function was successful.
 *   - HERCULES_COMMS__STATUS__ERROR_NULL: `hState` was NULL, or `data` was NULL when `dataLen` is non-zero.
 *   - HERCULES_COMMS__STATUS__ERROR_NOT_INITIALIZED: `hState` was not initialized.
 *   - HERCULES_COMMS__STATUS__ERROR_TX_OVERFLOW: Could not fit all of the data in `buffer` into the UART's internal
 *                                                transmit buffer.
 *   - HERCULES_COMMS__STATUS__ERROR_UART_TX_FAILURE: Got an error when `UART__transmit` was invoked.
 */
static HerculesComms__Status HerculesComms__txHerculesMsg(HerculesComms__State* hState,
                                                          uint16_t resetValue,
                                                          uint16_t lowerSeqNum,
                                                          uint16_t lowerOpCode,
                                                          const uint8_t* data,
                                                          size_t dataLen);

/**
 * @private
 *
 * @brief A simple wrapper around UART__transmit that sends the given buffer of data.
 *
 * @param hState The comms instance.
 * @param buffer The data buffer to send.
 * @param len The length of `buffer`.
 *
 * @return One of the following:
 *   - HERCULES_COMMS__STATUS__SUCCESS: The function was successful.
 *   - HERCULES_COMMS__STATUS__ERROR_TX_OVERFLOW: Could not fit all of the data in `buffer` into the UART's internal
 *                                                transmit buffer.
 *   - HERCULES_COMMS__STATUS__ERROR_UART_TX_FAILURE: Got an error when `UART__transmit` was invoked.
 */
static HerculesComms__Status HerculesComms__transmitBuffer(HerculesComms__State* hState,
                                                           const uint8_t* buffer,
                                                           size_t len);

//###########################################################
// Public function definitions
//###########################################################

HerculesComms__Status HerculesComms__init(HerculesComms__State** hState, UART__State* uartState)
{
    if (NULL == hState || NULL == uartState) {
        return HERCULES_COMMS__STATUS__ERROR_NULL;
    }

    if (theState.initialized) {
        return HERCULES_COMMS__STATUS__ERROR_ALREADY_INITIALIZED;
    }

    theState.uartState = uartState;

    theState.herculesMsg.dataBuffer = theState.rxMsgBuffer;
    theState.herculesMsg.dataBufferLen = SIZE_OF_ARRAY(theState.rxMsgBuffer);

    HerculesMpsm__Status mpsmStatus = HerculesMpsm__initMsg(&(theState.herculesMsg));
    
    if (HERCULES_MPSM__STATUS__SUCCESS != mpsmStatus) {
        return HERCULES_COMMS__STATUS__ERROR_MPSM_INIT_FAILURE;
    }

    theState.initialized = TRUE;
    *hState = &theState; 

    return HERCULES_COMMS__STATUS__SUCCESS;
}

HerculesComms__Status HerculesComms__tryGetMessage(HerculesComms__State* hState,
                                                   HerculesMsgCallback callback,
                                                   void* userArg)
{
    static uint8_t uartRxData[64] = { 0 };
 
    if (NULL == hState || NULL == userArg) {
        return HERCULES_COMMS__STATUS__ERROR_NULL;
    }

    if (!hState->initialized) {
        return HERCULES_COMMS__STATUS__ERROR_NOT_INITIALIZED;
    }

    HerculesComms__Status returnStatus = HERCULES_COMMS__STATUS__SUCCESS;
    BOOL tryToGetMoreData = TRUE;

    while (tryToGetMoreData) {
        // Zero out the static buffer on each iteration for easier debugging
        memset(uartRxData, 0, SIZE_OF_ARRAY(uartRxData));

        size_t numReceived = 0;

        UART__Status uartStatus = UART__receive(hState->uartState,
                                                uartRxData,
                                                SIZE_OF_ARRAY(uartRxData),
                                                &numReceived);

        if (UART__STATUS__SUCCESS != uartStatus) {
            return HERCULES_COMMS__STATUS__ERROR_UART_RX_FAILURE;
        }     

        if (numReceived != 0) {
            //DebugComms__printfToLander("Got %u bytes of msg data from Hercules\n", numReceived);
        }

        // Iterate through all data, adding it to the hercules mpsm until packet data has been found or
        // we use up all of the data from the UART
        for (size_t i = 0; i < numReceived; ++i) {
            BOOL resetMpsmMsg = FALSE;
            HerculesMpsm__Status mpsmStatus = HerculesMpsm__process(&(hState->herculesMsg), uartRxData[i]);

            if (HERCULES_MPSM__STATUS__PARSED_MESSAGE == mpsmStatus) {
                // We've gotten our data. Copy the data into the output buffer and call our callback
                callback(&(hState->herculesMsg.header),
                         hState->herculesMsg.dataBuffer,
                         hState->herculesMsg.msgLen,
                         userArg);

                resetMpsmMsg = TRUE;
            } else if (HERCULES_MPSM__STATUS__NEED_MORE_DATA != mpsmStatus) {
                DebugComms__printfToLander("Unexpected return value from HerculesMpsm__process: %d "
                                           "in HerculesComms__tryGetMessage\n", mpsmStatus);
                // Some kind of unexpected error occurred. At a minimum we need to reset the MPSM
                resetMpsmMsg = TRUE;
                numReceived = 0;
                returnStatus = HERCULES_COMMS__STATUS__ERROR_MPSM_PROCESS_FAILURE;
            }

            if (resetMpsmMsg) {
                mpsmStatus = HerculesMpsm__initMsg(&(hState->herculesMsg));

                if (HERCULES_MPSM__STATUS__SUCCESS != mpsmStatus) {
                    DebugComms__printfToLander("Unexpected return value from HerculesMpsm__initMsg: %d "
                                               "in HerculesComms__tryGetMessage\n", mpsmStatus);

                    // Don't overwrite an existing error return status with this one, but if we haven't had an
                    // error before now then set our return status to indicate this failure.
                    if (HERCULES_COMMS__STATUS__SUCCESS == returnStatus) {
                        returnStatus = HERCULES_COMMS__STATUS__ERROR_MPSM_INIT_FAILURE;
                    }

                    // If we weren't able to reset the MPSM we shouldn't try to process any more data
                    return returnStatus;
                }
            }
        }

        // Call receive again if our buffer for getting data from the uart was saturated with data in the last call.
        tryToGetMoreData = (numReceived == SIZE_OF_ARRAY(uartRxData));
    }

    return returnStatus;
}

// Sends data from lander to Hercules. A header will be sent first with the length of the data,
// a reset value of zero, an internally tracked sequence number, and the opcode
// HERCULES_COMMS__MSG_OPCODE__UPLINK. Following the header, the given data is sent with no modification.
HerculesComms__Status HerculesComms__txUplinkMsg(HerculesComms__State* hState, const uint8_t* data, size_t dataLen)
{
    if (dataLen == 0) {
        return HERCULES_COMMS__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    return HerculesComms__txHerculesMsg(hState,
                                        0,
                                        hState->uplinkSequenceNumber++,
                                        (uint16_t) HERCULES_COMMS__MSG_OPCODE__UPLINK,
                                        data,
                                        dataLen);
}

// Sends a response to a message from Hercules back to Hercules. A header containing the length
// of the given data as well as the reset value, sequence number, and opcode from the given header,
// will be sent first and then the data will be sent without  modification (if there is any data to send).
// If dataLen is zero, data is allowed to be NULL.
HerculesComms__Status HerculesComms__txResponseMsg(HerculesComms__State* hState,
                                                   const HercMsgs__Header* sourceCommandHeader,
                                                   const uint8_t* data,
                                                   size_t dataLen)
{
    if (NULL == sourceCommandHeader) {
        return HERCULES_COMMS__STATUS__ERROR_NULL;
    }

    return HerculesComms__txHerculesMsg(hState,
                                        sourceCommandHeader->resetValue,
                                        sourceCommandHeader->lowerSeqNum,
                                        sourceCommandHeader->lowerOpCode,
                                        data,
                                        dataLen);
}

// Clear the underlying UART rx buffer and reset the message parsing state machine
HerculesComms__Status HerculesComms__resetState(HerculesComms__State* hState)
{
    static uint8_t uartRxData[64] = { 0 };

    if (NULL == hState) {
        return HERCULES_COMMS__STATUS__ERROR_NULL;
    }

    if (!hState->initialized) {
        return HERCULES_COMMS__STATUS__ERROR_NOT_INITIALIZED;
    }

    // Empty out the UART rx buffer by getting data from it until we get no more.
    // We simply ignore the data, since we're also going to be resetting the
    // message parsing state machine.
    size_t numReceived = 1;

    //!< @todo Do we want to add a timeout to this loop?
    while (numReceived > 0) {
        UART__Status uartStatus = UART__receive(hState->uartState,
                                                uartRxData,
                                                SIZE_OF_ARRAY(uartRxData),
                                                &numReceived);

        if (UART__STATUS__SUCCESS != uartStatus) {
            return HERCULES_COMMS__STATUS__ERROR_UART_RX_FAILURE;
        }
    }

    HerculesMpsm__Status mpsmStatus = HerculesMpsm__reset(&(hState->herculesMsg));

    if (HERCULES_MPSM__STATUS__SUCCESS != mpsmStatus) {
        return HERCULES_COMMS__STATUS__ERROR_MPSM_RESET_FAILURE;
    }

    return HERCULES_COMMS__STATUS__SUCCESS;
}

//###########################################################
// Private function definitions
//###########################################################

static HerculesComms__Status HerculesComms__transmitBuffer(HerculesComms__State* hState,
                                                           const uint8_t* buffer,
                                                           size_t len)
{
    UART__Status uartStatus = UART__transmit(hState->uartState, buffer, len);

    if (UART__STATUS__SUCCESS == uartStatus) {
        return HERCULES_COMMS__STATUS__SUCCESS;
    } else if (UART__STATUS__ERROR_NOT_ENOUGH_SPACE == uartStatus) {
        return HERCULES_COMMS__STATUS__ERROR_TX_OVERFLOW;
    } else {
        return HERCULES_COMMS__STATUS__ERROR_UART_TX_FAILURE;
    }
}

static HerculesComms__Status HerculesComms__txHerculesMsg(HerculesComms__State* hState,
                                                          uint16_t resetValue,
                                                          uint16_t lowerSeqNum,
                                                          uint16_t lowerOpCode,
                                                          const uint8_t* data,
                                                          size_t dataLen)
{
    if (NULL == hState || (dataLen > 0 && NULL == data)) {
        return HERCULES_COMMS__STATUS__ERROR_NULL;
    }

    if (!hState->initialized) {
        return HERCULES_COMMS__STATUS__ERROR_NOT_INITIALIZED;
    }

    // Fill in and serialize the header
    hState->headerStruct.payloadLength = dataLen;
    hState->headerStruct.resetValue = resetValue;
    hState->headerStruct.lowerSeqNum = lowerSeqNum;
    hState->headerStruct.lowerOpCode = lowerOpCode;

    HercMsgs__Status hMsgStatus = HercMsgs__serializeHeader(&(hState->headerStruct),
                                                            hState->headerSerializationBuffer,
                                                            sizeof(hState->headerSerializationBuffer));

    DEBUG_ASSERT_EQUAL(HERC_MSGS__STATUS__SUCCESS, hMsgStatus);

    // Insert the header into the UART tx ring buffer
    HerculesComms__Status hStatus = HerculesComms__transmitBuffer(hState,
                                                                  hState->headerSerializationBuffer,
                                                                  sizeof(hState->headerSerializationBuffer));

    if (HERCULES_COMMS__STATUS__SUCCESS != hStatus) {
        return hStatus;
    }

    // Insert the data into the UART tx ring buffer, if we have any
    if (dataLen > 0) {
        return HerculesComms__transmitBuffer(hState, data, dataLen);
    } else {
        return HERCULES_COMMS__STATUS__SUCCESS;
    }
}
