
#include <assert.h>
#include <msp430.h>
#include <stdint.h>
#include <string.h>

#include "comms/debug_comms.h"
#include "comms/ip_udp.h"
#include "comms/i2c_sensors.h"
#include "drivers/adc.h"
#include "drivers/uart.h"
#include "drivers/bsp.h"
#include "drivers/blimp.h"
#include "event/event.h"
#include "event/event_queue.h"
#include "flags.h"
#include "watchdog.h"
#include "utils/time.h"

// Make sure this matches enum in `stateMachine/RoverState.hpp` (can't just incl. that b/c it's C).
#define ROVERSTATE_AS_UINT__MISSION 16

// These two only exist so I can be 100% confident that the Ptr variables
// below them will never be NULL. Since the Ptr values should be set
// to non-NULL values in watchdog_init prior to enabling the interrupts,
// these three shouldn't ever actually be used.
static volatile uint32_t shouldBeUnusedWatchdogFlags = 0;
static volatile uint16_t shouldBeUnusedTimeCount = 0;

// static volatile uint32_t* watchdogFlagsPtr = &shouldBeUnusedWatchdogFlags;
static volatile uint16_t *timeCountCentisecondsPtr = &shouldBeUnusedTimeCount;

static volatile BOOL lookingForWdIntFallingEdge = TRUE;
static volatile uint16_t wdIntLastEdgeTime = 0;
static volatile uint16_t wdIntTimeBetweenLastEdges = 0;

/**
 * Set up the ISRs for the watchdog
 */
int watchdog_init(volatile uint32_t *watchdogFlags,
                  volatile uint16_t *timeCountCentiseconds,
                  const HeaterParams *hParams)
{
    DEBUG_LOG_NULL_CHECK_RETURN(watchdogFlags, "Parameter is NULL", -1);
    DEBUG_LOG_NULL_CHECK_RETURN(timeCountCentiseconds, "Parameter is NULL", -1);
    DEBUG_LOG_NULL_CHECK_RETURN(hParams, "Parameter is NULL", -1);

    //    watchdogFlagsPtr = watchdogFlags;
    timeCountCentisecondsPtr = timeCountCentiseconds;

    /* =====================================================
     Timer_A setup (used for getting interrupts to do periodic actions like sending heartbeat)
     ===================================================== */

    // Use ACLK (which has a frequency of about 9.4 kHz (typ.)) as Timer_A clock source. This also ensures
    // that the timer is stopped, as all bits in TA0CTL other than TASSEL will be cleared. This means the
    // MC bits will be set to zero, which means that the timer is stopped.
    TA0CTL = TASSEL_1;

    // The input divider for the input clock is determined by the ID bits in TAxCTL and the TAIDEX bits
    // in TAxEX0. If the resultant timer frequency is f_timer, the frequency of the input clock is f_clk,
    // the clock divisor determined by the ID bits is d_ID, and the clock divisor determined by the TAIDEX
    // bits is d_TAIDEX, then their relationship is:
    //
    //    f_timer = (f_clk / d_ID ) / d_TAIDEX
    //    f_timer = f_clk / (d_ID * d_TAIDEX)
    //
    // Here, we use f_clk = 9.4 kHz, d_ID = 1, and d_TAIDEX = 1, so f_timer = 9.4 kHz

    // Sets ID to 00b, making it divide by 1 (i.e. does not divide).
    TA0CTL |= ID_0;

    // Sets TAIDEX to 00b makes it divide by 1 (i.e. does not divide).
    TA0EX0 = TAIDEX_0;

    // Per the note in section 25.2.1.1 (pg 645) of the user guide, TACLR should be set after modifying
    // the ID or TAIDEX bits. Doing so resets the clock divider logic, and the new settings are
    // used once the TACLR bit is cleared. We do not need to manually clear this bit after setting it,
    // as it is reset automatically.
    TA0CTL |= TACLR;

    // Set CCR1 to compare mode and enable the CCR1 interrupt, which is part of the TA0IV interrupt vector (CCR0, on
    // the other hand, has its own interrupt vector).
    TA0CCTL1 = CCIE;

    // Set the value of Timer_A that will cause a CCR1 compare interrupt to occur.
    // This determines the period of the PWM output. With this set to 94 and the timer clock
    // frequency of 9400, the time between each interrupt is 94 / 9400 = 0.01 seconds, or 10 ms.
    // We subtract one from the desired value because the timer counts starting at 0, rather than 1.
    TA0CCR1 = 94 - 1;

    // Enable the TAIFG interrupt request. This interrupt occurs when the timer overflows.
    // Since Timer_A is a 16-bit timer and the timer clock frequency is ~9.4kHz, this means
    // that we will get the TAIFG interrupt once about every 7 seconds (2^16 = 65536, and
    // 65536 / 9400 = ~6.97 seconds)
    TA0CTL |= TAIE;

    // Finished setting up Timer_A, so start the timer in "Continuous" mode (i.e. timer will count up to 0xFFFF,
    // then overflow to zero)
    TA0CTL |= MC_2;

    /* =====================================================
     Timer_B setup (used to generate PWM for heater control)
     ===================================================== */

    // Use SMCLK (which has a frequency of 8 MHz) as Timer_B clock source. This also ensures
    // that the timer is stopped, as all bits in TB0CTL other than TBSSEL will be cleared. This means the
    // MC bits will be set to zero, which means that the timer is stopped.
    TB0CTL = TBSSEL__SMCLK;

    // The input divier for the input clock is determined by the ID bits in TBxCTL and the TBIDEX bits
    // in TAxEX0. If the resultant timer frequency is f_timer, the frequency of the input clock is f_clk,
    // the clock divisor determined by the ID bits is d_ID, and the clock divisor determined by the TBIDEX
    // bits is d_TBIDEX, then their relationship is:
    //
    //    f_timer = (f_clk / d_ID ) / d_TBIDEX
    //    f_timer = f_clk / (d_ID * d_TBIDEX)
    //
    // Here, we use f_clk = 8 MHz, d_ID = 1, and d_TBIDEX = 1, so f_timer = 8 MHz

    // ID bits in TB0CTL will have been set to 00b in the previous assignment, making it divide by 1 (i.e.
    // does not divide).

    // Sets TBIDEX to 00b makes it divide by 1 (i.e. does not divide).
    TB0EX0 = TAIDEX_0;

    // Per the note in section 25.2.1.1 (pg 645) of the user guide, TBCLR should be set after modifying
    // the ID or TBIDEX bits. Doing so resets the clock divider logic, and the new settings are
    // used once the TBCLR bit is cleared. We do not need to manually clear this bit after setting it,
    // as it is reset automatically. More specifically, the state of the output changes when the
    TB0CTL |= TBCLR;

    // Set the maximum count value of Timer_B (when it is set to "Up" mode) to 10000.
    // This determines the period of the PWM output. With this set to 10000 and the timer clock
    // frequency of 8000000, the period of the PWM is 10000 / 8000000 = 0.00125 seconds, or 1.25 ms.
    // We subtract one from the desired value because the timer counts starting at 0, rather than 1.
    TB0CCR0 = hParams->m_heaterDutyCyclePeriod - 1;

    // Set the output mode of capture/compare block 2 to "Reset/Set". This means that the output is
    // reset (i.e. made low) when Timer_B counts to the value in TB0CCR2, and the output is set (i.e.
    // made high) when Timer_B counts to TB0CL0 (which is set from the value in TB0CCR0).
    TB0CCTL2 = OUTMOD_7;

    // Set the initial duty cycle of the PWM. Specifically, this is the counter value at which the output
    // will reset, or go low. When this has a value of zero, it means that the heater is effectively
    // disabled.
    TB0CCR2 = hParams->m_heaterDutyCycle;

    // Finished setting up Timer_B, so we start the timer in "Up" mode (i.e. timer will count up to the
    // value in TB0CL0 (which is set automatically from the value in TB0CCR0), then overflow to zero).
    TB0CTL |= MC__UP;

    // Set which edge we're looking for as the opposite of the current state (in case we boot and
    // Radio is already connected - i.e. WD_INT is HIGH and we want to look for a falling edge):
    lookingForWdIntFallingEdge = getWdIntState() ? TRUE : FALSE;

    return 0;
}

