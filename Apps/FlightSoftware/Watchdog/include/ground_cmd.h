/**
 * @file ground_cmd.h
 * @brief File header containing function definitions for ground_cmd.c file
 */

#ifndef __GROUND_CMD_INC
#define __GROUND_CMD_INC

#include <stdint.h>
#include "include/comms/watchdog_cmd_msgs.h"
#include "include/common.h"

typedef enum GroundCmd__Status
{
    GND_CMD__STATUS__SUCCESS = 0, /* Operation suceeded. */
    GND_CMD__STATUS__ERROR_NULL = -1, /* A required argument or a member of an argument was NULL */
    GND_CMD__STATUS__ERROR_BUFFER_TOO_SMALL = -2, 
    GND_CMD__STATUS__ERROR_SERIALIZATION_ERROR = -3, 
    GND_CMD__STATUS__ERROR_UNKNOWN_MESSAGE_ID = -4,
    GND_CMD__STATUS__ERROR_UNKNOWN_MAGIC_NUMBER = -5,

    GND_CMD__STATUS__ERROR__INTERNAL = -255 /* An unexpected internal error occurred. */
} GroundCmd__Status;

GroundCmd__Status GroundCmd__performWatchdogCommand(const WdCmdMsgs__Message* msg,
                                                    WdCmdMsgs__Response* response,
                                                    WdCmdMsgs__Response* deployNotificationResponse,
                                                    BOOL* sendDeployNotificationResponse);

GroundCmd__Status GroundCmd__parseGroundCommand(uint8_t* src,
                                                size_t srcLen,
                                                GroundResponse* gndResp);

uint8_t handle_watchdog_reset_cmd(uint8_t cmd);
void parse_ground_cmd(struct buffer *pp);
void send_earth_heartbeat(I2C_Sensors__Readings *i2cReadings);

#endif /* __GROUND_CMD_INC */


