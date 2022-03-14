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

// TODO: re-organize all these variable defs
_iq g_currentPhaseA;
_iq g_currentPhaseB;
_iq g_currentPhaseC;
_iq g_feedforwardFW;

// Used for calibration
_iq g_currentOffsetPhaseA;
_iq g_currentOffsetPhaseB;
_iq g_currentOffsetPhaseC;
_iq g_openLoopTorque;

_iq g_closeLoopThreshold;

uint8_t g_commState;
uint8_t g_oldCommState;
HallSensor g_hallSensor;
uint8_t g_hallMap[8];


uint16_t g_controlPrescaler;

volatile PI_CONTROLLER g_piSpd;
volatile PI_CONTROLLER g_piCur;
IMPULSE_TIMER impulse_timer;
uint8_t comm_cycle_counter;


volatile uint8_t g_maxSpeed;

volatile MOTOR_STATE g_motor_state;

volatile uint16_t g_accelRate;
volatile uint16_t g_decelRate;

uint8_t g_statusRegister;
volatile uint8_t g_controlRegister;
volatile uint8_t g_faultRegister;
volatile uint32_t g_drivingTimeoutCtr;
uint8_t g_errorCounter= 0; // incremented every time inner control loop is reached and motor is acting strange
                           // if it exceeds ERROR_ITERATION_THRESHOLD then motor is stopped

bool g_readSensors = false;

/**
 * @brief      Gets the speed.
 *
 * @return     The speed.
 */
_iq getSpeed(volatile MOTOR_STATE* motor_state){
  // Normalize speed to -128 ticks < diff < 127 to -1.0 < diff < +1.0
  // 255 ticks per (PI_SPD_CONTROL_PRESCALER * PWM_PERIOD_TICKS) represents 9.600 eRPM
  // which equates to ~25.22 cm/sec, much faster than we should be going in normal operation
  int32_t deltaPos = _IQsat(motor_state->current_position - motor_state->last_position, 256, -256);
  // shift left 7 to scale 9 bit deltaPos (-255,255) to 15 decimal fixed point _IQ (-1.0,1.0)
  motor_state->current_speed = deltaPos << 7;
  motor_state->last_position = motor_state->current_position;
  return motor_state->current_speed;
}

/**
 * @brief      Initializes the adc module.
 */
inline void initializeAdcModule(void){
  // Configure ADC for motor current sensing
  ADC12_B_initParam adcParam;
  adcParam.clockSourceDivider = ADC12_B_CLOCKDIVIDER_1;
  adcParam.clockSourcePredivider = ADC12_B_CLOCKPREDIVIDER__1;
  adcParam.clockSourceSelect = ADC12_B_CLOCKSOURCE_ADC12OSC;
  adcParam.internalChannelMap = ADC12_B_NOINTCH;
  adcParam.sampleHoldSignalSourceSelect = ADC12_B_SAMPLEHOLDSOURCE_3; // Synchronize to TB0 CCR0
  ADC12_B_init(ADC12_B_BASE, &adcParam);

  ADC12_B_enable(ADC12_B_BASE);

  ADC12_B_setupSamplingTimer(ADC12_B_BASE,
                             ADC12_B_CYCLEHOLD_16_CYCLES,
                             ADC12_B_CYCLEHOLD_4_CYCLES,
                             ADC12_B_MULTIPLESAMPLESENABLE);

  ADC12_B_setResolution(ADC12_B_BASE, ADC12_B_RESOLUTION_12BIT);

  ADC12_B_configureMemoryParam configureMemoryParam = {0};
  // Configure ADC channel for phase A current measurement
  configureMemoryParam.memoryBufferControlIndex = ADC12_B_MEMORY_0;
  configureMemoryParam.inputSourceSelect = IPHASE_A_ADC_CHAN;
  configureMemoryParam.refVoltageSourceSelect = ADC12_B_VREFPOS_AVCC_VREFNEG_VSS;
  configureMemoryParam.endOfSequence = ADC12_B_NOTENDOFSEQUENCE;
  configureMemoryParam.windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE;
  configureMemoryParam.differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE;
  ADC12_B_configureMemory(ADC12_B_BASE, &configureMemoryParam);

  // Configure ADC channel for phase B current measurement
  configureMemoryParam.memoryBufferControlIndex = ADC12_B_MEMORY_1;
  configureMemoryParam.inputSourceSelect = IPHASE_B_ADC_CHAN;
  ADC12_B_configureMemory(ADC12_B_BASE, &configureMemoryParam);

  // Configure ADC channel for phase C current measurement
  configureMemoryParam.memoryBufferControlIndex = ADC12_B_MEMORY_2;
  configureMemoryParam.inputSourceSelect = IPHASE_C_ADC_CHAN;
  configureMemoryParam.endOfSequence = ADC12_B_ENDOFSEQUENCE;
  ADC12_B_configureMemory(ADC12_B_BASE, &configureMemoryParam);

  ADC12_B_startConversion(ADC12_B_BASE,
                          ADC12_B_START_AT_ADC12MEM0,
                          ADC12_B_SEQOFCHANNELS);
}