// Handler for Hercules Monitoring (called as sub-task of watchdog_monitor):
void hercules_monitor(HerculesComms__State *hState,
                      volatile uint32_t *watchdogFlags,
                      uint8_t *watchdogOpts,
                      BOOL *writeIOExpander,
                      WatchdogStateDetails *details,
                      UART__State *uart0State)
{
    // Quick and dirty parameters for tuning Hercules Monitoring:
    // How many consequtive kicks has hercules missed since being reset:
    static uint8_t herc_conseq_missed_kicks_since_reset = 0;
    // How many consequtive missed kicks until a reset (testing has shown it has to be at least 2):
    // -- setting this too low won't give Hercules time to reboot after a crash and reset before
    // being reset again...
    static const uint8_t HERC_CONSEQ_MISSED_KICK_THRESHOLD = 3;

    // How many times have we reset Hercules (overall) since last power cycle:
    static uint8_t herc_monitor_reset_count_since_power_cycle = 0;
    // If we have needed to auto reset hercules too many times, it's possible
    // something else is wrong and we should power cycle Hercules:
    static const uint8_t HERC_MONITOR_RESET_COUNT_POWER_CYCLE_THRESHOLD = 2;

    /* check if hercules has given a valid kick */
    if (*watchdogFlags & WDFLAG_HERCULES_KICK)
    {
        *watchdogFlags ^= WDFLAG_HERCULES_KICK;
        herc_conseq_missed_kicks_since_reset = 0;
        // Let Ground know Hercules is still alive
        // (if Hercules was rebooted while our comms were only over Wifi,
        // we wouldn't know when it would be good to send SWITCH TO WIFI MODE).
        DPRINTF("Hercules Alive.");
    }
    else
    {
        if (!(*watchdogOpts & WDOPT_MONITOR_HERCULES))
        {
            // missed a kick, but don't reset the hercules.
            // If monitoring is off, only notify ground about this event if
            // Hercules is on:
            // Ground doesn't really need to know about this but it could be
            // in some hypothetical edge case.
            if (details->m_outputPinBits & OPSBI_MASK(OPSBI__HERCULES_ON))
            {
                DPRINTF("Hercules Unresponsive. Hercules ON. Hercules Monitoring OFF.");
            }
        }
        else
        {
            // Only inc. counter if Herc is being monitored.
            herc_conseq_missed_kicks_since_reset += 1;
            DPRINTF("Hercules Unresponsive. %d/%d", herc_conseq_missed_kicks_since_reset, HERC_CONSEQ_MISSED_KICK_THRESHOLD);
            // Only reset if counter too big:
            if (herc_conseq_missed_kicks_since_reset >= HERC_CONSEQ_MISSED_KICK_THRESHOLD)
            {
                // Too many missed kicks. Try to recover Hercules.

                if (herc_monitor_reset_count_since_power_cycle >= HERC_MONITOR_RESET_COUNT_POWER_CYCLE_THRESHOLD)
                {
                    DPRINTF("No Hercules Kick. Resets didn't work. Power cycling Hercules . . .");
                    herc_conseq_missed_kicks_since_reset = 0; // count this as a reset so it doesn't immediately re-trigger
                    herc_monitor_reset_count_since_power_cycle = 0;
                    // Kill the power:
                    powerOffHercules();
                    SET_RABI_IN_UINT(details->m_resetActionBits, RABI__HERCULES_POWER_OFF);
                    // Queue up the power coming back on:
                    *watchdogFlags |= WDFLAG_POWER_ON_HERCULES;
                }
                else
                {
                    // reset the hercules
                    herc_monitor_reset_count_since_power_cycle += 1;
                    DPRINTF("No Hercules Kick. Resetting Hercules . . . %d/%d", herc_monitor_reset_count_since_power_cycle, HERC_MONITOR_RESET_COUNT_POWER_CYCLE_THRESHOLD);
                    herc_conseq_missed_kicks_since_reset = 0;
                    setHerculesReset();

                    // queue up hercules unreset
                    *watchdogFlags |= WDFLAG_UNRESET_HERCULES;
                    SET_RABI_IN_UINT(details->m_resetActionBits, RABI__HERCULES_WATCHDOG_RESET);

                    // if the issue was due to a comms MPSM breakdown, reset the comms state
                    if (NULL != hState && UART__isInitialized(uart0State))
                    {
                        DPRINTF("\t Resetting Hercules Comms . . .");
                        HerculesComms__Status hcStatus = HerculesComms__resetState(hState);

                        //!< @todo Replace with returning watchdog error code once that is implemented.
                        // This will reset WD if resetting Herc comms fails. Bold but maybe necessary
                        // since the problem *could* be on our side. If we're in Mission, we'll come
                        // back up in Mission, so this is probably okay.
                        DEBUG_ASSERT_EQUAL(HERCULES_COMMS__STATUS__SUCCESS, hcStatus);
                    }

                    *writeIOExpander = TRUE;
                } // Reset count check
            }     // Missed Kicks check
        }         // Hercules Monitoring check
    }             // Hercules Kick check
}

