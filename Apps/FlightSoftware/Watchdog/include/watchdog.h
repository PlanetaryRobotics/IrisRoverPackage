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

#endif /* __WATCHDOG_INC */


