// ======================================================================
// \title  main.c
// \author cedric
// \edited by Jonathan
// \brief  Runs control loop to drive motors, as well as initializing
//         everything needed for the rest of the code. Also reads from
//         current sensors on motor driver and hall sensors on motor
//
// ======================================================================

#include "include/main.h"

//// Used for calibration
//bool motor->read_sensors = false;

_iq g_openLoopTorque;
_iq g_closeLoopThreshold;



uint16_t g_controlPrescaler;

volatile PI_CONTROLLER g_piSpd;
volatile PI_CONTROLLER g_piCur;
IMPULSE_TIMER g_impulse_timer;

volatile uint8_t g_maxSpeed;

volatile MOTOR g_motor;

volatile uint16_t g_accelRate;
volatile uint16_t g_decelRate;

uint8_t g_errorCounter= 0; // incremented every time inner control loop is reached and motor is acting strange
                           // if it exceeds ERROR_ITERATION_THRESHOLD then motor is stopped


/*
 * @brief       TODO
 */
void initializeSoftwareControlVariables(volatile MOTOR* motor){
    // software control related variables (rate groups, internal state machine)
    g_controlPrescaler = PI_SPD_CONTROL_PRESCALER;
    motor->closed_loop = false;
    motor->state_machine.state = RUNNING;
    motor->state_machine.command = NO_CMD;
    motor->registers.control_register = 0; // see main.h for bits
}

/*
 * @breif   initialize motor controller related variables (PI controllers for speed and current)
 */
void initializeControllerVariables(volatile MOTOR* motor){
    g_maxSpeed = MAX_TARGET_SPEED;

    g_openLoopTorque = _IQ(OPEN_LOOP_TORQUE);
    g_impulse_timer.period = PERIOD_IMPULSE;
    motor->hall_sensor.comm_cycle_counter = 0;

    resetPiController(&g_piSpd);
    resetPiController(&g_piCur);

    g_piSpd.Kp = _IQ(KP_SPD);
    g_piSpd.Ki = _IQ(KI_SPD);
    g_piCur.Kp = _IQ(KP_CUR);
    g_piCur.Ki = _IQ(KI_CUR);

    g_closeLoopThreshold = _IQ(CLOSE_LOOP_THRESHOLD);

    motor->target_direction = 1;
    motor->closed_loop = false;
}

/*
 * @brief   Do everything necessary to init MSP and begin driving
 */
void initController(volatile MOTOR* motor){
    initializeGpios();

    //Set DCO frequency to 16MHz
    CS_setDCOFreq(CS_DCORSEL_1, CS_DCOFSEL_4);

    CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);

    // Initialize variables
    initializeSensorVariables(motor);
    initializeSoftwareControlVariables(motor);
    initializeControllerVariables(motor);

    // initialize hardware components
    initializeI2cModule();
    initializePwmModules();
    initializeAdcModule();
    initializeHallInterface(motor);

    currentOffsetCalibration(motor); // get initial estimate of current offsets
    __bis_SR_register(GIE); // enable interrupts (timer & i2c)
    enableGateDriver(); // get ready to move
}

/*
 * @brief       TODO
 */
void moderatePIControllers(volatile MOTOR* motor){
    // apply constant torque if still in open loop
    if(motor->closed_loop == false && motor->target_reached == false){
      g_piCur.i1 = 0;
      g_piCur.ui = 0;
      g_piSpd.i1 = 0;
      g_piSpd.ui = 0;
      g_piCur.Out = g_openLoopTorque;
      if(_IQabs(motor->current_speed) > g_closeLoopThreshold){
          motor->closed_loop = true;
      }
    }
    // set integrator and output to 0 if target is reached
    if(motor->target_reached == true){
        g_piCur.i1 = 0;
        g_piCur.ui = 0;
        g_piSpd.i1 = 0;
        g_piSpd.ui = 0;
        g_piCur.Out = 0;
        motor->closed_loop = false;
    }
//           if controllers are saturated, reset its integrator & output
    if(g_piSpd.w1){
        __disable_interrupt();
       g_piSpd.i1 = 0;
       g_piSpd.ui = 0;
       g_piSpd.v1 = 0;
       __enable_interrupt();
    }
    if(g_piCur.w1){
        __disable_interrupt();
        g_piCur.i1 = _IQ(motor->target_direction*0.5); // full wipe of integrator causes jumpy stop-start behavior
        g_piCur.ui = 0;
        g_piCur.v1 = 0;
        __enable_interrupt();
    }
}

/*
 * @brief      TODO
 */
