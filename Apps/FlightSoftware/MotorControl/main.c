// ======================================================================
// \title  main.c
// \author cedric
// \edited by Jonathan
// \brief  Runs control loop to drive motors, as well as initializing
//         everything needed for the rest of the code. Also reads from
//         current sensors on motor driver and hall sensors on motor
//
// ======================================================================

#include "main.h"

// Emergency Reset-Mediated-Driving Implementation:
// **CONTROLS:**
// (these are defined in the compile flags, like `Debug_BootDrive_RWD_MotorA`)
// #define DRIVE_ON_BOOT
// #define DRIVE_ON_BOOT_REAR_DRIVE
// #define DRIVE_ON_BOOT_FRONT_DRIVE
// #define MOTOR_A
// #define MOTOR_B
// #define MOTOR_C
// #define MOTOR_D
// **SETTINGS:**
#define DRIVE_ON_BOOT_OPEN_LOOP
#define DRIVE_ON_BOOT_CM 15 /*bigger question for testing is how many degrees can we turn*/
#define TICKS_PER_CM 158    /*approx based on math not measurements*/
#define DRIVE_ON_BOOT_TICKS (DRIVE_ON_BOOT_CM * TICKS_PER_CM)
#define DRIVE_ON_BOOT_START_DELAY (20 * DELAY_100_ms) /*2 sec*/
// Technically, out of 1.0
#define DRIVE_ON_BOOT_SPEED_PERCENT 0.3

// TODO: re-organize all these variable defs
_iq g_currentPhaseA;
_iq g_currentPhaseB;
_iq g_currentPhaseC;
_iq g_feedforwardFW;

// Used for calibration
_iq g_currentOffsetPhaseA;
_iq g_currentOffsetPhaseB;
_iq g_currentOffsetPhaseC;
_iq g_currentSpeed;
_iq g_openLoopTorque;

_iq g_closeLoopThreshold;

uint8_t g_commState;
uint8_t g_oldCommState;
HallSensor g_hallSensor;
uint8_t g_hallMap[8];

volatile int32_t g_currentPosition;
volatile int32_t g_targetPosition;
int32_t g_oldPosition;

uint16_t g_controlPrescaler;

volatile PI_CONTROLLER g_piSpd;
volatile PI_CONTROLLER g_piCur;
volatile IMPULSE g_impulse;
volatile MOD6CNT g_mod6cnt;

bool g_closedLoop;
bool g_targetReached;
volatile uint8_t g_maxSpeed;

volatile StateMachine g_state;
volatile CmdState g_cmdState;

volatile uint16_t g_accelRate;
volatile uint16_t g_decelRate;

int8_t g_targetDirection;

uint8_t g_statusRegister;
volatile uint8_t g_controlRegister;
volatile uint8_t g_faultRegister;
volatile uint32_t g_drivingTimeoutCtr;
uint8_t g_errorCounter = 0; // incremented every time inner control loop is reached and motor is acting strange
                            // if it exceeds ERROR_ITERATION_THRESHOLD then motor is stopped

bool g_readSensors = false;

/**
 * @brief      Gets the speed.
 *
 * @return     The speed.
 */
inline _iq getSpeed(void)
{
    // Normalize speed to -128 ticks < diff < 127 to -1.0 < diff < +1.0
    // 255 ticks per (PI_SPD_CONTROL_PRESCALER * PWM_PERIOD_TICKS) represents 9.600 eRPM
    // which equates to ~25.22 cm/sec, much faster than we should be going in normal operation
    int32_t deltaPos = _IQsat(g_currentPosition - g_oldPosition, 256, -256);
    // shift left 7 to scale 9 bit deltaPos (-255,255) to 15 decimal fixed point _IQ (-1.0,1.0)
    g_currentSpeed = deltaPos << 7;
    g_oldPosition = g_currentPosition;
    return g_currentSpeed;
}

/**
 * @brief      Initializes the adc module.
 */
