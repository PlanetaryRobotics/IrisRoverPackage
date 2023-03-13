
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
#include "event/event.h"
#include "event/event_queue.h"
#include "flags.h"
#include "watchdog.h"

// These two only exist so I can be 100% confident that the Ptr variables
// below them will never be NULL. Since the Ptr values should be set
// to non-NULL values in watchdog_init prior to enabling the interrupts,
// these three shouldn't ever actually be used.
static volatile uint16_t shouldBeUnusedWatchdogFlags = 0;
static volatile uint16_t shouldBeUnusedTimeCount = 0;

// static volatile uint16_t* watchdogFlagsPtr = &shouldBeUnusedWatchdogFlags;
static volatile uint16_t *timeCountCentisecondsPtr = &shouldBeUnusedTimeCount;

static volatile BOOL lookingForWdIntFallingEdge = TRUE;
static volatile uint16_t wdIntLastEdgeTime = 0;
static volatile uint16_t wdIntTimeBetweenLastEdges = 0;

/**
 * Set up the ISRs for the watchdog
 */
int watchdog_init(volatile uint16_t *watchdogFlags,
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

/**
 * Perform the watchdog monitoring steps here
 *
 * Function called every ~5s
 */
int watchdog_monitor(HerculesComms__State *hState,
                     volatile uint16_t *watchdogFlags,
                     uint8_t *watchdogOpts,
                     BOOL *writeIOExpander,
                     WatchdogStateDetails *details)
{
    // NOTE: hState can be NULL if the Hercules comm link isn't up
    DEBUG_LOG_NULL_CHECK_RETURN(watchdogFlags, "Parameter is NULL", -1);
    DEBUG_LOG_NULL_CHECK_RETURN(watchdogOpts, "Parameter is NULL", -1);
    DEBUG_LOG_NULL_CHECK_RETURN(writeIOExpander, "Parameter is NULL", -1);
    DEBUG_LOG_NULL_CHECK_RETURN(details, "Parameter is NULL", -1);

    // Quick and dirty parameters for tuning Hercules Monitoring:
    // How many consequtive kicks has hercules missed since being reset:
    static uint8_t herc_conseq_missed_kicks_since_reset = 0;
    // How many consequtive missed kicks until a reset (testing has shown it has to be at least 2):
    static const uint8_t HERC_CONSEQ_MISSED_KICK_THRESHOLD = 2;

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
    }

    /* check ADC values */
    if (*watchdogFlags & WDFLAG_ADC_READY)
    {
        /* ensure ADC values are in spec */
        *watchdogFlags ^= WDFLAG_ADC_READY;
    }

    /* check if hercules has given a valid kick */
    if (*watchdogFlags & WDFLAG_HERCULES_KICK)
    {
        *watchdogFlags ^= WDFLAG_HERCULES_KICK;
        herc_conseq_missed_kicks_since_reset = 0;
    }
    else
    {
        herc_conseq_missed_kicks_since_reset += 1;
        if (*watchdogOpts & WDOPT_MONITOR_HERCULES && herc_conseq_missed_kicks_since_reset >= HERC_CONSEQ_MISSED_KICK_THRESHOLD)
        {
            // reset the hercules
            DPRINTF("No Hercules Kick. Resetting Hercules . . .");
            herc_conseq_missed_kicks_since_reset = 0;
            setHerculesReset();

            // queue up hercules unreset
            *watchdogFlags |= WDFLAG_UNRESET_HERCULES;
            SET_RABI_IN_UINT(details->m_resetActionBits, RABI__HERCULES_RESET);

            // if the issue was due to a comms breakdown, reset the comms state
            if (NULL != hState)
            {
                DPRINTF("\t Resetting Hercules Comms . . .");
                HerculesComms__Status hcStatus = HerculesComms__resetState(hState);

                //!< @todo Replace with returning watchdog error code once that is implemented.
                DEBUG_ASSERT_EQUAL(HERCULES_COMMS__STATUS__SUCCESS, hcStatus);
            }

            *writeIOExpander = TRUE;
        }
        else
        {
            // missed a kick, but don't reset the hercules.
        }
    }

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
