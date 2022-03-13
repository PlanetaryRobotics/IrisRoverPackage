// i2c_sensors.c

#include "comms/debug_comms.h"
#include "comms/i2c_sensors.h"
#include "drivers/i2c.h"
#include "utils/time.h"
#include "common.h"

#include <string.h>
#include <assert.h>

//###########################################################
// Private definitions and globals
//###########################################################

typedef enum I2C_Sensors__FuelGaugeRegisterAddrs
{
    FG_REG_ADDR__STATUS = 0,                 //read only
    FG_REG_ADDR__CONTROL = 1,
    FG_REG_ADDR__ACCUMULATED_CHARGE_MSB = 2,
    FG_REG_ADDR__ACCUMULATED_CHARGE_LSB = 3,
    FG_REG_ADDR__CHARGE_THRESHOLD_HIGH_MSB = 4,
    FG_REG_ADDR__CHARGE_THRESHOLD_HIGH_LSB = 5,
    FG_REG_ADDR__CHARGE_THRESHOLD_LOW_MSB = 6,
    FG_REG_ADDR__CHARGE_THRESHOLD_LOW_LSB = 7,
    FG_REG_ADDR__VOLTAGE_MSB = 8,                //read only
    FG_REG_ADDR__VOLTAGE_LSB = 9,                //read only
    FG_REG_ADDR__VOLTAGE_THRESHOLD_HIGH_MSB = 10,
    FG_REG_ADDR__VOLTAGE_THRESHOLD_HIGH_LSB = 11,
    FG_REG_ADDR__VOLTAGE_THRESHOLD_LOW_MSB = 12,
    FG_REG_ADDR__VOLTAGE_THRESHOLD_LOW_LSB = 13,
    FG_REG_ADDR__CURRENT_MSB = 14,                //read only
    FG_REG_ADDR__CURRENT_LSB = 15,                //read only
    FG_REG_ADDR__CURRENT_THRESHOLD_HIGH_MSB = 16,
    FG_REG_ADDR__CURRENT_THRESHOLD_HIGH_LSB = 17,
    FG_REG_ADDR__CURRENT_THRESHOLD_LOW_MSB = 18,
    FG_REG_ADDR__CURRENT_THRESHOLD_LOW_LSB = 19,
    FG_REG_ADDR__TEMPERATURE_MSB = 20,           //read only
    FG_REG_ADDR__TEMPERATURE_LSB = 21,           //read only
    FG_REG_ADDR__TEMPERATURE_THRESHOLD_HIGH = 22,
    FG_REG_ADDR__TEMPERATURE_THRESHOLD_LOW = 23,
    FG_REG_ADDR__MAX_NB_CMDS = 24
} I2C_Sensors__FuelGaugeRegisterAddrs;

typedef enum GaugeReadingState
{
    GRS__UNKNOWN = 0,
    GRS__CHARGE_LSB,
    GRS__CHARGE_MSB,
    GRS__VOLTAGE_LSB,
    GRS__VOLTAGE_MSB,
    GRS__CURRENT_LSB,
    GRS__CURRENT_MSB,
    GRS__GAUGE_TEMP_LSB,
    GRS__GAUGE_TEMP_MSB,
    GRS__DONE
} GaugeReadingState;

typedef enum FuelGaugeInitState
{
    FGI__UNKNOWN = 0,
    FGI__ACCUMULATED_CHARGE_MSB,
    FGI__ACCUMULATED_CHARGE_LSB,
    FGI__CONTROL,
    FGI__DONE,
    FGI__FAILED_NACK
} FuelGaugeInitState;

typedef enum ReadControlState
{
    RC__UNKNOWN = 0,
    RC__READING,
    RC__DONE,
    RC__FAILED_NACK
} ReadControlState;

typedef enum WriteLowPowerState
{
    WLP__UNKNOWN = 0,
    WLP__WRITING,
    WLP__DONE,
    WLP__FAILED_NACK
} WriteLowPowerState;

typedef enum InitIOExpanderState
{
    IIE__UNKNOWN = 0,
    IIE__WRITE_PORT_0_CONFIG,
    IIE__WRITE_PORT_1_CONFIG,
    IIE__DONE,
    IIE__FAILED_NACK
} InitIOExpanderState;

typedef enum WriteIOExpanderState
{
    WIE__UNKNOWN = 0,
    WIE__WRITE_PORT_0_VALUE,
    WIE__WRITE_PORT_1_VALUE,
    WIE__DONE,
    WIE__FAILED_NACK
} WriteIOExpanderState;

typedef enum ReadIOExpanderState
{
    RIE__UNKNOWN = 0,
    RIE__READING,
    RIE__DONE,
    RIE__FAILED_NACK
} ReadIOExpanderState;

typedef struct INS_Sensors__InternalState
{
    I2C_Sensors__Action activeAction;

    GaugeReadingState grsState;
    FuelGaugeInitState fgiState;
    ReadControlState rcState;
    WriteLowPowerState wlpState;
    InitIOExpanderState iieState;
    WriteIOExpanderState wieState;
    ReadIOExpanderState rieState;

    I2C_Sensors__Readings readings;
    uint8_t ioExpanderPort0DirectionValue;
    uint8_t ioExpanderPort1DirectionValue;

    uint8_t controlRegisterReadValue;
    uint8_t ioExpanderPort1ReadValue;

    uint8_t ioExpanderPort0WriteValue;
    uint8_t ioExpanderPort1WriteValue;
} INS_Sensors__InternalState;

static const uint8_t FUEL_GAUGE_CONTROL_LOW_POWER = 0b00101001;

// set control_reg[7:6] to 01 do one conversion, 10 to convert every 10s,
//      set to 00 to sleep, set to 11 to continuously convert
// set control_reg[5:3] to 101 for M of 1024 for coulomb counter (see datasheet)
// control_ref[2:1] not used on SBC (pin its related to is floating)
// must leave control_reg[0] to 0
static const uint8_t FUEL_GAUGE_CONTROL_INIT = 0b10101000;
static const uint8_t FUEL_GAUGE_CHARGE_ACCUM_MSB_INIT = 0xA0;
static const uint8_t FUEL_GAUGE_CHARGE_ACCUM_LSB_INIT = 0xD8;

