#ifndef MAIN_H_
#define MAIN_H_

/* Include the IQmath header file. */
#define GLOBAL_IQ   15
#include "IQmathLib.h"

#include "driverlib.h"
#include "bsp.h"
#include "pi.h"

#define PWM_PERIOD_TICKS        512     // 15.6KHz @ 16MHz
#define PWM_HALF_PERIOD_TICKS   256
#define PI_CONTROL_PRESCALER    10     // 1.5KHz, speed control

#define KP_POS                  0.1
#define KP_SPD                  0.1
#define KI_SPD                  0.0
#define KP_CUR                  0.1
#define KI_CUR                  0.0


#define ONE_OVER_4096           0.0002441

inline _iq _IQ15mpy_inline(_iq,_iq);

typedef struct HallSensor{
    uint8_t pattern;
    uint8_t oldPattern;
    bool event;
    bool error;
}HallSensor;

#endif /* MAIN_H_ */
