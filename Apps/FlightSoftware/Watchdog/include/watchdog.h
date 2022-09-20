/**
 * @file watchdog.h
 * @brief File header containing function definitions for watchdog
 */

#ifndef __WATCHDOG_INC
#define __WATCHDOG_INC

#include <stdint.h>
#include <stddef.h>

#include "comms/hercules_comms.h"
#include "comms/i2c_sensors.h"
#include "drivers/adc.h"
#include "flags.h"

#ifdef __cplusplus
extern "C"
{
#endif

// MPS: I'm not documenting these because this code will soon be restructured into a state-based paradigm.

int watchdog_init(volatile uint16_t* watchdogFlags,
                  volatile uint16_t* timeCountCentiseconds,
                  const HeaterParams* hParams);

void watchdog_build_hercules_telem(const I2C_Sensors__Readings *i2cReadings,
                                   const AdcValues* adcValues,
                                   BOOL hasDeployed,
                                   uint8_t* telbuf,
                                   size_t telbufSize);

int watchdog_monitor(HerculesComms__State* hState,
                     volatile uint16_t* watchdogFlags,
                     uint8_t* watchdogOpts,
                     BOOL* writeIOExpander,
                     WatchdogStateDetails* details);

uint16_t watchdog_get_wd_int_flat_duration(void);

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __WATCHDOG_INC */