// On rev I, the I/O expander contains the following pins:
//     0.0: MC_RST_A, now INPUT due to needing it to program motors and never using reset functionality
//     0.1: MC_RST_B, now INPUT due to needing it to program motors and never using reset functionality
//     0.2: MC_RST_C, now INPUT due to needing it to program motors and never using reset functionality
//     0.3: MC_RST_D, now INPUT due to needing it to program motors and never using reset functionality
//     0.4: Hercules_nRST, output to reset Hercules (reset is active when low)
//     0.5: Hercules_nPORRST, output to power-on reset Hercules (reset is active when low)
//     0.6: FPGA_nRST, output to reset FPGA (reset is active when low)
//     0.7: LATCH_RST, now INPUT due to power loop issue
//
//     1.0: Radio_nRST, output to reset wifi chip (reset is active when low)
//     1.1: CHARGE_STAT2, input connected to STAT2 pin of BQ24650RVAR charge controller
//     1.2: LATCH_STAT, input connected to output of battery enable latch on BLiMP
//     1.3: LATCH_SET, now INPUT due to power loop issue
//     1.5: Radio_ON, output that controls supply of power to wifi chip (power supplied when high)
//     1.6: BMS_BOOT, output currently unused as BMS circuit wasn't completed.
//     1.7: Not connected
// Per the datasheet (https://www.nxp.com/docs/en/data-sheet/PCA9575.pdf):
// Register 8 is the register to configure the directions of the port 0 pins, where "0" is output. As described
//   above, all port 0 pins are outputs.
// Register 9 is the register to configure the directions of the port 1 pins, where "0" is output. As described
//   above, pins 0, 3, 5, and 6 are outputs, and the rest are either inputs or not connected.
static const uint8_t IO_EXPANDER_CONFIG_PORT_0_REG_ADDR = 8;
static const uint8_t IO_EXPANDER_CONFIG_PORT_0_VALUE = 0b10001111;
static const uint8_t IO_EXPANDER_CONFIG_PORT_1_REG_ADDR = 9;
static const uint8_t IO_EXPANDER_CONFIG_PORT_1_VALUE = 0b00011110;

// Per the datasheet (https://www.nxp.com/docs/en/data-sheet/PCA9575.pdf):
// Register 1 is the register to read the incoming logic levels of the pins in port 1.
static const uint8_t IO_EXPANDER_INPUT_PORT_1_REG_ADDR = 1;

// Per the datasheet (https://www.nxp.com/docs/en/data-sheet/PCA9575.pdf):
// Register 10 is the port 0 output value register.
// Register 11 is the port 0 output value register.
static const uint8_t IO_EXPANDER_OUTPUT_PORT_0_REG_ADDR = 10;
static const uint8_t IO_EXPANDER_OUTPUT_PORT_1_REG_ADDR = 11;

static INS_Sensors__InternalState internals =
        {
          .activeAction = I2C_SENSORS__ACTIONS__INACTIVE,
          .grsState = GRS__UNKNOWN,
          .fgiState = FGI__UNKNOWN,
          .rcState = RC__UNKNOWN,
          .wlpState = WLP__UNKNOWN,
          .iieState = IIE__UNKNOWN,
          .wieState = WIE__UNKNOWN,
          .rieState = RIE__UNKNOWN,
          .readings = { 0 },
          .ioExpanderPort0DirectionValue = 0,
          .ioExpanderPort1DirectionValue = 0,
          .controlRegisterReadValue = 0,
          .ioExpanderPort1ReadValue = 0,
          .ioExpanderPort0WriteValue = 0,
          .ioExpanderPort1WriteValue = 0
        };

static uint8_t ioExpanderPort0OutputValues = 0;
static uint8_t ioExpanderPort1OutputValues = 0;

//###########################################################
// Private function declarations
//###########################################################

/**
 * @private
 *
 * @brief Attempts to progress the process of reading a value from a specified device register. Will not block.
 *
 * This will start a new read if no transaction is active, monitor the status of an ongoing transaction if one is
 * active, and handle returning the read value and transitioning the state machine if a value has been received. This
 * will also handle the case that the transaction completed due to not receiving an acknowledgment from the other
 * device in response to a byte transmitted from this device. If the `nackMaskBit` is already set in the `I2C_Readings`
 * then this read will be skipped.
 *
 * @param devAddr The device address to read from.
 * @param regAddr The register address to read from.
 * @param nackMaskBit The bit flag to set if no response is received to a transmitted byte.
 * @param isGaugeReading Whether or not this call is part of a gauge sensor reading.
 * @param nextState The next state to transition to when this read completes.
 * @param output A return parameter that will be set to the value read from the specified device register if `done` and
 *               `gotOutput` are both set to TRUE.
 * @param done A return parameter that will specify whether or not reading this device register is complete.
 * @param gotOutput A return parameter that, if `done` is TRUE, will specify if whether or not we successfully read an
 *                  output byte from the specified device register.
 *
 * @return Whether or not the state machine should continue spinning.
 */
static BOOL I2C_Sensors__readRegNonBlocking(uint8_t devAddr,
                                            uint8_t regAddr,
                                            uint8_t nackMaskBit,
                                            BOOL isGaugeReading,
                                            GaugeReadingState nextState,
                                            void* output,
                                            BOOL* done,
                                            BOOL* gotOutput);

/**
 * @private
 *
 * @private Attempts to progress the process of writing a value to a specified device register. Will not block.
 *
 * This will start a new write if no transaction is active, monitor the status of an ongoing transaction if one is
 * active, and handle the case of the transaction completed due to not receiving an acknowledgment from the other device
 * in response to a byte transmitted from this device.
 *
 * @param devAddr The device address to write to.
 * @param regAddr The register address to write to.
 * @param data The value to write to the specified device register.
 * @param isGaugeInit Whether or not this write is part of a fuel gauge initialization sequence.
 * @param nextState The next fuel gauge initialization state to transition to after this read is done if `isGaugeInit`
 *                  is TRUE.
 * @param done A return parameter that will specify whether or not the write is complete.
 * @param success A return parameter that, if `done` is TRUE, will specify whether or not the write completed
 *                successfully.
 */
static void I2C_Sensors__writeRegNonBlocking(uint8_t devAddr,
                                             uint8_t regAddr,
                                             uint8_t data,
                                             BOOL isGaugeInit,
                                             FuelGaugeInitState nextState,
                                             BOOL* done,
                                             BOOL* success);

/**
 * @private
 *
 * @brief Spins the `GRS__CHARGE_LSB` state, which involves reading the LSB of the battery charge. Will not block.
 *
 * Populates the `raw_battery_charge[1]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the gauge reading state machine to the `GRS__CHARGE_MSB` state.
 *
 * @return Whether or not to continue spinning the gauge reading state machine.
 */
static BOOL I2C_Sensors__chargeLsb(void);

/**
 * @private
 *
 * @brief Spins the `GRS__CHARGE_MSB` state, which involves reading the MSB of the battery charge. Will not block.
 *
 * Populates the `raw_battery_charge[0]` and `batt_charge_telem` values in the `I2C_Readings` struct if the read
 * completes successfully. Upon completion, will transition the gauge reading state machine to the `GRS__VOLTAGE_LSB`
 * state.
 *
 * @return Whether or not to continue spinning the gauge reading state machine.
 */
static BOOL I2C_Sensors__chargeMsb(void);

/**
 * @private
 *
 * @brief Spins the `GRS__VOLTAGE_LSB` state, which involves reading the LSB of the battery voltage. Will not block.
 *
 * Populates the `raw_battery_voltage[1]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the gauge reading state machine to the `GRS__VOLTAGE_MSB` state.
 *
 * @return Whether or not to continue spinning the gauge reading state machine.
 */
static BOOL I2C_Sensors__voltageLsb(void);

