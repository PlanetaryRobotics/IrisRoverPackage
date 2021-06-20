/**
 * @file ground_cmd.h
 * @brief File header containing function definitions for ground_cmd.c file
 */

#ifndef __GROUND_CMD_INC
#define __GROUND_CMD_INC

#include <stdint.h>
#include "include/comms/watchdog_cmd_msgs.h"
#include "include/common.h"

#ifdef __cplusplus
extern "C"
{
#endif

// MPS: I'm not documenting these because this code will soon be restructured into a state-based paradigm.

typedef enum GroundCmd__Status
{
    GND_CMD__STATUS__SUCCESS = 0, /* Operation suceeded. */
    GND_CMD__STATUS__ERROR_NULL = -1, /* A required argument or a member of an argument was NULL */
    GND_CMD__STATUS__ERROR_BUFFER_TOO_SMALL = -2, 
    GND_CMD__STATUS__ERROR_SERIALIZATION_ERROR = -3, 
    GND_CMD__STATUS__ERROR_UNKNOWN_MESSAGE_ID = -4,
    GND_CMD__STATUS__ERROR_UNKNOWN_MAGIC_NUMBER = -5,
    GND_CMD__STATUS__ERROR_WRONG_STATE = -6,
    GND_CMD__STATUS__ERROR_UNKNOWN_RESET_VALUE = -7,

    GND_CMD__STATUS__ERROR__INTERNAL = -255 /* An unexpected internal error occurred. */
} GroundCmd__Status;

GroundCmd__Status GroundCmd__performResetCommand(WdCmdMsgs__ResetSpecificId resetValue,
                                                 WdCmdMsgs__Response* response);

GroundCmd__Status GroundCmd__performWatchdogCommand(const WdCmdMsgs__Message* msg,
                                                    WdCmdMsgs__Response* response,
                                                    WdCmdMsgs__Response* deployNotificationResponse,
                                                    BOOL* sendDeployNotificationResponse);

GroundCmd__Status GroundCmd__generateEarthHeartbeat(I2C_Sensors__Readings* i2cReadings,
                                                    uint8_t* heartbeatOutBuffer,
                                                    size_t heartbeatOutBufferLen,
                                                    size_t* outputHeartbeatSize);

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __GROUND_CMD_INC */


