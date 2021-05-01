// [DEBUG] Switches
//#define IRIS_ALL_OFF
//#define IRIS_CLEAR_FAULT
//#define IRIS_SPIN_MOTOR
//#define IRIS_SPIN_MOTOR_REVERSE
//#define IRIS_SPIN_MOTOR_INDEF

#include "main.h"

_iq g_currentPhaseA;
_iq g_currentPhaseB;
_iq g_currentPhaseC;
_iq g_feedforwardFW;

// Used for calibration
volatile bool g_calibrating;
volatile bool g_calibrationDone;
volatile _iq g_currentOffsetPhaseA;
volatile _iq g_currentOffsetPhaseB;
volatile _iq g_currentOffsetPhaseC;
volatile _iq g_currentSpeed;
volatile _iq g_openLoopTorque;

volatile _iq g_currentRefTest;
volatile _iq g_speedRefTest;

volatile _iq g_closeLoopThreshold;

volatile uint8_t g_commState;
volatile uint8_t g_oldCommState;
HallSensor g_hallSensor;

volatile int32_t g_currentPosition;
volatile int32_t g_oldPosition;
volatile int32_t g_targetPosition;

volatile uint16_t g_controlPrescaler;
uint8_t g_hallMap[8];

volatile PI_CONTROLLER g_piSpd;
volatile PI_CONTROLLER g_piCur;
volatile IMPULSE g_impulse;
volatile MOD6CNT g_mod6cnt;

volatile bool g_closedLoop;
volatile bool g_targetReached;
volatile uint8_t g_maxSpeed;

volatile StateMachine g_state;
volatile CmdState g_cmdState;

volatile uint16_t g_accelRate;
volatile uint16_t g_decelRate;

volatile int8_t g_targetDirection;
volatile int8_t g_oldTargetDirection;

uint8_t g_statusRegister;
volatile uint8_t g_controlRegister;
volatile uint8_t g_faultRegister;
volatile uint32_t g_drivingTimeoutCtr;
uint8_t g_errorCounter= 0; // incremented every time inner control loop is reached and motor is acting strange
                           // if it exceeds ERROR_ITERATION_THRESHOLD then motor is stopped

// backup plan for if we cannot get encoder readings
volatile bool driveOpenLoop = false;

int g_testeroni = 0;
volatile uint8_t g_rxBuffer_tmp[I2C_RX_BUFFER_MAX_SIZE];

// timing debug
bool g_readSensors = false;


/**
 * @brief      Initializes the pwm modules.
 */
void initializePwmModules(){
  //Start Timer
  Timer_B_initUpDownModeParam initUpDownParam = {0};
  initUpDownParam.clockSource = TIMER_B_CLOCKSOURCE_SMCLK; // 16 MHz, or maybe 8..
  initUpDownParam.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
  initUpDownParam.timerPeriod = PWM_PERIOD_TICKS;
  initUpDownParam.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
  initUpDownParam.captureCompareInterruptEnable_CCR0_CCIE = TIMER_B_CCIE_CCR0_INTERRUPT_ENABLE;
  initUpDownParam.timerClear = TIMER_B_DO_CLEAR;
  initUpDownParam.startTimer = true;
  Timer_B_initUpDownMode(TIMER_B0_BASE, &initUpDownParam);

  Timer_B_clearCaptureCompareInterrupt(TIMER_B0_BASE,
                                       TIMER_B_CAPTURECOMPARE_REGISTER_0);

  //Initialize compare registers to generate PWMA_H
  Timer_B_initCompareModeParam initComp1Param = {0};
  initComp1Param.compareRegister = PWMA_H_CCR_REGISTER;
  initComp1Param.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
  initComp1Param.compareOutputMode = TIMER_B_OUTPUTMODE_TOGGLE_SET;
  initComp1Param.compareValue = 0; // 0% duty cycle
  Timer_B_initCompareMode(TIMER_B0_BASE, &initComp1Param);

  //Initialize compare registers to generate PWMB_H
  Timer_B_initCompareModeParam initComp2Param = {0};
  initComp2Param.compareRegister = PWMB_H_CCR_REGISTER;
  initComp2Param.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
  initComp2Param.compareOutputMode = TIMER_B_OUTPUTMODE_TOGGLE_SET;
  initComp2Param.compareValue = 0; // 0% duty cycle
  Timer_B_initCompareMode(TIMER_B0_BASE, &initComp2Param);

  //Initialize compare registers to generate PWMC_H
  Timer_B_initCompareModeParam initComp3Param = {0};
  initComp3Param.compareRegister = PWMC_H_CCR_REGISTER;
  initComp3Param.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
  initComp3Param.compareOutputMode = TIMER_B_OUTPUTMODE_TOGGLE_SET;
  initComp3Param.compareValue = 0; // 0% duty cycle
  Timer_B_initCompareMode(TIMER_B0_BASE, &initComp3Param);

  // Initialize CCR1 for ADC acquisition
  Timer_B_initCompareModeParam initComp4Param = {0};
  initComp4Param.compareRegister = ADC_CCR_REGISTER;
  initComp4Param.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
  initComp4Param.compareOutputMode = TIMER_B_OUTPUTMODE_TOGGLE_RESET;
  initComp4Param.compareValue = PWM_PERIOD_TICKS - 1;
  Timer_B_initCompareMode(TIMER_B0_BASE, &initComp4Param);
}