inline void initializeAdcModule(void)
{
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
inline void currentOffsetCalibration(void)
{

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
    disableGateDriver();  // turn off motor driver for time being
}

/**
 * @brief      Generate the pwm period for the 6 pwm channels
 *
 * @param[in]  commutation  The commutation
 * @param[in]  dutyCycle    The duty cycle
 */
inline void pwmGenerator(const uint8_t commutation, _iq dutyCycle)
{
    uint16_t dc = 0;     // duty cycle
    uint16_t dcCmpl = 0; // complement

    // Normalize duty cycle -1.0 < dc < +1.0 to 0 < dc < 512
    dc = (uint16_t)(dutyCycle >> 7) + PWM_HALF_PERIOD_TICKS;
    dcCmpl = PWM_PERIOD_TICKS - dc;

    _iq PWM_A = 0;
    _iq PWM_B = 0;
    _iq PWM_C = 0;

    if (dutyCycle != 0)
    {
        switch (commutation)
        {
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
    if (PWM_A == 0)
    {
        disableHalfBridgeA();
    }
    else if (PWM_B == 0)
    { // can do else-if because only 1 will be 0
        disableHalfBridgeB();
    }
    else if (PWM_C == 0)
    {
        disableHalfBridgeC();
    }
}

/**
 * @brief      Reads a hall sensor.
 */
inline void readHallSensor(void)
{
    g_hallSensor.Pattern = READ_HALL_W >> 1;  // W
    g_hallSensor.Pattern |= READ_HALL_V >> 4; // V
    g_hallSensor.Pattern |= READ_HALL_U >> 6; // U
    g_hallSensor.Event = g_hallSensor.Pattern ^ g_hallSensor.OldPattern;
    g_hallSensor.OldPattern = g_hallSensor.Pattern;
    if (g_hallSensor.Pattern & 0x07)
    {
        g_hallSensor.Error = 1;
    }
    if (g_hallSensor.Pattern == 0x00)
    {
        g_hallSensor.Error = 1;
    }
}

/**
 * @brief      Initializes the hall interface.
 */
void initializeHallInterface(void)
{
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
 * @param      pi    { controller for either speed or current }
 */
void resetPiController(volatile PI_CONTROLLER *pi)
{
    pi->i1 = _IQ(0.0); // reset integrator storage (stores pi->ui from last time step)
    pi->ui = _IQ(0.0); // reset integral term (sums error over time steps)
    pi->v1 = _IQ(0.0);
    pi->up = _IQ(0.0);
    pi->Umax = _IQ(PI_OUTPUT_BOUNDS);
    pi->Umin = _IQ(-PI_OUTPUT_BOUNDS);
}

/**
 * @brief      Disable the drive
 */
inline void disable(void)
{
    if (g_state == IDLE)
    {
        return; // already in IDLE, nothing to do
    }

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
inline void run(void)
{
    if (g_state == RUNNING)
    {
        return; // already in RUNNING, nothing to do
    }
    __disable_interrupt();
    enableGateDriver();
    g_targetDirection = (g_targetPosition - g_currentPosition >= 0) ? 1 : -1;
    g_currentPosition = 0;
    g_targetReached = false;
    g_state = RUNNING;
    __enable_interrupt();
}

/**
 * @brief      Update the drive state machine
 */
void updateStateMachine(void)
{
    if (g_cmdState == RUN && g_state == IDLE)
    {
        run();
    }
    else if (g_cmdState == DISABLE && g_state == RUNNING)
    {
        disable();
    }
    g_cmdState = NO_CMD;
}

/**
 * @brief      Clears the DRV8304 Driver Fault Register.
 */
void clear_driver_fault(void)
{
    __disable_interrupt(); // entering critical section
    // Pull high first so you can then pull it low:
    //    GPIO_setOutputHighOnPin(GPIO_PORT_PJ, GPIO_PIN0);
    PJOUT |= GPIO_PIN0;
    __delay_cycles(DELAY_100_ms);
    // Reset Fault Register by pulsing ENABLE for 5-32us (18.5us):
    //    GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_PIN0);
    PJOUT &= ~GPIO_PIN0;
    __delay_cycles(296); // 18.5 us
                         //    GPIO_setOutputHighOnPin(GPIO_PORT_PJ, GPIO_PIN0);
    PJOUT |= GPIO_PIN0;
    __enable_interrupt();
}
/**
 * @brief      Reads the whether the DRV8304 driver is in a "fault condition" (and should be cleared). Active low.
 */
bool read_driver_fault(void)
{
    return !(PJIN & 0x02);
}

/*
 * @brief       TODO
 */
void initializeSensorVariables(void)
{
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
}

/*
 * @brief       TODO
 */
void initializeSoftwareControlVariables(void)
{
    // software control related variables (rate groups, internal state machine)
    g_controlPrescaler = PI_SPD_CONTROL_PRESCALER;
    g_closedLoop = false;
    g_state = RUNNING;
    g_cmdState = NO_CMD;
    g_controlRegister = 0; // see main.h for bits
}

/*
 * @breif   initialize motor controller related variables (PI controllers for speed and current)
 */
void initializeControllerVariables(void)
{
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
}

/*
 * @brief   Do everything necessary to init MSP and begin driving
 */
void initController(void)
{
    initializeGpios();

    // Set DCO frequency to 16MHz
    CS_setDCOFreq(CS_DCORSEL_1, CS_DCOFSEL_4);

    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // Initialize variables
    initializeSensorVariables();
    initializeSoftwareControlVariables();
    initializeControllerVariables();

    // initialize hardware components
    initializeI2cModule();
    initializePwmModules();
    initializeAdcModule();
    initializeHallInterface();

    currentOffsetCalibration(); // get initial estimate of current offsets
    __bis_SR_register(GIE);     // enable interrupts (timer & i2c)
    enableGateDriver();         // get ready to move
}

/*
 * @brief  update sensor (Hall & current) readings for when driving in closed loop
 */
void readSensors(void)
{

    // measure hall sensors
    readHallSensor();

    if (g_closedLoop == false && g_targetReached == false)
    {
        // Execute macro to generate ramp up
        IMPULSE_MACRO(g_impulse);
        if (g_impulse.Out)
        {
            MOD6CNT_MACRO(g_mod6cnt); // iterate to next commutation
            if (g_targetDirection > 0)
            {
                g_commState = g_mod6cnt.Counter;
            }
            else
            {
                g_commState = 5 - g_mod6cnt.Counter; // account for driving in reverse (comm cycle reverses)
            }
        }
    }
    else
    {
        // allow commutation to be controlled by hall sensor readings (will time them way better than we could)
        g_commState = g_hallMap[g_hallSensor.Pattern];
    }

    // update current position based on hall sensor readings
    if (g_hallSensor.Event)
    {
        if (g_hallMap[g_hallSensor.Pattern] == 5 && g_oldCommState == 0)
        {
            g_currentPosition--;
        }
        else if (g_hallMap[g_hallSensor.Pattern] == 0 && g_oldCommState == 5)
        {
            g_currentPosition++;
        }
        else if (g_hallMap[g_hallSensor.Pattern] > g_oldCommState)
        {
            g_currentPosition++;
        }
        else
        {
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

/*
 * @brief       TODO
 */
void moderatePIControllers(void)
{
    // apply constant torque if still in open loop
    if (g_closedLoop == false && g_targetReached == false)
    {
        g_piCur.i1 = 0;
        g_piCur.ui = 0;
        g_piSpd.i1 = 0;
        g_piSpd.ui = 0;
        g_piCur.Out = g_openLoopTorque;
        if (_IQabs(g_currentSpeed) > g_closeLoopThreshold)
        {
            g_closedLoop = true;
        }
    }
    // set integrator and output to 0 if target is reached
    if (g_targetReached == true)
    {
        g_piCur.i1 = 0;
        g_piCur.ui = 0;
        g_piSpd.i1 = 0;
        g_piSpd.ui = 0;
        g_piCur.Out = 0;
        g_closedLoop = false;
    }
    //           if controllers are saturated, reset its integrator & output
    if (g_piSpd.w1)
    {
        __disable_interrupt();
        g_piSpd.i1 = 0;
        g_piSpd.ui = 0;
        g_piSpd.v1 = 0;
        __enable_interrupt();
    }
    if (g_piCur.w1)
    {
        __disable_interrupt();
        g_piCur.i1 = _IQ(g_targetDirection * 0.5); // full wipe of integrator causes jumpy stop-start behavior
        g_piCur.ui = 0;
        g_piCur.v1 = 0;
        __enable_interrupt();
    }
}

/*
 * @brief      TODO
 */
void checkTargetReached(void)
{
    if (_IQabs(g_targetPosition - g_currentPosition) < POSITION_CONVERGENCE_THRESHOLD)
    {
        // target has been reached
        g_targetReached = true;
        g_statusRegister |= POSITION_CONVERGED;
        // turn off output
        _iq output = 0;
        pwmGenerator(g_commState, output);
    }
    else
    {
        // target not reached yet
        g_targetReached = false;
        g_statusRegister &= ~POSITION_CONVERGED;
    }
}

/*
 * @brief   TODO
 */
void driveOpenLoop(void)
{
    if (!g_targetReached)
    {
        // Iterate through commutations & apply impulse to desired motor windings
        OPEN_LOOP_IMPULSE_MACRO(g_impulse);
        if (g_impulse.Out)
        {
            MOD6CNT_MACRO(g_mod6cnt);
            g_commState = (g_targetDirection > 0) ? g_mod6cnt.Counter : 5 - g_mod6cnt.Counter;
        }

        _iq output;
        if (g_controlRegister & OPEN_LOOP_TORQUE_OVERRIDE)
        {
            output = _IQ(DRIVE_ON_BOOT_SPEED_PERCENT); // division here didn't work and always gave 0. just replacing with a different pre-set figure. _IQ(g_maxSpeed / MAX_TARGET_SPEED); // apply user specified output
        }
        else
        {
            output = _IQ(FULLY_OPEN_LOOP_PWM); // apply constant output (30% duty cycle)
        }

        // apply output as PWM
        if (g_targetDirection > 0)
        {
            pwmGenerator(g_commState, output);
        }
        else
        {
            pwmGenerator(g_commState, -output);
        }
    }

    // control loop - updates current position with speed estimate & ticks timeout counter
    if (g_controlPrescaler <= 0)
    {
        g_controlPrescaler = PI_SPD_CONTROL_PRESCALER;
        g_currentPosition += g_targetDirection * OPEN_LOOP_SPEED;

        if (!g_targetReached)
        {
            g_drivingTimeoutCtr++;
        }
    }
}

/*
 * @brief       TODO
 */
void closedLoopCurrentLoop(void)
{
    // inner control loop (current), apply output as PWM duty cycle
    // Normalize current values from  -2047 < adc < +2048 to iq15 --> -1.0 < adc < 1.0 and convert to iq format
    g_piCur.Fbk = (g_currentPhaseA + g_currentPhaseB + g_currentPhaseC) << 4;
    g_piCur.Ref = g_piSpd.Out;

    pi_iteration(&g_piCur);

    // check if we can pivot to closed loop control
    g_closedLoop = (_IQabs(g_currentSpeed) > g_closeLoopThreshold && !g_targetReached) ? true : false;

    moderatePIControllers(); // clamp and/or zero values as necessary

    // apply duty cycle based on current PI controller's output
    pwmGenerator(g_commState, g_piCur.Out);
}

/*
 * @brief       TODO
 */
void closedLoopSpeedLoop(void)
{
    if (g_targetDirection > 0)
    {
        g_piSpd.Ref = g_maxSpeed << 8;
    }
    else
    {
        g_piSpd.Ref = -g_maxSpeed << 8;
    }

    g_piSpd.Fbk = getSpeed();
    pi_iteration(&g_piSpd);
}

/*
 * @brief       TODO
 */
void checkForClosedLoopErrors(void)
{
    // check for errors in controller operation
    if (g_currentPosition == g_oldPosition && !g_targetReached)
    {
        // position isn't updating; hall sensors likely not powered or broken
        g_errorCounter++;
        g_faultRegister |= POSITION_NO_CHANGE;
    }
    else if ((g_currentPosition - g_oldPosition) * g_targetDirection < 0 && !g_targetReached)
    {
        // moving in wrong direction
        g_errorCounter++;
        g_faultRegister |= DRIVING_WRONG_DIRECTION;
    }
    else
    {
        // operating normally; no error
        g_statusRegister &= ~CONTROLLER_ERROR;
        g_errorCounter = 0;                                                 // reset error counter
        g_faultRegister &= ~(POSITION_NO_CHANGE & DRIVING_WRONG_DIRECTION); // clear faults in register
    }

    // errors on last ERROR_ITERATION_THRESHOLD time steps; time to stop trying to drive motor
    if (g_errorCounter >= ERROR_ITERATION_THRESHOLD)
    {
        if (g_controlRegister & OVERRIDE_FAULT_DETECTION == 0x00) // check if we should stop controller given fault
            g_targetPosition = g_currentPosition = 0;             // stop controller
        g_statusRegister |= CONTROLLER_ERROR;                     // add flag to status register
    }
}

/*
 *  @brief      TODO
 */
void handleMotorTimeout(void)
{
    g_targetReached = true;
    g_targetPosition = g_currentPosition; // so motor won't flip g_targetReached again
    g_faultRegister |= DRIVING_TIMEOUT;
    g_statusRegister |= (POSITION_CONVERGED | CONTROLLER_ERROR);
    g_drivingTimeoutCtr = 0;
}

/**
 * @brief      main function
 */
void main(void)
{

    // Turn off the watchdog
    WDT_A_hold(WDT_A_BASE);

    initController(); // init all variables and functionality needed to drive

// Emergency Reset-Mediated-Driving Implementation:
#ifdef DRIVE_ON_BOOT
    g_targetPosition = DRIVE_ON_BOOT_TICKS;
    g_controlRegister = 32;
    #ifdef DRIVE_ON_BOOT_OPEN_LOOP
        g_controlRegister |= DRIVE_OPEN_LOOP;
        // Override open-loop torque for custom speed (DRIVE_ON_BOOT_SPEED_PERCENT):
        g_controlRegister |= OPEN_LOOP_TORQUE_OVERRIDE;
    #endif

    // Flip direction if this wheel should be spinning CW instead of CCW:
    #if (defined(MOTOR_B) || defined(MOTOR_D)) && defined(DRIVE_ON_BOOT_REAR_DRIVE) || \
        (defined(MOTOR_A) || defined(MOTOR_C)) && defined(DRIVE_ON_BOOT_FRONT_DRIVE)
        g_targetPosition = -g_targetPosition;
    #endif

    // Wait before starting up (safety):
    __delay_cycles(DRIVE_ON_BOOT_START_DELAY);

#endif

    while (1)
    {
        checkTargetReached();

        // target position sets direction motor drives in
        if (g_targetPosition - g_currentPosition >= 0)
        {
            g_targetDirection = 1;
        }
        else
        {
            g_targetDirection = -1;
        }

        // check if driving in open or closed loop (& have been told to execute command), act accordingly
        if (g_controlRegister & DRIVE_OPEN_LOOP && g_controlRegister & EXECUTE_COMMAND)
        {
            // driving in fully open loop
            driveOpenLoop();
        }
        else if (g_controlRegister & EXECUTE_COMMAND)
        {
            // driving closed loop

            if (g_readSensors)
            { // update sensor readings if requested by timer interrupt
                readSensors();
                g_readSensors = false;
            }

            closedLoopCurrentLoop(); // inner loop of controller

            if (g_controlPrescaler <= 0)
            {

                closedLoopSpeedLoop(); // outer loop of controller
                checkForClosedLoopErrors();

                // reset control prescaler & tick timeout counter
                g_controlPrescaler = PI_SPD_CONTROL_PRESCALER;
                if (!g_targetReached)
                {
                    g_drivingTimeoutCtr++;
                }
            }
        }

        // check if motor has taken too long to converge, act accordingly if so
        if (g_drivingTimeoutCtr > DRIVING_TIMEOUT_THRESHOLD)
        {
            handleMotorTimeout();
        }
    } // end of while loop
}

/**
 * @brief      Timer interrupt that sets rate for controller and indicates when to read sensors again
 */

#pragma CODE_SECTION(TIMER0_B0_ISR, ".TI.ramfunc")
#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void)
{

    g_readSensors = true; // read sensors (hall/encoders & current) again every time this interrupts fires

    // without conditional can get huge and negative
    if (g_controlPrescaler > 0)
        g_controlPrescaler = g_controlPrescaler - 1;

    return;
}