/**
 * @private
 *
 * @brief Spins the `GRS__VOLTAGE_MSB` state, which involves reading the MSB of the battery voltage. Will not block.
 *
 * Populates the `raw_battery_voltage[0]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the gauge reading state machine to the `GRS__CURRENT_LSB` state.
 *
 * @return Whether or not to continue spinning the gauge reading state machine.
 */
static BOOL I2C_Sensors__voltageMsb(void);

/**
 * @private
 *
 * @brief Spins the `GRS__CURRENT_LSB` state, which involves reading the LSB of the battery current. Will not block.
 *
 * Populates the `raw_battery_current[1]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the gauge reading state machine to the `GRS__CURRENT_MSB` state.
 *
 * @return Whether or not to continue spinning the gauge reading state machine.
 */
static BOOL I2C_Sensors__currentLsb(void);

/**
 * @private
 *
 * @brief Spins the `GRS__CURRENT_MSB` state, which involves reading the MSB of the battery current. Will not block.
 *
 * Populates the `raw_battery_current[0]` and `batt_curr_telem` values in the `I2C_Readings` struct when the read
 * completes successfully. Upon completion, will transition the gauge reading state machine to the `GRS__GAUGE_TEMP_LSB`
 * state.
 *
 * @return Whether or not to continue spinning the gauge reading state machine.
 */
static BOOL I2C_Sensors__currentMsb(void);

/**
 * @private
 *
 * @brief Spins the `GRS__GAUGE_TEMP_LSB` state, which involves reading the LSB of the fuel gauge temperature. Will not
 *        block.
 *
 * Populates the `raw_fuel_gauge_temp[1]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the gauge reading state machine to the `GRS__GAUGE_TEMP_MSB` state.
 *
 * @return Whether or not to continue spinning the gauge reading state machine.
 */
static BOOL I2C_Sensors__gaugeTempLsb(void);

/**
 * @private
 *
 * @brief Spins the `GRS__GAUGE_TEMP_MSB` state, which involves reading the MSB of the fuel gauge temperature. Will not
 *        block.
 *
 * Populates the `raw_fuel_gauge_temp[0]` value in the `I2C_Readings` struct when the read completes successfully. Upon
 * completion, will transition the gauge reading state machine to the `GRS__DONE` state.
 *
 * @return Whether or not to continue spinning the gauge reading state machine.
 */
static BOOL I2C_Sensors__gaugeTempMsb(void);

/**
 * @private
 *
 * @brief Spins the `FGI__ACCUMULATED_CHARGE_MSB` state, which involves writing the MSB of the fuel gauge accumulated
 *        charge. Will not block.
 *
 * Upon completion, will transition the fuel gauge initialization state machine to the `FGI__ACCUMULATED_CHARGE_LSB`
 *  or `FGI__FAILED_NACK` state.
 *
 * @return Whether or not to continue spinning the fuel gauge initialization state machine.
 */
static BOOL I2C_Sensors__accumulatedChargeMsb(void);

/**
 * @private
 *
 * @brief Spins the `FGI__ACCUMULATED_CHARGE_LSB` state, which involves writing the LSB of the fuel gauge accumulated
 *        charge. Will not block.
 *
 * Upon completion, will transition the fuel gauge initialization state machine to the `FGI__CONTROL` or
 * `FGI__FAILED_NACK`state.
 *
 * @return Whether or not to continue spinning the fuel gauge initialization state machine.
 */
static BOOL I2C_Sensors__accumulatedChargeLsb(void);

/**
 * @private
 *
 * @brief Spins the `FGI__CONTROL` state, which involves writing the fuel gauge control byte. Will not block.
 *
 * Upon completion, will transition the fuel gauge initialization state machine to the `FGI__DONE` or `FGI__FAILED_NACK`
 * state.
 *
 * @return Whether or not to continue spinning the fuel gauge initialization state machine.
 */
static BOOL I2C_Sensors__writeControl(void);

/**
 * @private
 *
 * @brief Spins the read of the control byte to the control register of the fuel gauge. Will not block.
 *
 * Upon completion, will transition the read control state machine to the `RC__DONE` or `RC__FAILED_NACK` state.
 *
 * @return Whether or not to continue spinning the read control state machine.
 */
static BOOL I2C_Sensors__readControl(void);

/**
 * @private
 *
 * @brief Spins the write to put the fuel gauge in low power mode. Will not block.
 *
 * Upon completion, will transition the write low power state machine to the `WLP__DONE` or `WLP__FAILED_NACK` state.
 *
 * @return Whether or not to continue spinning the write low power state machine.
 */
static BOOL I2C_Sensors__lowPower(void);

/**
 * @private
 *
 * @brief Spins the write to the IO Expander port 0 configuration register (the `IIE__WRITE_PORT_0_CONFIG` state). Will
 *        not block.
 *
 * Upon completion, will transition the initialize IO expander state machine to the `IIE__WRITE_PORT_1_CONFIG` or
 * `IIE__FAILED_NACK` state.
 *
 * @return Whether or not to continue spinning the initialize IO expander state machine.
 */
static BOOL I2C_Sensors__ioExpanderInitPort0(void);

/**
 * @private
 *
 * @brief Spins the write to the IO Expander port 1 configuration register (the `IIE__WRITE_PORT_1_CONFIG` state). Will
 *        not block.
 *
 * Upon completion, will transition the initialize IO expander state machine to the `IIE__DONE` or `IIE__FAILED_NACK`
 * state.
 *
 * @return Whether or not to continue spinning the initialize IO expander state machine.
 */
static BOOL I2C_Sensors__ioExpanderInitPort1(void);

/**
 * @private
 *
 * @brief Spins the write to the IO Expander port 0 value register (the `WIE__WRITE_PORT_0_VALUE` state). Will
 *        not block.
 *
 * Upon completion, will transition the write IO expander state machine to the `WIE__WRITE_PORT_1_VALUE` or
 * `WIE__FAILED_NACK` state.
 *
 * @return Whether or not to continue spinning the write IO expander state machine.
 */
static BOOL I2C_Sensors__ioExpanderWritePort0(void);

/**
 * @private
 *
 * @brief Spins the write to the IO Expander port 1 value register (the `WIE__WRITE_PORT_1_VALUE` state). Will
 *        not block.
 *
 * Upon completion, will transition the write IO expander state machine to the `WIE__DONE` or `WIE__FAILED_NACK` state.
 *
 * @return Whether or not to continue spinning the write IO expander state machine.
 */
static BOOL I2C_Sensors__ioExpanderWritePort1(void);

/**
 * @private
 *
 * @brief Spins the read of the IO Expander port 1 input register. Will not block.
 *
 * Upon completion, will transition the read IO expander machine to the `RIE__DONE` or `RIE__FAILED_NACK` state.
 *
 * @return Whether or not to continue spinning the read IO expander state machine.
 */
static BOOL I2C_Sensors__ioExpanderReadPort1(void);

//###########################################################
// Public function definitions
//###########################################################

void I2C_Sensors__init(void)
{
    // Just init the generic I2C module we depend on
    I2C__init();
}

