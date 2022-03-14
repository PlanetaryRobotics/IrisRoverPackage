/*
 * state_machine.c
 *
 *  Created on: Mar 13, 2022
 *      Author: jonbs
 */

#include "include/state_machine.h"


/**
 * @brief      Disable the drive
 */
void disable(volatile MOTOR_STATE* motor_state){
  if(motor_state->state_machine.state == IDLE){
      return; // already in IDLE, nothing to do
  }

  __disable_interrupt();
  disableGateDriver();
  motor_state->state_machine.state = IDLE;
  motor_state->target_position = 0;
  motor_state->current_position = 0;
  __enable_interrupt();
}


/**
 * @brief      Enter run state
 */
void run(volatile MOTOR_STATE* motor_state){
  if(motor_state->state_machine.state == RUNNING){
      return; // already in RUNNING, nothing to do
  }
  __disable_interrupt();
  enableGateDriver();

  if(motor_state->target_position -  motor_state->current_position>= 0){
      motor_state->target_direction = 1;
  } else {
      motor_state->target_direction = -1;
  }

  motor_state->current_position = 0;
  motor_state->target_reached = false;
  motor_state->state_machine.state = RUNNING;
  __enable_interrupt();
}


/**
 * @brief      Update the drive state machine
 */
void updateStateMachine(volatile MOTOR_STATE* motor_state){
    if(motor_state->state_machine.command == RUN && motor_state->state_machine.state == IDLE){
        run(motor_state);
    } else if (motor_state->state_machine.command == DISABLE && motor_state->state_machine.state == RUNNING){
        disable(motor_state);
    }
    motor_state->state_machine.command = NO_CMD;
}
