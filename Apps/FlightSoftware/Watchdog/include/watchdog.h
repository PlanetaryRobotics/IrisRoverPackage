/**
 * @file watchdog.h
 * @brief File header containing function definitions for watchdog
 */

#ifndef __WATCHDOG_INC
#define __WATCHDOG_INC

#include "include/i2c_sensors.h"

int watchdog_init();
int watchdog_monitor();
void watchdog_handle_hercules(I2C_Sensors__Readings *i2cReadings);
void heaterControl();

extern uint8_t heatingControlEnabled, heaterStatus; // from heater controller
extern uint16_t Kp_heater, heater_setpoint, heater_window, PWM_limit, heater_on_val, heater_off_val;
extern uint8_t watchdog_opts;

#endif /* __WATCHDOG_INC */


