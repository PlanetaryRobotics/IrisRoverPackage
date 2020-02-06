#include "main.h"

float g_currentPhaseA;
float g_currentPhaseB;
float g_currentPhaseC;

// Used for calibration
volatile bool g_calibrating;
volatile bool g_calibrationDone;
volatile uint16_t g_currentOffsetPhaseA;
volatile uint16_t g_currentOffsetPhaseB;
volatile uint16_t g_currentOffsetPhaseC;

uint8_t g_commState;

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
    Timer_B_setCompareValue(TIMER_B0_BASE, PWMA_H_CCR_REGISTER, PWM_PERIOD_TICKS - period);
}

inline void setPwmBPeriod(uint16_t period){
    Timer_B_setCompareValue(TIMER_B0_BASE, PWMB_H_CCR_REGISTER, PWM_PERIOD_TICKS - period);
}

inline void setPwmCPeriod(uint16_t period){
    Timer_B_setCompareValue(TIMER_B0_BASE, PWMC_H_CCR_REGISTER, PWM_PERIOD_TICKS -period);
}

inline void enableHalfBridgeA() {
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6);
}

inline void disableHalfBridgeA() {
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);
}

inline void enableHalfBridgeB() {
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN4);
}

inline void disableHalfBridgeB() {
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN4);
}

inline void enableHalfBridgeC() {
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
}

inline void disableHalfBridgeC() {
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
}

inline void enableGateDriver(){
    GPIO_setOutputHighOnPin(GPIO_PORT_PJ, GPIO_PIN0);
}

inline void disableGateDriver(){
    GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_PIN0);
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
    enableGateDriver();
    // Enable calibration
    GPIO_setOutputHighOnPin(GPIO_PORT_P4,
                            GPIO_PIN4);

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

inline void pwmGenerator(const uint8_t state, float dutyCycle){
    uint16_t period;
    uint16_t periodCmpl;
    period = (dutyCycle * PWM_PERIOD_TICKS * 0.5) + PWM_PERIOD_TICKS * 0.5;
    periodCmpl = PWM_PERIOD_TICKS - period;
    switch(state){
    case 1:
        setPwmAPeriod(period);
        enableHalfBridgeA();
        setPwmBPeriod(periodCmpl);
        enableHalfBridgeB();
        setPwmCPeriod(0);
        disableHalfBridgeC();
        break;
    case 2:
        setPwmAPeriod(period);
        enableHalfBridgeA();
        setPwmBPeriod(0);
        disableHalfBridgeB();
        setPwmCPeriod(periodCmpl);
        enableHalfBridgeC();
        break;
    case 3:
        setPwmAPeriod(0);
        disableHalfBridgeA();
        setPwmBPeriod(period);
        enableHalfBridgeB();
        setPwmCPeriod(periodCmpl);
        enableHalfBridgeC();
        break;
    case 4:
        setPwmAPeriod(periodCmpl);
        enableHalfBridgeA();
        setPwmBPeriod(period);
        enableHalfBridgeB();
        setPwmCPeriod(0);
        disableHalfBridgeC();
        break;
    case 5:
        setPwmAPeriod(periodCmpl);
        enableHalfBridgeA();
        setPwmBPeriod(0);
        disableHalfBridgeB();
        setPwmCPeriod(period);
        enableHalfBridgeC();
        break;
    case 6:
        setPwmAPeriod(0);
        disableHalfBridgeA();
        setPwmBPeriod(periodCmpl);
        enableHalfBridgeB();
        setPwmCPeriod(period);
        enableHalfBridgeC();
        break;
    default:
        break;
    }
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
    g_calibrating = false;
    g_currentOffsetPhaseA = 0;
    g_currentOffsetPhaseB = 0;
    g_currentOffsetPhaseC = 0;
    g_commState = 1;

    //initializeI2cModule();
    initializePwmModules();
    initializeAdcModule();

    __bis_SR_register(GIE);

    currentOffsetCalibration();



    setPwmAPeriod(25);
    enableHalfBridgeA();
    setPwmBPeriod(200);
    enableHalfBridgeB();
    setPwmCPeriod(75);
    enableHalfBridgeC();

    while(1);

}

// Control Loop
#pragma CODE_SECTION(TIMER0_B0_ISR, ".TI.ramfunc")
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR (void){
    float dutyCycle = 0.25;

    // Prepare ADC conversion for next round
    HWREG8(ADC12_B_BASE + OFS_ADC12CTL0_L) &= ~(ADC12ENC);
    HWREG8(ADC12_B_BASE + OFS_ADC12CTL0_L) |= ADC12ENC + ADC12SC;

    uint16_t interruptSrc = __even_in_range(TB0IV,14);
    if(g_calibrating){
       g_currentOffsetPhaseA = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_0));
       g_currentOffsetPhaseB = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_1));
       g_currentOffsetPhaseC = HWREG16(ADC12_B_BASE + (OFS_ADC12MEM0 + ADC12_B_MEMORY_2));
       //g_calibrationDone = true;
       return;
    }
    else{
       // pwmGenerator(g_commState, dutyCycle);
    }
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR(void){
    asm("  NOP");
}

