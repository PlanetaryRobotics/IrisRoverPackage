/*
 * Implemenation of critical control loop.
 *
 * TODO:
 *  - Implement (pull over the EVM test code).
 *  - Deal with integral overflow in a better than thresholding.
 *  - Add midpoint or Range-Kutta to increase calc. fidelity (if necessary).
 *
 * Author: Connor W. Colombo, CMU
 */

#ifndef _CR_MOT_CONTROL_H_
#define _CR_MOT_CONTROL_H_

//
// Include files
//
#include "Driver_HAL.h"
#include "I2C_registers.h"

    // Window / Saturation Threshold Settings:
    const int32_t minErrorPosition = -1000000; // all arbitrary rn
    const int32_t maxErrorPosition = -1000000;
    const int32_t minErrorIntegral = -1000000;
    const int32_t maxErrorIntegral = -1000000;
    const int32_t maxDeceleration = -1000000;
    const int32_t maxAcceleration = -1000000;
    const int16_t maxCurrent = 400; // [mA]

    /*
     * Helper function. Ensures x is between min and max.
     * If not, it saturates it.
     */
    int32_t saturate(int32_t x, int32_t min, int32_t max){
        if(x > max){
            return max;
        } else if(x < min){
            return min;
        } else{
            return x;
        }
    }

    static uint32_t time;

    // State Variables (measured values):
    static int32_t position;
    static int32_t velocity;
    static int16_t current;

    // State Errors:
    // TODO: Do this with less memory without sacrificing CPU cycles (union across each phase?).
    static int32_t ex; //   - Error in position
    static int32_t ex_prev;
    static int32_t dex; //  - Derivative of Error in position
    static int32_t iex; //  - Integral of Error in position

    static int32_t ev; //   - Error in velocity
    static int32_t ev_prev;
    static int32_t dev; //  - Derivative of Error in velocity
    static int32_t iev; //  - Integral of Error in velocity

    static int32_t ei; //   - Error in current
    static int32_t iei; //  - Integral of Error in current

    // Internal Setpoints:
    static int32_t sp_vel; //   - Velocity Setpoint
    static int32_t sp_curr;//   - Current Setpoint
    static uint16_t sp_pwm;// - PWM Setpoint

    void Update_Control() {
        // Note: if CPU cycles become scarce, run this at a slower but timer-driven frequency and then remove Dt from the calcs (it will be absorbed by the constants).

        static uint32_t Dt;
        Dt = time - getTotalTimeDelta();
        time += Dt;

        // Update State Variables (grab them all at the same moment in time):
        static int32_t Dx;
        Dx = getPosition() - position; // Change in position
        position += Dx;
        velocity += Dx / Dt;
        current = getCurrentNorm();

        //
        // PHASE 1: Position -> Velocity
        //
        // Update Errors:
        ex = saturate(position - i2c_register.data.target_pos, minErrorPosition, maxErrorPosition); // arbitrary saturation window rn
        dex = (ex-ex_prev) / Dt;
        iex += saturate(ex * Dt, minErrorIntegral, maxErrorIntegral); // arbitrary saturation window rn
        ex_prev = ex;
        // Compute Control Signal:
        sp_vel = saturate(i2c_register.data.kP_position * ex + i2c_register.data.kI_position * iex + i2c_register.data.kD_position * dex, -i2c_register.data.target_speed, i2c_register.data.target_speed);
        // Compute what acceleration we'd be commanding (and threshold it):
        static int32_t ac;
        ac = saturate((sp_vel-velocity) / Dt, maxDeceleration, maxAcceleration);
        sp_vel = ac*Dt + velocity; // Recompute setpoint by integrating thresholded accel.

        //
        // PHASE 2: Velocity -> Current
        //
        // Update Errors:
        ev = sp_vel - velocity;
        dev = (ev-ev_prev) / Dt;
        iev += saturate(ev * Dt, minErrorIntegral, maxErrorIntegral);
        ev_prev = ev;
        // Compute Control Signal:
        sp_curr = saturate(i2c_register.data.kP_velocity * ev + i2c_register.data.kI_velocity * iev + i2c_register.data.kD_velocity * dev, 0,maxCurrent);

        //
        // PHASE 3: Current -> PWM
        //
        // Update Errors:
        ei = sp_curr - getCurrentNorm();
        iei += saturate(ei * Dt, minErrorIntegral, maxErrorIntegral);
        // Compute Control Signal:
        sp_pwm = saturate(i2c_register.data.kP_velocity * ei + i2c_register.data.kI_velocity * iei, 0,1000);

        setPWM(sp_pwm);
    }

#endif // _CR_MOT_CONTROL_H_
