/*
 * RAD_mc.hpp
 *
 *  Created on: May 28, 2023
 *      Author: iris
 */

#ifndef RAD_MC_HPP_
#define RAD_MC_HPP_

/******************************************************************************
 *                  #INCLUDE
 *****************************************************************************/

//#include <CubeRover/MotorControl/MotorControlComponent.hpp>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
 *                  #DEFINE
 *****************************************************************************/

/*
 * Define MC Registers & WRITABLE bitmask
 * READ_ONLY registers defined with 0
 */
#define MC_REG_I2C_ADDR             0
#define MC_REG_TARGET_REL_POS       0x000001
#define MC_REG_TARGET_REL_SPEED     0x000002
#define MC_REG_SENSE_POS            0
#define MC_REG_SENSE_CURR           0
#define MC_REG_VEL_P_VAL            0x000020
#define MC_REG_VEL_I_VAL            0x000040
#define MC_REG_CURR_P_VAL           0x000080
#define MC_REG_CURR_I_VAL           0x000100
#define MC_REG_ACC_RATE             0x000200
#define MC_REG_DEC_RATE             0x000400
#define MC_REG_CMD_REG              0x001000    // OG: Execute_Command
#define MC_REG_STATUS_REG           0
#define MC_REG_FAULT_REG            0x004000


/******************************************************************************
 *                  NAMESPACE IRISMC
 *****************************************************************************/
//namespace IrisMC


/******************************************************************************
 *                  I2C REGISTER DEFINITION
 *****************************************************************************/

/*
 * Define Register Address enum for using w/arrays
 *
 * mc_i2c_addr and Regs_Total are only 2 not subject #define
 */
typedef enum {
    mc_i2c_addr = 0,        // RO   uint8   1B (OG = 1)
    target_rel_pos,         // RW   int32   4B  (ticks)
    target_speed,           // RW   uint8   1B  (% max speed)

    sense_pos,               // RW   int32   4B  (ticks) ?? relative?
    // sense_rel_pos,
    // sense_rotor_pos,
    sens_vel,               // RO   uint16   2B  (ticks/sec)
    sens_curr,              // RO   int16   2B  (mA)

    curr_p_val,             // RW   linear  2B
    curr_i_val,             // RW   linear  2B
//    pos_p_val,              // RW   linear  2B
//    pos_i_val,              // RW   linear  2B
    vel_p_val,              // RW   linear  2B
    vel_i_val,              // RW   linear  2B
    acc_rate,               // RW   uint16  2B  (ticks/sec)
    dec_rate,               // RW   uint16  2B  (ticks/sec)

    cmd_reg,               // RW   uint8   1B  write cmd executes new target position
//    enable_drv,          // RW   uint8   1B  write cmd enables driver
//    disable_drv,         // RW   uint8   1B  write cmd disables driver
//    reset_controller,       // RW   uint8   1B  write cmd resets controller
    status_reg,               // RW   uint16  1B  bitwise register (? 1B or 2B ?)
    fault_reg,              // RW   uint16  1B  bitwise register (? 1B or 2B ?)
    clear_fault,            // RW   uint8   1B  write cmd reset fault

//    pos_sens_curr_combo,     // RO   uint8   1B  Bit 0: hall sens 1, Bit 1: hall sens 2, Bit 2: hall sens 3, Bit 3...7: reserved

    NUM_REGS
} McRegAddr_t;

// Control Register
typedef union
{
    uint8_t value;
    struct
    {
        uint8_t open_loop : 1;
        uint8_t clear_fault : 1;
        uint8_t fsm_disable : 1;
        uint8_t fsm_run : 1;
        uint8_t override_fault_detection : 1;
        uint8_t unused : 3;
    } bits;
} ControlRegister_t;

// Status Register
typedef union
{
    uint8_t value;
    struct
    {
        uint8_t open_loop : 1;
        uint8_t clear_fault : 1;
        uint8_t fsm_disable : 1;
        uint8_t position_converged : 1;
        uint8_t controller_error : 1;
        uint8_t unused : 3;
    } bits;
} StatusRegister_t;

//Fault Register
typedef union
{
    uint8_t value;
    struct
    {
        uint8_t driver_fault : 1;
        uint8_t position_no_change : 1;
        uint8_t driving_wrong_direction : 1;
        uint8_t bad_command : 1;
        uint8_t watchdog : 1;
        uint8_t unexpected : 1;
        uint8_t unused : 2;
    } bits;
} FaultRegister_t;


struct McRegAttrs {
    uint8_t     id;
    bool        writable;
    bool        cmd_set;
    uint8_t     data_size;
    void *      data;
};

static const McRegAttrs mc_regs[NUM_REGS];


/******************************************************************************
 *                  MOTOR DEFINITION
 *****************************************************************************/

typedef enum
{
    MOTOR_A = 0,
    MOTOR_B,
    MOTOR_C,
    MOTOR_D,
    NUM_MOTORS
} MCMotor_t;

//enum class MotorAddr : uint8_t {
//    Motor_A = 0x48,
//    Motor_B = 0x49,
//    Motor_C = 0x4A,
//    Motor_D = 0x4B,
//    Motor_all = 0x00,
//    Motor_err
//};

//enum class MotorMaskID : uint8_t {
//    Motor_A = 1,
//    Motor_B = 2,
//    Motor_C = 4,
//    Motor_D = 8
//};

struct McAttrs {
    uint8_t id;
    uint8_t i2c_addr;
    McRegAttrs regs_data[NUM_REGS];
};

static const McAttrs motors[NUM_MOTORS];

/******************************************************************************
 *                  F' MC DEFS
 *****************************************************************************/

typedef enum
{
    MC_NO_ERROR,
    MC_I2C_TIMEOUT_ERROR,
    MC_BAD_COMMAND_INPUT,
    MC_UNEXPECTED_ERROR
} MCError_t;

//motors[MotorID::Motor_A].

#endif /* RAD_MC_HPP_ */