/**
 * @brief      Do the offset calibration of the current sensor
 */
inline void currentOffsetCalibration(void){

  enableCalibration(); // pull p4.4 high (calibration pin on DRV8304 motor driver)
  enableGateDriver();  // let current flow through motor to get accurate offset measurement

  // Prepare ADC conversion for next round
  HWREG8(ADC12_B_BASE + OFS_ADC12CTL0_L) &= ~(ADC12ENC);
  HWREG8(ADC12_B_BASE + OFS_ADC12CTL0_L) |= ADC12ENC + ADC12SC;

  // read from ADC channels for 3 motor channels, save readings as offsets
  g_currentOffsetPhaseA = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_0));
  g_currentOffsetPhaseB = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_1));
  g_currentOffsetPhaseC = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_2));

  disableCalibration(); // pull p4.4 low
  disableGateDriver(); // turn off motor driver for time being
}


/**
 * @brief      Generate the pwm period for the 6 pwm channels
 *
 * @param[in]  commutation  The commutation
 * @param[in]  dutyCycle    The duty cycle
 */
inline void pwmGenerator(const uint8_t commutation, _iq dutyCycle){
  uint16_t dc = 0; //duty cycle
  uint16_t dcCmpl = 0; //complement

  // Normalize duty cycle -1.0 < dc < +1.0 to 0 < dc < 512
  dc = (uint16_t)(dutyCycle >> 7) + PWM_HALF_PERIOD_TICKS;
  dcCmpl = PWM_PERIOD_TICKS - dc;

  _iq PWM_A = 0;
  _iq PWM_B = 0;
  _iq PWM_C = 0;

  if(dutyCycle != 0){
      switch(commutation){
          case 0:
            PWM_A = dc;
            PWM_B = dcCmpl;
            break;
          case 1:
            PWM_A = dc;
            PWM_C = dcCmpl;
            break;
          case 2:
            PWM_B = dc;
            PWM_C = dcCmpl;
            break;
          case 3:
            PWM_A = dcCmpl;
            PWM_B = dc;
            break;
          case 4:
            PWM_A = dcCmpl;
            PWM_C = dc;
            break;
          case 5:
            PWM_B = dcCmpl;
            PWM_C = dc;
            break;
          default:
            break;
        }
  }


  setPwmAPeriod(PWM_A);
  enableHalfBridgeA();
  setPwmBPeriod(PWM_B);
  enableHalfBridgeB();
  setPwmCPeriod(PWM_C);
  enableHalfBridgeC();

  // turn off half bridge that isn't being used (PWM period of 0)
  if(PWM_A == 0){
      disableHalfBridgeA();
  } else if (PWM_B == 0){ // can do else-if because only 1 will be 0
      disableHalfBridgeB();
  } else if (PWM_C == 0){
      disableHalfBridgeC();
  }
}


/**
 * @brief      Reads a hall sensor.
 */
inline void readHallSensor(void){
  g_hallSensor.Pattern = READ_HALL_W >> 1;   // W
  g_hallSensor.Pattern |= READ_HALL_V >> 4;  // V
  g_hallSensor.Pattern |= READ_HALL_U >> 6;  // U
  g_hallSensor.Event = g_hallSensor.Pattern ^ g_hallSensor.OldPattern;
  g_hallSensor.OldPattern = g_hallSensor.Pattern;
  if(g_hallSensor.Pattern & 0x07){
      g_hallSensor.Error = 1;
  }
  if(g_hallSensor.Pattern == 0x00){
      g_hallSensor.Error = 1;
  }
}


/**
 * @brief      Initializes the hall interface.
 */
void initializeHallInterface(void){
  g_hallMap[0] = 0xff;
  g_hallMap[7] = 0xff;

  g_hallMap[5] = 0;
  g_hallMap[1] = 1;
  g_hallMap[3] = 2;
  g_hallMap[2] = 3;
  g_hallMap[6] = 4;
  g_hallMap[4] = 5;
}


