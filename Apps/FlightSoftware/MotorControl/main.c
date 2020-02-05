#include "driverlib.h"
#include "bsp.h"

void initializePwmModules(){
    //Start Timer
    Timer_B_initUpDownModeParam initUpDownParam = {0};
    initUpDownParam.clockSource = TIMER_B_CLOCKSOURCE_SMCLK; // 16 MHz
    initUpDownParam.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
    initUpDownParam.timerPeriod = 400;  // 20KHz
    initUpDownParam.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_ENABLE;
    initUpDownParam.captureCompareInterruptEnable_CCR0_CCIE = TIMER_B_CCIE_CCR0_INTERRUPT_DISABLE;
    initUpDownParam.timerClear = TIMER_B_DO_CLEAR;
    initUpDownParam.startTimer = false;
    Timer_B_initUpDownMode(TIMER_B0_BASE, &initUpDownParam);

    //Initialze compare registers to generate PWMA_H
    Timer_B_initCompareModeParam initComp1Param = {0};
    initComp1Param.compareRegister = PWMA_H_CCR_REGISTER;
    initComp1Param.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp1Param.compareOutputMode = TIMER_B_OUTPUTMODE_SET_RESET;
    initComp1Param.compareValue = 0; // 0% duty cycle
    Timer_B_initCompareMode(TIMER_B0_BASE, &initComp1Param);

    //Initialze compare registers to generate PWMB_H
    Timer_B_initCompareModeParam initComp2Param = {0};
    initComp2Param.compareRegister = PWMB_H_CCR_REGISTER;
    initComp2Param.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp2Param.compareOutputMode = TIMER_B_OUTPUTMODE_SET_RESET;
    initComp2Param.compareValue = 0; // 0% duty cycle
    Timer_B_initCompareMode(TIMER_B0_BASE, &initComp2Param);

    //Initialze compare registers to generate PWMC_H
    Timer_B_initCompareModeParam initComp3Param = {0};
    initComp3Param.compareRegister = PWMC_H_CCR_REGISTER;
    initComp3Param.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp3Param.compareOutputMode = TIMER_B_OUTPUTMODE_SET_RESET;
    initComp3Param.compareValue = 0; // 0% duty cycle
    Timer_B_initCompareMode(TIMER_B0_BASE, &initComp3Param);
}

inline void SetPwmAPeriod(uint16_t period){
    Timer_B_setCompareValue(TIMER_B0_BASE, PWMA_H_CCR_REGISTER, period);
}

inline void SetPwmBPeriod(uint16_t period){
    Timer_B_setCompareValue(TIMER_B0_BASE, PWMB_H_CCR_REGISTER, period);
}

inline void SetPwmCPeriod(uint16_t period){
    Timer_B_setCompareValue(TIMER_B0_BASE, PWMC_H_CCR_REGISTER, period);
}

void initializeAdcModule(){
    // Configure ADC for motor current sensing
    ADC12_B_initParam adcParam;
    adcParam.clockSourceDivider = ADC12_B_CLOCKDIVIDER_1;
    adcParam.clockSourcePredivider = ADC12_B_CLOCKPREDIVIDER__1;
    adcParam.clockSourceSelect = ADC12_B_CLOCKSOURCE_SMCLK;
    adcParam.internalChannelMap = ADC12_B_MAPINTCH0;
    adcParam.sampleHoldSignalSourceSelect = ADC12_B_SAMPLEHOLDSOURCE_2; // TB0 CCR0

    ADC12_B_init(ADC12_B_BASE, &adcParam);
}

void initializeI2cModule(){
    // Configure I2C interface for slave interface
    EUSCI_B_I2C_initSlaveParam i2cParam;
    i2cParam.slaveAddress = 0x00;
    i2cParam.slaveAddressOffset = 0;
    i2cParam.slaveOwnAddressEnable = 0;

    EUSCI_B_I2C_initSlave(EUSCI_B0_BASE, &i2cParam);
}

void main(void){

    // Turn off the watchdog
    WDT_A_hold(WDT_A_BASE);

    initializeGpios();

    //Set DCO frequency to 16MHz
    CS_setDCOFreq(CS_DCORSEL_1, CS_DCOFSEL_4);

    CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);

    //initializeI2cModule();
    //initializeAdcModule();
    initializePwmModules();

    SetPwmAPeriod(25);
    SetPwmBPeriod(50);
    SetPwmCPeriod(75);

    while(1);

}

// Control Loop
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR (void){
    uint16_t interruptSrc = __even_in_range(TB0IV,14);

}
