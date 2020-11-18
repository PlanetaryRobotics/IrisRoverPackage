#include "include/bsp.h"

#define PROGRAM_MOTOR_CONTROLLERS

/**
 * @brief      Initializes the gpios.
 */
void initializeGpios(){
  // P1 configuration
  P1DIR &= 0x00;  // All bits as input
  P1OUT &= ~(BIT4 | BIT5); // Initially everything is off
#ifndef PROGRAM_MOTOR_CONTROLLERS
  P1DIR |= BIT4;  // P1.4 output Motor control reset B
  P1DIR |= BIT5;  // P1.5 output Motor control reset C
#endif

  // UART configuration done in uart_init()

  // Configure i2c interface
  P1SEL1 |= BIT6; // P1.6 SDA
  P1SEL1 |= BIT7; // P1.7 SCL

  /*TODO: It may be better coding conventions to use the respective
   * on/off functions than to set P2OUT directly, in case things change */
  // P2 configuration
  P2DIR &= 0x00;
  P2OUT &= ~(BIT2 | BIT3 | BIT4); // Initially everything is off
  P2DIR |= BIT2;  // P2.2 output Heater
#ifndef PROGRAM_MOTOR_CONTROLLERS
  P2DIR |= BIT3;  // P2.3 output Motor control reset A
#endif
  P2DIR |= BIT4;  // P2.4 output Radio ON
  P2DIR &= ~BIT7;  // P2.7 input Power good 1V2

  // P3 configuration
  P3DIR &= 0x00;
  P3OUT &= ~(BIT1 | BIT2 | BIT3 | BIT4 | BIT6 | BIT7); // Initially everything is off
  P3DIR &= ~BIT0;  // P3.0 input battery
  P3DIR |= BIT1; // P3.1 output Hercules POR
  P3DIR |= BIT2; // P3.2 output Hercules Reset
  P3DIR |= BIT3; // P3.3 output Radio Reset
  P3DIR |= BIT4; // P3.4 output deployment
  P3DIR &= ~BIT5; // P3.5 input FPGA kick
  P3DIR |= BIT6; // P3.6 output FPGA reset
  P3DIR |= BIT7; // P3.7 output 3V3 enable

  // P4 configuration
  P4DIR &= 0x00;
  P4OUT |= BIT6; // Initially everything is off (NOTE: 24V0 OFF = HI)
  P4DIR &= ~BIT4; // P4.4 input power good input (1V8)
  P4DIR &= ~BIT5; // P4.5 input power good input (3V3)
  P4DIR |= BIT6; // P4.6 output 24V0 enable
  P4DIR &= ~BIT7; // P4.7 input power good input (5V0)

  //Analog input configuration done in adc_init()

  // PJ configuration
  PJOUT &= ~(BIT0 | BIT1 | BIT2 | BIT4 | BIT5); // Initially everything is off
  PJDIR &= 0x00;
  PJDIR |= BIT0; // PJ.0 output Hercules ON
  PJDIR |= BIT1; // PJ.1 output FPGA ON
  PJDIR |= BIT2; // PJ.2 output MOTORS ON
#ifndef PROGRAM_MOTOR_CONTROLLERS
  PJDIR |= BIT4; // PJ.4 output Motor control reset D
#endif
  PJDIR |= BIT5; // PJ.5 output BATTERY
  PJDIR &= ~BIT3; // PJ.3 input CHRG
}

/**
 * @brief      Enables the heater. (HI = ON)
 */
inline void enableHeater(){ P2OUT |= BIT2; }

/**
 * @brief      Disables the heater. (LO = OFF)
 */
inline void disableHeater(){ P2OUT &= ~BIT2; }

/**
 * @brief      Enables the 3.3 v power rail. (HI = ON)
 */
inline void enable3V3PowerRail() { P3OUT |= BIT7; }

/**
 * @brief      Disables the 3.3 v power rail. (LO = OFF)
 */
inline void disable3V3PowerRail() { P3OUT &= ~BIT7; }

/**
 * @brief      Enables the 24 v power rail. (LO = ON)
 */
inline void enable24VPowerRail() { P4OUT &= ~BIT6; }

/**
 * @brief      Disables the 24 v power rail. (HI = OFF)
 */
inline void disable24VPowerRail() { P4OUT |= BIT6; }

/**
 * @brief      Releases a hercules reset. (HI = NORMAL)
 */
inline void releaseHerculesReset() { P3OUT |= BIT1 | BIT2; }

/**
 * @brief      Sets the hercules reset. (LO = RESET)
 */
inline void setHerculesReset() { P3OUT &= ~(BIT1 | BIT2); }

/**
 * @brief      Releases a radio reset. (HI = NORMAL)
 */
inline void releaseRadioReset() { P3OUT |= BIT3; }

/**
 * @brief      Sets the radio reset. (LO = RESET)
 */
inline void setRadioReset() { P3OUT &= ~BIT3; }

/**
 * @brief      Releases a fpga reset. (HI = NORMAL)
 */
inline void releaseFPGAReset() { P3OUT |= BIT6; }

/**
 * @brief      Sets the fpga reset. (LO = RESET)
 */
inline void setFPGAReset() { P3OUT &= ~BIT6; }

/**
 * @brief      Releases the motor resets. (HI = NORMAL)
 */
inline void releaseMotorsReset() {
#ifndef PROGRAM_MOTOR_CONTROLLERS
    P1OUT |= BIT4 | BIT5; P2OUT |= BIT3; PJOUT |= BIT4;
#endif
}

/**
 * @brief      Sets the motors to reset. (LO = RESET)
 */
inline void setMotorsReset() {
#ifndef PROGRAM_MOTOR_CONTROLLERS
    P1OUT &= ~(BIT4 | BIT5); P2OUT &= ~BIT3; PJOUT &= ~BIT4;
#endif
}

/**
 * @brief      Power the hercules MCU (HI = ON)
 */
inline void powerOnHercules() { PJOUT |= BIT0; }

/**
 * @brief      Power off the hercules MCU (LO = OFF)
 */
inline void powerOffHercules() { PJOUT &= ~BIT0; }

/**
 * @brief      Power on the Radio (HI = ON)
 */
inline void powerOnRadio() { P2OUT |= BIT4; }

/**
 * @brief      Power off the Radio (LO = OFF)
 */
inline void powerOffRadio() { P2OUT &= ~BIT4; }

/**
 * @brief      Power on the FPGA (HI = ON)
 */
inline void powerOnFpga() { PJOUT |= BIT1; }

/**
 * @brief      Power off the FPGA (LO = OFF)
 */
inline void powerOffFpga() { PJOUT &= ~BIT1; }

/**
 * @brief      Power on the motors (HI = ON)
 */
inline void powerOnMotors() { PJOUT |= BIT2; }

/**
 * @brief      Power off the motors (LO = OFF)
 */
inline void powerOffMotors() { PJOUT &= ~BIT2; }

/**
 * @brief      Enable the batteries (HI = ON)
 */
inline void enableBatteries() { PJOUT |= BIT5; }

/**
 * @brief      Disable the batteries (LO = OFF)
 */
inline void disableBatteries() { PJOUT &= ~BIT5; }

