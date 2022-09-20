#ifndef __BSP_H__
#define __BSP_H__

#include <stdint.h>
#include "flags.h"

#ifdef __cplusplus
extern "C"
{
#endif

const char* getResetReasonString(void);

void initializeGpios(WatchdogStateDetails* details);
void enableWdIntFallingEdgeInterrupt(void);
void enableWdIntRisingEdgeInterrupt(void);
void disableWdIntInterrupt(void);
uint8_t getWdIntState(void);
void enableUart0Pins(void);
void disableUart0Pins(void);
void enableUart1Pins(void);
void disableUart1Pins(void);

void clockInit(void);

void enableHeater(void);
void disableHeater(void);
void enable3V3PowerRail(void);
void disable3V3PowerRail(void);
void enableVSysAllPowerRail(void);
void disableVSysAllPowerRail(void);
void releaseHerculesReset(void);
void setHerculesReset(void);
void releaseRadioReset(void);
void setRadioReset(void);
void releaseFPGAReset(void);
void setFPGAReset(void);
void fpgaCameraSelectHi(void);
void fpgaCameraSelectLo(void);
void releaseMotorsReset(void);
void releaseMotor1Reset(void);
void releaseMotor2Reset(void);
void releaseMotor3Reset(void);
void releaseMotor4Reset(void);
void setMotorsReset(void);
void setMotor1Reset(void);
void setMotor2Reset(void);
void setMotor3Reset(void);
void setMotor4Reset(void);
void powerOnHercules(void);
void powerOffHercules(void);
void powerOnRadio(void);
void powerOffRadio(void);
void powerOnFpga(void);
void powerOffFpga(void);
void powerOnMotors(void);
void powerOffMotors(void);
void enableBatteries(void);
void disableBatteries(void);
void setDeploy(void);
void unsetDeploy(void);
void startChargingBatteries(void);
void stopChargingBatteries(void);

void readOnChipInputs(void);

#ifdef __cplusplus
} /* close extern "C" */
#endif


#endif // __BSP_H__

