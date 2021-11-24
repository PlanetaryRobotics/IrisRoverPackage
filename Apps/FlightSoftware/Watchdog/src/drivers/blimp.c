/*
 * blimp.c
 *
 *  Created on: Oct 17, 2021
 *      Author: iris
 */

#include "comms/i2c_sensors.h"
#include "drivers/blimp.h"
#include "common.h"

#include <msp430.h>
#include <stddef.h>

static WatchdogStateDetails* detailsPtr = NULL;

void blimp_initialize(WatchdogStateDetails* details)
{
    DEBUG_LOG_NULL_CHECK_RETURN(details, "Parameter is NULL", /* nothing, b/c void return */);
    detailsPtr = details;
    SET_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__LATCH_SET_IS_INPUT);
    SET_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__LATCH_RESET_IS_INPUT);
}

void blimp_enterSleep()
{
    blimp_latchBattOff();
    blimp_battEnOff();
    blimp_latchBattUpdate();
    // set these OFF to avoid putting voltage on pin of unpowered latch IC (becomes unpowered when BCTRLE goes LOW).
    blimp_latchSetOff();
    // set these OFF to avoid putting voltage on pin of unpowered latch IC (becomes unpowered when BCTRLE goes LOW).
    blimp_latchResetOff();
}

void blimp_normalBoot()
{
    // Disable Asynchronous Latch Controls (LS, LR):
    blimp_latchSetOff();
    blimp_latchResetOff();

    // Absorb the state of `BSTAT` (what it was pre-boot) in case we just recovered from a mid-mission reboot:
//    if (blimp_bstat()) {
//        blimp_battEnOn();
//    } else {
//        blimp_battEnOff();
//    }

    // Only touch `LATCH_BATT` after absorbing `BSTAT` state (in case you cause a clock pulse):
//    blimp_latchBattOff();

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
    SET_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__LATCH_BATT);
}

void blimp_latchBattOff()
{
    P3OUT &= ~BIT6;
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__LATCH_BATT);
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
    SET_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__CHRG_EN);
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__CHRG_EN_FORCE_HIGH);
}

void blimp_chargerEnForceHigh()
{
    // Set as output and drive high
    PJDIR |= BIT3;
    PJOUT |= BIT3;
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__CHRG_EN);
    SET_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__CHRG_EN_FORCE_HIGH);
}

void blimp_chargerEnOff()
{
    // Set as output and drive low
    PJDIR |= BIT3;
    PJOUT &= ~BIT3;
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__CHRG_EN);
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__CHRG_EN_FORCE_HIGH);
}

void blimp_regEnOn()
{
    P1OUT |= BIT5;
    SET_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__V_LANDER_REG_EN);
}

void blimp_regEnOff()
{
    P1OUT &= ~BIT5;
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__V_LANDER_REG_EN);
}

void blimp_battEnOn()
{
    PJOUT |= BIT5;
    SET_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__BATTERY_EN);
}

void blimp_battEnOff()
{
    PJOUT &= ~BIT5;
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__BATTERY_EN);
}

void blimp_vSysAllEnOn()
{
    // Set as output and drive high:
    PJDIR |= BIT7;
    PJOUT |= BIT7;
    SET_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__V_SYS_ALL_EN);
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__V_SYS_ALL_EN_FORCE_LOW);
}

void blimp_vSysAllEnOff()
{
    // Go HiZ (input with no pulls) to let external pull-down to VSAE do the work.
    // NOTE: Weird power-draw has been observed with BLiMP+BACK setup when VSAE was driven LOW instead of floated.
    PJDIR &= ~BIT7;
    PJREN &= ~BIT7;
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__V_SYS_ALL_EN);
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__V_SYS_ALL_EN_FORCE_LOW);
}

void blimp_vSysAllEnForceLow()
{
    // Set as output and drive low:
    PJDIR |= BIT7;
    PJOUT &= ~BIT7;
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__V_SYS_ALL_EN);
    SET_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__V_SYS_ALL_EN_FORCE_LOW);
}

uint8_t blimp_cstat1()
{
    if (P1IN & BIT2) {
        SET_IPASBI_IN_UINT(detailsPtr->m_inputPinAndStateBits, IPASBI__CHARGE_STAT1);
    } else {
        CLEAR_IPASBI_IN_UINT(detailsPtr->m_inputPinAndStateBits, IPASBI__CHARGE_STAT1);
    }

    return P1IN & BIT2;
}

