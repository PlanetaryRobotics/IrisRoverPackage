#ifndef MAIN_H_
#define MAIN_H_

/* Include the IQmath header file. */
#define GLOBAL_IQ                   15
#include "IQmathLib.h"

#include "driverlib.h"
#include "bsp.h"

#include "pi.h"
#include "mod6_cnt.h"
#include "impulse.h"

#define PWM_PERIOD_TICKS            512     // 15.6 KHz @ 16MHz
#define PWM_HALF_PERIOD_TICKS       256
#define PI_SPD_CONTROL_PRESCALER    1000    // 15.6 Hz, speed control

#define KP_SPD                  0.35
#define KI_SPD                  0.003
#define KP_CUR                  1.5
#define KI_CUR                  0.0004

#define OPEN_LOOP_TORQUE        0.1       // Normalized to 1.0, 1.0 being maximum current system can produce
#define PERIOD_IMPULSE          200
#define CLOSE_LOOP_THRESHOLD    0.01      // Close loop threshold from open to close loop

#define ONE_OVER_4096           0.0002441

#define MAX_TARGET_WINDOW           63
#define MIN_TARGET_WINDOW           -MAX_TARGET_WINDOW

#define I2C_RX_BUFFER_MAX_SIZE      8
#define I2C_TX_BUFFER_MAX_SIZE      8

#define EXPECTED_I2C_PACKET_SIZE    8

inline _iq _IQ15mpy_inline(_iq,_iq);

typedef struct HallSensor{
    uint8_t Pattern;
    uint8_t OldPattern;
    bool Event;
    bool Error;
}HallSensor;

typedef enum StateMachine{
    UNINITIALIZED,
    IDLE,
    RUNNING,
    STOPPED
}StateMachine;

typedef enum CmdState{
    RUN,
    STOP,
    DISABLE,
    NO_CMD
}CmdState;

typedef enum I2cRegister{
    I2C_ADDRESS = 0x01,
    RELATIVE_TARGET_POSITION = 0x02,
    TARGET_SPEED = 0x03,
    CURRENT_POSITION = 0x04,
    CURRENT_SPEED = 0x05,
    MOTOR_CURRENT = 0x06,
    P_CURRENT = 0x07,
    I_CURRENT = 0x08,
    P_SPEED = 0x09,
    I_SPEED = 0x0A,
    P_POSITION = 0x0B,
    ACC_RATE = 0x0C,
    DEC_RATE = 0x0D,
    CONTROL_REGISTER = 0x0E,
    STATUS_REGISTER = 0x0F,
    FAULT_REGISTER = 0x10
}I2cRegister;

#endif /* MAIN_H_ */
