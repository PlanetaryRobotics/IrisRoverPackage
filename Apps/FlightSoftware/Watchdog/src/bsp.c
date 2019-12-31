#include "include/bsp.h"

/**
 * @brief      Initializes the gpios.
 */
void initializeGpios(){
  // P1 configuration
  P1DIR &= 0x00;  // All bits as input  

  // Configure i2c interface
  P1SEL1 |= BIT6; // P1.6 SDA
  P1SEL1 |= BIT7; // P1.7 SCL

  // P2 configuration
  P2DIR &= 0x00;  
  P2DIR |= BIT2;  // P2.2 output Heater
  P2DIR |= BIT3;  // P2.3 output Motor control reset
  P2DIR |= BIT4;  // P2.4 output Radio ON 
  P2DIR &= ~BIT7;  // P2.7 input Power good 1V2

  P2SEL1 |= BIT0; // P2.0 UCA0TXD
  P2SEL1 |= BIT1; // P2.1 UCA0RXD

  P2SEL1 |= BIT5; // P2.5 UCA1TXD
  P2SEL1 |= BIT6; // P2.6 UCA1RXD

  // P3 configuration
  P3DIR &= 0x00; 
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
  P4DIR &= ~BIT4; // P4.4 input power good input (1V8)
  P4DIR &= ~BIT5; // P4.5 input power good input (3V3)
  P4DIR |= BIT6; // P4.6 output 24V0 enable
  P4DIR &= ~BIT7; // P4.7 input power good input (5V0)
                   
  //Analog input configuration
  P4SEL0 |= BIT0; // P4.0 Analog input 8 (VCC 2V5)
  P4SEL1 |= BIT0; // P4.0 Analog input 8 (VCC 2V5)

  P4SEL0 |= BIT1; // P4.1 Analog input 9 (VCC 2V8)
  P4SEL1 |= BIT1; // P4.1 Analog input 9 (VCC 2V8)

  P4SEL0 |= BIT2; // P4.2 Analog input 10 (VCC 28V0)
  P4SEL1 |= BIT2; // P4.2 Analog input 10 (VCC 28V0)

  P4SEL0 |= BIT3; // P4.3 Analog input 11 (VCC 24V0)
  P4SEL1 |= BIT3; // P4.3 Analog input 11 (VCC 24V0)

  // PJ configuration
  PJDIR &= 0x00;
  PJDIR |= BIT0; // PJ.0 output Hercules ON
  PJDIR |= BIT1; // PJ.1 output FPGA ON
  PJDIR |= BIT2; // PJ.2 output MOTORS ON
  PJDIR &= ~BIT3; // PJ.3 input CHRG 
}


/**
 * @brief      Enables the heater.
 */
inline void enableHeater(){ P2OUT |= BIT2; }

/**
 * @brief      Disables the heater.
 */
inline void disableHeater(){ P2OUT &= ~BIT2; }

/**
 * @brief      Enables the 24 v power rail.
 */
inline void enable24VPowerRail() { P4DIR &= ~BIT6; P4OUT &= ~BIT6; }

/**
 * @brief      Disables the 24 v power rail.
 */
inline void disable24VPowerRail() { P4DIR |= BIT6; P4OUT |= BIT6; }

/**
 * @brief      Releases a hercules reset.
 */
inline void releaseHerculesReset() { P3OUT |= BIT1 + BIT2; }

/**
 * @brief      Sets the hercules reset.
 */
inline void setHerculesReset() { P3OUT &= ~(BIT1 + BIT2); }

/**
 * @brief      Releases a radio reset.
 */
inline void releaseRadioReset() { P3OUT |= BIT3; }

/**
 * @brief      Sets the radio reset.
 */
inline void setRadioReset() { P3OUT &= ~BIT3; }

/**
 * @brief      Releases a fpga reset.
 */
inline void releaseFPGAReset() { P3OUT |= BIT6; }

/**
 * @brief      Sets the fpga reset.
 */
inline void setFPGAReset() { P3OUT &= ~BIT6; }

/**
 * @brief      Power the hercules MCU
 */
inline void powerOnHercules() { PJOUT |= BIT0; }


/**
 * @brief      Power off the hercules MCU
 */
inline void powerOffHercules() { PJOUT &= ~BIT0; }


/**
 * @brief      Power on the FPGA
 */
inline void powerOnFpga() { PJOUT |= BIT1; }

/**
 * @brief      Power off the FPGA
 */
inline void powerOffFpga() { PJOUT &= BIT1; }

/**
 * @brief      Power on the motors
 */
inline void powerOnMotors() { PJOUT |= BIT2; }

/**
 * @brief      Power off the motors
 */
inline void powerOffMotors() { PJOUT &= BIT2; }