uint8_t blimp_cstat2()
{
    static uint8_t cstat2, lstat;
    /**
     * @todo NOTE: This is a pretty dumb way of doing this (passing both in here) but need this result quickly and this
     *       will work for now. Change it but don't make it a once per cycle thing since we typ. will need that state
     *       asap if calling this function.
     */
    I2C_Sensors__readIoExpanderBlocking(&cstat2, &lstat, 50);

    if (cstat2) {
        SET_IPASBI_IN_UINT(detailsPtr->m_inputPinAndStateBits, IPASBI__CHARGE_STAT2);
    } else {
        CLEAR_IPASBI_IN_UINT(detailsPtr->m_inputPinAndStateBits, IPASBI__CHARGE_STAT2);
    }

    if (lstat) {
        SET_IPASBI_IN_UINT(detailsPtr->m_inputPinAndStateBits, IPASBI__LATCH_STAT);
    } else {
        CLEAR_IPASBI_IN_UINT(detailsPtr->m_inputPinAndStateBits, IPASBI__LATCH_STAT);
    }

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

    if (cstat2) {
        SET_IPASBI_IN_UINT(detailsPtr->m_inputPinAndStateBits, IPASBI__CHARGE_STAT2);
    } else {
        CLEAR_IPASBI_IN_UINT(detailsPtr->m_inputPinAndStateBits, IPASBI__CHARGE_STAT2);
    }

    if (lstat) {
        SET_IPASBI_IN_UINT(detailsPtr->m_inputPinAndStateBits, IPASBI__LATCH_STAT);
    } else {
        CLEAR_IPASBI_IN_UINT(detailsPtr->m_inputPinAndStateBits, IPASBI__LATCH_STAT);
    }

    return lstat;
}

uint8_t blimp_bstat()
{
    // NB: **don't** restore to input here in-case it's being forced high to keep batteries connected to recover from
    // an OR-Gate fault.
    if (PJIN & BIT6) {
        SET_IPASBI_IN_UINT(detailsPtr->m_inputPinAndStateBits, IPASBI__BATT_STAT);
    } else {
        CLEAR_IPASBI_IN_UINT(detailsPtr->m_inputPinAndStateBits, IPASBI__BATT_STAT);
    }

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

    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__BATT_STAT_IS_INPUT);
    SET_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__BATT_STAT);
}

void blimp_bstat_dangerous_forceLow()
{
    // Set as output and drive low:
    PJDIR |= BIT6;
    PJOUT &= ~BIT6;

    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__BATT_STAT_IS_INPUT);
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__BATT_STAT);
}

void blimp_bstat_safe_restoreInput()
{
    PJDIR &= ~BIT6;
    PJREN &= ~BIT6;

    SET_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__BATT_STAT_IS_INPUT);
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__BATT_STAT);
}

void blimp_latchSetOff()
{
    uint8_t port0DirectionValue = 0;
    uint8_t port1DirectionValue = 0;

    I2C_Sensors__getIoExpanderPortDirections(&port0DirectionValue, &port1DirectionValue);

    // 0 is output, 1 is input, we want it to be input so if it is cleared then we need to change the port directions
    if (!(port1DirectionValue & I2C_SENSORS__IOE_P1_BIT__LATCH_SET)) {
        port1DirectionValue |= I2C_SENSORS__IOE_P1_BIT__LATCH_SET;
        I2C_Sensors__writeIoExpanderPortDirectionsBlocking(port0DirectionValue, port1DirectionValue, 50);
    }

    SET_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__LATCH_SET_IS_INPUT);
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__LATCH_SET);
}

void blimp_latchSetHigh()
{
    uint8_t port0DirectionValue = 0;
    uint8_t port1DirectionValue = 0;

    I2C_Sensors__getIoExpanderPortDirections(&port0DirectionValue, &port1DirectionValue);

    // 0 is output, 1 is input, we want it to be input so if it is set then we need to change the port directions
    if (port1DirectionValue & I2C_SENSORS__IOE_P1_BIT__LATCH_SET) {
        port1DirectionValue &= ~I2C_SENSORS__IOE_P1_BIT__LATCH_SET;
        I2C_Sensors__writeIoExpanderPortDirectionsBlocking(port0DirectionValue, port1DirectionValue, 50);
    }

    I2C_Sensors__setIoExpanderPort1OutputBits(I2C_SENSORS__IOE_P1_BIT__LATCH_SET);
    // Force write now:
    I2C_Sensors__writeIoExpanderCurrentValuesBlocking(50);

    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__LATCH_SET_IS_INPUT);
    SET_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__LATCH_SET);
}

