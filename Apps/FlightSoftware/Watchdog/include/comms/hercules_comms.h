#ifndef __WATCHDOG_HERCULES_COMMS_H__
#define __WATCHDOG_HERCULES_COMMS_H__

#include "include/drivers/uart.h"
#include "include/comms/hercules_msgs.h"
#include "include/common.h"

typedef struct HerculesComms__State HerculesComms__State;

typedef enum HerculesComms__Status
{
    HERCULES_COMMS__STATUS__SUCCESS = 0,
    HERCULES_COMMS__STATUS__ERROR_NULL = -1,
    HERCULES_COMMS__STATUS__ERROR_ALREADY_INITIALIZED = -2,
    HERCULES_COMMS__STATUS__ERROR_NOT_INITIALIZED = -3,
    HERCULES_COMMS__STATUS__ERROR_MORE_THAN_ONE_MSG_RECEIVED = -4,
    HERCULES_COMMS__STATUS__ERROR_BUFFER_TOO_SMALL = -5,
    HERCULES_COMMS__STATUS__ERROR_TX_OVERFLOW = -6,

    HERCULES_COMMS__STATUS__ERROR_MPSM_INIT_FAILURE = -10,
    HERCULES_COMMS__STATUS__ERROR_MPSM_PROCESS_FAILURE = -11,
    HERCULES_COMMS__STATUS__ERROR_MPSM_RESET_FAILURE = -12,

    HERCULES_COMMS__STATUS__ERROR_UART_RX_FAILURE = -20,
    HERCULES_COMMS__STATUS__ERROR_UART_TX_FAILURE = -21


} HerculesComms__Status;

typedef enum HerculesComms__MsgOpcode
{
    HERCULES_COMMS__MSG_OPCODE__STROKE = 0,
    HERCULES_COMMS__MSG_OPCODE__DOWNLINK = 1,
    HERCULES_COMMS__MSG_OPCODE__UPLINK = 2
} HerculesComms__MsgOpcode;

HerculesComms__Status HerculesComms__init(HerculesComms__State** hState, UART__State* uartState);

typedef void(*HerculesMsgCallback)(HercMsgs__Header* header,
                                   uint8_t* payloadBuffer,
                                   size_t payloadSize,
                                   void* userArg);

// If we received a message, will return the data. The message is received with an
// eight byte header, but this function drops the header and returns only the data.
// This function takes a callback so that it can handle parsing multiple messages
// from the data currently available in the UART rx buffer.
HerculesComms__Status HerculesComms__tryGetMessage(HerculesComms__State* hState,
                                                   HerculesMsgCallback callback,
                                                   void* userArg,
                                                   uint8_t* buffer,
                                                   size_t bufferLen);

// Sends data from lander to Hercules. A header will be sent first with the length of the data,
// a reset value of zero, an internally tracked sequence number, and the opcode
// HERCULES_COMMS__MSG_OPCODE__UPLINK. Following the header, the given data is sent with no modification.
HerculesComms__Status HerculesComms__txUplinkMsg(HerculesComms__State* hState, const uint8_t* data, size_t dataLen);

// Sends a response to a message from Hercules back to Hercules. A header containing the length
// of the given data as well as the reset value, sequence number, and opcode from the given header,
// will be sent first and then the data will be sent without  modification (if there is any data to send).
// If dataLen is zero, data is allowed to be NULL.
HerculesComms__Status HerculesComms__txResponseMsg(HerculesComms__State* hState,
                                                   const HercMsgs__Header* sourceCommandHeader,
                                                   const uint8_t* data,
                                                   size_t dataLen);

// Clear the underlying UART rx buffer and reset the message parsing state machine
HerculesComms__Status HerculesComms__resetState(HerculesComms__State* hState);

#endif /* __WATCHDOG_HERCULES_COMMS_H__ */