// Checks for and corrects any data irregularities, possibly due to a bitflip:
void safety_timer__bitflip_check(HerculesComms__State *hState,
                                 volatile uint32_t *watchdogFlags,
                                 uint8_t *watchdogOpts,
                                 BOOL *writeIOExpander,
                                 WatchdogStateDetails *details,
                                 UART__State *uart0State)
{
    //*
    //* Check for bitflips (so we don't do any reboots accidentally or not do one when we should):
    // Make sure Reboot Control is in a safe state:
    if (
        details->m_safetyTimerParams.timerRebootControlOn != SAFETY_TIMER__REBOOT_CONTROL_ON &&
        details->m_safetyTimerParams.timerRebootControlOn != SAFETY_TIMER__REBOOT_CONTROL_OFF)
    {
        DPRINTF("SAFETY TIMER: Bitflip in Reboot Control: 0x%x. Defaulting to ON.", details->m_safetyTimerParams.timerRebootControlOn);
        details->m_safetyTimerParams.timerRebootControlOn = SAFETY_TIMER__REBOOT_CONTROL_ON;
    }
    if ( // These bits should always agree. If XOR, they disagree and there's been a flip (! b/c we have to do a logical xor not bitwise since the bits won't agree)
        !(*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_OFF_1A) ^
        !(*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_OFF_1B))
    {
        // Probable bit flip. More likely to have activated one bit during the
        // very long windows we want this to be off than deactivated a bit
        // during the short window where we have both on, so default to off.
        DPRINTF("SAFETY TIMER: Bitflip in WD Flags PWR_OFF_1X: 0x%x%x:%x%x:%x%x:%x%x. Turning bits off.",
                (uint8_t)((*watchdogFlags >> 28) & 0xF),
                (uint8_t)((*watchdogFlags >> 24) & 0xF),
                (uint8_t)((*watchdogFlags >> 20) & 0xF),
                (uint8_t)((*watchdogFlags >> 16) & 0xF),
                (uint8_t)((*watchdogFlags >> 12) & 0xF),
                (uint8_t)((*watchdogFlags >> 8) & 0xF),
                (uint8_t)((*watchdogFlags >> 4) & 0xF),
                (uint8_t)(*watchdogFlags & 0xF)
                //
        );
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_OFF_1A;
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_OFF_1B;
    }
    if (
        !(*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_OFF_2A) ^
        !(*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_OFF_2B))
    {
        DPRINTF("SAFETY TIMER: Bitflip in WD Flags PWR_OFF_2X: 0x%x%x:%x%x:%x%x:%x%x. Turning bits off.",
                (uint8_t)((*watchdogFlags >> 28) & 0xF),
                (uint8_t)((*watchdogFlags >> 24) & 0xF),
                (uint8_t)((*watchdogFlags >> 20) & 0xF),
                (uint8_t)((*watchdogFlags >> 16) & 0xF),
                (uint8_t)((*watchdogFlags >> 12) & 0xF),
                (uint8_t)((*watchdogFlags >> 8) & 0xF),
                (uint8_t)((*watchdogFlags >> 4) & 0xF),
                (uint8_t)(*watchdogFlags & 0xF)
                //
        );
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_OFF_2A;
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_OFF_2B;
    }
    if (
        !(*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_ON_1A) ^
        !(*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_ON_1B))
    {
        DPRINTF("SAFETY TIMER: Bitflip in WD Flags PWR_ON_1X: 0x%x%x:%x%x:%x%x:%x%x. Turning bits off.",
                (uint8_t)((*watchdogFlags >> 28) & 0xF),
                (uint8_t)((*watchdogFlags >> 24) & 0xF),
                (uint8_t)((*watchdogFlags >> 20) & 0xF),
                (uint8_t)((*watchdogFlags >> 16) & 0xF),
                (uint8_t)((*watchdogFlags >> 12) & 0xF),
                (uint8_t)((*watchdogFlags >> 8) & 0xF),
                (uint8_t)((*watchdogFlags >> 4) & 0xF),
                (uint8_t)(*watchdogFlags & 0xF)
                //
        );
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_ON_1A;
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_ON_1B;
    }
    if (
        !(*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_ON_2A) ^
        !(*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_ON_2B))
    {
        DPRINTF("SAFETY TIMER: Bitflip in WD Flags PWR_ON_2X: 0x%x%x:%x%x:%x%x:%x%x. Turning bits off.",
                (uint8_t)((*watchdogFlags >> 28) & 0xF),
                (uint8_t)((*watchdogFlags >> 24) & 0xF),
                (uint8_t)((*watchdogFlags >> 20) & 0xF),
                (uint8_t)((*watchdogFlags >> 16) & 0xF),
                (uint8_t)((*watchdogFlags >> 12) & 0xF),
                (uint8_t)((*watchdogFlags >> 8) & 0xF),
                (uint8_t)((*watchdogFlags >> 4) & 0xF),
                (uint8_t)(*watchdogFlags & 0xF)
                //
        );
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_ON_2A;
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_ON_2B;
    }
    if (
        !(*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_ON_3A) ^
        !(*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_ON_3B))
    {
        DPRINTF("SAFETY TIMER: Bitflip in WD Flags PWR_ON_3X: 0x%x%x:%x%x:%x%x:%x%x. Turning bits off.",
                (uint8_t)((*watchdogFlags >> 28) & 0xF),
                (uint8_t)((*watchdogFlags >> 24) & 0xF),
                (uint8_t)((*watchdogFlags >> 20) & 0xF),
                (uint8_t)((*watchdogFlags >> 16) & 0xF),
                (uint8_t)((*watchdogFlags >> 12) & 0xF),
                (uint8_t)((*watchdogFlags >> 8) & 0xF),
                (uint8_t)((*watchdogFlags >> 4) & 0xF),
                (uint8_t)(*watchdogFlags & 0xF)
                //
        );
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_ON_3A;
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_ON_3B;
    }
    if (
        !(*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_ON_4A) ^
        !(*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_ON_4B))
    {
        DPRINTF("SAFETY TIMER: Bitflip in WD Flags PWR_ON_4X: 0x%x%x:%x%x:%x%x:%x%x. Turning bits off.",
                (uint8_t)((*watchdogFlags >> 28) & 0xF),
                (uint8_t)((*watchdogFlags >> 24) & 0xF),
                (uint8_t)((*watchdogFlags >> 20) & 0xF),
                (uint8_t)((*watchdogFlags >> 16) & 0xF),
                (uint8_t)((*watchdogFlags >> 12) & 0xF),
                (uint8_t)((*watchdogFlags >> 8) & 0xF),
                (uint8_t)((*watchdogFlags >> 4) & 0xF),
                (uint8_t)(*watchdogFlags & 0xF)
                //
        );
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_ON_4A;
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_ON_4B;
    }

    if (
        details->m_safetyTimerParams.tenthTimerExpirationCount > SAFETY_TIMER__TENTH_TIMER_EXPIRATION_COUNT_TRIGGER)
    {
        // Shouldn't be possible w/out a bitflip.
        // Don't *immediately* reboot but set the timer close to the end (-2) and let ground know:
        DPRINTF(
            "SAFETY TIMER: Bitflip in expiration count: 0x%x. Setting count to %d/%d.",
            details->m_safetyTimerParams.tenthTimerExpirationCount,
            SAFETY_TIMER__TENTH_TIMER_EXPIRATION_COUNT_TRIGGER - 2,
            SAFETY_TIMER__TENTH_TIMER_EXPIRATION_COUNT_TRIGGER //
        );
    }
}

