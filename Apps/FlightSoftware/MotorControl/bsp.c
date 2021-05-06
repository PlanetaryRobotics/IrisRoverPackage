#include "bsp.h"
#include "driverlib.h"

// initialize all GPIO pins needed for operation
void initializeGpios(){

  // Configure Port 1
  GPIO_setAsInputPin(GPIO_PORT_P1,
                     GPIO_PIN1 /* MC_TRIG */ +
                     GPIO_PIN2 /* SHUNT_AMP_A */);

  GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1,
                                              GPIO_PIN2, /* Phase A current sense */
                                              GPIO_TERNARY_MODULE_FUNCTION);

  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                              GPIO_PIN6 + GPIO_PIN7, /* I2C */
                                              GPIO_SECONDARY_MODULE_FUNCTION);

  // Configure Port 2
  GPIO_setAsInputPin(GPIO_PORT_P2,
                       GPIO_PIN5 + /* V */
                       GPIO_PIN6 /* U */);

  GPIO_setAsOutputPin(GPIO_PORT_P2,
                      GPIO_PIN1 /* LOW_C */ +
                      GPIO_PIN2 /* HIGH_C */);

  // Configure PWM mode for phase A
  GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,
                                              GPIO_PIN2, /*  HIGH_C */
                                              GPIO_PRIMARY_MODULE_FUNCTION);

  // Configure Port 3
  GPIO_setAsInputPin(GPIO_PORT_P3,
                     GPIO_PIN0 /* Phase B current sensing*/ +
                     GPIO_PIN1 /* Phase C current sensing*/);
  GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3,
                                              GPIO_PIN0 + /* Phase B current sensing */
                                              GPIO_PIN1, /* Phase C current sensing */
                                              GPIO_TERNARY_MODULE_FUNCTION);

  // Configure PWM mode for phase B and C
  GPIO_setAsOutputPin(GPIO_PORT_P3,
                      GPIO_PIN4 /* LOW_B */ +
                      GPIO_PIN5 /* HIGH_B */ +
                      GPIO_PIN6 /* LOW_A */ +
                      GPIO_PIN7 /* HIGH_A */);

  GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3,
                                              GPIO_PIN5 + GPIO_PIN7, /* HIGH_B, HIGH_A */
                                              GPIO_PRIMARY_MODULE_FUNCTION);

  // Configure Port 4
  GPIO_setAsInputPin(GPIO_PORT_P4,
                       GPIO_PIN0 /* ADD1 */ +
                       GPIO_PIN1 /* ADD2 */ +
                       GPIO_PIN3 /* HALL_W */ );
  GPIO_setAsOutputPin(GPIO_PORT_P4,
                      GPIO_PIN4 /* CAL */);

  //Configure Port J
  GPIO_setAsInputPin(GPIO_PORT_PJ,
                     GPIO_PIN1 /* DRV_FAULT */);
  GPIO_setAsOutputPin(GPIO_PORT_PJ,
                      GPIO_PIN0 /* ENABLE */);

  // Enable changes to port registers
  PM5CTL0 &= ~LOCKLPM5;
}

/**
 * @brief      Initializes the pwm modules.
 */
