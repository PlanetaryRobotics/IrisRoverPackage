// i2c_sensors.c

#include "include/i2c_sensors.h"
#include "include/i2c.h"

#include <string.h>

//###########################################################
// Private definitions and globals
//###########################################################

// Some definitions to identify magic numbers in the code
#define BOOL short
static const BOOL TRUE = 1;
static const BOOL FALSE = 0;

static I2C_Sensors__InternalState internals = { 0 };

// Start all as LOW unless otherwise indicated (LR and LS need to start HIGH):
uint8_t ioExpanderPort0OutputValues = 0x00u | I2C_SENSORS__IOE_P0_BIT__LATCH_RST;
uint8_t ioExpanderPort1OutputValues = 0x00u | I2C_SENSORS__IOE_P1_BIT__LATCH_SET;

// TODO uncomment to program MC
#define PROGRAM_MOTOR_CONTROLLERS

//###########################################################
// Private function declarations
//###########################################################

BOOL I2C_Sensors__readRegNonBlocking(uint8_t devAddr,
                                     uint8_t regAddr,
                                     uint8_t nackMaskBit,
                                     GaugeReadingState nextState,
                                     void* output,
                                     BOOL* done,
                                     BOOL* gotOutput);


void I2C_Sensors__writeRegNonBlocking(uint8_t devAddr,
                                      uint8_t regAddr,
                                      uint8_t data,
                                      BOOL* done,
                                      BOOL* success);

BOOL I2C_Sensors__chargeLsb();
BOOL I2C_Sensors__chargeMsb();
BOOL I2C_Sensors__voltageLsb();
BOOL I2C_Sensors__voltageMsb();
BOOL I2C_Sensors__currentLsb();
BOOL I2C_Sensors__currentMsb();
BOOL I2C_Sensors__gaugeTempLsb();
BOOL I2C_Sensors__gaugeTempMsb();

//###########################################################
// Public function definitions
//###########################################################
void I2C_Sensors__init()
{
    // Just init the generic I2C module we depend on
    I2C__init();
}

void I2C_Sensors__initiateGaugeReadings()
{
    internals.gState = GRS__CHARGE_LSB;
    internals.readings.nackMask = 0U;
}

I2C_Sensors__Status
I2C_Sensors__getGaugeReadingStatus(I2C_Sensors__Readings* readings)
{
    if (NULL == readings) {
        return I2C_SENSORS__STATUS__ERROR__NULL;
    }

    switch (internals.gState) {
        case GRS__UNKNOWN:
            return I2C_SENSORS__STATUS__ERROR__READINGS_NOT_STARTED;

        case GRS__CHARGE_LSB:
        case GRS__CHARGE_MSB:
        case GRS__VOLTAGE_LSB:
        case GRS__VOLTAGE_MSB:
        case GRS__CURRENT_LSB:
        case GRS__CURRENT_MSB:
        case GRS__GAUGE_TEMP_LSB:
        case GRS__GAUGE_TEMP_MSB:
            return I2C_SENSORS__STATUS__INCOMPLETE;

        case GRS__DONE:
            memcpy(readings, &internals.readings, sizeof(*readings));

            if (internals.readings.nackMask == 0U) {
                return I2C_SENSORS__STATUS__SUCCESS_DONE;
            } else {
                return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;
            }

        default:
            return I2C_SENSORS__STATUS__ERROR__INTERNAL;
    }
}

I2C_Sensors__Status I2C_Sensors__fuelGaugeLowPowerBlocking()
{
    static const uint8_t FUEL_GAUGE_LOW_POWER = 0b00101001;

    if (!((GRS__DONE == internals.gState)
          || (GRS__UNKNOWN == internals. gState))) {
        return I2C_SENSORS__STATUS__ERROR__READINGS_IN_PROGRESS;
    }

    BOOL inProgress = TRUE;
    BOOL success = FALSE;

    while (inProgress) {
        BOOL done = FALSE;

        I2C__spinOnce();

        I2C_Sensors__writeRegNonBlocking(I2C_FUEL_GAUGE_SLAVE_ADDR,
                                         REG_ADDR__CONTROL,
                                         FUEL_GAUGE_LOW_POWER,
                                         &done,
                                         &success);

        inProgress = !done;

        if (inProgress) {
            __delay_cycles(100);
        }
    }

    if (success) {
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;
    }
}