// Check on the timer and update it, emit messages, or trigger the reset SM as needed:
void safety_timer__process_timer(HerculesComms__State *hState,
                                 volatile uint32_t *watchdogFlags,
                                 uint8_t *watchdogOpts,
                                 BOOL *writeIOExpander,
                                 WatchdogStateDetails *details,
                                 UART__State *uart0State)
{
    // Statically allocate so this doesn't go on the stack:
    static uint16_t time_elapsed_cs;

    //*
    //* Check for timer kick:
    if (*watchdogFlags & WDFLAG_SAFETY_TIMER_KICK)
    {
        *watchdogFlags ^= WDFLAG_SAFETY_TIMER_KICK;
        // Reset the timer to now:
        details->m_safetyTimerParams.centisecondsAtLastEvent = Time__getTimeInCentiseconds();
        details->m_safetyTimerParams.tenthTimerExpirationCount = 0;
        DPRINTF("SAFETY TIMER: Kick processed. Timer reset.");
    }
    else
    {
        // Haven't received a kick since the last monitor event.
        // How long has elapsed since the last kick / tenth-timer expiration:
        time_elapsed_cs = (Time__getTimeInCentiseconds() - details->m_safetyTimerParams.centisecondsAtLastEvent);

        // Check if the tenth-timer has expired:
        if (time_elapsed_cs > details->m_safetyTimerParams.timerRebootCutoffCentisecondsTenth)
        {
            // Increment the count of the number of times this has happened:
            details->m_safetyTimerParams.tenthTimerExpirationCount += 1;

            // Check if this was the final expiration:
            if (details->m_safetyTimerParams.tenthTimerExpirationCount == SAFETY_TIMER__TENTH_TIMER_EXPIRATION_COUNT_TRIGGER)
            {
                // REBOOT TIME: GND hasn't checked-in in a while. Need to reboot (if reboot control is on):
                if (!details->m_safetyTimerParams.timerRebootControlOn)
                {
                    // Can't reboot b/c timer is not on.
                    DPRINTF("SAFETY TIMER: Timer expired at 0x%x*%dcs. NO REBOOT b/c control is OFF.", details->m_safetyTimerParams.timerRebootCutoffCentisecondsTenth, details->m_safetyTimerParams.tenthTimerExpirationCount);
                }
                else
                {
                    DPRINTF("SAFETY TIMER: Timer expired at 0x%x*%dcs. Performing Reboot . . .", details->m_safetyTimerParams.timerRebootCutoffCentisecondsTenth, details->m_safetyTimerParams.tenthTimerExpirationCount);
                    // Queue up both bits to trigger the first state of the full power reboot:
                    *watchdogFlags |= WDFLAG_SAFETY_TIMER__PWR_OFF_1A;
                    *watchdogFlags |= WDFLAG_SAFETY_TIMER__PWR_OFF_1B;
                }
                // Only reset the expiration count if this was the final expiration:
                details->m_safetyTimerParams.tenthTimerExpirationCount = 0;
            }
            else
            {
                // Just emit a countdown warning:
                if (details->m_safetyTimerParams.timerRebootControlOn)
                {
                    DPRINTF(
                        "SAFETY TIMER: Tenth-timer expired at 0x%x cs / 0x%x cs for the %d/10 time. Reboot Ctrl: ON. ROVER WILL REBOOT in 0x%x*%dcs. Send ACK to reset timer.",
                        time_elapsed_cs,
                        details->m_safetyTimerParams.timerRebootCutoffCentisecondsTenth,
                        details->m_safetyTimerParams.tenthTimerExpirationCount,
                        details->m_safetyTimerParams.timerRebootCutoffCentisecondsTenth,
                        (SAFETY_TIMER__TENTH_TIMER_EXPIRATION_COUNT_TRIGGER - details->m_safetyTimerParams.tenthTimerExpirationCount)
                        //
                    );
                }
                else
                {
                    // Slightly more low-key since we won't reboot if the timer expires.
                    DPRINTF(
                        "SAFETY TIMER: Tenth-timer expired at 0x%x cs / 0x%x cs for the %d/10 time. Reboot Ctrl: OFF. Expires in 0x%x*%dcs. Send ACK to reset timer.",
                        time_elapsed_cs,
                        details->m_safetyTimerParams.timerRebootCutoffCentisecondsTenth,
                        details->m_safetyTimerParams.tenthTimerExpirationCount,
                        details->m_safetyTimerParams.timerRebootCutoffCentisecondsTenth,
                        (SAFETY_TIMER__TENTH_TIMER_EXPIRATION_COUNT_TRIGGER - details->m_safetyTimerParams.tenthTimerExpirationCount)
                        //
                    );
                }
            }

            // Either way, reset the timer:
            details->m_safetyTimerParams.centisecondsAtLastEvent = Time__getTimeInCentiseconds();
        }
        else
        {
            // Timer hasn't expired but see if we should emit a final warning before reboot:
            if (
                // We will reboot on the final expiration:
                details->m_safetyTimerParams.timerRebootControlOn &&
                // The next expiration is the final expiration:
                details->m_safetyTimerParams.tenthTimerExpirationCount == (SAFETY_TIMER__TENTH_TIMER_EXPIRATION_COUNT_TRIGGER - 1) &&
                // And we're in the final countdown range:
                (details->m_safetyTimerParams.timerRebootCutoffCentisecondsTenth - time_elapsed_cs) < SAFETY_TIMER__FINAL_COUNTDOWN_START_TIME_CS)
            {
                DPRINTF(
                    "SAFETY TIMER: 0x%x/0x%x @ %d. FINAL COUNTDOWN! Reboot Ctrl: ON. ROVER WILL REBOOT in 0x%x cs. Send ACK to reset timer.",
                    time_elapsed_cs,
                    details->m_safetyTimerParams.timerRebootCutoffCentisecondsTenth,
                    details->m_safetyTimerParams.tenthTimerExpirationCount,
                    (details->m_safetyTimerParams.timerRebootCutoffCentisecondsTenth - time_elapsed_cs) //
                );
            } // final countdown check
        }     // timeout check
    }         // kick check
}

