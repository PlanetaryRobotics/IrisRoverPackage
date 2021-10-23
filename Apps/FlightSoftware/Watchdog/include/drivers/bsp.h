#ifndef __BSP_H__
#define __BSP_H__

#include <msp430.h>
#include <stdint.h>
#include "flags.h"

#ifdef __cplusplus
extern "C"
{
#endif

void initializeGpios(PersistentState* pState);
void enableUart0Pins(void);
void disableUart0Pins(void);
void enableUart1Pins(void);
void disableUart1Pins(void);

void clockInit(void);

void enableHeater(void);
void disableHeater(void);
void enable3V3PowerRail(void);
void disable3V3PowerRail(void);
void enable24VPowerRail(void);
void disable24VPowerRail(void);
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

inline void blimp_bctrlEnOn();
inline void blimp_bctrlEnForceHigh();
inline void blimp_bctrlEnOff();
inline void blimp_latchBattOn();
inline void blimp_latchBattOff();
void blimp_latchBattUpdate();
inline void blimp_chargerEnOn();
inline void blimp_chargerEnForceHigh();
inline void blimp_chargerEnOff();
inline void blimp_regEnOn();
inline void blimp_regEnOff();
void blimp_battEnOn();
void blimp_battEnOff();
void blimp_vSysAllEnOn();
void blimp_vSysAllEnOff();
void blimp_vSysAllEnForceLow();
inline BOOL blimp_cstat1();
inline BOOL blimp_bstat();
inline BOOL blimp_batteryState();

uint8_t getIOExpanderPort0OutputValue(void);
uint8_t getIOExpanderPort1OutputValue(void);

#ifdef __cplusplus
} /* close extern "C" */
#endif


#endif // __BSP_H__