void initializePwmModules(void){
  //Start Timer
  Timer_B_initUpDownModeParam initUpDownParam = {0};
  initUpDownParam.clockSource = TIMER_B_CLOCKSOURCE_SMCLK; // 16 MHz
  initUpDownParam.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
  initUpDownParam.timerPeriod = PWM_PERIOD_TICKS;
  initUpDownParam.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
  initUpDownParam.captureCompareInterruptEnable_CCR0_CCIE = TIMER_B_CCIE_CCR0_INTERRUPT_ENABLE;
  initUpDownParam.timerClear = TIMER_B_DO_CLEAR;
  initUpDownParam.startTimer = true;
  Timer_B_initUpDownMode(TIMER_B0_BASE, &initUpDownParam);

  Timer_B_clearCaptureCompareInterrupt(TIMER_B0_BASE,
                                       TIMER_B_CAPTURECOMPARE_REGISTER_0);

  //Initialize compare registers to generate PWMA_H on pins 3.6 & 3.7
  //   First of 3 motor windings (see pins on motor connector or SBC schematic or layout for more info)
  Timer_B_initCompareModeParam initComp1Param = {0};
  initComp1Param.compareRegister = PWMA_H_CCR_REGISTER;
  initComp1Param.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
  initComp1Param.compareOutputMode = TIMER_B_OUTPUTMODE_TOGGLE_SET;
  initComp1Param.compareValue = 0; // 0% duty cycle
  Timer_B_initCompareMode(TIMER_B0_BASE, &initComp1Param);

  //Initialize compare registers to generate PWMB_H on pins 3.4 & 3.5
  //   Second of 3 motor windings (see pins on motor connector or SBC schematic or layout for more info)
  Timer_B_initCompareModeParam initComp2Param = {0};
  initComp2Param.compareRegister = PWMB_H_CCR_REGISTER;
  initComp2Param.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
  initComp2Param.compareOutputMode = TIMER_B_OUTPUTMODE_TOGGLE_SET;
  initComp2Param.compareValue = 0; // 0% duty cycle
  Timer_B_initCompareMode(TIMER_B0_BASE, &initComp2Param);

  //Initialize compare registers to generate PWMC_H on pins 2.1 & 2.2
  //   Third of 3 motor windings (see pins on motor connector or SBC schematic or layout for more info)
  Timer_B_initCompareModeParam initComp3Param = {0};
  initComp3Param.compareRegister = PWMC_H_CCR_REGISTER;
  initComp3Param.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
  initComp3Param.compareOutputMode = TIMER_B_OUTPUTMODE_TOGGLE_SET;
  initComp3Param.compareValue = 0; // 0% duty cycle
  Timer_B_initCompareMode(TIMER_B0_BASE, &initComp3Param);

  // Initialize CCR1 for ADC acquisition
  //    Prepare internal ADC to read from pins (reads from p1.2, p3.0, p3.1)
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
void setPwmAPeriod(uint16_t period){
//  if(period > 65535 || period < 0 || )
  HWREG16(TIMER_B0_BASE + PWMA_H_CCR_REGISTER + OFS_TBxR) = PWM_PERIOD_TICKS - period;
}


/**
 * @brief      Sets the pwm b period.
 *
 * @param[in]  period  The period
 */
void setPwmBPeriod(uint16_t period){
  HWREG16(TIMER_B0_BASE + PWMB_H_CCR_REGISTER + OFS_TBxR) = PWM_PERIOD_TICKS - period;
}

void setPwmCPeriod(uint16_t period){
  HWREG16(TIMER_B0_BASE + PWMC_H_CCR_REGISTER + OFS_TBxR) = PWM_PERIOD_TICKS - period;
}


/**
 * @brief      Enables the half bridge a.
 */
void enableHalfBridgeA(void) {
  P3OUT |= GPIO_PIN6;
}


/**
 * @brief      Disables the half bridge a.
 */
void disableHalfBridgeA(void) {
  P3OUT &= ~GPIO_PIN6;
}


/**
 * @brief      Enables the half bridge b.
 */
void enableHalfBridgeB(void) {
  P3OUT |= GPIO_PIN4;
}


/**
 * @brief      Disables the half bridge b.
 */
void disableHalfBridgeB(void) {
  P3OUT &= ~GPIO_PIN4;
}


/**
 * @brief      Enables the half bridge c.
 */
void enableHalfBridgeC(void) {
  P2OUT |= GPIO_PIN1;
}


/**
 * @brief      Disables the half bridge c.
 */
void disableHalfBridgeC(void) {
  P2OUT &= ~GPIO_PIN1;
}


/**
 * @brief      Disables the calibration.
 */
void disableCalibration(void) {
  GPIO_setOutputLowOnPin(GPIO_PORT_P4,
                         GPIO_PIN4);
}


/**
 * @brief      Enables the calibration.
 */
void enableCalibration(void) {
  GPIO_setOutputHighOnPin(GPIO_PORT_P4,
                          GPIO_PIN4);
}


/**
 * @brief      Enables the gate driver.
 */
void enableGateDriver(void){
    __disable_interrupt();    // entering critical section
  GPIO_setOutputHighOnPin(GPIO_PORT_PJ, GPIO_PIN0);
  __delay_cycles(DELAY_100_ms);
  __enable_interrupt();
}


/**
 * @brief      Disables the gate driver.
 */
void disableGateDriver(void){
  __disable_interrupt();    // entering critical section
  GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_PIN0);
  __delay_cycles(DELAY_100_ms);
  __enable_interrupt();
}