/**
 * @brief      Sets the pwm a period.
 *
 * @param[in]  period  The period
 */
inline void setPwmAPeriod(uint16_t period){
  HWREG16(TIMER_B0_BASE + PWMA_H_CCR_REGISTER + OFS_TBxR) = PWM_PERIOD_TICKS - period;
}


/**
 * @brief      Sets the pwm b period.
 *
 * @param[in]  period  The period
 */
inline void setPwmBPeriod(uint16_t period){
  HWREG16(TIMER_B0_BASE + PWMB_H_CCR_REGISTER + OFS_TBxR) = PWM_PERIOD_TICKS - period;
}

inline void setPwmCPeriod(uint16_t period){
  HWREG16(TIMER_B0_BASE + PWMC_H_CCR_REGISTER + OFS_TBxR) = PWM_PERIOD_TICKS - period;
}


/**
 * @brief      Enables the half bridge a.
 */
inline void enableHalfBridgeA() {
  P3OUT |= GPIO_PIN6;
}


/**
 * @brief      Disables the half bridge a.
 */
inline void disableHalfBridgeA() {
  P3OUT &= ~GPIO_PIN6;
}


/**
 * @brief      Enables the half bridge b.
 */
inline void enableHalfBridgeB() {
  P3OUT |= GPIO_PIN4;
}


/**
 * @brief      Disables the half bridge b.
 */
inline void disableHalfBridgeB() {
  P3OUT &= ~GPIO_PIN4;
}


/**
 * @brief      Enables the half bridge c.
 */
inline void enableHalfBridgeC() {
  P2OUT |= GPIO_PIN1;
}


/**
 * @brief      Disables the half bridge c.
 */
inline void disableHalfBridgeC() {
  P2OUT &= ~GPIO_PIN1;
}


/**
 * @brief      Disables the calibration.
 */
inline void disableCalibration() {
  GPIO_setOutputLowOnPin(GPIO_PORT_P4,
                         GPIO_PIN4);
}


/**
 * @brief      Enables the calibration.
 */
inline void enableCalibration() {
  GPIO_setOutputHighOnPin(GPIO_PORT_P4,
                          GPIO_PIN4);
}


/**
 * @brief      Enables the gate driver.
 */
inline void enableGateDriver(){
  // enter critical part of the code do not interrupt it
  GPIO_setOutputHighOnPin(GPIO_PORT_PJ, GPIO_PIN0);
  __delay_cycles(1600000);    // 100 ms
}


/**
 * @brief      Disables the gate driver.
 */
inline void disableGateDriver(){
  // enter critical part of the code do not interrupt it
  GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_PIN0);
  __delay_cycles(1600000);    // 100 ms
}


/**
 * @brief      Gets the speed.
 *
 * @return     The speed.
 */
inline _iq getSpeed(){
  // Normalize speed to -128 ticks < diff < 127 to -1.0 < diff < +1.0
  // 255 ticks per (PI_SPD_CONTROL_PRESCALER * PWM_PERIOD_TICKS) represents 9.600 eRPM
  int32_t deltaPos = _IQsat(g_currentPosition - g_oldPosition, 256, -256);
  g_currentSpeed = deltaPos << 7;
  g_oldPosition = g_currentPosition;
  return g_currentSpeed;
}

/**
 * @brief      Initializes the adc module.
 */
