/**
 * This file contains the code that does most of the watching of
 * the rest of the subsystems
 */

int watchdog_watch() {
    // TODO: write code here
    return 1;
}

// TODO: put ISR for things here
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
 * - P3.5 is FPGA Kick
 *
 *
 * Things that might want an ISR:
 * - Watchdog timer itself
 *      it is currently disabled. Do we want to use it?
 * - Misc. timer
 *      in LPM, we want to wake up periodically to check power systems, etc.
 *      only; this can be done with a timer to wake every X seconds
 */