I2C_Sensors__Status I2C_Sensors__initializeFuelGaugeBlocking()
{
    // set control_reg[7:6] to 01 do one conversion, 10 to convert every 10s,
    //      set to 00 to sleep, set to 11 to continuously convert
    // set control_reg[5:3] to 101 for M of 1024 for coulomb counter (see datasheet)
    // control_ref[2:1] not used on SBC (pin its related to is floating)
    // must leave control_reg[0] to 0
    static const uint8_t FUEL_GAUGE_INIT = 0b10101000;

    if (!((GRS__DONE == internals.gState)
          || (GRS__UNKNOWN == internals. gState))) {
        return I2C_SENSORS__STATUS__ERROR__READINGS_IN_PROGRESS;
    }


    BOOL inProgress = TRUE;
    BOOL success = TRUE;
    uint8_t stage = 1;

    while (inProgress) {
        BOOL done = FALSE;
        BOOL stageSuccess = FALSE;

        I2C__spinOnce();

        switch (stage) {
            // initialize charge register with maximum battery capacity (see data sheet
            // for conversion from 3500 mAh, M is 1048)
            case 1:
                I2C_Sensors__writeRegNonBlocking(I2C_FUEL_GAUGE_SLAVE_ADDR,
                                                 REG_ADDR__ACCUMULATED_CHARGE_MSB,
                                                 0xA0,
                                                 &done,
                                                 &stageSuccess);
                break;

            case 2:
                I2C_Sensors__writeRegNonBlocking(I2C_FUEL_GAUGE_SLAVE_ADDR,
                                                 REG_ADDR__ACCUMULATED_CHARGE_LSB,
                                                 0xD8,
                                                 &done,
                                                 &stageSuccess);
                break;

            case 3:
                I2C_Sensors__writeRegNonBlocking(I2C_FUEL_GAUGE_SLAVE_ADDR,
                                                 REG_ADDR__CONTROL,
                                                 FUEL_GAUGE_INIT,
                                                 &done,
                                                 &stageSuccess);
                break;
        }

        if (done) {
            // Only consider successful if all are successful
            success = success && stageSuccess;

            if (stage >= 3) {
                inProgress = FALSE;
            } else {
                stage++;
            }
        }

        if (inProgress) {
            __delay_cycles(100);
        }
    }

    if (success) {
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;
    }
}

I2C_Sensors__Status I2C_Sensors__readFuelGaugeControlRegisterBlocking(uint8_t* data)
{
    if (NULL == data) {
        return I2C_SENSORS__STATUS__ERROR__NULL;
    }

    if (!((GRS__DONE == internals.gState)
          || (GRS__UNKNOWN == internals. gState))) {
        return I2C_SENSORS__STATUS__ERROR__READINGS_IN_PROGRESS;
    }

    BOOL inProgress = TRUE;
    BOOL success = FALSE;
    GaugeReadingState initialState = internals.gState;
    uint8_t initialNackMask = internals.readings.nackMask;

    while (inProgress) {
        BOOL done = FALSE;

        I2C__spinOnce();

        I2C_Sensors__readRegNonBlocking(I2C_FUEL_GAUGE_SLAVE_ADDR,
                                        REG_ADDR__CONTROL,
                                        0,
                                        initialState,
                                        data,
                                        &done,
                                        &success);

        inProgress = !done;

        if (inProgress) {
            __delay_cycles(100);
        }
    }

    internals.gState = initialState;
    internals.readings.nackMask = initialNackMask;

    if (success) {
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;
    }
}

