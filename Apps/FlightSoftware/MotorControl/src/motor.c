/*
 * state_machine.c
 *
 *  Created on: Mar 13, 2022
 *      Author: jonbs
 */

#include <include/motor.h>


/**
 * @brief      Disable the drive
 */
void disable(volatile MOTOR* motor){
  if(motor->state_machine.state == IDLE){
      return; // already in IDLE, nothing to do
  }

  __disable_interrupt();
  disableGateDriver();
  motor->state_machine.state = IDLE;
  motor->target_position = 0;
  motor->current_position = 0;
  __enable_interrupt();
}


/**
 * @brief      Enter run state
 */
void run(volatile MOTOR* motor){
  if(motor->state_machine.state == RUNNING){
      return; // already in RUNNING, nothing to do
  }
  __disable_interrupt();
  enableGateDriver();

  if(motor->target_position -  motor->current_position>= 0){
      motor->target_direction = 1;
  } else {
      motor->target_direction = -1;
  }

  motor->current_position = 0;
  motor->target_reached = false;
  motor->state_machine.state = RUNNING;
  __enable_interrupt();
}


/**
 * @brief      Update the drive state machine
 */
void updateStateMachine(volatile MOTOR* motor){
    if(motor->state_machine.command == RUN && motor->state_machine.state == IDLE){
        run(motor);
    } else if (motor->state_machine.command == DISABLE && motor->state_machine.state == RUNNING){
        disable(motor);
    }
    motor->state_machine.command = NO_CMD;
}


/*
 *  @brief      TODO
 */
void handleMotorTimeout(volatile MOTOR* motor){
    motor->target_reached = true;
    motor->target_position = motor->current_position; // so motor won't flip g_targetReached again
    motor->registers.fault_register |= DRIVING_TIMEOUT;
    motor->registers.status_register |= (POSITION_CONVERGED | CONTROLLER_ERROR);
    motor->driving_timeout_ctr = 0;
}


/* ========================================================================
 *                      Sensor Functions
 * =======================================================================
 */

/**
* @brief      Initializes the hall interface.
*/
void initializeHallInterface(volatile MOTOR* motor){
    motor->hall_sensor.HALL_MAP[0] = 0xff;
    motor->hall_sensor.HALL_MAP[7] = 0xff;

    motor->hall_sensor.HALL_MAP[5] = 0;
    motor->hall_sensor.HALL_MAP[1] = 1;
    motor->hall_sensor.HALL_MAP[3] = 2;
    motor->hall_sensor.HALL_MAP[2] = 3;
    motor->hall_sensor.HALL_MAP[6] = 4;
    motor->hall_sensor.HALL_MAP[4] = 5;
}


/*
* @brief       TODO
*/
void initializeSensorVariables(volatile MOTOR* motor){
     motor->current_sensor.current_offset_phase_A = 0;
     motor->current_sensor.current_offset_phase_B = 0;
     motor->current_sensor.current_offset_phase_C = 0;
     motor->hall_sensor.comm_state = 0;
     motor->hall_sensor.pattern = 0;
     motor->hall_sensor.last_pattern = 0;
     motor->current_position = 0;
     motor->last_position = motor->current_position;
     motor->target_position = 0;
     motor->driving_timeout_ctr = 0;
}

/**
* @brief      Do the offset calibration of the current sensor
*/
void currentOffsetCalibration(volatile MOTOR* motor){

    enableCalibration(); // pull p4.4 high (calibration pin on DRV8304 motor driver)
    enableGateDriver();  // let current flow through motor to get accurate offset measurement

    // Prepare ADC conversion for next round
    HWREG8(ADC12_B_BASE + OFS_ADC12CTL0_L) &= ~(ADC12ENC);
    HWREG8(ADC12_B_BASE + OFS_ADC12CTL0_L) |= ADC12ENC + ADC12SC;

    // read from ADC channels for 3 motor channels, save readings as offsets
    motor->current_sensor.current_offset_phase_A = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_0));
    motor->current_sensor.current_offset_phase_B = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_1));
    motor->current_sensor.current_offset_phase_C = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_2));

    disableCalibration(); // pull p4.4 low
    disableGateDriver(); // turn off motor driver for time being
}


/**
* @brief      Reads a hall sensor.
*/
void readHallSensor(volatile MOTOR* motor){
    motor->hall_sensor.pattern = READ_HALL_W >> 1;   // W
    motor->hall_sensor.pattern |= READ_HALL_V >> 4;  // V
    motor->hall_sensor.pattern |= READ_HALL_U >> 6;  // U
    motor->hall_sensor.Event = motor->hall_sensor.pattern ^ motor->hall_sensor.last_pattern;
    motor->hall_sensor.last_pattern = motor->hall_sensor.pattern;

    if(motor->hall_sensor.pattern & 0x07){
       motor->hall_sensor.Error = 1;
    }
    if(motor->hall_sensor.pattern == 0x00){
       motor->hall_sensor.Error = 1;
    }
}


/**
* @brief      Gets the speed.
*
* @return     The speed.
*/
_iq getSpeed(volatile MOTOR* motor){
    // Normalize speed to -128 ticks < diff < 127 to -1.0 < diff < +1.0
    // 255 ticks per (PI_SPD_CONTROL_PRESCALER * PWM_PERIOD_TICKS) represents 9.600 eRPM
    // which equates to ~25.22 cm/sec, much faster than we should be going in normal operation
    int32_t deltaPos = _IQsat(motor->current_position - motor->last_position, 256, -256);
    // shift left 7 to scale 9 bit deltaPos (-255,255) to 15 decimal fixed point _IQ (-1.0,1.0)
    motor->current_speed = deltaPos << 7;
    motor->last_position = motor->current_position;
    return motor->current_speed;
}

