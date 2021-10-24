/*
 * blimp.c
 *
 *  Created on: Oct 17, 2021
 *      Author: iris
 */

#include "include/i2c_sensors.h"
#include "include/blimp.h"

// Number of cycles to hold a pulse on the D-Latch for (should be at least 2us):
#define IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES 500

/**
 * @brief   Boots the BLiMP into a safe mode for testing by disabling everything.
 * NOTE: This should **NOT** be used automatically during mission since it will
 * unlatch and disconnect the batteries, which would end the mission if we were on
 * the surface.
 */
void blimp_enterSleep(){
    blimp_latchBattOff();
    blimp_battEnOff();
    blimp_latchBattUpdate();
    blimp_latchSetLow(); // set these LOW to avoid putting voltage on pin of unpowered latch IC (becomes unpowered when BCTRLE goes LOW).
    blimp_latchResetLow(); // set these LOW to avoid putting voltage on pin of unpowered latch IC (becomes unpowered when BCTRLE goes LOW).
    blimp_bctrlEnOff();
}

/**
 * @brief   Sets up all BLiMP interfaces on boot.
 */
void blimp_normalBoot(){
    // Make sure we're not influencing BCTRLE by default:
    blimp_bctrlEnOn();

    // Disable Asynchronous Latch Controls (LS, LR):
    blimp_latchSetHigh();
    blimp_latchResetHigh();

    // Absorb the state of `BSTAT` (what it was pre-boot) in case we just recovered from a mid-mission reboot:
    if(blimp_bstat()){
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

/**
 * @brief   Boots the BLiMP into a safe mode for testing by disabling everything.
 * NOTE: This should **NOT** be used during boot during mission since it will
 * unlatch and disconnect the batteries, which would end the mission if we were on
 * the surface.
 */
void blimp_safeBoot(){
    blimp_enterSleep();

    blimp_vSysAllEnOff();
    blimp_chargerEnOff();
    blimp_regEnOff();
}

/**
 * @brief     Releases BATT_CTRL_EN to enable the 3V3_LATCH regulator and power the battery control (battery latching and battery switch) circuitry.
 */
void blimp_bctrlEnOn(){
    // Go HiZ (input with no pulls) to let external pull-up to VIN do the work.
    P2DIR &= ~BIT3;
    P2REN &= ~BIT3;
}

/**
 * @brief      Forces BATT_CTRL_EN high to enable the 3V3_LATCH regulator and power the battery control (battery latching and battery switch) circuitry at the expense of a couple mA of current at 3V3 draining through the WD.
 * NOTE: Normally you'd not want to do this because there's an external pull-up to V_LANDER/V_BATT that controls
 * this but if we can't talk to latching circuitry, it might be because one of the resistors vibed loose, in which
 * case this can be helpful (and possibly save our mission).
 */
void blimp_bctrlEnForceHigh(){
    // Set as output and drive high
    P2DIR |= BIT3;
    P2OUT |= BIT3;
}

/**
 * @brief     Drives BATT_CTRL_EN low to disable the 3V3_LATCH regulator and power off the battery control (battery latching and battery switch) circuitry.
 */
void blimp_bctrlEnOff(){
    // Set as output and drive low
    P2DIR |= BIT3;
    P2OUT &= ~BIT3;
}

/**
 * @brief     Sets the clock signal to the D-Latch high (note: by itself, this doesn't latch the batteries - see: blimp_latchBattUpdate)
 */
void blimp_latchBattOn(){
    P3OUT |= BIT6;
}

/**
 * @brief     Sets the clock signal to the D-Latch low (note: by itself, this doesn't un-latch the batteries - see: blimp_latchBattUpdate)
 */
void blimp_latchBattOff(){
    P3OUT &= ~BIT6;
}

/**
 * @brief     Pulses the clock signal to the D-Latch so the latch absorbs the state of BATT_EN.
 * NOTE: Intentionally blocking for `IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES` cycles.
 */
void blimp_latchBattUpdate(){
    // Pulse LB low-high-low to have the latch absorb the state of BE
    P3OUT &= ~BIT6; // set low first in case LB stayed driven high due to error or became high due to cosmic radiation
    __delay_cycles(IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES);
    P3OUT |= BIT6;
    __delay_cycles(IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES);
    P3OUT &= ~BIT6;
}


/**
 * @brief Sets the `LATCH_SET` (set) pin on the BLiMP's D-Latch HIGH.
 * NOTE: `LATCH_SET` is normally not used on the D-latch and should be kept high.
 */
void blimp_latchSetHigh()
{
    ioExpanderPort1OutputValues |= I2C_SENSORS__IOE_P1_BIT__LATCH_SET;
    // Force write now:
    I2C_Sensors__writeIOExpanderOutputsBlocking(getIOExpanderPort0OutputValue(), getIOExpanderPort1OutputValue());

}

/**
 * @brief Sets the `LATCH_SET` (set) pin on the BLiMP's D-Latch LOW.
 * NOTE: `LATCH_SET` is normally not used on the D-latch and should be kept high.
 */
void blimp_latchSetLow()
{
    ioExpanderPort1OutputValues &= ~I2C_SENSORS__IOE_P1_BIT__LATCH_SET;
    // Force write now:
    I2C_Sensors__writeIOExpanderOutputsBlocking(getIOExpanderPort0OutputValue(), getIOExpanderPort1OutputValue());

}

/**
 * @brief     Pulses the set signal (`LATCH_SET`) low to the D-Latch, asynchronously forcibly changing the latch output (LSTAT) state to HIGH.
 * NOTE: Normally unused. Only use to intentionally override and bypass latch logic.
 * NOTE: Intentionally blocking for `IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES` cycles.
 */
void blimp_latchSetPulseLow(){
    // Pulse LS high-low-high:
    blimp_latchSetHigh(); // set high first in case LS became low due to cosmic radiation hitting expander IO register
    __delay_cycles(IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES);
    blimp_latchSetLow();
    __delay_cycles(IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES);
    blimp_latchSetHigh();
}


/**
 * @brief Sets the `LATCH_RST` (reset) pin on the BLiMP's D-Latch HIGH.
 * NOTE: `LATCH_RST` is normally not used on the D-latch and should be kept high.
 */
void blimp_latchResetHigh()
{
    ioExpanderPort0OutputValues |= I2C_SENSORS__IOE_P0_BIT__LATCH_RST;
    // Force write now:
    I2C_Sensors__writeIOExpanderOutputsBlocking(getIOExpanderPort0OutputValue(), getIOExpanderPort1OutputValue());

}

/**
 * @brief Sets the `LATCH_RST` (reset) pin on the BLiMP's D-Latch LOW.
 * NOTE: `LATCH_RST` is normally not used on the D-latch and should be kept high.
 */
void blimp_latchResetLow()
{
    ioExpanderPort0OutputValues &= ~I2C_SENSORS__IOE_P0_BIT__LATCH_RST;
    // Force write now:
    I2C_Sensors__writeIOExpanderOutputsBlocking(getIOExpanderPort0OutputValue(), getIOExpanderPort1OutputValue());

}

/**
 * @brief     Pulses the reset signal (`LATCH_RST`) low to the D-Latch, asynchronously forcibly changing the latch output (LSTAT) state to LOW.
 * NOTE: Normally unused. Only use to intentionally override and bypass latch logic.
 * NOTE: Intentionally blocking for `IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES` cycles.
 */
void blimp_latchResetPulseLow(){
    // Pulse LS high-low-high:
    blimp_latchResetHigh(); // set high first in case LR became low due to cosmic radiation hitting expander IO register
    __delay_cycles(IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES);
    blimp_latchResetLow();
    __delay_cycles(IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES);
    blimp_latchResetHigh();
}

/**
 * @brief      Enables the charging IC. NOTE: You'll also need to turn on REGE (28V charging regulator) before you can actually charge. Do this first.
 */
void blimp_chargerEnOn(){
    // Go HiZ (input with no pulls) to let external pull-up to VIN do the work.
    PJDIR &= ~BIT3;
    PJREN &= ~BIT3;
}

/**
 * @brief      Forces the charging IC input high.
 * NOTE: Normally you'd not want to do this because there's an external voltage divider that controls
 * this but if we can't enable charging, it might be because one of the resistors vibed loose, in which
 * case this can be helpful (and possibly save our mission).
 */
void blimp_chargerEnForceHigh(){
    // Set as output and drive high
    PJDIR |= BIT3;
    PJOUT |= BIT3;
}

/**
 * @brief      Disables the charging IC. NOTE: If you're no longer charging, you'll also want to turn off REGE (28V charging regulator) to not waste power.
 */
void blimp_chargerEnOff(){
    // Set as output and drive low
    PJDIR |= BIT3;
    PJOUT &= ~BIT3;
}

/**
 * @brief      Turns on the 28V lander power regulator used by the charging IC (REGE).
 */
void blimp_regEnOn()
{
    P1OUT |= BIT5;
}

/**
 * @brief      Turns off the 28V lander power regulator used by the charging IC (REGE).
 */
void blimp_regEnOff()
{
    P1OUT &= ~BIT5;
}

/**
 * @brief      Enables the battery enable override. NOTE: (by design) this won't latch the batteries on. You'll need to pulse LBATT for the latch absorbs the state of BATT_EN if you want the state to be persistent.
 */
void blimp_battEnOn(){
    PJOUT |= BIT5;
}

/**
 * @brief      Disables the battery enable override. NOTE: (by design) this won't turn off the batteries if they are also latched (LSTAT=1). To turn off the latch, you'll need to pulse LBATT so the latch absorbs the state of BATT_EN.
 */
void blimp_battEnOff(){
    PJOUT &= ~BIT5;
}

/**
 * @brief      Enables the V_SYS_ALL switch which connects V_SYS_ALL to V_SYS and powers everything beside the WD (which is already powered by V_SYS).
 */
void blimp_vSysAllEnOn(){
    // Set as output and drive high:
    PJDIR |= BIT7;
    PJOUT |= BIT7;
}

/**
 * @brief      Disables the V_SYS_ALL switch which connects V_SYS_ALL to V_SYS and powers everything beside the WD (which is already powered by V_SYS).
 */
void blimp_vSysAllEnOff(){
    // Go HiZ (input with no pulls) to let external pull-down to VSAE do the work.
    // NOTE: Weird power-draw has been observed with BLiMP+BACK setup when VSAE was driven LOW instead of floated.
    PJDIR &= ~BIT7;
    PJREN &= ~BIT7;
}


/**
 * @brief      Forces the charging IC input high.
 * NOTE: Normally you'd not want to do this because there's an external pull-down that controls
 * this but if we can't kill VSA, it might be because one of the resistors vibed loose, in which
 * case this can be helpful (and possibly save our mission).
 * NOTE: Weird power-draw has been observed with BLiMP+BACK setup when VSAE was driven LOW
 * instead of floated; so use this with caution.
 */
void blimp_vSysAllEnForceLow(){
    // Set as output and drive low:
    PJDIR |= BIT7;
    PJOUT &= ~BIT7;
}

/**
 * @brief      Reads STAT1 from BQ24650 charging IC. If H and STAT2 is H then fault. If L and STAT2 is H, then charging.
 */
uint8_t blimp_cstat1(){
    return P1IN & BIT2;
}

/**
 * @brief      Reads STAT1 from BQ24650 charging IC. If H and STAT2 is H then fault. If L and STAT2 is H, then charging.
 */
uint8_t blimp_cstat2(){
    static uint8_t cstat2, lstat;
    //! TODO: NOTE: This is a pretty dumb way of doing this (passing both in here) but need this result quickly and this will
    // work for now. Change it but don't make it a once per cycle thing since we typ. will need that state asap if calling this function.
    I2C_Sensors__readIOExpanderBlocking(&cstat2, &lstat);
    return cstat2;
}

uint8_t blimp_isCharging(){
    return !blimp_cstat1() && blimp_cstat2();
}

/**
 * @brief      Reads the present state of the battery latch output (LSTAT). ON is HIGH.
 */
uint8_t blimp_lstat(){
    static uint8_t cstat2, lstat;
    //! TODO: NOTE: This is a pretty dumb way of doing this (passing both in here) but need this result quickly and this will
    // work for now. Change it but don't make it a once per cycle thing since we typ. will need that state asap if calling this function.
    I2C_Sensors__readIOExpanderBlocking(&cstat2, &lstat);
    return lstat;
}

/**
 * @brief      Reads the present state of the battery switch (BSTAT). ON is HIGH.
 */
uint8_t blimp_bstat(){
    // NB: **don't** restore to input here in-case it's being forced high to keep batteries connected to recover from an OR-Gate fault.
    return PJIN & BIT6;
}


/**
 * @brief      Reads the present state of the battery switch (BSTAT). ON is HIGH.
 */
uint8_t blimp_batteryState(){
    // Alias for blimp_bstat();
    return blimp_bstat();
}

/*
* @brief      Changes WD `BSTAT` pin to an output and drives it high. **NEVER** do this under normal circumstances.
* This is only here to allow for recovery from a VERY specific failure mode where the OR-Gate fails during cis-lunar transit
* and ends up floating the output. If that failure were to happen and this command were not to exist, we'd have no way of
* enabling the batteries (necessary before deployment).
* **NOTE:** Using this command when there *isn't* a fault in the OR-gate could cause the OR gate and WD to drive the `BSTAT`
* line to different levels, likely blowing one or both of them. This is very much a command of last-resort.
*/
void blimp_bstat_dangerous_forceHigh(){
    // Set as output and drive high:
    PJDIR |= BIT6;
    PJOUT |= BIT6;
}


/*
* @brief      Changes WD `BSTAT` pin to an output and drives it low. Incorrect use is dangerous. **NEVER** do this under normal circumstances.
* This is only here to allow for recovery from a VERY specific failure mode where the OR-Gate fails during cis-lunar transit
* and ends up floating the output. If that failure were to happen and this command were not to exist, we'd have no way of
* disabling the batteries (to keep them from draining during transit).
* **NOTE:** Using this command when there *isn't* a fault in the OR-gate could cause the OR gate and WD to drive the `BSTAT`
* line to different levels, likely blowing one or both of them. This is very much a command of last-resort.
*/
void blimp_bstat_dangerous_forceLow(){
    // Set as output and drive low:
    PJDIR |= BIT6;
    PJOUT &= ~BIT6;
}

/*
* @brief      Returns the WD `BSTAT` pin to its proper state as an input (to restore from using the "dangerous_force" commands)
*/
void blimp_bstat_safe_restoreInput(){
    PJDIR &= ~BIT6;
    PJREN &= ~BIT6;
}