void blimp_latchSetLow()
{
    uint8_t port0DirectionValue = 0;
    uint8_t port1DirectionValue = 0;

    I2C_Sensors__getIoExpanderPortDirections(&port0DirectionValue, &port1DirectionValue);

    // 0 is output, 1 is input, we want it to be input so if it is set then we need to change the port directions
    if (port1DirectionValue & I2C_SENSORS__IOE_P1_BIT__LATCH_SET) {
        port1DirectionValue &= ~I2C_SENSORS__IOE_P1_BIT__LATCH_SET;
        I2C_Sensors__writeIoExpanderPortDirectionsBlocking(port0DirectionValue, port1DirectionValue, 50);
    }

    I2C_Sensors__clearIoExpanderPort1OutputBits(I2C_SENSORS__IOE_P1_BIT__LATCH_SET);
    // Force write now:
    I2C_Sensors__writeIoExpanderCurrentValuesBlocking(50);

    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__LATCH_SET_IS_INPUT);
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__LATCH_SET);
}

void blimp_latchSetPulseLow()
{
    // Pulse LS high-low-high:
    blimp_latchSetHigh(); // set high first in case LS became low due to cosmic radiation hitting expander IO register
    __delay_cycles(IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES);
    blimp_latchSetLow();
    __delay_cycles(IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES);
    blimp_latchSetHigh();
}

void blimp_latchResetOff()
{
    uint8_t port0DirectionValue = 0;
    uint8_t port1DirectionValue = 0;

    I2C_Sensors__getIoExpanderPortDirections(&port0DirectionValue, &port1DirectionValue);

    // 0 is output, 1 is input, we want it to be input so if it is cleared then we need to change the port directions
    if (!(port0DirectionValue & I2C_SENSORS__IOE_P0_BIT__LATCH_RST)) {
        port0DirectionValue |= I2C_SENSORS__IOE_P0_BIT__LATCH_RST;
        I2C_Sensors__writeIoExpanderPortDirectionsBlocking(port0DirectionValue, port1DirectionValue, 50);
    }

    SET_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__LATCH_RESET_IS_INPUT);
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__LATCH_RESET);
}

void blimp_latchResetHigh()
{
    uint8_t port0DirectionValue = 0;
    uint8_t port1DirectionValue = 0;

    I2C_Sensors__getIoExpanderPortDirections(&port0DirectionValue, &port1DirectionValue);

    // 0 is output, 1 is input, we want it to be input so if it is set then we need to change the port directions
    if (port0DirectionValue & I2C_SENSORS__IOE_P0_BIT__LATCH_RST) {
        port0DirectionValue &= ~I2C_SENSORS__IOE_P0_BIT__LATCH_RST;
        I2C_Sensors__writeIoExpanderPortDirectionsBlocking(port0DirectionValue, port1DirectionValue, 50);
    }

    I2C_Sensors__setIoExpanderPort0OutputBits(I2C_SENSORS__IOE_P0_BIT__LATCH_RST);
    // Force write now:
    I2C_Sensors__writeIoExpanderCurrentValuesBlocking(50);

    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__LATCH_RESET_IS_INPUT);
    SET_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__LATCH_RESET);
}

void blimp_latchResetLow()
{
    uint8_t port0DirectionValue = 0;
    uint8_t port1DirectionValue = 0;

    I2C_Sensors__getIoExpanderPortDirections(&port0DirectionValue, &port1DirectionValue);

    // 0 is output, 1 is input, we want it to be input so if it is set then we need to change the port directions
    if (port0DirectionValue & I2C_SENSORS__IOE_P0_BIT__LATCH_RST) {
        port0DirectionValue &= ~I2C_SENSORS__IOE_P0_BIT__LATCH_RST;
        I2C_Sensors__writeIoExpanderPortDirectionsBlocking(port0DirectionValue, port1DirectionValue, 50);
    }

    I2C_Sensors__clearIoExpanderPort0OutputBits(I2C_SENSORS__IOE_P0_BIT__LATCH_RST);
    // Force write now:
    I2C_Sensors__writeIoExpanderCurrentValuesBlocking(50);

    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__LATCH_RESET_IS_INPUT);
    CLEAR_OPSBI_IN_UINT(detailsPtr->m_outputPinBits, OPSBI__LATCH_RESET);
}

void blimp_latchResetPulseLow()
{
    // Pulse LS high-low-high:
    blimp_latchResetHigh(); // set high first in case LR became low due to cosmic radiation hitting expander IO register
    __delay_cycles(IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES);
    blimp_latchResetLow();
    __delay_cycles(IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES);
    blimp_latchResetHigh();
}
