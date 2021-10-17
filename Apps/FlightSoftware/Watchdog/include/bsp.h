#ifndef __BSP_H__
#define __BSP_H__

#include <msp430.h>
#include <stdint.h>

extern uint8_t heaterStatus;
extern uint8_t hasDeployed;

void initializeGpios();

inline void enableHeater();
inline void disableHeater();
inline void enable3V3PowerRail();
inline void disable3V3PowerRail();
inline void enable24VPowerRail();
inline void disable24VPowerRail();
inline void releaseHerculesReset();
inline void setHerculesReset();
inline void releaseRadioReset();
inline void setRadioReset();
inline void releaseFPGAReset();
inline void setFPGAReset();
inline void fpgaCameraSelectHi();
inline void fpgaCameraSelectLo();
inline void releaseMotorsReset();
inline void releaseMotor1Reset();
inline void releaseMotor2Reset();
inline void releaseMotor3Reset();
inline void releaseMotor4Reset();
inline void setMotorsReset();
inline void setMotor1Reset();
inline void setMotor2Reset();
inline void setMotor3Reset();
inline void setMotor4Reset();
inline void powerOnHercules();
inline void powerOffHercules();
inline void powerOnRadio();
inline void powerOffRadio();
inline void powerOnFpga();
inline void powerOffFpga();
inline void powerOnMotors();
inline void powerOffMotors();
inline void enableBatteries();
inline void disableBatteries();
inline void setDeploy();
inline void unsetDeploy();
inline void startChargingBatteries();
inline void stopChargingBatteries();


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
inline uint8_t blimp_cstat1();
uint8_t blimp_cstat2();
inline uint8_t blimp_isCharging();
uint8_t blimp_lstat();
inline uint8_t blimp_bstat();
inline uint8_t blimp_batteryState();


inline uint8_t getIOExpanderPort0OutputValue();
inline uint8_t getIOExpanderPort1OutputValue();
#endif // __BSP_H__