/*
 * @brief       TODO
 */
void initializeSensorVariables(volatile MOTOR_STATE* motor_state){
    g_currentOffsetPhaseA = 0;
    g_currentOffsetPhaseB = 0;
    g_currentOffsetPhaseC = 0;
    g_commState = 0;
    g_hallSensor.Pattern = 0;
    g_hallSensor.OldPattern = 0;
    motor_state->current_position = 0;
    motor_state->last_position = motor_state->current_position;
    motor_state->target_position = 0;
    g_drivingTimeoutCtr = 0;
}

/*
 * @brief       TODO
 */
void initializeSoftwareControlVariables(volatile MOTOR_STATE* motor_state){
    // software control related variables (rate groups, internal state machine)
    g_controlPrescaler = PI_SPD_CONTROL_PRESCALER;
    motor_state->closed_loop = false;
    motor_state->state_machine.state = RUNNING;
    motor_state->state_machine.command = NO_CMD;
    g_controlRegister = 0; // see main.h for bits
}

/*
 * @breif   initialize motor controller related variables (PI controllers for speed and current)
 */
void initializeControllerVariables(volatile MOTOR_STATE* motor_state){
    g_maxSpeed = MAX_TARGET_SPEED;

    g_openLoopTorque = _IQ(OPEN_LOOP_TORQUE);
    impulse_timer.period = PERIOD_IMPULSE;
    comm_cycle_counter = 0;

    resetPiController(&g_piSpd);
    resetPiController(&g_piCur);

    g_piSpd.Kp = _IQ(KP_SPD);
    g_piSpd.Ki = _IQ(KI_SPD);
    g_piCur.Kp = _IQ(KP_CUR);
    g_piCur.Ki = _IQ(KI_CUR);

    g_closeLoopThreshold = _IQ(CLOSE_LOOP_THRESHOLD);

    motor_state->target_direction = 1;
    motor_state->closed_loop = false;
}

/*
 * @brief   Do everything necessary to init MSP and begin driving
 */
void initController(volatile MOTOR_STATE* motor_state){
    initializeGpios();

    //Set DCO frequency to 16MHz
    CS_setDCOFreq(CS_DCORSEL_1, CS_DCOFSEL_4);

    CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);

    // Initialize variables
    initializeSensorVariables(motor_state);
    initializeSoftwareControlVariables(motor_state);
    initializeControllerVariables(motor_state);

    // initialize hardware components
    initializeI2cModule();
    initializePwmModules();
    initializeAdcModule();
    initializeHallInterface();

    currentOffsetCalibration(); // get initial estimate of current offsets
    __bis_SR_register(GIE); // enable interrupts (timer & i2c)
    enableGateDriver(); // get ready to move
}

/*
 * @brief  update sensor (Hall & current) readings for when driving in closed loop
 */
void readSensors(volatile MOTOR_STATE* motor_state){

    // measure hall sensors
    readHallSensor();

    if(motor_state->closed_loop == false && motor_state->target_reached == false){
        // Execute macro to generate ramp up
        iterate_impulse_timer(&impulse_timer, /*driving_open_loop=*/false);
        if(impulse_timer.cycle){
            comm_cycle_counter = iterate_mod6_counter(comm_cycle_counter, /*driving_open_loop=*/false);

            if (motor_state->target_direction > 0){
                g_commState = comm_cycle_counter;
            } else {
                g_commState = 5 - comm_cycle_counter;
            }
        }
    }
    else{
        // allow commutation to be controlled by hall sensor readings (will time them way better than we could)
        g_commState = g_hallMap[g_hallSensor.Pattern];
    }

    // update current position based on hall sensor readings
    if(g_hallSensor.Event){
        if(g_hallMap[g_hallSensor.Pattern] == 5 && g_oldCommState == 0){
            motor_state->current_position--;
        }
        else if(g_hallMap[g_hallSensor.Pattern] == 0 && g_oldCommState == 5){
            motor_state->current_position++;
        }
        else if(g_hallMap[g_hallSensor.Pattern] > g_oldCommState){
            motor_state->current_position++;
        }
        else{
            motor_state->current_position--;
        }
        g_oldCommState = g_hallMap[g_hallSensor.Pattern];
    }

    // update current readings
    // Prepare ADC conversion for next round
    HWREG8(ADC12_B_BASE + OFS_ADC12CTL0_L) &= ~(ADC12ENC);
    HWREG8(ADC12_B_BASE + OFS_ADC12CTL0_L) |= ADC12ENC + ADC12SC;
    // Remove offset
    g_currentPhaseA = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_0)) - g_currentOffsetPhaseA;
    g_currentPhaseB = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_1)) - g_currentOffsetPhaseB;
    g_currentPhaseC = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_2)) - g_currentOffsetPhaseC;
}