// Executor for the Reboot "State Machine":
void safety_timer__update_reboot_state_machine(HerculesComms__State *hState,
                                               volatile uint32_t *watchdogFlags,
                                               uint8_t *watchdogOpts,
                                               BOOL *writeIOExpander,
                                               WatchdogStateDetails *details,
                                               UART__State *uart0State)
{
    // We have to turn off Hercules monitoring during the Reboot, so we store
    // the status at the start here:
    static bool herc_mon_status_at_reboot_start = 0;

    //* Advance Full Power Reboot "State Machine":
    // Check states in order of OFF -> ON so in case a bitflip turns on a stage
    // errantly, the worst that will happen is that we restart the reboot
    // process earlier (as opposed to skipping a power-on step).
    if (
        (*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_OFF_1A) &&
        (*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_OFF_1B) //
    )
    {
        DPRINTF("\t SAFETY TIMER: PWR_OFF_1");
        // Store Hercules monitoring status at start then disable it so it
        // doesn't interfere:
        if (*watchdogOpts & WDOPT_MONITOR_HERCULES)
        {
            herc_mon_status_at_reboot_start = true;
            *watchdogOpts &= ~WDOPT_MONITOR_HERCULES;
            DPRINTF("\t\t SAFETY TIMER: Disabling Hercules Monitor for Reboot.");
        }
        else
        {
            herc_mon_status_at_reboot_start = false;
        }

        // Perform MISSION -> SERVICE power transitions:
        // transition to EnteringService: Nothing.
        // EnteringService::transitionTo: Nothing.
        // EnteringService::spinOnce: Nothing.
        // EnteringService::handleTimerTick: Nothing.

        // transition to Service: Nothing.
        // Service::transitionTo: Nothing.
        // Service::spinOnce: Nothing.
        // Service::handleTimerTick: Nothing.

        // Turns out nothing needs to happen here rn. Keeping this state
        // around just in case that changes (the added 5s delay to reboot start
        // won't really matter).

        // Make sure timer stays off during this state:
        details->m_safetyTimerParams.centisecondsAtLastEvent = Time__getTimeInCentiseconds();
        details->m_safetyTimerParams.tenthTimerExpirationCount = 0;
        // Turn off these bits:
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_OFF_1A;
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_OFF_1B;
        // Move to the next state:
        *watchdogFlags |= WDFLAG_SAFETY_TIMER__PWR_OFF_2A;
        *watchdogFlags |= WDFLAG_SAFETY_TIMER__PWR_OFF_2B;
        return;
    }

    if (
        (*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_OFF_2A) &&
        (*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_OFF_2B) //
    )
    {
        DPRINTF("\t SAFETY TIMER: PWR_OFF_2");
        // Perform SERVICE -> KA power transitions:

        // Having Hercules Monitor on during this can cause a sec violation.
        // So, to be EXTRA sure it's off and stays off (even against bitflips),
        // let's re-disable it at the start of every reboot state:
        *watchdogOpts &= ~WDOPT_MONITOR_HERCULES;

        // transition to EnteringKeepAlive: Nothing.
        // EnteringKeepAlive::transitionTo: Nothing.
        // EnteringKeepAlive::transitionToWaitingForIoExpanderWrite:
        powerOffFpga();
        powerOffMotors();
        powerOffRadio();
        powerOffHercules();
        setRadioReset();
        setFPGAReset();
        setMotorsReset();
        setHerculesReset();
        disable3V3PowerRail();
        disableVSysAllPowerRail();
        // blimp_normalBoot(); *DON'T* call this b/c we don't want to mess with batteries here.
        // Power controls from blimp_normalBoot:
        blimp_vSysAllEnOff(); // Makes sure VSA is off (technically happened above)
        blimp_chargerEnOff(); // Make sure charger is off (in case something got excited here and turned on, causing mayhem)
        blimp_regEnOff();     // REGE for charger off
        // Make sure to disable the Hercules uart so we don't dump current through that tx pin:
        // This is okay to do here, even though we're still in MISSION, b/c
        // MISSION only re-enables comms if Hercules is ON.
        if (HerculesComms__isInitialized(hState))
        {
            DebugComms__registerHerculesComms(NULL);
            HerculesComms__Status hcStatus = HerculesComms__uninitialize(&(hState));
            // DON'T DEBUG-ASSERT here b/c we don't want to reset the WD during
            // a safety timer event if uninitializing fails b/c, say, it was
            // already uninitalized.
            // DEBUG_ASSERT_EQUAL(HERCULES_COMMS__STATUS__SUCCESS, hcStatus);
        }
        UART__uninit0(&(uart0State));
        // EnteringKeepAlive::transitionToFinishUpSetup: Nothing.
        // EnteringKeepAlive::spinOnce: Nothing.
        // EnteringKeepAlive::handleTimerTick: Nothing.

        // transition to KeepAlive: Nothing.
        // KeepAlive::transitionTo: Nothing.
        // KeepAlive::spinOnce: Nothing.
        // KeepAlive::handleTimerTick: Nothing.

        *writeIOExpander = TRUE; // some of the above (mainly resets) require IOEX writes

        // Make sure timer stays off during this state:
        details->m_safetyTimerParams.centisecondsAtLastEvent = Time__getTimeInCentiseconds();
        details->m_safetyTimerParams.tenthTimerExpirationCount = 0;
        // Turn off these bits:
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_OFF_2A;
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_OFF_2B;
        // Move to the next state:
        *watchdogFlags |= WDFLAG_SAFETY_TIMER__PWR_ON_1A;
        *watchdogFlags |= WDFLAG_SAFETY_TIMER__PWR_ON_1B;
        return;
    }

    if (
        (*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_ON_1A) &&
        (*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_ON_1B) //
    )
    {
        DPRINTF("\t SAFETY TIMER: PWR_ON_1");
        // Perform KA -> SERVICE power transitions:

        // Be EXTRA sure Hercules Monitoring is still off:
        *watchdogOpts &= ~WDOPT_MONITOR_HERCULES;

        // transition to EnteringService: Nothing.
        // EnteringService::transitionTo: Nothing.
        // EnteringService::spinOnce: Nothing.
        // EnteringService::handleTimerTick: Nothing.

        // transition to Service: Nothing.
        // Service::transitionTo: Nothing.
        // Service::spinOnce: Nothing.
        // Service::handleTimerTick: Nothing.

        // Turns out nothing needs to happen here rn. Keeping this state
        // around just in case that changes (the added 5s with everything off
        // won't really matter and might even be beneficial).

        // Make sure timer stays off during this state:
        details->m_safetyTimerParams.centisecondsAtLastEvent = Time__getTimeInCentiseconds();
        details->m_safetyTimerParams.tenthTimerExpirationCount = 0;
        // Turn off these bits:
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_ON_1A;
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_ON_1B;
        // Move to the next state:
        *watchdogFlags |= WDFLAG_SAFETY_TIMER__PWR_ON_2A;
        *watchdogFlags |= WDFLAG_SAFETY_TIMER__PWR_ON_2B;
        return;
    }

    if (
        (*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_ON_2A) &&
        (*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_ON_2B) //
    )
    {
        DPRINTF("\t SAFETY TIMER: PWR_ON_2");
        // Perform SERVICE -> MISSION power transitions:

        // Be EXTRA sure Hercules Monitoring is still off:
        *watchdogOpts &= ~WDOPT_MONITOR_HERCULES;

        // transition to EnteringMission: Nothing.
        // EnteringMission::transitionTo: Nothing.
        // EnteringMission::transitionToWaitingForI2cDone: Nothing.
        // EnteringMission::transitionToWaitingForIoExpanderWrite1:
        blimp_vSysAllEnOn();
        enable3V3PowerRail();
        releaseRadioReset();
        releaseFPGAReset();

        *writeIOExpander = TRUE; // some of the above require IOEX writes

        // EnteringMission::spinOnce: Nothing (directly, but dispatched to transition commands below):
        // waits for I2C to complete, so we'll insert a new state here... (after IOEX and ~5s wait)

        // Make sure timer stays off during this state:
        details->m_safetyTimerParams.centisecondsAtLastEvent = Time__getTimeInCentiseconds();
        details->m_safetyTimerParams.tenthTimerExpirationCount = 0;
        // Turn off these bits:
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_ON_2A;
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_ON_2B;
        // Move to the next state:
        *watchdogFlags |= WDFLAG_SAFETY_TIMER__PWR_ON_3A;
        *watchdogFlags |= WDFLAG_SAFETY_TIMER__PWR_ON_3B;
        return;
    }

    if (
        (*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_ON_3A) &&
        (*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_ON_3B) //
    )
    {
        DPRINTF("\t SAFETY TIMER: PWR_ON_3");

        // Be EXTRA sure Hercules Monitoring is still off:
        *watchdogOpts &= ~WDOPT_MONITOR_HERCULES;

        // EnteringMission::transitionToWaitingForIoExpanderWrite2:
        powerOffFpga();   // Keep Peripherals off here
        powerOffMotors(); // Keep Peripherals off here
        powerOnRadio();
        *writeIOExpander = TRUE; // some of the above require IOEX writes

        // EnteringMission::transitionToWaitingForFuelGaugeOrTimeout: Nothing.
        // EnteringMission::transitionToWatitingForWifiReadyOrTimeout: Nothing.
        // waits for Radio connection or 25s... so, we'll insert a new state here (after IOEX and ~5s wait)

        // Make sure timer stays off during this transition:
        details->m_safetyTimerParams.centisecondsAtLastEvent = Time__getTimeInCentiseconds();
        details->m_safetyTimerParams.tenthTimerExpirationCount = 0;
        // Turn off these bits:
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_ON_3A;
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_ON_3B;
        // Move to the next state:
        *watchdogFlags |= WDFLAG_SAFETY_TIMER__PWR_ON_4A;
        *watchdogFlags |= WDFLAG_SAFETY_TIMER__PWR_ON_4B;
        return;
    }

    if (
        (*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_ON_4A) &&
        (*watchdogFlags & WDFLAG_SAFETY_TIMER__PWR_ON_4B) //
    )
    {
        DPRINTF("\t SAFETY TIMER: PWR_ON_4");
        // EnteringMission::transitionToWaitingForIoExpanderWrite3:
        powerOnHercules();
        releaseMotorsReset();
        releaseHerculesReset();
        *writeIOExpander = TRUE; // some of the above require IOEX writes
        // *waits for I2C to complete, then:
        // RoverStateBase::enableHerculesComms: We don't have to this here b/c
        // next timer MISSION tick will notice that herc is on but comms are
        // not and it will activate them by calling enableHerculesComms.
        // then transitions to Mission...
        // EnteringMission::handleTimerTick: Nothing.

        // transition to Mission: Nothing.
        // Mission::transitionTo: Nothing.
        // Mission::spinOnce: Nothing.
        // Mission::handleTimerTick: Nothing.

        // Make sure timer stays off during this state:
        details->m_safetyTimerParams.centisecondsAtLastEvent = Time__getTimeInCentiseconds();
        details->m_safetyTimerParams.tenthTimerExpirationCount = 0;
        // Turn off these bits:
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_ON_4A;
        *watchdogFlags &= ~WDFLAG_SAFETY_TIMER__PWR_ON_4B;
        // All done, so we can just stop at turning off these bits.

        // Restore original Hercules Monitor state:
        if (herc_mon_status_at_reboot_start)
        {
            *watchdogOpts |= WDOPT_MONITOR_HERCULES;
            DPRINTF("\t\t SAFETY TIMER: Restoring Hercules Monitor after Reboot.");
        }
        else
        {
            *watchdogOpts &= ~WDOPT_MONITOR_HERCULES;
        }

        DPRINTF("\t SAFETY TIMER: REBOOT COMPLETE.");
        return;
    }
}