I2C_Sensors__Status I2C_Sensors__initializeIOExpanderBlocking()
{
    // On rev I, the I/O expander contains the following pins:
    //     0.0: MC_RST_A, output to reset motor A controller (reset is active when low)
    //     0.1: MC_RST_B, output to reset motor B controller (reset is active when low)
    //     0.2: MC_RST_C, output to reset motor C controller (reset is active when low)
    //     0.3: MC_RST_D, output to reset motor D controller (reset is active when low)
    //     0.4: Hercules_nRST, output to reset Hercules (reset is active when low)
    //     0.5: Hercules_nPORRST, output to power-on reset Hercules (reset is active when low)
    //     0.6: FPGA_nRST, output to reset FPGA (reset is active when low)
    //     0.7: LATCH_RST output to force LATCH_STAT LOW, only used as a manual override in case there is an issue with
    //            the normal data/clock latching. In nominal operation this should remain unused (and pulled HIGH)
    //
    //     1.0: Radio_nRST, output to reset wifi chip (reset is active when low)
    //     1.1: CHARGE_STAT2, input connected to STAT2 pin of BQ24650RVAR charge controller
    //     1.2: LATCH_STAT, input connected to output of battery enable latch on BLiMP
    //     1.3: LATCH_SET, output to force LATCH_STAT HIGH, only used as a manual override in case there is an issue
    //            with the normal data/clock latching. In nominal operation this should remain unused (and pulled HIGH)
    //     1.4: Not connected
    //     1.5: Radio_ON, output that controls supply of power to wifi chip (power supplied when high)
    //     1.6: BMS_BOOT, output currently unused as BMS circuit wasn't completed.
    //     1.7: Not connected


    // Per the datasheet (https://www.nxp.com/docs/en/data-sheet/PCA9575.pdf):
    // Register 8 is the register to configure the directions of the port 0 pins, where "0" is output. As described
    //   above, all port 0 pins are outputs.
    // Register 9 is the register to configure the directions of the port 1 pins, where "0" is output. As described
    //   above, pins 0, 3, 5, and 6 are outputs, and the rest are either inputs or not connected.
    static const uint8_t CONFIG_PORT_0_REG_ADDR = 8;
    // ! TODO: Look into pins 4 and 5 hercules resets; Set as inputs to not drive during programming; Normally outputs
    //static const uint8_t CONFIG_PORT_0_VALUE = 0b00001111; // resets as inputs - worked for initial programming
    static const uint8_t CONFIG_PORT_0_VALUE = 0b10001111; // 0.0 - 0.3 as inputs + LR is input, others as outputs (for programming motors)
//    static const uint8_t CONFIG_PORT_0_VALUE = 0b11111111; // all as inputs?
    static const uint8_t CONFIG_PORT_1_REG_ADDR = 9;
    // ! TODO: Config port 1 value was 9 bits (0b011010011) but looking at the above comments and port 0 value we
    //   removed the last bit to get (0b01101001)
    // ! TODO: Is this backwards?
//    static const uint8_t CONFIG_PORT_1_VALUE = 0b01101001; // orig
    static const uint8_t CONFIG_PORT_1_VALUE = 0b1001110; // testing (w/ LS as input now too)
//    static const uint8_t CONFIG_PORT_1_VALUE = 0b00000000; // all output?



    if (!((GRS__DONE == internals.gState)
          || (GRS__UNKNOWN == internals. gState))) {
        return I2C_SENSORS__STATUS__ERROR__READINGS_IN_PROGRESS;
    }


    BOOL inProgress = TRUE;
    BOOL success = TRUE;
    uint8_t stage = 1;

    while (inProgress) {
        BOOL done = FALSE;
        BOOL stageSuccess = FALSE;

        I2C__spinOnce();

        switch (stage) {
            case 1:
                I2C_Sensors__writeRegNonBlocking(I2C_IO_EXPANDER_SLAVE_ADDR,
                                                 CONFIG_PORT_0_REG_ADDR,
                                                 CONFIG_PORT_0_VALUE,
                                                 &done,
                                                 &stageSuccess);
                break;

            case 2:
                I2C_Sensors__writeRegNonBlocking(I2C_IO_EXPANDER_SLAVE_ADDR,
                                                 CONFIG_PORT_1_REG_ADDR,
                                                 CONFIG_PORT_1_VALUE,
                                                 &done,
                                                 &stageSuccess);
                break;
        }

        if (done) {
            // Only consider successful if all are successful
            success = success && stageSuccess;

            if (stage >= 2) {
                inProgress = FALSE;
            } else {
                stage++;
            }
        }

        if (inProgress) {
            __delay_cycles(10);
        }
    }

    if (success) {
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;
    }
}

