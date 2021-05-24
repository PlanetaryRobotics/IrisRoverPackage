/**
 * @file ground_cmd.h
 * @brief File header containing function definitions for ground_cmd.c file
 */

#ifndef __GROUND_CMD_INC
#define __GROUND_CMD_INC

#include <stdint.h>

uint8_t handle_watchdog_reset_cmd(uint8_t cmd);
void parse_ground_cmd(struct buffer *pp);
void send_earth_heartbeat(I2C_Sensors__Readings *i2cReadings);

#endif /* __GROUND_CMD_INC */


