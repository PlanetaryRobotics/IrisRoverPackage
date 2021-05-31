#ifndef __WATCHDOG_HERCULES_COMMS_H__
#define __WATCHDOG_HERCULES_COMMS_H__

#include "include/drivers/uart.h"
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

HerculesComms__Status HerculesComms__init(HerculesComms__State** hState, UART__State* uartState);

// If we received a message, will return the data. The message is received with an
// eight byte header, but this function drops the header and returns only the data
HerculesComms__Status HerculesComms__tryGetMessage(HerculesComms__State* hState,
                                                   BOOL* gotMessage,
                                                   uint8_t* buffer,
                                                   size_t bufferLen,
                                                   size_t* rxDataLen);

// Sends data directly over the UART, with no modification
HerculesComms__Status HerculesComms__txData(HerculesComms__State* hState, const uint8_t* data, size_t dataLen);

// Clear the underlying UART rx buffer and reset the message parsing state machine
HerculesComms__Status HerculesComms__resetState(HerculesComms__State* hState);

#endif /* __WATCHDOG_HERCULES_COMMS_H__ */