// Handler for Supervisor Safety Timer (called as sub-task of watchdog_monitor):
void safety_timer_handler(HerculesComms__State *hState,
                          volatile uint32_t *watchdogFlags,
                          uint8_t *watchdogOpts,
                          BOOL *writeIOExpander,
                          WatchdogStateDetails *details,
                          UART__State *uart0State)
{
    //*
    //* Check for Preconditions:
    // Hold timer at 0 if not in Mission (i.e. make sure we don't reboot or
    // emit countdown messages):
    if (details->m_stateAsUint != ROVERSTATE_AS_UINT__MISSION)
    {
        details->m_safetyTimerParams.centisecondsAtLastEvent = Time__getTimeInCentiseconds();
        details->m_safetyTimerParams.tenthTimerExpirationCount = 0;
    }

    safety_timer__bitflip_check(hState, watchdogFlags, watchdogOpts, writeIOExpander, details, uart0State);

    safety_timer__process_timer(hState, watchdogFlags, watchdogOpts, writeIOExpander, details, uart0State);

    safety_timer__update_reboot_state_machine(hState, watchdogFlags, watchdogOpts, writeIOExpander, details, uart0State);
}

/**
 * Perform the watchdog monitoring steps here
 *
 * Function called every ~5s
 */
int watchdog_monitor(HerculesComms__State *hState,
                     volatile uint32_t *watchdogFlags,
                     uint8_t *watchdogOpts,
                     BOOL *writeIOExpander,
                     WatchdogStateDetails *details,
                     UART__State *uart0State)
{
    // NOTE: hState can be NULL if the Hercules comm link isn't up
    DEBUG_LOG_NULL_CHECK_RETURN(watchdogFlags, "Parameter is NULL", -1);
    DEBUG_LOG_NULL_CHECK_RETURN(watchdogOpts, "Parameter is NULL", -1);
    DEBUG_LOG_NULL_CHECK_RETURN(writeIOExpander, "Parameter is NULL", -1);
    DEBUG_LOG_NULL_CHECK_RETURN(details, "Parameter is NULL", -1);

    /* temporarily disable interrupts */
    __disable_interrupt();

    /* check that kicks have been received */
    if (*watchdogFlags & WDFLAG_RADIO_KICK)
    {
        /* radio kick received, all ok! */
        *watchdogFlags ^= WDFLAG_RADIO_KICK;
    }
    else
    {
        /* MISSING radio kick! don't bother resetting, though */
    }

    /* unreset wifi chip */
    if (*watchdogFlags & WDFLAG_UNRESET_RADIO2)
    {
        releaseRadioReset();
        *watchdogFlags ^= WDFLAG_UNRESET_RADIO2;
        *writeIOExpander = TRUE;
        SET_RABI_IN_UINT(details->m_resetActionBits, RABI__RADIO_UNRESET);
    }

    /* unreset wifi chip */
    if ((*watchdogFlags & WDFLAG_UNRESET_RADIO1) && !(*watchdogFlags & WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE))
    {
        *watchdogFlags |= WDFLAG_UNRESET_RADIO2;
        *watchdogFlags ^= WDFLAG_UNRESET_RADIO1;
    }

    /* unreset hercules */
    if ((*watchdogFlags & WDFLAG_UNRESET_HERCULES) && !(*watchdogFlags & WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE))
    {
        releaseHerculesReset();
        *watchdogFlags ^= WDFLAG_UNRESET_HERCULES;
        *writeIOExpander = TRUE;
        SET_RABI_IN_UINT(details->m_resetActionBits, RABI__HERCULES_UNRESET);
        DPRINTF("Unreset Hercules.");
    }

    /* Turn Hercules (back) on, e.g. after a power cycle: */
    if (*watchdogFlags & WDFLAG_POWER_ON_HERCULES)
    {
        powerOnHercules();
        SET_RABI_IN_UINT(details->m_resetActionBits, RABI__HERCULES_POWER_ON);
        DPRINTF("(Re)booted Hercules.");
    }

    /* unreset motor 1 */
    if ((*watchdogFlags & WDFLAG_UNRESET_MOTOR1) && !(*watchdogFlags & WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE))
    {
        releaseMotor1Reset();
        *watchdogFlags ^= WDFLAG_UNRESET_MOTOR1;
        *writeIOExpander = TRUE;
    }

    /* unreset motor 2 */
    if ((*watchdogFlags & WDFLAG_UNRESET_MOTOR2) && !(*watchdogFlags & WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE))
    {
        releaseMotor2Reset();
        *watchdogFlags ^= WDFLAG_UNRESET_MOTOR2;
        *writeIOExpander = TRUE;
    }

    /* unreset motor 3 */
    if ((*watchdogFlags & WDFLAG_UNRESET_MOTOR3) && !(*watchdogFlags & WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE))
    {
        releaseMotor3Reset();
        *watchdogFlags ^= WDFLAG_UNRESET_MOTOR3;
        *writeIOExpander = TRUE;
    }

    /* unreset motor 4 */
    if ((*watchdogFlags & WDFLAG_UNRESET_MOTOR4) && !(*watchdogFlags & WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE))
    {
        releaseMotor4Reset();
        *watchdogFlags ^= WDFLAG_UNRESET_MOTOR4;
        *writeIOExpander = TRUE;
    }

    /* unreset FPGA */
    if ((*watchdogFlags & WDFLAG_UNRESET_FPGA) && !(*watchdogFlags & WDFLAG_WAITING_FOR_IO_EXPANDER_WRITE))
    {
        releaseFPGAReset();
        *watchdogFlags ^= WDFLAG_UNRESET_FPGA;
        *writeIOExpander = TRUE;
        SET_RABI_IN_UINT(details->m_resetActionBits, RABI__CAM_FPGA_UNRESET);
    }

    /* bring 3V3 on again */
    if (*watchdogFlags & WDFLAG_UNRESET_3V3)
    {
        enable3V3PowerRail();
        *watchdogFlags ^= WDFLAG_UNRESET_3V3;
        SET_RABI_IN_UINT(details->m_resetActionBits, RABI__3V3_EN_UNRESET);
    }

    /* turn VSA on again */
    if (*watchdogFlags & WDFLAG_POWER_ON_V_SYS_ALL)
    {
        enableVSysAllPowerRail();
        *watchdogFlags ^= WDFLAG_POWER_ON_V_SYS_ALL;
        SET_RABI_IN_UINT(details->m_resetActionBits, RABI__V_SYS_ALL_ON__UNRESET);
        DPRINTF("Powered on VSA.");
    }

    /* check ADC values */
    if (*watchdogFlags & WDFLAG_ADC_READY)
    {
        /* ensure ADC values are in spec */
        *watchdogFlags ^= WDFLAG_ADC_READY;
    }

    /* Check if hercules has given a valid kick: */
    hercules_monitor(hState, watchdogFlags, watchdogOpts, writeIOExpander, details, uart0State);

    /* Handle Updating the Safety Timer: */
    safety_timer_handler(hState, watchdogFlags, watchdogOpts, writeIOExpander, details, uart0State);


    /* re-enable interrupts */
    __enable_interrupt();
    return 0;
}