void I2C_Sensors__stop(void)
{
    I2C__stop();
    I2C_Sensors__clearLastAction();
}

void I2C_Sensors__clearLastAction(void)
{
    internals.activeAction = I2C_SENSORS__ACTIONS__INACTIVE;

    // We could check what the last action was and only reset that one... or we could just reset them all.
    internals.grsState = GRS__UNKNOWN;
    internals.fgiState = FGI__UNKNOWN;
    internals.rcState = RC__UNKNOWN;
    internals.wlpState = WLP__UNKNOWN;
}

I2C_Sensors__Status I2C_Sensors__initiateGaugeReadings(void)
{
    if (I2C_SENSORS__ACTIONS__INACTIVE == internals.activeAction) {
        internals.activeAction = I2C_SENSORS__ACTIONS__GAUGE_READING;
        internals.grsState = GRS__CHARGE_LSB;
        internals.readings.nackMask = 0U;
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__ACTION_ALREADY_IN_PROGRESS;
    }
}

I2C_Sensors__Status I2C_Sensors__initiateFuelGaugeInitialization(void)
{
    if (I2C_SENSORS__ACTIONS__INACTIVE == internals.activeAction) {
        internals.activeAction = I2C_SENSORS__ACTIONS__GAUGE_INIT;
        internals.fgiState = FGI__ACCUMULATED_CHARGE_MSB;
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__ACTION_ALREADY_IN_PROGRESS;
    }
}

I2C_Sensors__Status I2C_Sensors__initiateReadControl(void)
{
    if (I2C_SENSORS__ACTIONS__INACTIVE == internals.activeAction) {
        internals.activeAction = I2C_SENSORS__ACTIONS__READ_GAUGE_CONTROL_REGISTER;
        internals.rcState = RC__READING;
        internals.controlRegisterReadValue = 0;
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__ACTION_ALREADY_IN_PROGRESS;
    }
}

I2C_Sensors__Status I2C_Sensors__initiateWriteLowPower(void)
{
    if (I2C_SENSORS__ACTIONS__INACTIVE == internals.activeAction) {
        internals.activeAction = I2C_SENSORS__ACTIONS__WRITE_GAUGE_LOW_POWER;
        internals.wlpState = WLP__WRITING;
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__ACTION_ALREADY_IN_PROGRESS;
    }
}

I2C_Sensors__Status I2C_Sensors__initiateIoExpanderInitialization(void)
{
    if (I2C_SENSORS__ACTIONS__INACTIVE == internals.activeAction) {
        internals.activeAction = I2C_SENSORS__ACTIONS__INIT_IO_EXPANDER;
        internals.iieState = IIE__WRITE_PORT_0_CONFIG;
        internals.ioExpanderPort0DirectionValue = IO_EXPANDER_CONFIG_PORT_0_VALUE;
        internals.ioExpanderPort1DirectionValue = IO_EXPANDER_CONFIG_PORT_1_VALUE;

        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__ACTION_ALREADY_IN_PROGRESS;
    }
}

inline void I2C_Sensors__setIoExpanderPort0OutputBits(uint8_t bitsToSet)
{
    ioExpanderPort0OutputValues |= bitsToSet;
}

inline void I2C_Sensors__setIoExpanderPort1OutputBits(uint8_t bitsToSet)
{
    ioExpanderPort1OutputValues |= bitsToSet;
}

inline void I2C_Sensors__clearIoExpanderPort0OutputBits(uint8_t bitsToClear)
{
    ioExpanderPort0OutputValues &= ~bitsToClear;
}

inline void I2C_Sensors__clearIoExpanderPort1OutputBits(uint8_t bitsToClear)
{
    ioExpanderPort1OutputValues &= ~bitsToClear;
}

uint8_t I2C_Sensors__getIoExpanderPort0OutputValue(void)
{
    return ioExpanderPort0OutputValues;
}

uint8_t I2C_Sensors__getIoExpanderPort1OutputValue(void)
{
    return ioExpanderPort1OutputValues;
}

I2C_Sensors__Status I2C_Sensors__initiateWriteIoExpanderCurrentValues(void)
{
    return I2C_Sensors__initiateWriteIoExpander(ioExpanderPort0OutputValues, ioExpanderPort1OutputValues);
}

I2C_Sensors__Status I2C_Sensors__initiateWriteIoExpander(uint8_t port0Value, uint8_t port1Value)
{
    if (I2C_SENSORS__ACTIONS__INACTIVE == internals.activeAction) {
        internals.activeAction = I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER;
        internals.wieState = WIE__WRITE_PORT_0_VALUE;
        internals.ioExpanderPort0WriteValue = port0Value;
        internals.ioExpanderPort1WriteValue = port1Value;
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__ACTION_ALREADY_IN_PROGRESS;
    }
}

I2C_Sensors__Status I2C_Sensors__initiateReadIoExpander(void)
{
    if (I2C_SENSORS__ACTIONS__INACTIVE == internals.activeAction) {
        internals.activeAction = I2C_SENSORS__ACTIONS__READ_IO_EXPANDER;
        internals.rieState = RIE__READING;
        return I2C_SENSORS__STATUS__SUCCESS_DONE;
    } else {
        return I2C_SENSORS__STATUS__ERROR__ACTION_ALREADY_IN_PROGRESS;
    }
}

I2C_Sensors__Status I2C_Sensors__writeIoExpanderCurrentValuesBlocking(uint16_t timeoutCentiseconds)
{
    return I2C_Sensors__writeIoExpanderBlocking(ioExpanderPort0OutputValues,
                                                ioExpanderPort1OutputValues,
                                                timeoutCentiseconds);
}

I2C_Sensors__Status I2C_Sensors__writeIoExpanderBlocking(uint8_t port0Value,
                                                         uint8_t port1Value,
                                                         uint16_t timeoutCentiseconds)
{
    uint16_t startTimeCentiseconds = Time__getTimeInCentiseconds();
    uint16_t currentTimeCentiseconds = startTimeCentiseconds;
    uint16_t endTimeCentiseconds = startTimeCentiseconds + timeoutCentiseconds;

    I2C_Sensors__Status i2cStatus = I2C_Sensors__initiateWriteIoExpander(port0Value, port1Value);

    if (I2C_SENSORS__STATUS__SUCCESS_DONE != i2cStatus) {
        return i2cStatus;
    }

    while (currentTimeCentiseconds <= endTimeCentiseconds) {
        I2C_Sensors__spinOnce();

        I2C_Sensors__Action action = I2C_SENSORS__ACTIONS__INACTIVE;
        i2cStatus = I2C_Sensors__getActionStatus(&action, NULL, NULL);

        // Sanity check
        DEBUG_ASSERT_EQUAL(I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER, action);

        if (I2C_SENSORS__STATUS__INCOMPLETE == i2cStatus) {
            // Delay a tiny bit so we're not spinning completely tightly. We're single-threaded so it's not like anything
            // else runs while we delay, but this should hopefully reduce power usage.
            __delay_cycles(10);

            // Update the current time so we can timeout if necessary
            currentTimeCentiseconds = Time__getTimeInCentiseconds();
        } else {
            DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE, i2cStatus, "IO expander write failed");

            I2C_Sensors__clearLastAction();
            return i2cStatus;
        }
    }

    return I2C_SENSORS__STATUS__ERROR__TIMEOUT;
}