/*
 * @brief       TODO
 */
void moderatePIControllers(volatile MOTOR_STATE* motor_state){
    // apply constant torque if still in open loop
    if(motor_state->closed_loop == false && motor_state->target_reached == false){
      g_piCur.i1 = 0;
      g_piCur.ui = 0;
      g_piSpd.i1 = 0;
      g_piSpd.ui = 0;
      g_piCur.Out = g_openLoopTorque;
      if(_IQabs(motor_state->current_speed) > g_closeLoopThreshold){
          motor_state->closed_loop = true;
      }
    }
    // set integrator and output to 0 if target is reached
    if(motor_state->target_reached == true){
        g_piCur.i1 = 0;
        g_piCur.ui = 0;
        g_piSpd.i1 = 0;
        g_piSpd.ui = 0;
        g_piCur.Out = 0;
        motor_state->closed_loop = false;
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
        g_piCur.i1 = _IQ(motor_state->target_direction*0.5); // full wipe of integrator causes jumpy stop-start behavior
        g_piCur.ui = 0;
        g_piCur.v1 = 0;
        __enable_interrupt();
    }
}

/*
 * @brief      TODO
 */
void checkTargetReached(volatile MOTOR_STATE* motor_state){
    if  (_IQabs(motor_state->target_position - motor_state->current_position) < POSITION_CONVERGENCE_THRESHOLD) {
          // target has been reached
          motor_state->target_reached = true;
          g_statusRegister |= POSITION_CONVERGED;
          // turn off output
          _iq output = 0;
          pwmGenerator(g_commState, output);
      } else {
          // target not reached yet
          motor_state->target_reached = false;
          g_statusRegister &= ~POSITION_CONVERGED;
      }
}

/*
 * @brief   TODO
 */
void driveOpenLoop(volatile MOTOR_STATE* motor_state){
    if(!motor_state->target_reached){
        // Iterate through commutations & apply impulse to desired motor windings
        iterate_impulse_timer(&impulse_timer, /*driving_open_loop=*/true);

        if(impulse_timer.cycle){
            comm_cycle_counter = iterate_mod6_counter(comm_cycle_counter, /*driving_open_loop=*/true);

            if (motor_state->target_direction > 0){
                g_commState = comm_cycle_counter;
            } else {
                g_commState = 5 - comm_cycle_counter;
            }
        }

        _iq output;
        if(g_controlRegister & OPEN_LOOP_TORQUE_OVERRIDE){
            output = _IQ(g_maxSpeed / MAX_TARGET_SPEED); // apply user specified output
        }
        else{
            output = _IQ(FULLY_OPEN_LOOP_PWM); // apply constant output (30% duty cycle)
        }

        // apply output as PWM
        if(motor_state->target_direction > 0) {
            pwmGenerator(g_commState, output);
        } else{
            pwmGenerator(g_commState, -output);
        }

    }

    // control loop - updates current position with speed estimate & ticks timeout counter
    if (g_controlPrescaler<=0){
        g_controlPrescaler = PI_SPD_CONTROL_PRESCALER;
        motor_state->current_position += motor_state->target_direction * OPEN_LOOP_SPEED;

        if(!motor_state->target_reached){
            g_drivingTimeoutCtr++;
        }
    }
}

/*
 * @brief       TODO
 */
void closedLoopCurrentLoop(volatile MOTOR_STATE* motor_state){
    // inner control loop (current), apply output as PWM duty cycle
    // Normalize current values from  -2047 < adc < +2048 to iq15 --> -1.0 < adc < 1.0 and convert to iq format
    g_piCur.Fbk = (g_currentPhaseA + g_currentPhaseB + g_currentPhaseC) << 4;
    g_piCur.Ref = g_piSpd.Out;

    pi_iteration(&g_piCur);

    // check if we can pivot to closed loop control
    // todo: fix if statement
    if(_IQabs(motor_state->current_speed) > g_closeLoopThreshold && !motor_state->target_reached){
        motor_state->closed_loop = true;
    } else {
        motor_state->closed_loop = false;
    }

    moderatePIControllers(motor_state); // clamp and/or zero values as necessary

    // apply duty cycle based on current PI controller's output
    pwmGenerator(g_commState, g_piCur.Out);
}