void initializeAdcModule(){
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
void currentOffsetCalibration(){
  // Enable calibration
  enableCalibration();

  enableGateDriver();

  g_calibrationDone = false;
  g_calibrating = true;

  while(!g_calibrationDone); // wait here until calibration completed

  g_calibrating = false;

  disableCalibration();

  disableGateDriver();
}


/**
 * @brief      Generate the pwm period for the 6 pwm channels
 *
 * @param[in]  commutation  The commutation
 * @param[in]  dutyCycle    The duty cycle
 */
void pwmGenerator(const uint8_t commutation, _iq dutyCycle){
  uint16_t dc; //duty cycle
  uint16_t dcCmpl; //complement

  // Normalize duty cycle -1.0 < dc < +1.0 to 0 < dc < 512
  dc = (uint16_t)(dutyCycle >> 7) + PWM_HALF_PERIOD_TICKS;
  dcCmpl = PWM_PERIOD_TICKS - dc;

  switch(commutation){
    case 0:
      setPwmAPeriod(dc);
      enableHalfBridgeA();
      setPwmBPeriod(dcCmpl);
      enableHalfBridgeB();
      setPwmCPeriod(0);
      disableHalfBridgeC();
      break;
    case 1:
      setPwmAPeriod(dc);
      enableHalfBridgeA();
      setPwmBPeriod(0);
      disableHalfBridgeB();
      setPwmCPeriod(dcCmpl);
      enableHalfBridgeC();
      break;
    case 2:
      setPwmAPeriod(0);
      disableHalfBridgeA();
      setPwmBPeriod(dc);
      enableHalfBridgeB();
      setPwmCPeriod(dcCmpl);
      enableHalfBridgeC();
      break;
    case 3:
      setPwmAPeriod(dcCmpl);
      enableHalfBridgeA();
      setPwmBPeriod(dc);
      enableHalfBridgeB();
      setPwmCPeriod(0);
      disableHalfBridgeC();
      break;
    case 4:
      setPwmAPeriod(dcCmpl);
      enableHalfBridgeA();
      setPwmBPeriod(0);
      disableHalfBridgeB();
      setPwmCPeriod(dc);
      enableHalfBridgeC();
      break;
    case 5:
      setPwmAPeriod(0);
      disableHalfBridgeA();
      setPwmBPeriod(dcCmpl);
      enableHalfBridgeB();
      setPwmCPeriod(dc);
      enableHalfBridgeC();
      break;
    default:
      break;
  }
}


/**
 * @brief      Reads a hall sensor.
 */
inline void readHallSensor(){
  g_hallSensor.Pattern = READ_HALL_W >> 1;   // W
  g_hallSensor.Pattern |= READ_HALL_V >> 4;  // V
  g_hallSensor.Pattern |= READ_HALL_U >> 6;  // U
  g_hallSensor.Event = g_hallSensor.Pattern ^ g_hallSensor.OldPattern;
  g_hallSensor.OldPattern = g_hallSensor.Pattern;
  if(g_hallSensor.Pattern & 0x07) g_hallSensor.Error = 1;
  if(g_hallSensor.Pattern == 0x00) g_hallSensor.Error = 1;
}


/**
 * @brief      Initializes the hall interface.
 */
void initializeHallInterface(){
  g_hallMap[0] = 0xff;
  g_hallMap[7] = 0xff;

  g_hallMap[5] = 0;
  g_hallMap[1] = 1;
  g_hallMap[3] = 2;
  g_hallMap[2] = 3;
  g_hallMap[6] = 4;
  g_hallMap[4] = 5;
}


/**
 * @brief      Reset the PI controller
 *
 * @param      pi    { parameter_description }
 */
void resetPiController(volatile PI_CONTROLLER *pi){
  pi->i1 = _IQ(0.0);
  pi->ui = _IQ(0.0);
  pi->v1 = _IQ(0.0);
  pi->up = _IQ(0.0);
  pi->Umax = _IQ(1.0);
  pi->Umin = _IQ(-1.0);
}


/**
 * @brief      Do a multiplication using the hardware multiplier of the MSP430.
 *             This works only for IQ15 fixed point format.
 *
 * @param[in]  iq31Arg1  The iq 31 argument 1
 * @param[in]  iq31Arg2  The iq 31 argument 2
 *
 * @return     { description_of_the_return_value }
 */
inline _iq _IQ15mpy_inline(_iq iq31Arg1, _iq iq31Arg2){
  uint32_t ui31Result;
  uint16_t *ptr = (uint16_t *)&ui31Result + 1;
  uint16_t *ptrArg1 = (uint16_t *)&iq31Arg1 + 1;
  uint16_t *ptrArg2 = (uint16_t *)&iq31Arg2 + 1;

  /* Set the multiplier to fractional mode. */
  MPY32CTL0 = MPYFRAC;
  /* Perform multiplication and save result. */
  MPYS32L = iq31Arg1;
  MPYS32H = *ptrArg1;
  OP2L = iq31Arg2;
  OP2H = *ptrArg2;
  asm(" NOP");
  asm(" NOP");
  asm(" NOP");
  asm(" NOP");
  asm(" NOP");
  ui31Result = RES1;
  *ptr = RES2;

  return (_iq)ui31Result;
}


/**
 * @brief      Disable the drive
 */
inline void disable(){
  if(g_state == IDLE) return; // already in IDLE, nothing to do

  __disable_interrupt();
  disableGateDriver();
  g_state = IDLE;
  g_targetPosition = 0;
  g_currentPosition = 0;
  __enable_interrupt();
}


/**
 * @brief      Enter run state
 */
inline void run(){
  if(g_state == RUNNING) return; // already in RUNNING, nothing to do
  __disable_interrupt();
  enableGateDriver();
//  g_targetPosition = 20000; // [DEBUG]
  g_targetDirection = (g_targetPosition -  g_currentPosition>= 0) ? 1 : -1;
  g_currentPosition = 0;
  g_targetReached = false;
  g_state = RUNNING;
  __enable_interrupt();
}


/**
 * @brief      Update the drive state machine
 */
void updateStateMachine(){
  switch(g_cmdState){
    case RUN:
      switch(g_state){
          case IDLE:
            run();
            break;
          case UNINITIALIZED:
          case RUNNING:
          default:
          break;
      }
     break;
     case DISABLE:
       switch(g_state){
         case RUNNING:
           disable();
           break;
         case UNINITIALIZED:
         case IDLE:
         default:
         break;
       }
       break;
     default:
         break;
  }

  g_cmdState = NO_CMD;
}

/**
 * @brief      Clears the DRV8304 Driver Fault Register.
 */
inline void clear_driver_fault_register(){
    // Pull high first so you can then pull it low:
    GPIO_setOutputHighOnPin(GPIO_PORT_PJ, GPIO_PIN0);
    __delay_cycles(1600000);    // 100 ms
    // Reset Fault Register by pulsing ENABLE for 5-32us (18.5us):
    GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_PIN0);
    __delay_cycles(296);    // 18.5 us
    GPIO_setOutputHighOnPin(GPIO_PORT_PJ, GPIO_PIN0);
}