I2C_Sensors__Status I2C_Sensors__readIoExpanderBlocking(uint8_t* chargeStat2,
                                                        uint8_t* latchStat,
                                                        uint16_t timeoutCentiseconds)
{
    uint16_t startTimeCentiseconds = Time__getTimeInCentiseconds();
    uint16_t currentTimeCentiseconds = startTimeCentiseconds;
    uint16_t endTimeCentiseconds = startTimeCentiseconds + timeoutCentiseconds;

    I2C_Sensors__Status i2cStatus = I2C_Sensors__initiateReadIoExpander();

    if (I2C_SENSORS__STATUS__SUCCESS_DONE != i2cStatus) {
        return i2cStatus;
    }

    while (currentTimeCentiseconds <= endTimeCentiseconds) {
        I2C_Sensors__spinOnce();

        I2C_Sensors__Action action = I2C_SENSORS__ACTIONS__INACTIVE;
        uint8_t readValue = 0;
        i2cStatus = I2C_Sensors__getActionStatus(&action, NULL, &readValue);

        // Sanity check
        DEBUG_ASSERT_EQUAL(I2C_SENSORS__ACTIONS__READ_IO_EXPANDER, action);

        if (I2C_SENSORS__STATUS__INCOMPLETE == i2cStatus) {
            // Delay a tiny bit so we're not spinning completely tightly. We're single-threaded so it's not like anything
            // else runs while we delay, but this should hopefully reduce power usage.
            __delay_cycles(10);

            // Update the current time so we can timeout if necessary
            currentTimeCentiseconds = Time__getTimeInCentiseconds();
        } else {
            DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE, i2cStatus, "IO expander read failed");

            I2C_Sensors__clearLastAction();

            *chargeStat2 = (readValue & I2C_SENSORS__IOE_P1_BIT__CHARGE_STAT2 != 0) ? 1 : 0;
            *latchStat = (readValue & I2C_SENSORS__IOE_P1_BIT__LATCH_STAT != 0) ? 1 : 0;

            return i2cStatus;
        }
    }

    return I2C_SENSORS__STATUS__ERROR__TIMEOUT;
}

I2C_Sensors__Status I2C_Sensors__getIoExpanderPortDirections(uint8_t* port0Value, uint8_t* port1Value)
{
    if (NULL == port0Value || NULL == port1Value) {
        return I2C_SENSORS__STATUS__ERROR__NULL;
    }

    *port0Value = internals.ioExpanderPort0DirectionValue;
    *port1Value = internals.ioExpanderPort1DirectionValue;

    return I2C_SENSORS__STATUS__SUCCESS_DONE;
}

I2C_Sensors__Status I2C_Sensors__writeIoExpanderPortDirectionsBlocking(uint8_t port0Value,
                                                                       uint8_t port1Value,
                                                                       uint16_t timeoutCentiseconds)
{
    uint16_t startTimeCentiseconds = Time__getTimeInCentiseconds();
    uint16_t currentTimeCentiseconds = startTimeCentiseconds;
    uint16_t endTimeCentiseconds = startTimeCentiseconds + timeoutCentiseconds;

    I2C_Sensors__Status i2cStatus = I2C_Sensors__initiateIoExpanderInitialization();

    if (I2C_SENSORS__STATUS__SUCCESS_DONE != i2cStatus) {
        return i2cStatus;
    }

    // Modify the default direction values set in I2C_Sensors__initiateIoExpanderInitialization to the ones passed to
    // this function
    internals.ioExpanderPort0DirectionValue = port0Value;
    internals.ioExpanderPort1DirectionValue = port1Value;

    while (currentTimeCentiseconds <= endTimeCentiseconds) {
        I2C_Sensors__spinOnce();

        I2C_Sensors__Action action = I2C_SENSORS__ACTIONS__INACTIVE;
        uint8_t readValue = 0;
        i2cStatus = I2C_Sensors__getActionStatus(&action, NULL, &readValue);

        // Sanity check
        DEBUG_ASSERT_EQUAL(I2C_SENSORS__ACTIONS__INIT_IO_EXPANDER, action);

        if (I2C_SENSORS__STATUS__INCOMPLETE == i2cStatus) {
            // Delay a tiny bit so we're not spinning completely tightly. We're single-threaded so it's not like anything
            // else runs while we delay, but this should hopefully reduce power usage.
            __delay_cycles(10);

            // Update the current time so we can timeout if necessary
            currentTimeCentiseconds = Time__getTimeInCentiseconds();
        } else {
            DEBUG_LOG_CHECK_STATUS(I2C_SENSORS__STATUS__SUCCESS_DONE, i2cStatus, "IO expander init failed");

            I2C_Sensors__clearLastAction();
            return i2cStatus;
        }
    }

    return I2C_SENSORS__STATUS__ERROR__TIMEOUT;
}

