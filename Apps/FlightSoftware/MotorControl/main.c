#include "main.h"

_iq g_currentPhaseA;
_iq g_currentPhaseB;
_iq g_currentPhaseC;

// Used for calibration
volatile bool g_calibrating;
volatile bool g_calibrationDone;
volatile _iq g_currentOffsetPhaseA;
volatile _iq g_currentOffsetPhaseB;
volatile _iq g_currentOffsetPhaseC;
volatile _iq g_currentSpeed;

volatile uint8_t g_commState;
HallSensor g_hallSensor;

int32_t g_currentPosition;
int32_t g_targetPosition;

volatile uint8_t g_controlPrescaler;
uint8_t g_hallMap[8];

PI_CONTROLLER g_piSpd;
PI_CONTROLLER g_piCur;

void initializePwmModules(){
    //Start Timer
    Timer_B_initUpDownModeParam initUpDownParam = {0};
    initUpDownParam.clockSource = TIMER_B_CLOCKSOURCE_SMCLK; // 16 MHz
    initUpDownParam.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
    initUpDownParam.timerPeriod = PWM_PERIOD_TICKS;  // 20KHz
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

inline void setPwmAPeriod(uint16_t period){
    HWREG16(TIMER_B0_BASE + PWMA_H_CCR_REGISTER + OFS_TBxR) = PWM_PERIOD_TICKS - period;
}

inline void setPwmBPeriod(uint16_t period){
    HWREG16(TIMER_B0_BASE + PWMB_H_CCR_REGISTER + OFS_TBxR) = PWM_PERIOD_TICKS - period;
}

inline void setPwmCPeriod(uint16_t period){
    HWREG16(TIMER_B0_BASE + PWMC_H_CCR_REGISTER + OFS_TBxR) = PWM_PERIOD_TICKS - period;
}

inline void enableHalfBridgeA() {
    P3OUT |= GPIO_PIN6;
}

inline void disableHalfBridgeA() {
    P3OUT &= ~GPIO_PIN6;
}

inline void enableHalfBridgeB() {
    P3OUT |= GPIO_PIN4;
}

inline void disableHalfBridgeB() {
    P3OUT &= ~GPIO_PIN4;
}

inline void enableHalfBridgeC() {
    P2OUT |= GPIO_PIN1;
}

inline void disableHalfBridgeC() {
    P2OUT &= ~GPIO_PIN1;
}

inline void enableGateDriver(){
    GPIO_setOutputHighOnPin(GPIO_PORT_PJ, GPIO_PIN0);
    __delay_cycles(1600000);    // 100 ms
}

inline void disableGateDriver(){
    GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_PIN0);
    __delay_cycles(1600000);    // 100 ms
}

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

void currentOffsetCalibration(){
    // Enable calibration
    GPIO_setOutputHighOnPin(GPIO_PORT_P4,
                            GPIO_PIN4);

    enableGateDriver();

    g_calibrationDone = false;
    g_calibrating = true;

    while(!g_calibrationDone); // wait here until calibration completed

    g_calibrating = false;

    // Disable calibration
    GPIO_setOutputLowOnPin(GPIO_PORT_P4,
                            GPIO_PIN4);

    disableGateDriver();
}

void initializeI2cModule(){
    // Configure I2C interface for slave interface
    EUSCI_B_I2C_initSlaveParam i2cParam;
    i2cParam.slaveAddress = 0x00;
    i2cParam.slaveAddressOffset = 0;
    i2cParam.slaveOwnAddressEnable = 0;

    EUSCI_B_I2C_initSlave(EUSCI_B0_BASE, &i2cParam);
}