inline void clear_driver_fault(){
    // Pull high first so you can then pull it low:
    GPIO_setOutputHighOnPin(GPIO_PORT_PJ, GPIO_PIN0);
    __delay_cycles(1600000);    // 100 ms
    // Reset Fault Register by pulsing ENABLE for 5-32us (18.5us):
    GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_PIN0);
    __delay_cycles(296);    // 18.5 us
    GPIO_setOutputHighOnPin(GPIO_PORT_PJ, GPIO_PIN0);
}
/**
 * @brief      Reads the whether the DRV8304 driver is in a "fault condition" (and should be cleared). Active low.
 */
inline bool read_driver_fault(){
    return !(PJIN & 0x02);
}


/**
 * @brief      main function
 */
void main(void){

  // Turn off the watchdog
  WDT_A_hold(WDT_A_BASE);

  initializeGpios();

  // [DEBUG]
//  g_rxBuffer_tmp[0] = 75;

#ifdef IRIS_CLEAR_FAULT
  clear_driver_fault_register();
#endif

#ifndef IRIS_ALL_OFF

  //Set DCO frequency to 16MHz
  CS_setDCOFreq(CS_DCORSEL_1, CS_DCOFSEL_4);

  CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);
  CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);

  // Initialize global variables
  g_currentOffsetPhaseA = 0;
  g_currentOffsetPhaseB = 0;
  g_currentOffsetPhaseC = 0;
  g_commState = 0;
  g_hallSensor.Pattern = 0;
  g_hallSensor.OldPattern = 0;
  g_currentPosition = 0;
  g_oldPosition = g_currentPosition;
  g_targetPosition = 0;
  g_drivingTimeoutCtr = 0;
#ifdef IRIS_SPIN_MOTOR
  g_targetPosition = 8500;//9750; // [DEBUG]
#endif
#ifdef IRIS_SPIN_MOTOR_REVERSE
  g_targetPosition = -9750;
#endif
#ifdef IRIS_SPIN_MOTOR_INDEF
  g_targetPosition = 1000000; //approx 100 revolutions