/*
 * @brief       TODO
 */
void closedLoopSpeedLoop(volatile MOTOR_STATE* motor_state){
    if(motor_state->target_direction > 0){
          g_piSpd.Ref = g_maxSpeed << 8;
      }
      else {
          g_piSpd.Ref = -g_maxSpeed << 8;
      }

    g_piSpd.Fbk = getSpeed(motor_state);
    pi_iteration(&g_piSpd);
}

/*
 * @brief       TODO
 */
void checkForClosedLoopErrors(volatile MOTOR_STATE* motor_state){
    // check for errors in controller operation
    if (motor_state->current_position == motor_state->last_position && !motor_state->target_reached){
      // position isn't updating; hall sensors likely not powered or broken
      g_errorCounter++;
      g_faultRegister |= POSITION_NO_CHANGE;
    } else if ( (motor_state->current_position - motor_state->last_position)*motor_state->target_direction < 0 && !motor_state->target_reached){
      // moving in wrong direction
      g_errorCounter++;
      g_faultRegister |= DRIVING_WRONG_DIRECTION;
    } else {
      // operating normally; no error
      g_statusRegister &= ~CONTROLLER_ERROR;
      g_errorCounter = 0; // reset error counter
      g_faultRegister &= ~(POSITION_NO_CHANGE & DRIVING_WRONG_DIRECTION); //clear faults in register
    }

    // errors on last ERROR_ITERATION_THRESHOLD time steps; time to stop trying to drive motor
    if(g_errorCounter >= ERROR_ITERATION_THRESHOLD){
      if (g_controlRegister & OVERRIDE_FAULT_DETECTION == 0x00) //check if we should stop controller given fault
          motor_state->target_position = motor_state->current_position = 0; //stop controller
      g_statusRegister |= CONTROLLER_ERROR; // add flag to status register
    }
}

/*
 *  @brief      TODO
 */
void handleMotorTimeout(volatile MOTOR_STATE* motor_state){
    motor_state->target_reached = true;
    motor_state->target_position = motor_state->current_position; // so motor won't flip g_targetReached again
    g_faultRegister |= DRIVING_TIMEOUT;
    g_statusRegister |= (POSITION_CONVERGED | CONTROLLER_ERROR);
    g_drivingTimeoutCtr = 0;
}


/**
 * @brief      main function
 */
void main(void){

  // Turn off the watchdog
  WDT_A_hold(WDT_A_BASE);

  initController(&g_motor_state); //init all variables and functionality needed to drive
//  g_targetPosition = 10000;
//  g_controlRegister = 33;

  while(1){
      checkTargetReached(&g_motor_state);

      // target position sets direction motor drives in
      if(g_motor_state.target_position - g_motor_state.current_position >= 0){
          g_motor_state.target_direction = 1;
      } else {
          g_motor_state.target_direction = -1;
      }

      // check if driving in open or closed loop (& have been told to execute command), act accordingly
      if (g_controlRegister & DRIVE_OPEN_LOOP && g_controlRegister & EXECUTE_COMMAND) {
          // driving in fully open loop
          driveOpenLoop(&g_motor_state);

      } else if (g_controlRegister & EXECUTE_COMMAND){
        // driving closed loop

        if(g_readSensors){ // update sensor readings if requested by timer interrupt
            readSensors(&g_motor_state);
            g_readSensors = false;
        }

        closedLoopCurrentLoop(&g_motor_state); // inner loop of controller

        if(g_controlPrescaler <= 0){

          closedLoopSpeedLoop(&g_motor_state); // outer loop of controller
          checkForClosedLoopErrors(&g_motor_state);

          // reset control prescaler & tick timeout counter
          g_controlPrescaler = PI_SPD_CONTROL_PRESCALER;
          if(!g_motor_state.target_reached){
              g_drivingTimeoutCtr++;
          }
        }
      }

      // check if motor has taken too long to converge, act accordingly if so
      if(g_drivingTimeoutCtr > DRIVING_TIMEOUT_THRESHOLD){
          handleMotorTimeout(&g_motor_state);
      }
  } // end of while loop
}


/**
 * @brief      Timer interrupt that sets rate for controller and indicates when to read sensors again
 */

#pragma CODE_SECTION(TIMER0_B0_ISR, ".TI.ramfunc")
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR (void){

    g_readSensors=true; // read sensors (hall/encoders & current) again every time this interrupts fires

    // without conditional can get huge and negative
   if(g_controlPrescaler>0)
       g_controlPrescaler = g_controlPrescaler -1;

  return;

}