I2C_Sensors__Status I2C_Sensors__readIOExpanderBlocking(uint8_t* chargeStat2, uint8_t* latchStat)
{
    // Per the datasheet (https://www.nxp.com/docs/en/data-sheet/PCA9575.pdf):
    // Register 1 is the register to read the incoming logic levels of the pins in port 1.
    static const uint8_t INPUT_PORT_1_REG_ADDR = 1;

    if (NULL == chargeStat2 || NULL == latchStat) {
        return I2C_SENSORS__STATUS__ERROR__NULL;
    }

    if (!((GRS__DONE == internals.gState)
          || (GRS__UNKNOWN == internals. gState))) {
        return I2C_SENSORS__STATUS__ERROR__READINGS_IN_PROGRESS;
    }

    BOOL inProgress = TRUE;
    BOOL success = FALSE;
    GaugeReadingState initialState = internals.gState;
    uint8_t initialNackMask = internals.readings.nackMask;
    uint8_t regValue = 0;

    while (inProgress) {
        BOOL done = FALSE;

        I2C__spinOnce();

        I2C_Sensors__readRegNonBlocking(I2C_IO_EXPANDER_SLAVE_ADDR,
                                        INPUT_PORT_1_REG_ADDR,
                                        0,
                                        initialState,
                                        &regValue,
                                        &done,
                                        &success);

        inProgress = !done;

        if (inProgress) {
            __delay_cycles(100);
        }
    }

    internals.gState = initialState;
    internals.readings.nackMask = initialNackMask;

    if (success) {
        *chargeStat2 = (regValue & I2C_SENSORS__IOE_P1_BIT__CHARGE_STAT2 != 0) ? 1 : 0;
        *latchStat = (regValue & I2C_SENSORS__IOE_P1_BIT__LATCH_STAT != 0) ? 1 : 0;

        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;
    }
}

I2C_Sensors__Status I2C_Sensors__writeIOExpanderOutputsBlocking(uint8_t port0Value, uint8_t port1Value)
{
    // Per the datasheet (https://www.nxp.com/docs/en/data-sheet/PCA9575.pdf):
    // Register 10 is the port 0 output value register.
    // Register 11 is the port 0 output value register.
    static const uint8_t OUTPUT_PORT_0_REG_ADDR = 10;
    static const uint8_t OUTPUT_PORT_1_REG_ADDR = 11;

    if (!((GRS__DONE == internals.gState)
          || (GRS__UNKNOWN == internals. gState))) {
        return I2C_SENSORS__STATUS__ERROR__READINGS_IN_PROGRESS;
    }

    BOOL inProgress = TRUE;
    BOOL success = TRUE;
    uint8_t stage = 1;

    while (inProgress) {
        BOOL done = FALSE;
        BOOL stageSuccess = FALSE;

        I2C__spinOnce();

        switch (stage) {
            case 1:
                I2C_Sensors__writeRegNonBlocking(I2C_IO_EXPANDER_SLAVE_ADDR,
                                                 OUTPUT_PORT_0_REG_ADDR,
                                                 port0Value,
                                                 &done,
                                                 &stageSuccess);
                break;

            case 2:
                I2C_Sensors__writeRegNonBlocking(I2C_IO_EXPANDER_SLAVE_ADDR,
                                                 OUTPUT_PORT_1_REG_ADDR,
                                                 port1Value,
                                                 &done,
                                                 &stageSuccess);
                break;
        }

        if (done) {
            // Only consider successful if all are successful
            success = success && stageSuccess;

            if (stage >= 2) {
                inProgress = FALSE;
            } else {
                stage++;
            }
        }

        if (inProgress) {
            __delay_cycles(10);
        }
    }

    if (success) {
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;
    }
}


void I2C_Sensors__spinOnce()
{
    BOOL keepSpinning = TRUE;

    while (keepSpinning) {
        I2C__spinOnce();

        switch (internals.gState) {
            case GRS__UNKNOWN:
                keepSpinning = FALSE;
                break;

            case GRS__CHARGE_LSB:
                keepSpinning = I2C_Sensors__chargeLsb();
                break;

            case GRS__CHARGE_MSB:
                keepSpinning = I2C_Sensors__chargeMsb();
                break;

            case GRS__VOLTAGE_LSB:
                keepSpinning = I2C_Sensors__voltageLsb();
                break;

            case GRS__VOLTAGE_MSB:
                keepSpinning = I2C_Sensors__voltageMsb();
                break;

            case GRS__CURRENT_LSB:
                keepSpinning = I2C_Sensors__currentLsb();
                break;

            case GRS__CURRENT_MSB:
                keepSpinning = I2C_Sensors__currentMsb();
                break;

            case GRS__GAUGE_TEMP_LSB:
                keepSpinning = I2C_Sensors__gaugeTempLsb();
                break;

            case GRS__GAUGE_TEMP_MSB:
                keepSpinning = I2C_Sensors__gaugeTempMsb();
                break;

            case GRS__DONE:
                keepSpinning = FALSE;
                break;

            default:
                break;
        }
    }
}

//###########################################################
// Private function definitions
//###########################################################

