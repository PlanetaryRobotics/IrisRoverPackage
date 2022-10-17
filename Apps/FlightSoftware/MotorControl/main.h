#ifndef MAIN_H_
#define MAIN_H_

/* Include the IQmath header file. */
#define GLOBAL_IQ                   15
#include "IQmathLib.h"

#include "driverlib.h"
#include "bsp.h"
#include "i2c.h"

#include "pi.h"
#include "mod6_cnt.h"
#include "impulse.h"

#define PWM_PERIOD_TICKS            512     // 15.6 KHz @ 16MHz
#define PWM_HALF_PERIOD_TICKS       256
#define PI_SPD_CONTROL_PRESCALER    1000    // 15.6 Hz, speed control

#define KP_SPD                  0.3
#define KI_SPD                  0.001
#define KP_CUR                  0.95
#define KI_CUR                  0.001

#define OPEN_LOOP_TORQUE        0.1       // Normalized to 1.0, 1.0 being maximum current system can produce
#define PERIOD_IMPULSE          150
#define CLOSE_LOOP_THRESHOLD    0.01      // Close loop threshold from open to close loop

#define ONE_OVER_4096           0.0002441

#define MAX_TARGET_SPEED                   100
#define MIN_TARGET_SPEED                  -MAX_TARGET_SPEED

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

#endif /* MAIN_H_ */