void watchdog_build_hercules_telem(const I2C_Sensors__Readings *i2cReadings,
                                   const AdcValues *adcValues,
                                   BOOL hasDeployed,
                                   uint8_t *telbuf,
                                   size_t telbufSize)
{
    DEBUG_LOG_NULL_CHECK_RETURN(i2cReadings, "Parameter is NULL", /* nothing, b/c void return */);
    DEBUG_LOG_NULL_CHECK_RETURN(adcValues, "Parameter is NULL", /* nothing, b/c void return */);
    DEBUG_LOG_NULL_CHECK_RETURN(telbuf, "Parameter is NULL", /* nothing, b/c void return */);

    if (telbufSize < 16)
    {
        DPRINTF_ERR("telbufSize is too small: required = %d, actual = %d\n", 16, telbufSize);
        return;
    }

    telbuf[0] = (uint8_t)(adcValues->vcc2Point5);
    telbuf[1] = (uint8_t)(adcValues->vcc2Point5 >> 8);
    telbuf[2] = (uint8_t)(adcValues->vcc2Point8);
    telbuf[3] = (uint8_t)(adcValues->vcc2Point8 >> 8);
    telbuf[4] = (uint8_t)(adcValues->vBattSense);
    telbuf[5] = (uint8_t)(adcValues->vBattSense >> 8);
    // [6,7] won't be able to read lander power in mission mode
    telbuf[6] = (uint8_t)(adcValues->vLanderSense);
    telbuf[7] = (uint8_t)(adcValues->vLanderSense >> 8);
    telbuf[8] = (uint8_t)(adcValues->battRT >> 4);
    telbuf[9] = hasDeployed ? 1 : 0;
    telbuf[10] = (uint8_t)(i2cReadings->raw_battery_charge[0]);
    telbuf[11] = (uint8_t)(i2cReadings->raw_battery_charge[1]);
    telbuf[12] = (uint8_t)(i2cReadings->raw_battery_current[0]);
    telbuf[13] = (uint8_t)(i2cReadings->raw_battery_current[1]);
    telbuf[14] = (uint8_t)(i2cReadings->raw_battery_voltage[0]);
    telbuf[15] = (uint8_t)(i2cReadings->raw_battery_voltage[1]);
}

