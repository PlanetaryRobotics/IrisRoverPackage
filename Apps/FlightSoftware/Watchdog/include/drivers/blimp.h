/*
 * blimp.h
 *
 *  Created on: Oct 17, 2021
 *      Author: iris
 */

#ifndef _BLIMP_H_
#define _BLIMP_H_

#include <stdint.h>
#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Number of cycles to hold a pulse on the D-Latch for (should be at least 2us)
 */
#define IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES 500

/**
 * @brief Boots the BLiMP into a safe mode for testing by disabling everything.
 *
 * @note This should **NOT** be used automatically during mission since it will unlatch and disconnect the batteries,
 *       which would end the mission if we were on the surface.
 */
void blimp_enterSleep();

/**
 * @brief Sets up all BLiMP interfaces on boot.
 */
void blimp_normalBoot();

/**
 * @brief Boots the BLiMP into a safe mode for testing by disabling everything.
 *
 * @note This should **NOT** be used during boot during mission since it will unlatch and disconnect the batteries,
 *       which would end the mission if we were on the surface.
 */
void blimp_safeBoot();

/**
 * @brief Releases BATT_CTRL_EN to enable the 3V3_LATCH regulator and power the battery control (battery latching and
 *        battery switch) circuitry.
 */
void blimp_bctrlEnOn();

/**
 * @brief Forces BATT_CTRL_EN high to enable the 3V3_LATCH regulator and power the battery control (battery latching and
 *        battery switch) circuitry at the expense of a couple mA of current at 3V3 draining through the WD.
 *
 * @note Normally you'd not want to do this because there's an external pull-up to V_LANDER/V_BATT that controls this
 *       but if we can't talk to latching circuitry, it might be because one of the resistors vibed loose, in which case
 *       this can be helpful (and possibly save our mission).
 */
void blimp_bctrlEnForceHigh();

/**
 * @brief Drives BATT_CTRL_EN low to disable the 3V3_LATCH regulator and power off the battery control (battery latching
 *        and battery switch) circuitry.
 */
void blimp_bctrlEnOff();

/**
 * @brief Sets the clock signal to the D-Latch high
 *
 * @note By itself, this doesn't latch the batteries -- see: blimp_latchBattUpdate
 *
 * @see blimp_latchBattUpdate
 */
void blimp_latchBattOn();

/**
 * @brief Sets the clock signal to the D-Latch low
 *
 * @note By itself, this doesn't un-latch the batteries -- see: blimp_latchBattUpdate
 *
 * @see blimp_latchBattUpdate
 */
void blimp_latchBattOff();

/**
 * @brief Pulses the clock signal to the D-Latch so the latch absorbs the state of BATT_EN.
 *
 * @note Intentionally blocks for (2 * `IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES`) cycles.
 */
void blimp_latchBattUpdate();

/**
 * @brief Sets the `LATCH_SET` (set) pin on the BLiMP's D-Latch HIGH.
 *
 * @note `LATCH_SET` is normally not used on the D-latch and should be kept high.
 */
void blimp_latchSetHigh();

/**
 * @brief Sets the `LATCH_SET` (set) pin on the BLiMP's D-Latch LOW.
 *
 * @note `LATCH_SET` is normally not used on the D-latch and should be kept high.
 */
void blimp_latchSetLow();

/**
 * @brief Pulses the set signal (`LATCH_SET`) low to the D-Latch, asynchronously forcibly changing the latch output
 *        (LSTAT) state to HIGH.
 *
 * @note Normally unused. Only use to intentionally override and bypass latch logic.
 * @note Intentionally blocking for (2 * `IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES`) cycles.
 */
void blimp_latchSetPulseLow();

/**
 * @brief Sets the `LATCH_RST` (reset) pin on the BLiMP's D-Latch HIGH.
 *
 * @note `LATCH_RST` is normally not used on the D-latch and should be kept high.
 */
void blimp_latchResetHigh();

/**
 * @brief Sets the `LATCH_RST` (reset) pin on the BLiMP's D-Latch LOW.
 *
 * @note `LATCH_RST` is normally not used on the D-latch and should be kept high.
 */
void blimp_latchResetLow();

/**
 * @brief Pulses the reset signal (`LATCH_RST`) low to the D-Latch, asynchronously forcibly changing the latch output
 *        (LSTAT) state to LOW.
 *
 * @note Normally unused. Only use to intentionally override and bypass latch logic.
 * @note Intentionally blocking for (2 * `IRIS_BLIMP_DLATCH_PULSE_DURATION_CYCLES`) cycles.
 */
void blimp_latchResetPulseLow();


/**
 * @brief Enables the charging IC.
 *
 * @note You'll also need to turn on REGE (28V charging regulator) before you can actually charge. Do this first.
 */