void checkTargetReached(volatile MOTOR* motor){
    if  (_IQabs(motor->target_position - motor->current_position) < POSITION_CONVERGENCE_THRESHOLD) {
          // target has been reached
          motor->target_reached = true;
          motor->registers.status_register |= POSITION_CONVERGED;
          // turn off output
          _iq output = 0;
          pwmGenerator(motor->hall_sensor.comm_state, output);
      } else {
          // target not reached yet
          motor->target_reached = false;
          motor->registers.status_register &= ~POSITION_CONVERGED;
      }
}

/*
 * @brief   TODO
 */
void driveOpenLoop(volatile MOTOR* motor){
    if(!motor->target_reached){
        // Iterate through commutations & apply impulse to desired motor windings
        iterate_impulse_timer(&g_impulse_timer, /*driving_open_loop=*/true);

        if(g_impulse_timer.cycle){
            motor->hall_sensor.comm_cycle_counter = iterate_mod6_counter(motor->hall_sensor.comm_cycle_counter, /*driving_open_loop=*/true);

            if (motor->target_direction > 0){
                motor->hall_sensor.comm_state = motor->hall_sensor.comm_cycle_counter;
            } else {
                motor->hall_sensor.comm_state = 5 - motor->hall_sensor.comm_cycle_counter;
            }
        }

        _iq output;
        if(motor->registers.control_register & OPEN_LOOP_TORQUE_OVERRIDE){
            output = _IQ(g_maxSpeed / MAX_TARGET_SPEED); // apply user specified output
        }
        else{
            output = _IQ(FULLY_OPEN_LOOP_PWM); // apply constant output (30% duty cycle)
        }

        // apply output as PWM
        if(motor->target_direction > 0) {
            pwmGenerator(motor->hall_sensor.comm_state, output);
        } else{
            pwmGenerator(motor->hall_sensor.comm_state, -output);
        }

    }

    // control loop - updates current position with speed estimate & ticks timeout counter
    if (g_controlPrescaler<=0){
        g_controlPrescaler = PI_SPD_CONTROL_PRESCALER;
        motor->current_position += motor->target_direction * OPEN_LOOP_SPEED;

        if(!motor->target_reached){
            motor->driving_timeout_ctr++;
        }
    }
}

/*
 * @brief       TODO
 */
void closedLoopCurrentLoop(volatile MOTOR* motor){
    // inner control loop (current), apply output as PWM duty cycle
    // Normalize current values from  -2047 < adc < +2048 to iq15 --> -1.0 < adc < 1.0 and convert to iq format
    g_piCur.Fbk = (motor->current_sensor.current_phase_A + motor->current_sensor.current_phase_B + motor->current_sensor.current_phase_C) << 4;
    g_piCur.Ref = g_piSpd.Out;

    pi_iteration(&g_piCur);

    // check if we can pivot to closed loop control
    // todo: fix if statement
    if(_IQabs(motor->current_speed) > g_closeLoopThreshold && !motor->target_reached){
        motor->closed_loop = true;
    } else {
        motor->closed_loop = false;
    }

    moderatePIControllers(motor); // clamp and/or zero values as necessary

    // apply duty cycle based on current PI controller's output
    pwmGenerator(motor->hall_sensor.comm_state, g_piCur.Out);
}

/*
 * @brief       TODO
 */
void closedLoopSpeedLoop(volatile MOTOR* motor){
    if(motor->target_direction > 0){
          g_piSpd.Ref = g_maxSpeed << 8;
      }
      else {
          g_piSpd.Ref = -g_maxSpeed << 8;
      }

    g_piSpd.Fbk = getSpeed(motor);
    pi_iteration(&g_piSpd);
}

/*
 * @brief       TODO
 */
void checkForClosedLoopErrors(volatile MOTOR* motor){
    // check for errors in controller operation
    if (motor->current_position == motor->last_position && !motor->target_reached){
      // position isn't updating; hall sensors likely not powered or broken
      g_errorCounter++;
      motor->registers.fault_register |= POSITION_NO_CHANGE;
    } else if ( (motor->current_position - motor->last_position)*motor->target_direction < 0 && !motor->target_reached){
      // moving in wrong direction
      g_errorCounter++;
      motor->registers.fault_register |= DRIVING_WRONG_DIRECTION;
    } else {
      // operating normally; no error
      motor->registers.status_register &= ~CONTROLLER_ERROR;
      g_errorCounter = 0; // reset error counter
      motor->registers.fault_register &= ~(POSITION_NO_CHANGE & DRIVING_WRONG_DIRECTION); //clear faults in register
    }

    // errors on last ERROR_ITERATION_THRESHOLD time steps; time to stop trying to drive motor
    if(g_errorCounter >= ERROR_ITERATION_THRESHOLD){
      if (motor->registers.control_register & OVERRIDE_FAULT_DETECTION == 0x00) //check if we should stop controller given fault
          motor->target_position = motor->current_position = 0; //stop controller
      motor->registers.status_register |= CONTROLLER_ERROR; // add flag to status register
    }
}