I2C_Sensors__Status
I2C_Sensors__getActionStatus(I2C_Sensors__Action* action,
                             I2C_Sensors__Readings* readings,
                             uint8_t* readValue)
{
    if (NULL == action) {
        return I2C_SENSORS__STATUS__ERROR__NULL;
    }

    if (I2C_SENSORS__ACTIONS__INACTIVE == internals.activeAction) {
        *action = I2C_SENSORS__ACTIONS__INACTIVE;
        return I2C_SENSORS__STATUS__ERROR__NO_ACTION_IN_PROGRESS;
    }

    *action = internals.activeAction;

    switch (internals.activeAction) {
        case I2C_SENSORS__ACTIONS__GAUGE_READING:
            {
                if (NULL == readings) {
                    return I2C_SENSORS__STATUS__ERROR__NULL;
                }

                switch (internals.grsState) {
                    case GRS__UNKNOWN:
                        *action = I2C_SENSORS__ACTIONS__INACTIVE;
                        return I2C_SENSORS__STATUS__ERROR__READINGS_NOT_STARTED;

                    case GRS__CHARGE_LSB:
                        // Fall through
                    case GRS__CHARGE_MSB:
                        // Fall through
                    case GRS__VOLTAGE_LSB:
                        // Fall through
                    case GRS__VOLTAGE_MSB:
                        // Fall through
                    case GRS__CURRENT_LSB:
                        // Fall through
                    case GRS__CURRENT_MSB:
                        // Fall through
                    case GRS__GAUGE_TEMP_LSB:
                        // Fall through
                    case GRS__GAUGE_TEMP_MSB:
                        return I2C_SENSORS__STATUS__INCOMPLETE;

                    case GRS__DONE:
                        /* temporarily disable interrupts */
                        __disable_interrupt();

                        memcpy(readings, &internals.readings, sizeof(*readings));

                        /* re-enable interrupts */
                        __enable_interrupt();

                        if (internals.readings.nackMask == 0U) {
                            return I2C_SENSORS__STATUS__SUCCESS_DONE;
                        } else {
                            return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;
                        }

                    default:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;
                }
            }

        case I2C_SENSORS__ACTIONS__GAUGE_INIT:
            {
                switch (internals.fgiState) {
                    case FGI__UNKNOWN:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;

                    case FGI__ACCUMULATED_CHARGE_MSB:
                        // Fall through
                    case FGI__ACCUMULATED_CHARGE_LSB:
                        // Fall through
                    case FGI__CONTROL:
                        return I2C_SENSORS__STATUS__INCOMPLETE;

                    case FGI__DONE:
                        return I2C_SENSORS__STATUS__SUCCESS_DONE;

                    case FGI__FAILED_NACK:
                        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;

                    default:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;
                }
            }

        case I2C_SENSORS__ACTIONS__WRITE_GAUGE_LOW_POWER:
            {
                switch (internals.wlpState) {
                    case WLP__UNKNOWN:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;

                    case WLP__WRITING:
                        return I2C_SENSORS__STATUS__INCOMPLETE;

                    case WLP__DONE:
                        return I2C_SENSORS__STATUS__SUCCESS_DONE;

                    case WLP__FAILED_NACK:
                        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;

                    default:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;
                }
            }

        case I2C_SENSORS__ACTIONS__READ_GAUGE_CONTROL_REGISTER:
            {
                if (NULL == readValue) {
                    return I2C_SENSORS__STATUS__ERROR__NULL;
                }

                switch (internals.rcState) {
                    case RC__UNKNOWN:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;

                    case RC__READING:
                        return I2C_SENSORS__STATUS__INCOMPLETE;

                    case RC__DONE:
                        *readValue = internals.controlRegisterReadValue;
                        return I2C_SENSORS__STATUS__SUCCESS_DONE;

                    case RC__FAILED_NACK:
                        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;

                    default:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;
                }
            }

        case I2C_SENSORS__ACTIONS__INIT_IO_EXPANDER:
            {
                switch (internals.iieState) {
                    case IIE__UNKNOWN:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;

                    case IIE__WRITE_PORT_0_CONFIG:
                        // Fall through
                    case IIE__WRITE_PORT_1_CONFIG:
                        return I2C_SENSORS__STATUS__INCOMPLETE;

                    case IIE__DONE:
                        return I2C_SENSORS__STATUS__SUCCESS_DONE;

                    case IIE__FAILED_NACK:
                        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;

                    default:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;
                }
            }

        case I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER:
            {
                switch (internals.wieState) {
                    case WIE__UNKNOWN:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;

                    case WIE__WRITE_PORT_0_VALUE:
                        // Fall through
                    case WIE__WRITE_PORT_1_VALUE:
                        return I2C_SENSORS__STATUS__INCOMPLETE;

                    case WIE__DONE:
                        return I2C_SENSORS__STATUS__SUCCESS_DONE;

                    case WIE__FAILED_NACK:
                        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;

                    default:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;
                }
            }

        case I2C_SENSORS__ACTIONS__READ_IO_EXPANDER:
            {
                if (NULL == readValue) {
                    return I2C_SENSORS__STATUS__ERROR__NULL;
                }

                switch (internals.rieState) {
                    case RIE__UNKNOWN:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;

                    case RIE__READING:
                        return I2C_SENSORS__STATUS__INCOMPLETE;

                    case RIE__DONE:
                        *readValue = internals.ioExpanderPort1ReadValue;
                        return I2C_SENSORS__STATUS__SUCCESS_DONE;

                    case RIE__FAILED_NACK:
                        return I2C_SENSORS__STATUS__ERROR__DONE_WITH_NACKS;

                    default:
                        return I2C_SENSORS__STATUS__ERROR__INTERNAL;
                }
            }

        default:
            return I2C_SENSORS__STATUS__ERROR__INTERNAL;
    }
}

void I2C_Sensors__spinOnce(void)
{
    BOOL keepSpinning = TRUE;

    if (I2C_SENSORS__ACTIONS__INACTIVE == internals.activeAction) {
        return;
    }

    while (keepSpinning) {
        I2C__spinOnce();

        switch (internals.activeAction) {
            case I2C_SENSORS__ACTIONS__GAUGE_READING:
                {
                    switch (internals.grsState) {
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
                            // Fall through
                        default:
                            keepSpinning = FALSE;
                            break;
                    }
                }
                break;

            case I2C_SENSORS__ACTIONS__GAUGE_INIT:
                {
                    switch (internals.fgiState) {
                        case FGI__UNKNOWN:
                            keepSpinning = FALSE;
                            break;

                        case FGI__ACCUMULATED_CHARGE_MSB:
                            keepSpinning = I2C_Sensors__accumulatedChargeMsb();
                            break;

                        case FGI__ACCUMULATED_CHARGE_LSB:
                            keepSpinning = I2C_Sensors__accumulatedChargeLsb();
                            break;

                        case FGI__CONTROL:
                            keepSpinning = I2C_Sensors__writeControl();
                            break;

                        case FGI__DONE:
                            // Fall through
                        case FGI__FAILED_NACK:
                            // Fall through
                        default:
                            keepSpinning = FALSE;
                            break;
                    }
                }
                break;

            case I2C_SENSORS__ACTIONS__WRITE_GAUGE_LOW_POWER:
                {
                    switch (internals.wlpState) {
                        case WLP__UNKNOWN:
                            keepSpinning = FALSE;
                            break;

                        case WLP__WRITING:
                            keepSpinning = I2C_Sensors__lowPower();
                            break;

                        case WLP__DONE:
                            // Fall through
                        case WLP__FAILED_NACK:
                            // Fall through
                        default:
                            keepSpinning = FALSE;
                            break;
                    }
                }
                break;

            case I2C_SENSORS__ACTIONS__READ_GAUGE_CONTROL_REGISTER:
                {
                    switch (internals.rcState) {
                        case RC__UNKNOWN:
                            keepSpinning = FALSE;
                            break;

                        case RC__READING:
                            keepSpinning = I2C_Sensors__readControl();
                            break;

                        case RC__DONE:
                            // Fall through
                        case RC__FAILED_NACK:
                            // Fall through
                        default:
                            keepSpinning = FALSE;
                            break;
                    }
                }
                break;

            case I2C_SENSORS__ACTIONS__INIT_IO_EXPANDER:
                {
                    switch (internals.iieState) {
                        case IIE__UNKNOWN:
                            keepSpinning = FALSE;
                            break;

                        case IIE__WRITE_PORT_0_CONFIG:
                            keepSpinning = I2C_Sensors__ioExpanderInitPort0();
                            break;

                        case IIE__WRITE_PORT_1_CONFIG:
                            keepSpinning = I2C_Sensors__ioExpanderInitPort1();
                            break;

                        case IIE__DONE:
                            // Fall through
                        case IIE__FAILED_NACK:
                            // Fall through
                        default:
                            keepSpinning = FALSE;
                            break;
                    }
                }
                break;

            case I2C_SENSORS__ACTIONS__WRITE_IO_EXPANDER:
                {
                    switch (internals.wieState) {
                        case WIE__UNKNOWN:
                            keepSpinning = FALSE;
                            break;

                        case WIE__WRITE_PORT_0_VALUE:
                            keepSpinning = I2C_Sensors__ioExpanderWritePort0();
                            break;

                        case WIE__WRITE_PORT_1_VALUE:
                            keepSpinning = I2C_Sensors__ioExpanderWritePort1();
                            break;

                        case WIE__DONE:
                            // Fall through
                        case WIE__FAILED_NACK:
                            // Fall through
                        default:
                            keepSpinning = FALSE;
                            break;
                    }
                }
                break;

            case I2C_SENSORS__ACTIONS__READ_IO_EXPANDER:
                {
                    switch (internals.rieState) {
                        case RIE__UNKNOWN:
                            keepSpinning = FALSE;
                            break;

                        case RIE__READING:
                            keepSpinning = I2C_Sensors__ioExpanderReadPort1();
                            break;

                        case RIE__DONE:
                            // Fall through
                        case RIE__FAILED_NACK:
                            // Fall through
                        default:
                            keepSpinning = FALSE;
                            break;
                    }
                }
                break;

            default:
                keepSpinning = FALSE;
                break;
        }
    }
}