void blimp_chargerEnOn();

/**
 * @brief Forces the charging IC input high.
 *
 * @note Normally you'd not want to do this because there's an external voltage divider that controls this but if we
 *       can't enable charging, it might be because one of the resistors vibed loose, in which case this can be helpful
 *       (and possibly save our mission).
 */
void blimp_chargerEnForceHigh();

/**
 * @brief Disables the charging IC. NOTE: If you're no longer charging, you'll also want to turn off REGE (28V charging
 *        regulator) to not waste power.
 */
void blimp_chargerEnOff();

/**
 * @brief Turns on the 28V lander power regulator used by the charging IC (REGE).
 */
void blimp_regEnOn();

/**
 * @brief STurns off the 28V lander power regulator used by the charging IC (REGE).
 */
void blimp_regEnOff();

/**
 * @brief Enables the battery enable override.
 *
 * @note By design, this won't latch the batteries on. You'll need to pulse LBATT for the latch absorbs the state of
 *       BATT_EN if you want the state to be persistent.
 */
void blimp_battEnOn();

/**
 * @brief Disables the battery enable override.
 *
 * @note By design, this won't turn off the batteries if they are also latched (LSTAT=1). To turn off the latch, you'll
 *       need to pulse LBATT so the latch absorbs the state of BATT_EN.
 */
void blimp_battEnOff();

/**
 * @brief Enables the V_SYS_ALL switch which connects V_SYS_ALL to V_SYS and powers everything beside the WD (which is
 *        already powered by V_SYS).
 */
void blimp_vSysAllEnOn();

/**
 * @brief Disables the V_SYS_ALL switch which connects V_SYS_ALL to V_SYS and powers everything beside the WD (which is
 *        already powered by V_SYS).
 */
void blimp_vSysAllEnOff();

/**
 * @brief Forces the V_SYS_ALL switch low.
 *
 * @note Normally you'd not want to do this because there's an external pull-down that controls this but if we can't
 *       kill VSA, it might be because one of the resistors vibed loose, in which case this can be helpful (and possibly
 *       save our mission).
 * @note Weird power-draw has been observed with BLiMP+BACK setup when VSAE was driven LOW instead of floated; so use
 *       this with caution.
 */
void blimp_vSysAllEnForceLow();

/**
 * @brief Reads STAT1 from BQ24650 charging IC. If H and STAT2 is H then fault. If L and STAT2 is H, then charging.
 */
uint8_t blimp_cstat1();

/**
 * @brief Reads STAT2 from BQ24650 charging IC. If H and STAT2 is H then fault. If L and STAT2 is H, then charging.
 */
uint8_t blimp_cstat2();

uint8_t blimp_isCharging();

/**
 * @brief Reads the present state of the battery latch output (LSTAT). ON is HIGH.
 */
uint8_t blimp_lstat();

/**
 * @brief  Reads the present state of the battery switch (BSTAT). ON is HIGH.
 */
uint8_t blimp_bstat();

/**
 * @brief Reads the present state of the battery switch (BSTAT). ON is HIGH.
 */
uint8_t blimp_batteryState();

/**
 * @brief Changes WD `BSTAT` pin to an output and drives it high. **NEVER** do this under normal circumstances.
 *
 * This is only here to allow for recovery from a VERY specific failure mode where the OR-Gate fails during cis-lunar
 * transit and ends up floating the output. If that failure were to happen and this command were not to exist, we'd have
 * no way of enabling the batteries (necessary before deployment).
 *
 * @warning Using this command when there *isn't* a fault in the OR-gate could cause the OR gate and WD to drive the
 *          `BSTAT` line to different levels, likely blowing one or both of them. This is very much a command of
 *          last-resort.
 */
void blimp_bstat_dangerous_forceHigh();

/**
 * @brief Changes WD `BSTAT` pin to an output and drives it low. Incorrect use is dangerous. **NEVER** do this under
 *        normal circumstances.
 *
 * This is only here to allow for recovery from a VERY specific failure mode where the OR-Gate fails during cis-lunar
 * transit and ends up floating the output. If that failure were to happen and this command were not to exist, we'd have
 * no way of disabling the batteries (to keep them from draining during transit).
 *
 * @warning Using this command when there *isn't* a fault in the OR-gate could cause the OR gate and WD to drive the
 *          `BSTAT` line to different levels, likely blowing one or both of them. This is very much a command of
 *          last-resort.
*/
void blimp_bstat_dangerous_forceLow();

/**
 * @brief Returns the WD `BSTAT` pin to its proper state as an input (to restore from using the "dangerous_force"
 *        commands)
 */
void blimp_bstat_safe_restoreInput();

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* INCLUDE_BLIMP_H_ */