BOOL I2C_Sensors__readRegNonBlocking(uint8_t devAddr,
                                     uint8_t regAddr,
                                     uint8_t nackMaskBit,
                                     GaugeReadingState nextState,
                                     void* output,
                                     BOOL* done,
                                     BOOL* gotOutput)
{
    static I2C__TransactionStatus tStatus = { 0 };

    I2C__Status stat = I2C__getTransactionStatus(&tStatus);

    BOOL startNewRead = FALSE;
    BOOL continueSpinning = FALSE;

    if (I2C__STATUS__ERROR__NO_TRANSACTION == stat) {
        startNewRead = TRUE;
    } else if (I2C__STATUS__SUCCESS == stat) {
        BOOL statusForRequestedTransaction = FALSE;
        BOOL prevReadDone = FALSE;

        if ((tStatus.devAddr == devAddr)
                && (tStatus.regAddr == regAddr)
                && (tStatus.type == I2C__TYPE__READ)) {
            // This transaction status is for the currently requested read
            statusForRequestedTransaction = TRUE;
        }

        if (I2C__TRANSACTION__DONE_SUCCESS == tStatus.state) {
            // We're now done with this read (and it was successful)
            if (statusForRequestedTransaction) {
                internals.gState = nextState;
                *((uint8_t*) output) = tStatus.data;
                *done = TRUE;
                *gotOutput = TRUE;
                continueSpinning = TRUE;
            }

            prevReadDone = TRUE;
        } else if (I2C__TRANSACTION__DONE_ERROR_NACK == tStatus.state) {
            // We're now done with this read (and it failed)
            if (statusForRequestedTransaction) {
                internals.readings.nackMask |= nackMaskBit;
                internals.gState = nextState;
                *done = TRUE;
                *gotOutput = FALSE;
                continueSpinning = TRUE;
            }

            prevReadDone = TRUE;
        }

        startNewRead = (!statusForRequestedTransaction && prevReadDone);
    }

    if (startNewRead) {
        // If the NACK mask bit for this register is already set, just skip
        // this register
        if (internals.readings.nackMask & nackMaskBit != 0) {
            internals.gState = nextState;
            *done = TRUE;
            *gotOutput = FALSE;
            return TRUE;
        }

        stat = I2C__read(devAddr, regAddr);

        *done = FALSE;
        *gotOutput = FALSE;
    }


    return continueSpinning;
}

void I2C_Sensors__writeRegNonBlocking(uint8_t devAddr,
                                      uint8_t regAddr,
                                      uint8_t data,
                                      BOOL* done,
                                      BOOL* success)
{
    static I2C__TransactionStatus tStatus = { 0 };

    I2C__Status stat = I2C__getTransactionStatus(&tStatus);

    BOOL startNewWrite = FALSE;

    if (I2C__STATUS__ERROR__NO_TRANSACTION == stat) {
        startNewWrite = TRUE;
    } else if (I2C__STATUS__SUCCESS == stat) {
        BOOL statusForRequestedTransaction = FALSE;
        BOOL prevWriteDone = FALSE;

        if ((tStatus.devAddr == devAddr)
                        && (tStatus.regAddr == regAddr)
                        && (tStatus.type == I2C__TYPE__WRITE)) {
            // This transaction status is for the currently requested write
            statusForRequestedTransaction = TRUE;
        }

        if (I2C__TRANSACTION__DONE_SUCCESS == tStatus.state) {
            // We're now done with this write (and it was successful)
            if (statusForRequestedTransaction) {
                *done = TRUE;
                *success = TRUE;
            }

            prevWriteDone = TRUE;
        } else if (I2C__TRANSACTION__DONE_ERROR_NACK == tStatus.state) {
            // We're now done with this write (and it failed)
            if (statusForRequestedTransaction) {
                *done = TRUE;
                *success = FALSE;
            }

            prevWriteDone = TRUE;
        }

        startNewWrite = (!statusForRequestedTransaction && prevWriteDone);
    }

    if (startNewWrite) {
        stat = I2C__write(devAddr, regAddr, data);

        *done = FALSE;
        *success = TRUE;
    }
}

BOOL I2C_Sensors__chargeLsb()
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(I2C_FUEL_GAUGE_SLAVE_ADDR,
                                           REG_ADDR__ACCUMULATED_CHARGE_LSB,
                                           I2C_SENSORS__NACK__BATT_CHARGE,
                                           GRS__CHARGE_MSB,
                                           internals.readings.raw_battery_charge + 1,
                                           &done,
                                           &gotOutput);
}