uint16_t watchdog_get_wd_int_flat_duration(void)
{
    return wdIntTimeBetweenLastEdges;
}

/*
 * Pins that need an ISR:
 *
 * Power:
 * - P2.7 is 1V2 PowerGood
 * - P4.4 is 1V8 PowerGood
 * - P4.5 is 3V3 PowerGood (which PowerGood? Watchdog's or main systems'?)
 * - P4.7 is 5V0 PowerGood
 *
 * Kicks:
 * - P1.0 is Radio Kick
 * - P1.3 is Watchdog Int (Kick?)
 *
 * Things that might want an ISR:
 * - Watchdog timer itself
 *      it is currently disabled. Do we want to use it?
 * - Misc. timer
 *      in LPM, we want to wake up periodically to check power systems, etc.
 *      only; this can be done with a timer to wake every X seconds
 */

/* Port 1 handler */
#if 1
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = PORT1_VECTOR
__interrupt void port1_isr_handler(void)
{
#elif defined(__GNUC__)
void __attribute__((interrupt(PORT1_VECTOR))) port1_isr_handler(void)
{
#else
#error Compiler not supported!
#endif
    uint16_t now = *timeCountCentisecondsPtr;

    switch (__even_in_range(P1IV, P1IV__P1IFG7))
    {
    case P1IV__P1IFG3: // P1.3: WD_INT
        if (lookingForWdIntFallingEdge)
        {
            EventQueue__put(EVENT__TYPE__WD_INT_FALLING_EDGE);
            lookingForWdIntFallingEdge = FALSE;
            enableWdIntRisingEdgeInterrupt();
        }
        else
        {
            EventQueue__put(EVENT__TYPE__WD_INT_RISING_EDGE);
            lookingForWdIntFallingEdge = TRUE;
            enableWdIntFallingEdgeInterrupt();
        }

        wdIntTimeBetweenLastEdges = now - wdIntLastEdgeTime;
        wdIntLastEdgeTime = now;

        // exit LPM
        __low_power_mode_off_on_exit();
        break;
    default: // default: ignore
        break;
    }
}
#endif
#if 0
/* Port 3 handler */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = PORT3_VECTOR
__interrupt void port3_isr_handler(void) {
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT3_VECTOR))) port3_isr_handler (void) {
#else
#error Compiler not supported!
#endif
    switch(__even_in_range(P3IV, P3IV__P3IFG7)) {
        default: // default: ignore
            break;
    }
}
#endif

/* Timer 1 handler */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR(void)
{
#elif defined(__GNUC__)
void __attribute__((interrupt(TIMER0_A1_VECTOR))) Timer0_A1_ISR(void)
{
#else
#error Compiler not supported!
#endif
    switch (__even_in_range(TA0IV, TAIV__TAIFG))
    {
    case TAIV__TACCR1:
        *timeCountCentisecondsPtr = *timeCountCentisecondsPtr + 1;

        // Offset the count value we're looking for by by the number of timer ticks in one centisecond
        TA0CCR1 += 94;
        break;

    case TAIV__TAIFG: /* timer tick overflowed */
        EventQueue__put(EVENT__TYPE__TIMER_TICK);
        __low_power_mode_off_on_exit();
        break;

    default:
        break;
    }
}