#endif
  g_controlPrescaler = PI_SPD_CONTROL_PRESCALER;
  g_closedLoop = false;
  g_state = RUNNING; // g_statusRegister bit 3 is 0 to reflect this state
  g_cmdState = NO_CMD;

//  copyArray((uint8_t*)g_rxBuffer_tmp, (uint8_t*)&g_maxSpeed,sizeof(g_maxSpeed));

  g_maxSpeed = MAX_TARGET_SPEED;

  g_openLoopTorque = _IQ(OPEN_LOOP_TORQUE);
  g_impulse.Period = PERIOD_IMPULSE;
  g_targetDirection = 1;

  resetPiController(&g_piSpd);
  resetPiController(&g_piCur);

  g_piSpd.Kp = _IQ(KP_SPD);
  g_piSpd.Ki = _IQ(KI_SPD);
  g_piCur.Kp = _IQ(KP_CUR);
  g_piCur.Ki = _IQ(KI_CUR);

  g_closeLoopThreshold = _IQ(CLOSE_LOOP_THRESHOLD);
  g_closedLoop = false;
  g_controlRegister = 0; // 33 = (1 for driving open loop, 32 for execute command)

  initializeI2cModule();
  initializePwmModules();
  initializeAdcModule();
  initializeHallInterface();

  __bis_SR_register(GIE); // enable interrupts (timer & i2c)

  currentOffsetCalibration();

  enableGateDriver(); // TODO <<<< remove this line

#else
    disableGateDriver();
    asm("  NOP");