BOOL I2C_Sensors__chargeMsb()
{
    BOOL done = FALSE, gotOutput = FALSE;
    BOOL result = I2C_Sensors__readRegNonBlocking(I2C_FUEL_GAUGE_SLAVE_ADDR,
                                                  REG_ADDR__ACCUMULATED_CHARGE_MSB,
                                                  I2C_SENSORS__NACK__BATT_CHARGE,
                                                  GRS__VOLTAGE_LSB,
                                                  internals.readings.raw_battery_charge,
                                                  &done,
                                                  &gotOutput);

    if (done && gotOutput) {
        uint8_t *raw_battery_charge = internals.readings.raw_battery_charge;
        uint16_t chargeUShort = (uint16_t)(raw_battery_charge[1] + (raw_battery_charge[0] << 8));
        internals.readings.batt_charge_telem = (uint8_t)(chargeUShort >> 10) * 3;
    }

    return result;
}

BOOL I2C_Sensors__voltageLsb()
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(I2C_FUEL_GAUGE_SLAVE_ADDR,
                                           REG_ADDR__VOLTAGE_LSB,
                                           I2C_SENSORS__NACK__BATT_VOLTAGE,
                                           GRS__VOLTAGE_MSB,
                                           internals.readings.raw_battery_voltage + 1,
                                           &done,
                                           &gotOutput);
}

BOOL I2C_Sensors__voltageMsb()
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(I2C_FUEL_GAUGE_SLAVE_ADDR,
                                           REG_ADDR__VOLTAGE_MSB,
                                           I2C_SENSORS__NACK__BATT_VOLTAGE,
                                           GRS__CURRENT_LSB,
                                           internals.readings.raw_battery_voltage,
                                           &done,
                                           &gotOutput);
}

BOOL I2C_Sensors__currentLsb()
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(I2C_FUEL_GAUGE_SLAVE_ADDR,
                                           REG_ADDR__CURRENT_LSB,
                                           I2C_SENSORS__NACK__BATT_CURRENT,
                                           GRS__CURRENT_MSB,
                                           internals.readings.raw_battery_current + 1,
                                           &done,
                                           &gotOutput);
}

BOOL I2C_Sensors__currentMsb()
{
    BOOL done = FALSE, gotOutput = FALSE;
    BOOL result = I2C_Sensors__readRegNonBlocking(I2C_FUEL_GAUGE_SLAVE_ADDR,
                                                  REG_ADDR__CURRENT_MSB,
                                                  I2C_SENSORS__NACK__BATT_CURRENT,
                                                  GRS__GAUGE_TEMP_LSB,
                                                  internals.readings.raw_battery_current,
                                                  &done,
                                                  &gotOutput);

    if (done && gotOutput) {
        uint8_t *raw_battery_current = internals.readings.raw_battery_current;
        uint16_t bCurrUShort = (uint16_t)(32767 - raw_battery_current[1] - (raw_battery_current[0] << 8));

        if (bCurrUShort > 17407) {
            //exceeds maximum value of 0.6 A
            internals.readings.batt_curr_telem = 255;
        } else {
            internals.readings.batt_curr_telem = (uint8_t)(bCurrUShort >> 7 );
        }
    }

    return result;
}

BOOL I2C_Sensors__gaugeTempLsb()
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(I2C_FUEL_GAUGE_SLAVE_ADDR,
                                           REG_ADDR__TEMPERATURE_LSB,
                                           I2C_SENSORS__NACK__FUEL_GAUGE_TEMP,
                                           GRS__GAUGE_TEMP_MSB,
                                           internals.readings.raw_fuel_gauge_temp + 1,
                                           &done,
                                           &gotOutput);
}

BOOL I2C_Sensors__gaugeTempMsb()
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(I2C_FUEL_GAUGE_SLAVE_ADDR,
                                           REG_ADDR__TEMPERATURE_MSB,
                                           I2C_SENSORS__NACK__FUEL_GAUGE_TEMP,
                                           GRS__DONE,
                                           internals.readings.raw_fuel_gauge_temp,
                                           &done,
                                           &gotOutput);
}

inline uint8_t getIOExpanderPort0OutputValue()
{
    return ioExpanderPort0OutputValues;
}

inline uint8_t getIOExpanderPort1OutputValue()
{
    return ioExpanderPort1OutputValues;
}
