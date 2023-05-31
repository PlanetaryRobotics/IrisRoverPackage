/*
 * RAD_mc.hpp (copy)
 *
 *  Created on: May 28, 2023
 *      Author: iris
 */

#ifndef RAD_MC_HPP_
#define RAD_MC_HPP_

//#include <CubeRover/MotorControl/MotorControlComponent.hpp>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

enum class MotorID : uint8_t {
    Motor_A = 0,
    Motor_B,
    Motor_C,
    Motor_D,
    Motor_Total
};

/*const uint8_t MotorControlComponentImpl::motorIdAddressMap[NUM_MOTORS] = {
        FRONT_LEFT_MC_I2C_ADDR, FRONT_RIGHT_MC_I2C_ADDR,
        REAR_LEFT_MC_I2C_ADDR, REAR_RIGHT_MC_I2C_ADDR};
        */
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


/*
 * Define MC Registers & WRITABLE bitmask
 * READ_ONLY registers defined with 0
 */
#define MC_REG_I2C_ADDR             0
#define MC_REG_TARGET_REL_POS       0x000001
#define MC_REG_TARGET_REL_SPEED     0x000002
#define MC_REG_SENSE_POS            0
#ifndef MC_REG_SENSE_POS
#define MC_REG_SENSE_REL_POS        0x000004
#define MC_REG_SENSE_ROTOR_POS      0
#endif
//#define MC_REG_SENSE_VEL            0
#define MC_REG_SENSE_CURR           0
//#define MC_REG_POS_P_VAL            0x000008
//#define MC_REG_POS_I_VAL            0x000010
#define MC_REG_VEL_P_VAL            0x000020
#define MC_REG_VEL_I_VAL            0x000040
#define MC_REG_CURR_P_VAL           0x000080
#define MC_REG_CURR_I_VAL           0x000100
#define MC_REG_ACC_RATE             0x000200
#define MC_REG_DEC_RATE             0x000400
#define MC_REG_CMD_REG              0x001000    // OG: Execute_Command
#define MC_REG_STATUS_REG           0
#define MC_REG_FAULT_REG            0x004000
//Depricated Registers
//#define MC_REG_ENABLE_DRV           0x002000
//#define MC_REG_DISABLE_DRV          0x004000
//#define MC_REG_RESET_CONTROLLER     0x008000
//#define MC_REG_STATUS_REG           0x010000
//#define MC_REG_FAULT_REG            0x020000
//#define MC_REG_CLEAR_FAULT          0x040000
//#define MC_REG_SIZE_POS_SENS_CURR_COMBO 0

/*
 * Define Register Address enum for using w/arrays
 *
 * mc_i2c_addr and Regs_Total are only 2 not subject #define
 */
enum McRegAddr : uint8_t {
    mc_i2c_addr = 0,        // RO   uint8   1B (OG = 1)

#ifdef MC_REG_TARGET_REL_POS
    target_rel_pos,         // RW   int32   4B  (ticks)
#endif
#ifdef MC_REG_TARGET_REL_SPEED
    target_speed,           // RW   uint8   1B  (% max speed)
#endif

#ifdef MC_REG_SENSE_POS
    sense_pos,               // RW   int32   4B  (ticks) ?? relative?
#endif
#ifdef MC_REG_SENSE_REL_POS
    sense_rel_pos,
#endif
#ifdef MC_REG_SENSE_ROTOR_POS
    sense_rotor_pos,
#endif
#ifdef MC_REG_SENSE_VEL
    sens_vel,               // RO   uint16   2B  (ticks/sec)
#endif
#ifdef MC_REG_SENSE_CURR
    sens_curr,              // RO   int16   2B  (mA)
#endif

#ifdef MC_REG_CURR_P_VAL
    curr_p_val,             // RW   linear  2B
#endif
#ifdef MC_REG_CURR_I_VAL
    curr_i_val,             // RW   linear  2B
#endif
#ifdef MC_REG_POS_P_VAL
//    pos_p_val,              // RW   linear  2B
#endif
#ifdef MC_REG_POS_I_VAL
//    pos_i_val,              // RW   linear  2B
#endif
#ifdef MC_REG_VEL_P_VAL
    vel_p_val,              // RW   linear  2B
#endif
#ifdef MC_REG_VEL_I_VAL
    vel_i_val,              // RW   linear  2B
#endif

#ifdef MC_REG_ACC_RATE
    acc_rate,               // RW   uint16  2B  (ticks/sec)
#endif
#ifdef MC_REG_DEC_RATE
    dec_rate,               // RW   uint16  2B  (ticks/sec)
#endif

#ifdef MC_REG_CMD_REG
    cmd_reg,               // RW   uint8   1B  write cmd executes new target position
#endif
#ifdef MC_REG_ENABLE_DRV
//    enable_drv,          // RW   uint8   1B  write cmd enables driver
#endif
#ifdef MC_REG_DISABLE_DRV
//    disable_drv,         // RW   uint8   1B  write cmd disables driver
#endif
#ifdef MC_REG_RESET_CONTROLLER
//    reset_controller,       // RW   uint8   1B  write cmd resets controller
#endif

#ifdef MC_REG_STATUS_REG
    status_reg,               // RW   uint16  1B  bitwise register (? 1B or 2B ?)
#endif
#ifdef MC_REG_FAULT_REG
    fault_reg,              // RW   uint16  1B  bitwise register (? 1B or 2B ?)
#endif
#ifdef MC_REG_CLEAR_FAULT
    clear_fault,            // RW   uint8   1B  write cmd reset fault
#endif
#ifdef MC_REG_SIZE_POS_SENS_CURR_COMBO
//    pos_sens_curr_combo,     // RO   uint8   1B  Bit 0: hall sens 1, Bit 1: hall sens 2, Bit 2: hall sens 3, Bit 3...7: reserved
#endif

    Regs_Total
};

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







struct McRegDef {
    uint8_t     id;
    bool        writable;
    uint8_t     data_size;
    void *      data;
};

static const McRegDef mc_i2c_regs[(unsigned int) McRegAddr::Regs_Total];

// ENUM CLASSES
struct McAttrs {
    MotorID id;
    uint8_t i2c_addr;


};

static const McAttrs motors[(unsigned int) MotorID::Motor_Total];
//motors[MotorID::Motor_A].

#endif /* RAD_MC_HPP_ */
