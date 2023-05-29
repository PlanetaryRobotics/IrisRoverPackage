/*
 * RAD_mc.hpp
 *
 *  Created on: May 28, 2023
 *      Author: iris
 */

#ifndef RAD_MC_HPP_
#define RAD_MC_HPP_

#include <CubeRover/MotorControl/MotorControlComponent.hpp>


//enum class MotorAddr : uint8_t {
//    Motor_A = 0x48,
//    Motor_B = 0x49,
//    Motor_C = 0x4A,
//    Motor_D = 0x4B,
//    Motor_all = 0x00,
//    Motor_err
//};

enum class MotorID : uint8_t {
    Motor_A = 0,
    Motor_B,
    Motor_C,
    Motor_D,
    Motor_Total
};

//enum class MotorMaskID : uint8_t {
//    Motor_A = 1,
//    Motor_B = 2,
//    Motor_C = 4,
//    Motor_D = 8
//};

// ENUM CLASSES
struct MotorAttrs{
    MotorID id;
    uint8_t addr;
    uint8_t mask;
};

static const MotorAttrs motors[MotorID::Motor_Total];

#endif /* RAD_MC_HPP_ */
