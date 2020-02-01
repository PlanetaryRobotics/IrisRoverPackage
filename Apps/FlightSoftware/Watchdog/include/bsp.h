#ifndef __BSP_H__
#define __BSP_H__

#include <msp430.h>

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

void releaseFPGAReset() ;

void setFPGAReset();

void powerOnHercules();

void powerOffHercules();

void powerOnFpga();

void powerOffFpga();

void powerOnMotors();

void powerOffMotors();



#endif // __BSP_H__

