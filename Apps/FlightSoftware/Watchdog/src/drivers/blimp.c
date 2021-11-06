/*
 * blimp.c
 *
 *  Created on: Oct 17, 2021
 *      Author: iris
 */

#include "comms/i2c_sensors.h"
#include "drivers/blimp.h"

#include <msp430.h>

void blimp_enterSleep()
{
    blimp_latchBattOff();
    blimp_battEnOff();
    blimp_latchBattUpdate();
    // set these LOW to avoid putting voltage on pin of unpowered latch IC (becomes unpowered when BCTRLE goes LOW).
    blimp_latchSetLow();
    // set these LOW to avoid putting voltage on pin of unpowered latch IC (becomes unpowered when BCTRLE goes LOW).
    blimp_latchResetLow();
    blimp_bctrlEnOff();
}

void blimp_normalBoot()
{
    // Make sure we're not influencing BCTRLE by default:
    blimp_bctrlEnOn();

    // Disable Asynchronous Latch Controls (LS, LR):
    blimp_latchSetHigh();
    blimp_latchResetHigh();

    // Absorb the state of `BSTAT` (what it was pre-boot) in case we just recovered from a mid-mission reboot:
    if (blimp_bstat()) {
        blimp_battEnOn();
    } else {
        blimp_battEnOff();
    }

    // Only touch `LATCH_BATT` after absorbing `BSTAT` state (in case you cause a clock pulse):
    blimp_latchBattOff();

    blimp_vSysAllEnOff();
    blimp_chargerEnOff();
    blimp_regEnOff();
}

void blimp_safeBoot()
{
    blimp_enterSleep();

    blimp_vSysAllEnOff();
    blimp_chargerEnOff();
    blimp_regEnOff();
}

void blimp_latchBattOn()
{
    P3OUT |= BIT6;
}

void blimp_latchBattOff()
{
    P3OUT &= ~BIT6;
}

void blimp_latchBattUpdate()
{
    // Pulse LB low-high-low to have the latch absorb the state of BE
    P3OUT &= ~BIT6; // set low first in case LB stayed driven high due to error or became high due to cosmic radiation
    __delay_cycles(IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES);
    P3OUT |= BIT6;
    __delay_cycles(IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES);
    P3OUT &= ~BIT6;
}

void blimp_chargerEnOn()
{
    // Go HiZ (input with no pulls) to let external pull-up to VIN do the work.
    PJDIR &= ~BIT3;
    PJREN &= ~BIT3;
}

void blimp_chargerEnForceHigh()
{
    // Set as output and drive high
    PJDIR |= BIT3;
    PJOUT |= BIT3;
}

void blimp_chargerEnOff()
{
    // Set as output and drive low
    PJDIR |= BIT3;
    PJOUT &= ~BIT3;
}

void blimp_regEnOn()
{
    P1OUT |= BIT5;
}

void blimp_regEnOff()
{
    P1OUT &= ~BIT5;
}

void blimp_battEnOn()
{
    PJOUT |= BIT5;
}

void blimp_battEnOff()
{
    PJOUT &= ~BIT5;
}

void blimp_vSysAllEnOn()
{
    // Set as output and drive high:
    PJDIR |= BIT7;
    PJOUT |= BIT7;
}

void blimp_vSysAllEnOff()
{
    // Go HiZ (input with no pulls) to let external pull-down to VSAE do the work.
    // NOTE: Weird power-draw has been observed with BLiMP+BACK setup when VSAE was driven LOW instead of floated.
    PJDIR &= ~BIT7;
    PJREN &= ~BIT7;
}

void blimp_vSysAllEnForceLow()
{
    // Set as output and drive low:
    PJDIR |= BIT7;
    PJOUT &= ~BIT7;
}

uint8_t blimp_cstat1()
{
    return P1IN & BIT2;
}

uint8_t blimp_cstat2()
{
    static BOOL cstat2, lstat;
    /**
     * @todo NOTE: This is a pretty dumb way of doing this (passing both in here) but need this result quickly and this
     *       will work for now. Change it but don't make it a once per cycle thing since we typ. will need that state
     *       asap if calling this function.
     */
    I2C_Sensors__readIoExpanderBlocking(&cstat2, &lstat, 50);
    return cstat2;
}

uint8_t blimp_isCharging()
{
    return !blimp_cstat1() && blimp_cstat2();
}

uint8_t blimp_lstat()
{
    static uint8_t cstat2, lstat;

    I2C_Sensors__readIoExpanderBlocking(&cstat2, &lstat, 50);
    return lstat;
}

uint8_t blimp_bstat()
{
    // NB: **don't** restore to input here in-case it's being forced high to keep batteries connected to recover from
    // an OR-Gate fault.
    return PJIN & BIT6;
}

uint8_t blimp_batteryState()
{
    // Alias for blimp_bstat();
    return blimp_bstat();
}

void blimp_bstat_dangerous_forceHigh()
{
    // Set as output and drive high:
    PJDIR |= BIT6;
    PJOUT |= BIT6;
}

void blimp_bstat_dangerous_forceLow()
{
    // Set as output and drive low:
    PJDIR |= BIT6;
    PJOUT &= ~BIT6;
}

void blimp_bstat_safe_restoreInput()
{
    PJDIR &= ~BIT6;
    PJREN &= ~BIT6;
}