//###########################################################
// Private function definitions
//###########################################################

static BOOL I2C_Sensors__readRegNonBlocking(uint8_t devAddr,
                                            uint8_t regAddr,
                                            uint8_t nackMaskBit,
                                            BOOL isGaugeReading,
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
                if (isGaugeReading) {
                    internals.grsState = nextState;
                }

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

                if (isGaugeReading) {
                    internals.grsState = nextState;
                }

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
            if (isGaugeReading) {
                internals.grsState = nextState;
            }

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

static void I2C_Sensors__writeRegNonBlocking(uint8_t devAddr,
                                             uint8_t regAddr,
                                             uint8_t data,
                                             BOOL isGaugeInit,
                                             FuelGaugeInitState nextState,
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
                if (isGaugeInit) {
                    internals.fgiState = nextState;
                }
                *done = TRUE;
                *success = TRUE;
            }

            prevWriteDone = TRUE;
        } else if (I2C__TRANSACTION__DONE_ERROR_NACK == tStatus.state) {
            // We're now done with this write (and it failed)
            if (statusForRequestedTransaction) {
                if (isGaugeInit) {
                    internals.fgiState = FGI__FAILED_NACK;
                }

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

static BOOL I2C_Sensors__chargeLsb(void)
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(FUEL_GAUGE_I2C_SLAVE_ADDR,
                                           FG_REG_ADDR__ACCUMULATED_CHARGE_LSB,
                                           I2C_SENSORS__NACK__BATT_CHARGE,
                                           TRUE,
                                           GRS__CHARGE_MSB,
                                           internals.readings.raw_battery_charge + 1,
                                           &done,
                                           &gotOutput);
}

static BOOL I2C_Sensors__chargeMsb(void)
{
    BOOL done = FALSE, gotOutput = FALSE;
    BOOL result = I2C_Sensors__readRegNonBlocking(FUEL_GAUGE_I2C_SLAVE_ADDR,
                                                  FG_REG_ADDR__ACCUMULATED_CHARGE_MSB,
                                                  I2C_SENSORS__NACK__BATT_CHARGE,
                                                  TRUE,
                                                  GRS__VOLTAGE_LSB,
                                                  internals.readings.raw_battery_charge,
                                                  &done,
                                                  &gotOutput);

    if (done && gotOutput) {
        uint8_t *raw_battery_charge = internals.readings.raw_battery_charge;
        uint16_t chargeUShort = (uint16_t) (raw_battery_charge[1] + (raw_battery_charge[0] << 8));
        internals.readings.batt_charge_telem = (uint8_t) (chargeUShort >> 10) * 3;
    }

    return result;
}

static BOOL I2C_Sensors__voltageLsb(void)
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(FUEL_GAUGE_I2C_SLAVE_ADDR,
                                           FG_REG_ADDR__VOLTAGE_LSB,
                                           I2C_SENSORS__NACK__BATT_VOLTAGE,
                                           TRUE,
                                           GRS__VOLTAGE_MSB,
                                           internals.readings.raw_battery_voltage + 1,
                                           &done,
                                           &gotOutput);
}

static BOOL I2C_Sensors__voltageMsb(void)
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(FUEL_GAUGE_I2C_SLAVE_ADDR,
                                           FG_REG_ADDR__VOLTAGE_MSB,
                                           I2C_SENSORS__NACK__BATT_VOLTAGE,
                                           TRUE,
                                           GRS__CURRENT_LSB,
                                           internals.readings.raw_battery_voltage,
                                           &done,
                                           &gotOutput);
}

static BOOL I2C_Sensors__currentLsb(void)
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(FUEL_GAUGE_I2C_SLAVE_ADDR,
                                           FG_REG_ADDR__CURRENT_LSB,
                                           I2C_SENSORS__NACK__BATT_CURRENT,
                                           TRUE,
                                           GRS__CURRENT_MSB,
                                           internals.readings.raw_battery_current + 1,
                                           &done,
                                           &gotOutput);
}

static BOOL I2C_Sensors__currentMsb(void)
{
    BOOL done = FALSE, gotOutput = FALSE;
    BOOL result = I2C_Sensors__readRegNonBlocking(FUEL_GAUGE_I2C_SLAVE_ADDR,
                                                  FG_REG_ADDR__CURRENT_MSB,
                                                  I2C_SENSORS__NACK__BATT_CURRENT,
                                                  TRUE,
                                                  GRS__GAUGE_TEMP_LSB,
                                                  internals.readings.raw_battery_current,
                                                  &done,
                                                  &gotOutput);

    if (done && gotOutput) {
        uint8_t *raw_battery_current = internals.readings.raw_battery_current;
        uint16_t bCurrUShort = (uint16_t) (32767 - raw_battery_current[1] - (raw_battery_current[0] << 8));

        if (bCurrUShort > 17407) {
            //exceeds maximum value of 0.6 A
            internals.readings.batt_curr_telem = 255;
        } else {
            internals.readings.batt_curr_telem = (uint8_t) (bCurrUShort >> 7);
        }
    }

    return result;
}

static BOOL I2C_Sensors__gaugeTempLsb(void)
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(FUEL_GAUGE_I2C_SLAVE_ADDR,
                                           FG_REG_ADDR__TEMPERATURE_LSB,
                                           I2C_SENSORS__NACK__FUEL_GAUGE_TEMP,
                                           TRUE,
                                           GRS__GAUGE_TEMP_MSB,
                                           internals.readings.raw_fuel_gauge_temp + 1,
                                           &done,
                                           &gotOutput);
}