/*
* @brief  update sensor (Hall & current) readings for when driving in closed loop
*/
void readSensors(volatile MOTOR* motor){

    // measure hall sensors
    readHallSensor(motor);

    if(motor->closed_loop == false && motor->target_reached == false){
        // Execute macro to generate ramp up
        iterate_impulse_timer(&g_impulse_timer, /*driving_open_loop=*/false);
        if(g_impulse_timer.cycle){
            motor->hall_sensor.comm_cycle_counter = iterate_mod6_counter(motor->hall_sensor.comm_cycle_counter, /*driving_open_loop=*/false);

            if (motor->target_direction > 0){
                motor->hall_sensor.comm_state = motor->hall_sensor.comm_cycle_counter;
            } else {
                motor->hall_sensor.comm_state = 5 - motor->hall_sensor.comm_cycle_counter;
            }
        }
    }
    else{
        // allow commutation to be controlled by hall sensor readings (will time them way better than we could)
        motor->hall_sensor.comm_state = motor->hall_sensor.HALL_MAP[motor->hall_sensor.pattern];
    }

    // update current position based on hall sensor readings
    if(motor->hall_sensor.Event){
        if(motor->hall_sensor.HALL_MAP[motor->hall_sensor.pattern] == 5 && motor->hall_sensor.last_comm_state == 0){
            motor->current_position--;
        }
        else if(motor->hall_sensor.HALL_MAP[motor->hall_sensor.pattern] == 0 && motor->hall_sensor.last_comm_state == 5){
            motor->current_position++;
        }
        else if(motor->hall_sensor.HALL_MAP[motor->hall_sensor.pattern] > motor->hall_sensor.last_comm_state){
            motor->current_position++;
        }
        else{
            motor->current_position--;
        }
        motor->hall_sensor.last_comm_state = motor->hall_sensor.HALL_MAP[motor->hall_sensor.pattern];
    }

    // update current readings
    // Prepare ADC conversion for next round
    HWREG8(ADC12_B_BASE + OFS_ADC12CTL0_L) &= ~(ADC12ENC);
    HWREG8(ADC12_B_BASE + OFS_ADC12CTL0_L) |= ADC12ENC + ADC12SC;
    // Remove offset
    motor->current_sensor.current_phase_A = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_0)) - motor->current_sensor.current_offset_phase_A;
    motor->current_sensor.current_phase_B = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_1)) - motor->current_sensor.current_offset_phase_B;
    motor->current_sensor.current_phase_C = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_2)) - motor->current_sensor.current_offset_phase_C;
}


/**
 * @brief      main function
 */
void main(void){

  // Turn off the watchdog
  WDT_A_hold(WDT_A_BASE);

  initController(&g_motor); //init all variables and functionality needed to drive
  g_motor.target_position = 10000;
  g_motor.registers.control_register = 32;

  while(1){
      checkTargetReached(&g_motor);

      // target position sets direction motor drives in
      if(g_motor.target_position - g_motor.current_position >= 0){
          g_motor.target_direction = 1;
      } else {
          g_motor.target_direction = -1;
      }

      // check if driving in open or closed loop (& have been told to execute command), act accordingly
      if (g_motor.registers.control_register & DRIVE_OPEN_LOOP && g_motor.registers.control_register & EXECUTE_COMMAND) {
          // driving in fully open loop
          driveOpenLoop(&g_motor);

      } else if (g_motor.registers.control_register & EXECUTE_COMMAND){
        // driving closed loop

        if(g_motor.read_sensors){ // update sensor readings if requested by timer interrupt
            readSensors(&g_motor);
            g_motor.read_sensors = false;
        }

        closedLoopCurrentLoop(&g_motor); // inner loop of controller

        if(g_controlPrescaler <= 0){

          closedLoopSpeedLoop(&g_motor); // outer loop of controller
          checkForClosedLoopErrors(&g_motor);

          // reset control prescaler & tick timeout counter
          g_controlPrescaler = PI_SPD_CONTROL_PRESCALER;
          if(!g_motor.target_reached){
              g_motor.driving_timeout_ctr++;
          }
        }
      }

      // check if motor has taken too long to converge, act accordingly if so
      if(g_motor.driving_timeout_ctr > DRIVING_TIMEOUT_THRESHOLD){
          handleMotorTimeout(&g_motor);
      }
  } // end of while loop
}


/**
 * @brief      Timer interrupt that sets rate for controller and indicates when to read sensors again
 */

#pragma CODE_SECTION(TIMER0_B0_ISR, ".TI.ramfunc")
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR (void){

    g_motor.read_sensors=true; // read sensors (hall/encoders & current) again every time this interrupts fires

    // without conditional can get huge and negative
   if(g_controlPrescaler>0)
       g_controlPrescaler = g_controlPrescaler -1;

  return;

}