#endif


  while(1){
      // check if target reached
      if  (_IQabs(g_targetPosition - g_currentPosition) < 100) {
            g_targetReached = true;
            g_statusRegister |= POSITION_CONVERGED;
            // turn off output
            _iq output = _IQ(0.0);
            pwmGenerator(g_commState, output);
        } else {
            // target not reached yet
            g_targetReached = false;
            g_statusRegister &= ~POSITION_CONVERGED;
        }

      // check if driving in open or closed loop, act accordingly
      if (g_controlRegister & DRIVE_OPEN_LOOP && g_controlRegister & EXECUTE_COMMAND) {
            //driving open loop

            // target position sets direction motor drives in (will NOT converge though)
            g_targetDirection = (g_targetPosition - g_currentPosition >= 0) ? 1 : -1;

            if(!g_targetReached){
                // Execute macro to generate ramp up if needed
                  OPEN_LOOP_IMPULSE_MACRO(g_impulse);
                  if(g_impulse.Out){
                    MOD6CNT_MACRO(g_mod6cnt);
                    g_commState = (g_targetDirection > 0) ? g_mod6cnt.Counter : 5 - g_mod6cnt.Counter;
                  }

                // apply constant output
                _iq output;
                if(g_controlRegister & OPEN_LOOP_TORQUE_OVERRIDE)
                    output = _IQ(g_maxSpeed / MAX_TARGET_SPEED);
                else
                    output = _IQ(0.3);

                // apply output as PWM
                if(g_targetDirection > 0) {
                    pwmGenerator(g_commState, output);
                } else{
                    pwmGenerator(g_commState, -output);
                }

            }

            if (g_controlPrescaler<=0){
                g_controlPrescaler = PI_SPD_CONTROL_PRESCALER;
                g_currentPosition += g_targetDirection * OPEN_LOOP_SPEED;

                if(!g_targetReached)
                    g_drivingTimeoutCtr++;
            }

        } else if (g_controlRegister & EXECUTE_COMMAND){
            // driving closed loop
        // update sensor (Hall & current) readings
        if(g_readSensors){
            g_readSensors = false;

            // measure hall sensors
            readHallSensor();

            // Execute macro to generate ramp up
            if(g_closedLoop == false && g_targetReached == false){
                IMPULSE_MACRO(g_impulse);
                if(g_impulse.Out){
                    MOD6CNT_MACRO(g_mod6cnt);
                    g_commState = (g_targetDirection > 0) ? g_mod6cnt.Counter : 5 - g_mod6cnt.Counter;
                }
            }
            else{
                g_commState = g_hallMap[g_hallSensor.Pattern];
            }

            // update current position based on hall sensor readings
            if(g_hallSensor.Event){
                if(g_hallMap[g_hallSensor.Pattern] == 5 && g_oldCommState == 0){
                    g_currentPosition--;
                }
                else if(g_hallMap[g_hallSensor.Pattern] == 0 && g_oldCommState == 5){
                    g_currentPosition++;
                }
                else if(g_hallMap[g_hallSensor.Pattern] > g_oldCommState){
                    g_currentPosition++;
                }
                else{
                    g_currentPosition--;
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

          // inner control loop (current), apply output as PWM duty cycle
          // Normalize current values from  -2047 < adc < +2048 to iq15 --> -1.0 < adc < 1.0 and convert to iq format
          g_piCur.Fbk = (g_currentPhaseA + g_currentPhaseB + g_currentPhaseC) << 4;
          g_piCur.Ref = g_piSpd.Out;

          PI_MACRO(g_piCur);

          if(g_closedLoop == false && g_targetReached == false){
            g_piCur.i1 = 0;
            g_piCur.ui = 0;
            g_piSpd.i1 = 0;
            g_piSpd.ui = 0;
            g_piCur.Out = g_openLoopTorque;
            if(_IQabs(g_currentSpeed) > g_closeLoopThreshold){
                g_closedLoop = true;
            }
          }

          if(g_targetReached == true){
              g_piCur.i1 = 0;
              g_piCur.ui = 0;
              g_piSpd.i1 = 0;
              g_piSpd.ui = 0;
              g_piCur.Out = 0;
              g_closedLoop = false;
          }

          pwmGenerator(g_commState, g_piCur.Out);

          // outer control loop (speed)
          if(g_controlPrescaler <= 0){

              g_targetDirection = (g_targetPosition - g_currentPosition >= 0) ? 1 : -1;

              if(g_targetDirection > 0) g_piSpd.Ref = g_maxSpeed << 8;
              else g_piSpd.Ref = -g_maxSpeed << 8;

              // check for errors in controller operation
              if (g_currentPosition == g_oldPosition && !g_targetReached){
                  // position isn't updating; hall sensors likely not powered or broken
                  g_errorCounter++;
                  g_faultRegister |= POSITION_NO_CHANGE;
              } else if ( (g_currentPosition - g_oldPosition)*g_targetDirection < 0 && !g_targetReached){
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
                      g_targetPosition = g_currentPosition = 0; //stop controller
                  g_statusRegister |= CONTROLLER_ERROR; // add flag to status register
              }

              g_piSpd.Fbk = getSpeed();

              PI_MACRO(g_piSpd);

              g_controlPrescaler = PI_SPD_CONTROL_PRESCALER;
              if(!g_targetReached)
                  g_drivingTimeoutCtr++;
            }
          // reset controller internals if needed
        #ifndef IRIS_ALL_OFF
           asm("  NOP");
          g_closedLoop = (_IQabs(g_currentSpeed) > g_closeLoopThreshold && !g_targetReached) ? true : false;

           // if controllers are saturated, reset its integrator & output
           if(g_piSpd.w1){
               __disable_interrupt();
              g_piSpd.i1 = 0;
              g_piSpd.ui = 0;
              g_piSpd.v1 = 0;
              __enable_interrupt();
           }

           if(g_piCur.w1){
               __disable_interrupt();
               g_piCur.i1 = 0;
               g_piCur.ui = 0;
               g_piCur.v1 = 0;
               __enable_interrupt();
           }

        #endif
        }

      // check if motor has taken too long to converge, act accordingly if so
      if(g_drivingTimeoutCtr > DRIVING_TIMEOUT_THRESHOLD){
          g_targetReached = true;
          g_targetPosition = g_currentPosition; // so motor won't flip g_targetReached again
          g_faultRegister |= DRIVING_TIMEOUT;
          g_statusRegister |= (POSITION_CONVERGED | CONTROLLER_ERROR);
          g_drivingTimeoutCtr = 0;
      }
  }
}


/**
 * @brief      Main control loop
 */
#ifndef IRIS_ALL_OFF

#pragma CODE_SECTION(TIMER0_B0_ISR, ".TI.ramfunc")
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR (void){

    if(g_calibrating){
      // Prepare ADC conversion for next round
      HWREG8(ADC12_B_BASE + OFS_ADC12CTL0_L) &= ~(ADC12ENC);
      HWREG8(ADC12_B_BASE + OFS_ADC12CTL0_L) |= ADC12ENC + ADC12SC;

      g_currentOffsetPhaseA = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_0));
      g_currentOffsetPhaseB = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_1));
      g_currentOffsetPhaseC = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_2));
      g_calibrationDone = true;
    }

    if(!g_calibrationDone) return;

    g_readSensors=true;

    // without conditional can get huge and negative
   if(g_controlPrescaler>0)
       g_controlPrescaler = g_controlPrescaler -1;


  return;

}

#endif