static BOOL I2C_Sensors__gaugeTempMsb(void)
{
    BOOL done = FALSE, gotOutput = FALSE;
    return I2C_Sensors__readRegNonBlocking(FUEL_GAUGE_I2C_SLAVE_ADDR,
                                           FG_REG_ADDR__TEMPERATURE_MSB,
                                           I2C_SENSORS__NACK__FUEL_GAUGE_TEMP,
                                           TRUE,
                                           GRS__DONE,
                                           internals.readings.raw_fuel_gauge_temp,
                                           &done,
                                           &gotOutput);
}

static BOOL I2C_Sensors__accumulatedChargeMsb(void)
{
    BOOL done = FALSE, success = FALSE;
    I2C_Sensors__writeRegNonBlocking(FUEL_GAUGE_I2C_SLAVE_ADDR,
                                     FG_REG_ADDR__ACCUMULATED_CHARGE_MSB,
                                     FUEL_GAUGE_CHARGE_ACCUM_MSB_INIT,
                                     TRUE,
                                     FGI__ACCUMULATED_CHARGE_LSB,
                                     &done,
                                     &success);
    return (done && success) ? TRUE : FALSE;
}

static BOOL I2C_Sensors__accumulatedChargeLsb(void)
{
    BOOL done = FALSE, success = FALSE;
    I2C_Sensors__writeRegNonBlocking(FUEL_GAUGE_I2C_SLAVE_ADDR,
                                     FG_REG_ADDR__ACCUMULATED_CHARGE_LSB,
                                     FUEL_GAUGE_CHARGE_ACCUM_LSB_INIT,
                                     TRUE,
                                     FGI__CONTROL,
                                     &done,
                                     &success);
    return (done && success) ? TRUE : FALSE;
}

static BOOL I2C_Sensors__writeControl(void)
{
    BOOL done = FALSE, success = FALSE;
    I2C_Sensors__writeRegNonBlocking(FUEL_GAUGE_I2C_SLAVE_ADDR,
                                     FG_REG_ADDR__CONTROL,
                                     FUEL_GAUGE_CONTROL_INIT,
                                     TRUE,
                                     FGI__DONE,
                                     &done,
                                     &success);

    // Either we're not done, succeeded and are done with the gauge initialization, or failed due to a NACK. In
    // all of these cases, we want to stop spinning.
    return FALSE;
}

static BOOL I2C_Sensors__readControl(void)
{
    BOOL done = FALSE, gotOutput = FALSE;
    I2C_Sensors__readRegNonBlocking(FUEL_GAUGE_I2C_SLAVE_ADDR,
                                    FG_REG_ADDR__CONTROL,
                                    0,
                                    FALSE,
                                    internals.grsState,
                                    &(internals.controlRegisterReadValue),
                                    &done,
                                    &gotOutput);

    if (done) {
        internals.rcState = gotOutput ? RC__DONE : RC__FAILED_NACK;
    }

    // Either we're not done, succeeded and are done with this action, or failed due to a NACK. In all cases, we want
    // to stop spinning
    return FALSE;
}

static BOOL I2C_Sensors__lowPower(void)
{
    BOOL done = FALSE, success = FALSE;
    I2C_Sensors__writeRegNonBlocking(FUEL_GAUGE_I2C_SLAVE_ADDR,
                                     FG_REG_ADDR__CONTROL,
                                     FUEL_GAUGE_CONTROL_LOW_POWER,
                                     FALSE,
                                     FGI__UNKNOWN,
                                     &done,
                                     &success);

    if (done) {
        internals.wlpState = success ? WLP__DONE : WLP__FAILED_NACK;
    }

    // Either we're not done, succeeded and are done with this action, or failed due to a NACK. In all cases, we want
    // to stop spinning
    return FALSE;
}

static BOOL I2C_Sensors__ioExpanderInitPort0(void)
{
    BOOL done = FALSE, success = FALSE;
    I2C_Sensors__writeRegNonBlocking(IO_EXPANDER_I2C_SLAVE_ADDR,
                                     IO_EXPANDER_CONFIG_PORT_0_REG_ADDR,
                                     IO_EXPANDER_CONFIG_PORT_0_VALUE,
                                     FALSE,
                                     internals.fgiState,
                                     &done,
                                     &success);

    if (done) {
        internals.iieState = success ? IIE__WRITE_PORT_1_CONFIG : IIE__FAILED_NACK;
    }

    return (done && success) ? TRUE : FALSE;
}

static BOOL I2C_Sensors__ioExpanderInitPort1(void)
{
    BOOL done = FALSE, success = FALSE;
    I2C_Sensors__writeRegNonBlocking(IO_EXPANDER_I2C_SLAVE_ADDR,
                                     IO_EXPANDER_CONFIG_PORT_1_REG_ADDR,
                                     IO_EXPANDER_CONFIG_PORT_1_VALUE,
                                     FALSE,
                                     internals.fgiState,
                                     &done,
                                     &success);

    if (done) {
        internals.iieState = success ? IIE__DONE : IIE__FAILED_NACK;
    }

    // Either we're not done, succeeded and are done with this action, or failed due to a NACK. In all cases, we want
    // to stop spinning
    return FALSE;
}

static BOOL I2C_Sensors__ioExpanderWritePort0(void)
{
    BOOL done = FALSE, success = FALSE;
    I2C_Sensors__writeRegNonBlocking(IO_EXPANDER_I2C_SLAVE_ADDR,
                                     IO_EXPANDER_OUTPUT_PORT_0_REG_ADDR,
                                     internals.ioExpanderPort0WriteValue,
                                     FALSE,
                                     internals.fgiState,
                                     &done,
                                     &success);

    if (done) {
        internals.wieState = success ? WIE__WRITE_PORT_1_VALUE : WIE__FAILED_NACK;
    }

    return (done && success) ? TRUE : FALSE;
}

static BOOL I2C_Sensors__ioExpanderWritePort1(void)
{
    BOOL done = FALSE, success = FALSE;
    I2C_Sensors__writeRegNonBlocking(IO_EXPANDER_I2C_SLAVE_ADDR,
                                     IO_EXPANDER_OUTPUT_PORT_1_REG_ADDR,
                                     internals.ioExpanderPort1WriteValue,
                                     FALSE,
                                     internals.fgiState,
                                     &done,
                                     &success);

    if (done) {
        internals.wieState = success ? WIE__DONE : WIE__FAILED_NACK;
    }

    // Either we're not done, succeeded and are done with this action, or failed due to a NACK. In all cases, we want
    // to stop spinning
    return FALSE;
}

static BOOL I2C_Sensors__ioExpanderReadPort1(void)
{
    BOOL done = FALSE, gotOutput = FALSE;
    I2C_Sensors__readRegNonBlocking(IO_EXPANDER_I2C_SLAVE_ADDR,
                                    IO_EXPANDER_INPUT_PORT_1_REG_ADDR,
                                    0,
                                    FALSE,
                                    internals.grsState,
                                    &(internals.ioExpanderPort1ReadValue),
                                    &done,
                                    &gotOutput);

    if (done) {
        internals.rieState = gotOutput ? RIE__DONE : RIE__FAILED_NACK;
    }

    // Either we're not done, succeeded and are done with this action, or failed due to a NACK. In all cases, we want
    // to stop spinning
    return FALSE;
}
