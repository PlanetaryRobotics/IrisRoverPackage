#ifndef __BSP_H__
#define __BSP_H__

#include <msp430.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern uint8_t heaterStatus;
extern uint8_t hasDeployed;

void initializeGpios();

void enableHeater();
void disableHeater();
void enable3V3PowerRail();
void disable3V3PowerRail();
void enable24VPowerRail();
void disable24VPowerRail();
void releaseHerculesReset();
void setHerculesReset();
void releaseRadioReset();
void setRadioReset();
void releaseFPGAReset();
void setFPGAReset();
void fpgaCameraSelectHi();
void fpgaCameraSelectLo();
void releaseMotorsReset();
void releaseMotor1Reset();
void releaseMotor2Reset();
void releaseMotor3Reset();
void releaseMotor4Reset();
void setMotorsReset();
void setMotor1Reset();
void setMotor2Reset();
void setMotor3Reset();
void setMotor4Reset();
void powerOnHercules();
void powerOffHercules();
void powerOnRadio();
void powerOffRadio();
void powerOnFpga();
void powerOffFpga();
void powerOnMotors();
void powerOffMotors();
void enableBatteries();
void disableBatteries();
void setDeploy();
void unsetDeploy();
void startChargingBatteries();
void stopChargingBatteries();

#ifdef __cplusplus
} /* close extern "C" */
#endif


#endif // __BSP_H__

