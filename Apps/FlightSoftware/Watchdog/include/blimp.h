/*
 * blimp.h
 *
 *  Created on: Oct 17, 2021
 *      Author: iris
 */

#ifndef _BLIMP_H_
#define _BLIMP_H_

#include <stdint.h>
#include <msp430.h>

void blimp_enterSleep();
void blimp_normalBoot();
void blimp_safeBoot();

void blimp_bctrlEnOn();
void blimp_bctrlEnForceHigh();
void blimp_bctrlEnOff();
void blimp_latchBattOn();
void blimp_latchBattOff();
void blimp_latchBattUpdate();
void blimp_latchSetHigh();
void blimp_latchSetLow();
void blimp_latchSetPulseLow();
void blimp_latchResetHigh();
void blimp_latchResetLow();
void blimp_latchResetPulseLow();
void blimp_chargerEnOn();
void blimp_chargerEnForceHigh();
void blimp_chargerEnOff();
void blimp_regEnOn();
void blimp_regEnOff();
void blimp_battEnOn();
void blimp_battEnOff();
void blimp_vSysAllEnOn();
void blimp_vSysAllEnOff();
void blimp_vSysAllEnForceLow();

uint8_t blimp_cstat1();
uint8_t blimp_cstat2();
uint8_t blimp_isCharging();
uint8_t blimp_lstat();
uint8_t blimp_bstat();
uint8_t blimp_batteryState();

void blimp_bstat_dangerous_forceHigh();
void blimp_bstat_dangerous_forceLow();
void blimp_bstat_safe_restoreInput();


#endif /* INCLUDE_BLIMP_H_ */
