#include "include/bsp.h"

/**
 * @brief      Initializes the adc module.
 */
void initializeAdcModule(void){
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


/* Bit shift even ports
 */
uint16_t pinSelectEvenPorts(uint8_t selectedPort, uint16_t selectedPins) {
//    if((selectedPort & 1) ^ 1) {
//        selectedPins <<= 8;
//    }
    return selectedPins;
}

// unsigned char *port_ids[4];
// /* Get all addresses associated with port configs
//  */
// unsigned char *getPortIDs(uint8_t selectedPort) {

//     switch (selectedPort) {
//       case GPIO_PORT_P1:
//         port_ids[0] = P1DIR;
//         port_ids[1] = P1SEL0;
//         port_ids[2] = P1SEL1;
//         port_ids[3] = P1REN;
//         break;
//       case GPIO_PORT_P2:
//         port_ids[0] = P2DIR;
//         port_ids[1] = P2SEL0;
//         port_ids[2] = P2SEL1;
//         port_ids[3] = P2REN;
//         break;
//       case GPIO_PORT_P3:
//         port_ids[0] = P3DIR;
//         port_ids[1] = P3SEL0;
//         port_ids[2] = P3SEL1;
//         port_ids[3] = P3REN;
//         break;
//       case GPIO_PORT_P4:
//         port_ids[0] = P4DIR;
//         port_ids[1] = P4SEL0;
//         port_ids[2] = P4SEL1;
//         port_ids[3] = P4REN;
//         break;
//       case GPIO_PORT_PJ:
//         port_ids[0] = P5DIR;
//         port_ids[1] = P5SEL0;
//         port_ids[2] = P5SEL1;
//         port_ids[3] = P5REN;
//         break;
//     }
// //    port_ids = [p_dir, p_sel_0, p_sel_1, p_ren];
// //    return port_ids;
// }
// Usage:
    // unsigned char *port_ids = getPortIDs(selectedPort);
    // port_ids[0] &= ~selectedPins;
    // port_ids[1] &= ~selectedPins;
    // port_ids[2] &= ~selectedPins;
    // port_ids[3] &= ~selectedPins;


/* How to set as INPUT
 *    gpio.c :
 *    HWREG16(baseAddress + OFS_PASEL0) &= ~selectedPins;
 *    HWREG16(baseAddress + OFS_PASEL1) &= ~selectedPins;
 *    HWREG16(baseAddress + OFS_PADIR) &= ~selectedPins;
 *    HWREG16(baseAddress + OFS_PAREN) &= ~selectedPins;
 *
 *    manual :
 *    PxSEL0 &= ~selectedPins;
 *    PxSEL1 &= ~selectedPins;
 *    PxDIR &= ~selectedPins;
 *    PxREN &= ~selectedPins;
 */

void set_input(uint8_t selectedPort, uint16_t selectedPins) {

    switch (selectedPort) {
      case GPIO_PORT_P1:
        P1SEL0 &= ~selectedPins;
        P1SEL1 &= ~selectedPins;
        P1DIR &= ~selectedPins;
        P1REN &= ~selectedPins;
        break;

      case GPIO_PORT_P2:
        P2SEL0 &= ~selectedPins;
        P2SEL1 &= ~selectedPins;
        P2DIR &= ~selectedPins;
        P2REN &= ~selectedPins;
        break;

      case GPIO_PORT_P3:
        P3SEL0 &= ~selectedPins;
        P3SEL1 &= ~selectedPins;
        P3DIR &= ~selectedPins;
        P3REN &= ~selectedPins;
        break;

      case GPIO_PORT_P4:
        P4SEL0 &= ~selectedPins;
        P4SEL1 &= ~selectedPins;
        P4DIR &= ~selectedPins;
        P4REN &= ~selectedPins;
        break;

      case GPIO_PORT_PJ:
        PJSEL0 &= ~selectedPins;
        PJSEL1 &= ~selectedPins;
        PJDIR &= ~selectedPins;
        PJREN &= ~selectedPins;
        break;
    }
}

/* How to set as OUTPUT
 *    gpio.c :
 *    HWREG16(baseAddress + OFS_PASEL0) &= ~selectedPins;
 *    HWREG16(baseAddress + OFS_PASEL1) &= ~selectedPins;
 *    HWREG16(baseAddress + OFS_PADIR) |= selectedPins;
 *
 *    manual :
 *    PxSEL0 &= ~selectedPins;
 *    PxSEL1 &= ~selectedPins;
 *    PxDIR |= selectedPins;
 */

void set_output(uint8_t selectedPort, uint16_t selectedPins) {
    switch (selectedPort) {

      case GPIO_PORT_P1:
        P1SEL0 &= ~selectedPins;
        P1SEL1 &= ~selectedPins;
        P1DIR |= selectedPins;
        break;

      case GPIO_PORT_P2:
        P2SEL0 &= ~selectedPins;
        P2SEL1 &= ~selectedPins;
        P2DIR |= selectedPins;
        break;

      case GPIO_PORT_P3:
        P3SEL0 &= ~selectedPins;
        P3SEL1 &= ~selectedPins;
        P3DIR |= selectedPins;
        break;

      case GPIO_PORT_P4:
        P4SEL0 &= ~selectedPins;
        P4SEL1 &= ~selectedPins;
        P4DIR |= selectedPins;
        break;

      case GPIO_PORT_PJ:
        PJSEL0 &= ~selectedPins;
        PJSEL1 &= ~selectedPins;
        PJDIR |= selectedPins;
        break;
    }
}

/* PERIPHERAL OUTPUTS
 *    gpio.c :
 *    HWREG16(baseAddress + OFS_PADIR) |= selectedPins;
 *    case GPIO_PRIMARY_MODULE_FUNCTION:
 *        HWREG16(baseAddress + OFS_PASEL0) |= selectedPins;
 *        HWREG16(baseAddress + OFS_PASEL1) &= ~selectedPins;
 *    case GPIO_SECONDARY_MODULE_FUNCTION:
 *        HWREG16(baseAddress + OFS_PASEL0) &= ~selectedPins;
 *        HWREG16(baseAddress + OFS_PASEL1) |= selectedPins;
 *    case GPIO_TERNARY_MODULE_FUNCTION:
 *        HWREG16(baseAddress + OFS_PASEL0) |= selectedPins;
 *        HWREG16(baseAddress + OFS_PASEL1) |= selectedPins;
 *
 *    manual :
 *    PxDIR |= selectedPins;
 *    case GPIO_PRIMARY_MODULE_FUNCTION:
 *        PxSEL0 |= selectedPins;
 *        PxSEL1 &= ~selectedPins;
 *    case GPIO_SECONDARY_MODULE_FUNCTION:
 *        PxSEL0 &= ~selectedPins;
 *        PxSEL1 |= selectedPins;
 *    case GPIO_TERNARY_MODULE_FUNCTION:
 *        PxSEL0 |= selectedPins;
 *        PxSEL1 |= selectedPins;
 */

void set_primary(uint8_t selectedPort, uint16_t selectedPins, uint8_t output) {

  switch (selectedPort) {
      case GPIO_PORT_P1:
          if (output) {
              P1DIR |= selectedPins;
          } else {
              P1DIR &= ~selectedPins;
          }
          P1SEL0 |= selectedPins;
          P1SEL1 &= ~selectedPins;
          break;

      case GPIO_PORT_P2:
          if (output) {
              P2DIR |= selectedPins;
          } else {
              P2DIR &= ~selectedPins;
          }
          P2SEL0 |= selectedPins;
          P2SEL1 &= ~selectedPins;
          break;

      case GPIO_PORT_P3:
          if (output) {
              P3DIR |= selectedPins;
          } else {
              P3DIR &= ~selectedPins;
          }
          P3SEL0 |= selectedPins;
          P3SEL1 &= ~selectedPins;
          break;

      case GPIO_PORT_P4:
          if (output) {
              P4DIR |= selectedPins;
          } else {
              P4DIR &= ~selectedPins;
          }
          P4SEL0 |= selectedPins;
          P4SEL1 &= ~selectedPins;
          break;

      case GPIO_PORT_PJ:
          if (output) {
              PJDIR |= selectedPins;
          } else {
              PJDIR &= ~selectedPins;
          }
          PJSEL0 |= selectedPins;
          PJSEL1 &= ~selectedPins;
          break;
    }

}

void set_secondary(uint8_t selectedPort, uint16_t selectedPins, uint8_t output) {

  switch (selectedPort) {
      case GPIO_PORT_P1:
          if (output) {
              P1DIR |= selectedPins;
          } else {
              P1DIR &= ~selectedPins;
          }
          P1SEL0 &= ~selectedPins;
          P1SEL1 |= selectedPins;
          break;

      case GPIO_PORT_P2:
          if (output) {
              P2DIR |= selectedPins;
          } else {
              P2DIR &= ~selectedPins;
          }
          P2SEL0 &= ~selectedPins;
          P2SEL1 |= selectedPins;
          break;

      case GPIO_PORT_P3:
          if (output) {
              P3DIR |= selectedPins;
          } else {
              P3DIR &= ~selectedPins;
          }
          P3SEL0 &= ~selectedPins;
          P3SEL1 |= selectedPins;
          break;

      case GPIO_PORT_P4:
          if (output) {
              P4DIR |= selectedPins;
          } else {
              P4DIR &= ~selectedPins;
          }
          P4SEL0 &= ~selectedPins;
          P4SEL1 |= selectedPins;
          break;

      case GPIO_PORT_PJ:
          if (output) {
              PJDIR |= selectedPins;
          } else {
              PJDIR &= ~selectedPins;
          }
          PJSEL0 &= ~selectedPins;
          PJSEL1 |= selectedPins;
          break;
    }

}

void set_ternary(uint8_t selectedPort, uint16_t selectedPins, uint8_t output) {

  switch (selectedPort) {
      case GPIO_PORT_P1:
          if (output) {
              P1DIR |= selectedPins;
          } else {
              P1DIR &= ~selectedPins;
          }
          P1SEL0 |= selectedPins;
          P1SEL1 |= selectedPins;
          break;
      case GPIO_PORT_P2:
          if (output) {
              P2DIR |= selectedPins;
          } else {
              P2DIR &= ~selectedPins;
          }
          P2SEL0 |= selectedPins;
          P2SEL1 |= selectedPins;
          break;
      case GPIO_PORT_P3:
          if (output) {
              P3DIR |= selectedPins;
          } else {
              P3DIR &= ~selectedPins;
          }
          P3SEL0 |= selectedPins;
          P3SEL1 |= selectedPins;
          break;
      case GPIO_PORT_P4:
          if (output) {
              P4DIR |= selectedPins;
          } else {
              P4DIR &= ~selectedPins;
          }
          P4SEL0 |= selectedPins;
          P4SEL1 |= selectedPins;
          break;
      case GPIO_PORT_PJ:
          if (output) {
              PJDIR |= selectedPins;
          } else {
              PJDIR &= ~selectedPins;
          }
          PJSEL0 |= selectedPins;
          PJSEL1 |= selectedPins;
          break;
    }

}

void set_peripheral_output(uint8_t selectedPort, uint16_t selectedPins, uint8_t mode) {

    int io = 1; // output
    switch (mode) {
      case GPIO_PRIMARY_MODULE_FUNCTION:
          set_primary(selectedPort, selectedPins, io);
          break;
      case GPIO_SECONDARY_MODULE_FUNCTION:
          set_secondary(selectedPort, selectedPins, io);
          break;
      case GPIO_TERNARY_MODULE_FUNCTION:
          set_ternary(selectedPort, selectedPins, io); 
          break;
    }
}

/* PERIPHERAL INPUTS
 *    gpio.c :
 *    HWREG16(baseAddress + OFS_PADIR) &= ~selectedPins;
 *    case GPIO_PRIMARY_MODULE_FUNCTION:
 *        HWREG16(baseAddress + OFS_PASEL0) |= selectedPins;
 *        HWREG16(baseAddress + OFS_PASEL1) &= ~selectedPins;
 *    case GPIO_SECONDARY_MODULE_FUNCTION:
 *        HWREG16(baseAddress + OFS_PASEL0) &= ~selectedPins;
 *        HWREG16(baseAddress + OFS_PASEL1) |= selectedPins;
 *    case GPIO_TERNARY_MODULE_FUNCTION:
 *        HWREG16(baseAddress + OFS_PASEL0) |= selectedPins;
 *        HWREG16(baseAddress + OFS_PASEL1) |= selectedPins;
 *
 *    manual :
 *    PxDIR &= ~selectedPins;
 *    case GPIO_PRIMARY_MODULE_FUNCTION:
 *        PxSEL0 |= selectedPins;
 *        PxSEL1 &= ~selectedPins;
 *    case GPIO_SECONDARY_MODULE_FUNCTION:
 *        PxSEL0 &= ~selectedPins;
 *        PxSEL1 |= selectedPins;
 *    case GPIO_TERNARY_MODULE_FUNCTION:
 *        PxSEL0 |= selectedPins;
 *        PxSEL1 |= selectedPins;
 */

void set_peripheral_input(uint8_t selectedPort, uint16_t selectedPins, uint8_t mode) {

    int io = 0; // input
    switch (mode) {
      case GPIO_PRIMARY_MODULE_FUNCTION:
          set_primary(selectedPort, selectedPins, io);
          break;
      case GPIO_SECONDARY_MODULE_FUNCTION:
          set_secondary(selectedPort, selectedPins, io);
          break;
      case GPIO_TERNARY_MODULE_FUNCTION:
          set_ternary(selectedPort, selectedPins, io); 
          break;
    }
}

/* For each port:
    // Inputs
    // Outputs

    // Primary Function Outputs
    // Secondary Function Outputs
    // Ternary Function Outputs

    // Primary Function Inputs
    // Secondary Function Inputs
    // Ternary Function Inputs
 */


// --------- INIT GPIOS ---------
// initialize all GPIO pins needed for operation
void initializeGpios() {

/* --------------------------------------
 * ---------- Configure Port 1 ----------
 * --------------------------------------
 */

//old code
//GPIO_setAsInputPin(GPIO_PORT_P1,
//                   GPIO_PIN1 + // MC_TRIG
//                   GPIO_PIN2   // SHUNT_AMP_A
//                   );
//GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1,
//                                            GPIO_PIN2, // Phase A current sense
//                                            GPIO_TERNARY_MODULE_FUNCTION);
//GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
//                                           GPIO_PIN6 + GPIO_PIN7, // I2C
//                                           GPIO_SECONDARY_MODULE_FUNCTION);


    P1DIR = 0x00u;
    P1OUT = 0x00u;
    P1SEL0 = 0x00u;
    P1SEL1 = 0x00u;

    uint8_t selectedPort = GPIO_PORT_P1;
    uint16_t selectedPins = 0;

    // Inputs
    // P1.1   MC_TRIG
    // P1.2   SHUNT_AMP_A
    selectedPins = GPIO_PIN1 + GPIO_PIN2;
    selectedPins = pinSelectEvenPorts(GPIO_PORT_P1, selectedPins);
    set_input(GPIO_PORT_P1, selectedPins);
    selectedPins = 0;

    // Outputs
    // Primary Function Outputs
    // Secondary Function Outputs

    // Ternary Function Outputs
    // P1.2   Phase A current sense
    selectedPins = GPIO_PIN2;
    selectedPins = pinSelectEvenPorts(GPIO_PORT_P1, selectedPins);
    set_peripheral_output(GPIO_PORT_P1, selectedPins, GPIO_TERNARY_MODULE_FUNCTION);
    selectedPins = 0;

    // Primary Function Inputs

    // Secondary Function Inputs
    // P1.6 I2C SDA
    // P1.7 I2c SCL
    selectedPins = GPIO_PIN6 + GPIO_PIN7;
    selectedPins = pinSelectEvenPorts(GPIO_PORT_P1, selectedPins);
    set_peripheral_input(GPIO_PORT_P1, selectedPins, GPIO_SECONDARY_MODULE_FUNCTION);
    selectedPins = 0;

    // Ternary Function Inputs

/* --------------------------------------
 * ---------- Configure Port 2 ----------
 * --------------------------------------
 */

//old code
//GPIO_setAsInputPin(GPIO_PORT_P2,
//                   GPIO_PIN5 + // V
//                   GPIO_PIN6 // U
//                   );
//
//GPIO_setAsOutputPin(GPIO_PORT_P2,
//                    GPIO_PIN1 + // LOW_C
//                    GPIO_PIN2   // HIGH_C
//                    );
//
//// Configure PWM mode for phase C
//GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,
//                                            GPIO_PIN2, //  HIGH_C
//                                            GPIO_PRIMARY_MODULE_FUNCTION);

    P2DIR = 0x00u;
    P2OUT = 0x00u;
    P2SEL0 = 0x00u;
    P2SEL1 = 0x00u;

    selectedPort = GPIO_PORT_P2;
    selectedPins = 0;

    // Inputs
    // P1.5   V
    // P1.6   U
    selectedPins = GPIO_PIN5 + GPIO_PIN6;
    selectedPins = pinSelectEvenPorts(GPIO_PORT_P2, selectedPins);
    set_input(GPIO_PORT_P2, selectedPins);
    selectedPins = 0;

    // Outputs
    // P1.1   LOW_C
    // P1.2   HIGH_C
    selectedPins = GPIO_PIN1 + GPIO_PIN2;
    selectedPins = pinSelectEvenPorts(GPIO_PORT_P2, selectedPins);
    set_output(GPIO_PORT_P2, selectedPins);
    selectedPins = 0;

    // Primary Function Outputs
    // P1.2   HIGH_C
    selectedPins = GPIO_PIN2;
    selectedPins = pinSelectEvenPorts(GPIO_PORT_P2, selectedPins);
    set_peripheral_output(GPIO_PORT_P2, selectedPins, GPIO_PRIMARY_MODULE_FUNCTION);
    selectedPins = 0;

    // Secondary Function Outputs
    // Ternary Function Outputs

    // Primary Function Inputs
    // Secondary Function Inputs
    // Ternary Function Inputs

/* --------------------------------------
 * ---------- Configure Port 3 ----------
 * --------------------------------------
 */

//old code
//GPIO_setAsInputPin(GPIO_PORT_P3,
//                   GPIO_PIN0 + // Phase B current sensing
//                   GPIO_PIN1 // Phase C current sensing
//                  );
//GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3,
//                   GPIO_PIN0 + // Phase B current sensing
//                   GPIO_PIN1,  // Phase C current sensing
//                   GPIO_TERNARY_MODULE_FUNCTION
//                   );
//GPIO_setAsOutputPin(GPIO_PORT_P3,
//                    GPIO_PIN4 + // LOW_B
//                    GPIO_PIN5 + // HIGH_B
//                    GPIO_PIN6 + // LOW_A
//                    GPIO_PIN7 + // HIGH_A
//                    );
//GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3,
//                   GPIO_PIN5 + GPIO_PIN7, // HIGH_B, HIGH_A
//                   GPIO_PRIMARY_MODULE_FUNCTION
//                   );

    P3DIR = 0x00u;
    P3OUT = 0x00u;
    P3SEL0 = 0x00u;
    P3SEL1 = 0x00u;

    selectedPort = GPIO_PORT_P3;
    selectedPins = 0;

    // Inputs
    // P3.0   SHUNT_AMP_B
    // P3.1   SHUNT_AMP_C

    selectedPins = GPIO_PIN0 + GPIO_PIN1;
    selectedPins = pinSelectEvenPorts(GPIO_PORT_P3, selectedPins);
    set_input(GPIO_PORT_P3, selectedPins);
    selectedPins = 0;

    // Outputs
    // P3.4
    // P3.5
    // P3.6
    // P3.7
    // Configure PWM mode for phase B and A

    selectedPins = GPIO_PIN4 + GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7;
    selectedPins = pinSelectEvenPorts(GPIO_PORT_P3, selectedPins);
    set_output(GPIO_PORT_P3, selectedPins);
    selectedPins = 0;

    // Primary Function Outputs
    // P3.5
    // P3.7

    selectedPins = GPIO_PIN5 + GPIO_PIN7;
    selectedPins = pinSelectEvenPorts(GPIO_PORT_P3, selectedPins);
    set_peripheral_output(GPIO_PORT_P3, selectedPins, GPIO_PRIMARY_MODULE_FUNCTION);
    selectedPins = 0;

    // Secondary Function Outputs

    // Ternary Function Outputs
    // P3.0   Phase B current sensing
    // P3.1   Phase C current sensing

    selectedPins = GPIO_PIN0 + GPIO_PIN1;
    selectedPins = pinSelectEvenPorts(GPIO_PORT_P3, selectedPins);
    set_peripheral_output(GPIO_PORT_P3, selectedPins, GPIO_TERNARY_MODULE_FUNCTION);
    selectedPins = 0;

    // Primary Function Inputs
    // Secondary Function Inputs
    // Ternary Function Inputs


/* --------------------------------------
 * ---------- Configure Port 4 ----------
 * --------------------------------------
 */

  // GPIO_setAsInputPin(GPIO_PORT_P4,
  //                      GPIO_PIN0 /* ADD1 */ +
  //                      GPIO_PIN1 /* ADD2 */ +
  //                      GPIO_PIN3 /* HALL_W */ );
  // GPIO_setAsOutputPin(GPIO_PORT_P4,
  //                     GPIO_PIN4 /* CAL */);

    P4DIR = 0x00u;
    P4OUT = 0x00u;
    P4SEL0 = 0x00u;
    P4SEL1 = 0x00u;

    selectedPort = GPIO_PORT_P4;
    selectedPins = 0;

    // Inputs
    // P4.0   ADD1
    // P4.1   ADD2
    // P4.3   HALL_W

    selectedPins = GPIO_PIN0 + GPIO_PIN1 + GPIO_PIN3;
    selectedPins = pinSelectEvenPorts(GPIO_PORT_P4, selectedPins);
    set_input(GPIO_PORT_P4, selectedPins);
    selectedPins = 0;

    // Outputs
    // P4.4   CAL

    selectedPins = GPIO_PIN4;
    selectedPins = pinSelectEvenPorts(GPIO_PORT_P4, selectedPins);
    set_output(GPIO_PORT_P4, selectedPins);
    selectedPins = 0;

    // Primary Function Outputs
    // Secondary Function Outputs
    // Ternary Function Outputs

    // Primary Function Inputs
    // Secondary Function Inputs
    // Ternary Function Inputs

/* --------------------------------------
 * ---------- Configure Port J ----------
 * --------------------------------------
 */
  // GPIO_setAsInputPin(GPIO_PORT_PJ,
  //                    GPIO_PIN1 /* DRV_FAULT */);
  // GPIO_setAsOutputPin(GPIO_PORT_PJ,
  //                     GPIO_PIN0 /* ENABLE */);

    PJDIR = 0xFFu;
    PJOUT = 0x00u;
    PJSEL0 = 0x00u;
    PJSEL1 = 0x00u;

    selectedPort = GPIO_PORT_PJ;
    selectedPins = 0;

    // Inputs
    // PJ.1   DRV_FAULT

    selectedPins = GPIO_PIN1;
    selectedPins = pinSelectEvenPorts(GPIO_PORT_PJ, selectedPins);
    set_input(selectedPort, selectedPins);
    selectedPins = 0;

    // Outputs
    // PJ.0   ENABLE

    selectedPins = GPIO_PIN0;
    selectedPins = pinSelectEvenPorts(GPIO_PORT_PJ, selectedPins);
    set_output(selectedPort, selectedPins);
    selectedPins = 0;

    // Primary Function Outputs
    // Secondary Function Outputs
    // Ternary Function Outputs

    // Primary Function Inputs
    // Secondary Function Inputs
    // Ternary Function Inputs

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
 * @brief      Generate the pwm period for the 6 pwm channels
 *
 * @param[in]  commutation  The commutation
 * @param[in]  dutyCycle    The duty cycle
 */
void pwmGenerator(const uint8_t commutation, _iq dutyCycle){
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
 * @brief      Sets the pwm a period.
 *
 * @param[in]  period  The period
 */
void setPwmAPeriod(uint16_t period){
    TB0CCR6 = PWM_PERIOD_TICKS - period;
}


/**
 * @brief      Sets the pwm b period.
 *
 * @param[in]  period  The period
 */
void setPwmBPeriod(uint16_t period){
  TB0CCR4 = PWM_PERIOD_TICKS - period;
}

void setPwmCPeriod(uint16_t period){
  TB0CCR2 = PWM_PERIOD_TICKS - period;
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
//  GPIO_setOutputLowOnPin(GPIO_PORT_P4,
//                         GPIO_PIN4);
  P4OUT &= ~GPIO_PIN4;
}


/**
 * @brief      Enables the calibration.
 */
void enableCalibration(void) {
//  GPIO_setOutputHighOnPin(GPIO_PORT_P4,
//                          GPIO_PIN4);
  P4OUT |= GPIO_PIN4;
}


/**
 * @brief      Enables the gate driver.
 */
void enableGateDriver(void){
    __disable_interrupt();    // entering critical section
//  GPIO_setOutputHighOnPin(GPIO_PORT_PJ, GPIO_PIN0);
    PJOUT |= GPIO_PIN0;
  __delay_cycles(DELAY_100_ms);
  __enable_interrupt();
}


/**
 * @brief      Disables the gate driver.
 */
void disableGateDriver(void){
  __disable_interrupt();    // entering critical section
//  GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_PIN0);
  PJOUT &= ~GPIO_PIN0;
  __delay_cycles(DELAY_100_ms);
  __enable_interrupt();
}

/**
 * @brief      Clears the DRV8304 Driver Fault Register.
 */
void clear_driver_fault(void){
    __disable_interrupt(); // entering critical section
    // Pull high first so you can then pull it low:
//    GPIO_setOutputHighOnPin(GPIO_PORT_PJ, GPIO_PIN0);
    PJOUT |= GPIO_PIN0;
    __delay_cycles(DELAY_100_ms);
    // Reset Fault Register by pulsing ENABLE for 5-32us (18.5us):
//    GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_PIN0);
    PJOUT &= ~GPIO_PIN0;
    __delay_cycles(296);    // 18.5 us
//    GPIO_setOutputHighOnPin(GPIO_PORT_PJ, GPIO_PIN0);
    PJOUT |= GPIO_PIN0;
    __enable_interrupt();
}
/**
 * @brief      Reads the whether the DRV8304 driver is in a "fault condition" (and should be cleared). Active low.
 */
bool read_driver_fault(void){
    return !(PJIN & 0x02);
}
