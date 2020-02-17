#include "bsp.h"
#include "driverlib.h"

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

