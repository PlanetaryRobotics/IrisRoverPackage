/**
 * @file watchdog.h
 * @brief File header containing function definitions for watchdog
 */

#ifndef __WATCHDOG_INC
#define __WATCHDOG_INC

#include <stdint.h>
#include <stddef.h>

#include "include/comms/hercules_comms.h"
#include "include/comms/i2c_sensors.h"

#ifdef __cplusplus
extern "C"
{
#endif

// MPS: I'm not documenting these because this code will soon be restructured into a state-based paradigm.

int watchdog_init();

void watchdog_build_hercules_telem(I2C_Sensors__Readings *i2cReadings,
                                   uint8_t* telbuf,
                                   size_t telbufSize);

int watchdog_monitor(HerculesComms__State* hState);

void heaterControl();

extern uint8_t heatingControlEnabled, heaterStatus; // from heater controller
extern uint16_t Kp_heater, heater_setpoint, heater_window, PWM_limit, heater_on_val, heater_off_val;
extern uint8_t watchdog_opts;

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __WATCHDOG_INC */