inline void pwmGenerator(const uint8_t commutation, _iq dutyCycle){
    uint16_t dc; //duty cycle
    uint16_t dcCmpl; //complement

    // Normalize duty cycle -1.0 < dc < +1.0 to 0 < dc < 512
    dc = (uint16_t)(dutyCycle >> 7) + PWM_HALF_PERIOD_TICKS;
    dcCmpl = PWM_PERIOD_TICKS - dc;

    switch(commutation){
    case 1:
        setPwmAPeriod(dc);
        enableHalfBridgeA();
        setPwmBPeriod(dcCmpl);
        enableHalfBridgeB();
        setPwmCPeriod(0);
        disableHalfBridgeC();
        break;
    case 2:
        setPwmAPeriod(dc);
        enableHalfBridgeA();
        setPwmBPeriod(0);
        disableHalfBridgeB();
        setPwmCPeriod(dcCmpl);
        enableHalfBridgeC();
        break;
    case 3:
        setPwmAPeriod(0);
        disableHalfBridgeA();
        setPwmBPeriod(dc);
        enableHalfBridgeB();
        setPwmCPeriod(dcCmpl);
        enableHalfBridgeC();
        break;
    case 4:
        setPwmAPeriod(dcCmpl);
        enableHalfBridgeA();
        setPwmBPeriod(dc);
        enableHalfBridgeB();
        setPwmCPeriod(0);
        disableHalfBridgeC();
        break;
    case 5:
        setPwmAPeriod(dcCmpl);
        enableHalfBridgeA();
        setPwmBPeriod(0);
        disableHalfBridgeB();
        setPwmCPeriod(dc);
        enableHalfBridgeC();
        break;
    case 6:
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

inline void readHallSensor(){
    g_hallSensor.pattern = (P4IN & GPIO_PIN3) << 2;   // W
    g_hallSensor.pattern |= (P2IN & GPIO_PIN5) << 1;  // V
    g_hallSensor.pattern |= (P2IN & GPIO_PIN6);       // U
    g_hallSensor.event = g_hallSensor.pattern ^ g_hallSensor.oldPattern;
    g_hallSensor.oldPattern = g_hallSensor.pattern;
    if(g_hallSensor.pattern == 7) g_hallSensor.error = 1;
    if(g_hallSensor.pattern == 0) g_hallSensor.error = 1;
}

void initializeHallInterface(){
    g_hallMap[0] = 7;
    g_hallMap[7] = 7;

    g_hallMap[5] = 1;
    g_hallMap[1] = 2;
    g_hallMap[3] = 3;
    g_hallMap[2] = 4;
    g_hallMap[6] = 5;
    g_hallMap[4] = 6;
}

void resetPiController(PI_CONTROLLER *pi){
    pi->i1 = _IQ(0.0);
    pi->ui = _IQ(0.0);
    pi->v1 = _IQ(0.0);
    pi->up = _IQ(0.0);
    pi->Umax = _IQ(1.0);
    pi->Umin = _IQ(-1.0);
}

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

void main(void){

    // Turn off the watchdog
    WDT_A_hold(WDT_A_BASE);

    initializeGpios();

    //Set DCO frequency to 16MHz
    CS_setDCOFreq(CS_DCORSEL_1, CS_DCOFSEL_4);

    CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);

    // Initialize global variables
    g_currentOffsetPhaseA = 0;
    g_currentOffsetPhaseB = 0;
    g_currentOffsetPhaseC = 0;
    g_commState = 1;
    g_hallSensor.pattern = 0;
    g_hallSensor.oldPattern = 0;
    g_currentPosition = 0;
    g_targetPosition = 0;
    g_controlPrescaler = PI_CONTROL_PRESCALER;

    resetPiController(&g_piSpd);
    resetPiController(&g_piCur);

    g_piSpd.Kp = _IQ(KP_SPD);
    g_piSpd.Ki = _IQ(KI_SPD);
    g_piCur.Kp = _IQ(KP_CUR);
    g_piCur.Ki = _IQ(KI_CUR);

    g_piSpd.Ref = _IQ(0.1);

    //initializeI2cModule();
    initializePwmModules();
    initializeAdcModule();
    initializeHallInterface();

    __bis_SR_register(GIE);

    currentOffsetCalibration();

    enableGateDriver();

    while(1);

}

// Capture current
#pragma CODE_SECTION(TIMER0_B0_ISR, ".TI.ramfunc")
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR (void){

    // Prepare ADC conversion for next round
    HWREG8(ADC12_B_BASE + OFS_ADC12CTL0_L) &= ~(ADC12ENC);
    HWREG8(ADC12_B_BASE + OFS_ADC12CTL0_L) |= ADC12ENC + ADC12SC;

    if(g_calibrating){
       g_currentOffsetPhaseA = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_0));
       g_currentOffsetPhaseB = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_1));
       g_currentOffsetPhaseC = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_2));
       g_calibrationDone = true;
       return;
    }

    if(!g_calibrationDone) return;

    readHallSensor();
//    if(g_hallSensor.error) return;

    if(g_hallSensor.event){
        g_commState = g_hallMap[g_hallSensor.pattern];
    }

    if(g_controlPrescaler == 0){
        g_controlPrescaler = PI_CONTROL_PRESCALER;

        // Normalize from -256 ~ + 255 to -1.0 ~ 1.0
        g_piSpd.Ref = (g_targetPosition - g_currentPosition) << 7;
        g_piSpd.Fbk = g_currentSpeed;
        PI_MACRO(g_piSpd);
    }

    g_controlPrescaler = g_controlPrescaler -1;

   // remove offset
    g_currentPhaseA = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_0)) - g_currentOffsetPhaseA;
    g_currentPhaseB = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_1)) - g_currentOffsetPhaseB;
    g_currentPhaseC = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_2)) - g_currentOffsetPhaseC;
    // Normalize current values from  0 < adc < +4095 to iq15 --> -1.0 < adc < 1.0 and convert to iq format
    g_piCur.Fbk = (g_currentPhaseA + g_currentPhaseB + g_currentPhaseC) << 4;
    g_piCur.Ref = g_piSpd.Out;
    PI_MACRO(g_piCur);

    pwmGenerator(g_commState, g_piCur.Out);
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR(void){
    asm("  NOP");
}

