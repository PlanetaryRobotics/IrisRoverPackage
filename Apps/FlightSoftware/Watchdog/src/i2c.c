#include "include/i2c.h"

extern float battery_charge;
extern float battery_voltage;
extern float battery_current;
extern float fuel_gauge_temp;
int32_t raw_battery_charge;
int32_t raw_battery_voltage;
int32_t raw_battery_current;
int32_t raw_fuel_gauge_temp;

extern uint8_t fuel_gauge_status_reg;
uint8_t control_reg;

/*
 * File for interfacing with I2C protocol hardware module
 *
 * I2C communication uses eUSCI_B0 (power circuitry)
 */

/* Used to track the state of the software state machine*/
I2C_Mode MasterMode = IDLE_MODE;

/* The Register Address/Command to use*/
uint8_t TransmitRegAddr = 0;

// initialize buffers and their corresponding byte counters
uint8_t ReceiveBuffer[I2C_RX_BUFFER_MAX_SIZE] = {0};
uint8_t RXByteCtr = 0;
uint8_t ReceiveIndex = 0;
uint8_t TransmitBuffer[I2C_TX_BUFFER_MAX_SIZE] = {0};
uint8_t TXByteCtr = 0;
uint8_t TransmitIndex = 0;

/* uart0 rx handler (will only be called after SLIP packet fully received) */
void (*i2c_rx_handler)(uint16_t len, struct buffer *buf) = (void *)(0);


/* init function */
void i2c_init() {
    // Configure i2c interface
    // TODO: are these correct?
    P1SEL1 |= BIT6; // P1.6 SDA
    P1SEL1 |= BIT7; // P1.7 SCL

    // may want to try this for pins:
//    P7SEL0 |= BIT0 | BIT1;
//    P7SEL1 &= ~(BIT0 | BIT1);

    UCB2CTLW0 = UCSWRST;                      // Enable SW reset
    UCB2CTLW0 |= UCMODE_3 | UCMST | UCSSEL__SMCLK | UCSYNC; // I2C master mode, SMCLK
    UCB2BRW = 160;                            // fSCL = SMCLK/160 = ~100kHz
    UCB2I2CSA = I2C_SLAVE_ADDR;                   // Slave Address
    UCB2CTLW0 &= ~UCSWRST;                    // Clear SW reset, resume operation
    UCB2IE |= UCNACKIE;
}


/* For slave device with dev_addr, read the data specified in slaves reg_addr.
 * The received data is available in ReceiveBuffer
 *
 * dev_addr: The slave device address.
 *           Example: I2C_SLAVE_ADDR
 * reg_addr: The register or command to send to the slave.
 *           Example: CMD_TYPE_0_SLAVE
 * count: The length of data to read
 *           Example: TYPE_0_LENGTH
 *  */
I2C_Mode I2C_Master_ReadReg(uint8_t dev_addr, uint8_t reg_addr, uint8_t count)
{
    /* Initialize state machine */
    MasterMode = TX_REG_ADDRESS_MODE;
    TransmitRegAddr = reg_addr;
    RXByteCtr = count;
    TXByteCtr = 0;
    ReceiveIndex = 0;
    TransmitIndex = 0;

    /* Initialize slave address and interrupts */
    UCB2I2CSA = dev_addr;
    UCB2IFG &= ~(UCTXIFG + UCRXIFG);       // Clear any pending interrupts
    UCB2IE &= ~UCRXIE;                       // Disable RX interrupt
    UCB2IE |= UCTXIE;                        // Enable TX interrupt

    UCB2CTLW0 |= UCTR + UCTXSTT;             // I2C TX, start condition
    __bis_SR_register(LPM0_bits + GIE);              // Enter LPM0 w/ interrupts

    return MasterMode;

}

/* For slave device with dev_addr, writes the data specified in *reg_data
 *
 * dev_addr: The slave device address.
 *           Example: I2C_SLAVE_ADDR
 * reg_addr: The register or command to send to the slave.
 *           Example: CMD_TYPE_0_MASTER
 * *reg_data: The buffer to write
 *           Example: MasterType0
 * count: The length of *reg_data
 *           Example: TYPE_0_LENGTH
 *  */
I2C_Mode I2C_Master_WriteReg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t count)
{
    /* Initialize state machine */
    MasterMode = TX_REG_ADDRESS_MODE;
    TransmitRegAddr = reg_addr;

    //Copy register data to TransmitBuffer
    CopyArray(reg_data, TransmitBuffer, count);

    TXByteCtr = count;
    RXByteCtr = 0;
    ReceiveIndex = 0;
    TransmitIndex = 0;

    /* Initialize slave address and interrupts */
    UCB2I2CSA = dev_addr;
    UCB2IFG &= ~(UCTXIFG + UCRXIFG);       // Clear any pending interrupts
    UCB2IE &= ~UCRXIE;                       // Disable RX interrupt
    UCB2IE |= UCTXIE;                        // Enable TX interrupt

    UCB2CTLW0 |= UCTR + UCTXSTT;             // I2C TX, start condition
    __bis_SR_register(LPM0_bits + GIE);              // Enter LPM0 w/ interrupts

    return MasterMode;
}

void CopyArray(uint8_t *source, uint8_t *dest, uint8_t count)
{
    uint8_t copyIndex = 0;
    for (copyIndex = 0; copyIndex < count; copyIndex++)
    {
        dest[copyIndex] = source[copyIndex];
    }
}

void updateGaugeReadings(){
    // set ADC to read voltage/curr/temp every 10 sec
    control_reg = 0b01101001;
    // set control_reg[7:6] to 01 do one conversion, 10 to convert every 10s,
    //      set to 00 to sleep, set to 11 to continuously convert
    // set control_reg[5:3] to 101 for M of 1024 for coulomb counter (see datasheet)
    // control_ref[2:1] not used on SBC (pin its related to is floating)
    // set control_reg[0] to 0 to drastically reduce current consumption (no conversions though)
    I2C_Master_WriteReg(I2C_SLAVE_ADDR, CONTROL, &control_reg, 1);
}

void fuelGaugeLowPower(){
    // shut off all analog parts of fuel gauge circuit by setting LSB of control register to 0
    control_reg = 0b00101000;
    I2C_Master_WriteReg(I2C_SLAVE_ADDR, CONTROL, &control_reg, 1);
}

void readBatteryCharge(){
    I2C_Master_ReadReg(I2C_SLAVE_ADDR, ACCUMULATED_CHARGE_LSB, I2C_RX_BUFFER_MAX_SIZE);
    CopyArray((uint8_t*)ReceiveBuffer, (uint8_t*)&raw_battery_charge, I2C_RX_BUFFER_MAX_SIZE);
    raw_battery_charge = raw_battery_charge >> 16; //shift to make space for 2 MSB bytes
    I2C_Master_ReadReg(I2C_SLAVE_ADDR, ACCUMULATED_CHARGE_MSB, I2C_RX_BUFFER_MAX_SIZE);
    CopyArray((uint8_t*)ReceiveBuffer, (uint8_t*)&raw_battery_charge, I2C_RX_BUFFER_MAX_SIZE);

    // charge as a percent of maximum battery capacity
    battery_charge = 100 * raw_battery_charge / 41177;
}

void readBatteryVoltage(){
    I2C_Master_ReadReg(I2C_SLAVE_ADDR, VOLTAGE_LSB, I2C_RX_BUFFER_MAX_SIZE);
    CopyArray((uint8_t*)ReceiveBuffer, (uint8_t*)&raw_battery_voltage, I2C_RX_BUFFER_MAX_SIZE);
    raw_battery_voltage = raw_battery_voltage >> 16; //shift to make space for 2 MSB bytes
    I2C_Master_ReadReg(I2C_SLAVE_ADDR, VOLTAGE_MSB, I2C_RX_BUFFER_MAX_SIZE);
    CopyArray((uint8_t*)ReceiveBuffer, (uint8_t*)&raw_battery_voltage, I2C_RX_BUFFER_MAX_SIZE);

    // convert raw reading to decimal in Volts
    battery_voltage = 70.8 * (raw_battery_voltage >> 16); // shift instead of dividing by 65535;
}

void readBatteryCurrent(){
    I2C_Master_ReadReg(I2C_SLAVE_ADDR, CURRENT_LSB, I2C_RX_BUFFER_MAX_SIZE);
    CopyArray((uint8_t*)ReceiveBuffer, (uint8_t*)&raw_battery_current, I2C_RX_BUFFER_MAX_SIZE);
    raw_battery_current = raw_battery_current >> 16; //shift to make space for 2 MSB bytes
    I2C_Master_ReadReg(I2C_SLAVE_ADDR, CURRENT_MSB, I2C_RX_BUFFER_MAX_SIZE);
    CopyArray((uint8_t*)ReceiveBuffer, (uint8_t*)&raw_battery_current, I2C_RX_BUFFER_MAX_SIZE);

    // convert raw reading to decimal in milli-Amps
    battery_current = 64 * (raw_battery_current - 32767) / 50 / 32767;
}

void readGaugeTemp(){
    I2C_Master_ReadReg(I2C_SLAVE_ADDR, TEMPERATURE_LSB, I2C_RX_BUFFER_MAX_SIZE);
    CopyArray((uint8_t*)ReceiveBuffer, (uint8_t*)&raw_fuel_gauge_temp, I2C_RX_BUFFER_MAX_SIZE);
    raw_fuel_gauge_temp = raw_fuel_gauge_temp >> 16; //shift to make space for 2 MSB bytes
    I2C_Master_ReadReg(I2C_SLAVE_ADDR, TEMPERATURE_MSB, I2C_RX_BUFFER_MAX_SIZE);
    CopyArray((uint8_t*)ReceiveBuffer, (uint8_t*)&raw_fuel_gauge_temp, I2C_RX_BUFFER_MAX_SIZE);

    // convert raw reading to decimal in Kelvin
    fuel_gauge_temp = 510 * raw_fuel_gauge_temp / 65535;
}

void readFuelGaugeControlRegister(){
    I2C_Master_ReadReg(I2C_SLAVE_ADDR, STATUS, 1);
    fuel_gauge_status_reg = ReceiveBuffer[0];
}

void initializeFuelGauge(){
    // voltage reading thresholds [0 -> 26 V]
    uint8_t init_tx_buffer[I2C_TX_BUFFER_MAX_SIZE] = {0x5, 0xE}; //voltage upper lim MSB
    I2C_Master_WriteReg(I2C_SLAVE_ADDR, VOLTAGE_THRESHOLD_HIGH_MSB, init_tx_buffer, I2C_TX_BUFFER_MAX_SIZE);
    init_tx_buffer[0] = 0x0; //voltage upper lim LSB
    init_tx_buffer[1] = 0x2;
    I2C_Master_WriteReg(I2C_SLAVE_ADDR, VOLTAGE_THRESHOLD_HIGH_LSB, init_tx_buffer, I2C_TX_BUFFER_MAX_SIZE);
    // voltage lower limit defaults to 0 already; don't need to set

    // current thresholds default to being as big as possible, don't want to make them smaller
    //      range is about -1.28A to 1.28 A

    // initialize charge register with maximum battery capacity (see data sheet for conversion from mAh, M is 4096)
    init_tx_buffer[0] = 0xA;
    init_tx_buffer[1] = 0x0;
    I2C_Master_WriteReg(I2C_SLAVE_ADDR, ACCUMULATED_CHARGE_MSB, init_tx_buffer, I2C_TX_BUFFER_MAX_SIZE);
    init_tx_buffer[0] = 0xD;
    init_tx_buffer[1] = 0x9;
    I2C_Master_WriteReg(I2C_SLAVE_ADDR, ACCUMULATED_CHARGE_LSB, init_tx_buffer, I2C_TX_BUFFER_MAX_SIZE);


    // set ADC to read voltage/curr/temp every 10 sec
    control_reg = 0b01101001;
    // set control_reg[7:6] to 01 do one conversion, 10 to convert every 10s,
    //      set to 00 to sleep, set to 11 to continuously convert
    // set control_reg[5:3] to 101 for M of 1024 for coulomb counter (see datasheet)
    // control_ref[2:1] not used on SBC (pin its related to is floating)
    // set control_reg[0] to 0 to drastically reduce current consumption (no conversions though)
    I2C_Master_WriteReg(I2C_SLAVE_ADDR, CONTROL, &control_reg, 1);
}

// TODO: what was this supposed to do?
void i2c_tx_blocking(uint16_t len, struct buffer *buf) {
    
}


//******************************************************************************
// I2C Interrupt ***************************************************************
//******************************************************************************

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B2_VECTOR
__interrupt void USCI_B2_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B2_VECTOR))) USCI_B2_ISR (void)
#else
#error Compiler not supported!
#endif
{
  //Must read from UCB2RXBUF
  uint8_t rx_val = 0;
  switch(__even_in_range(UCB2IV, USCI_I2C_UCBIT9IFG))
  {
    case USCI_NONE:          break;         // Vector 0: No interrupts
    case USCI_I2C_UCALIFG:   break;         // Vector 2: ALIFG
    case USCI_I2C_UCNACKIFG:                // Vector 4: NACKIFG
      break;
    case USCI_I2C_UCSTTIFG:  break;         // Vector 6: STTIFG
    case USCI_I2C_UCSTPIFG:  break;         // Vector 8: STPIFG
    case USCI_I2C_UCRXIFG3:  break;         // Vector 10: RXIFG3
    case USCI_I2C_UCTXIFG3:  break;         // Vector 12: TXIFG3
    case USCI_I2C_UCRXIFG2:  break;         // Vector 14: RXIFG2
    case USCI_I2C_UCTXIFG2:  break;         // Vector 16: TXIFG2
    case USCI_I2C_UCRXIFG1:  break;         // Vector 18: RXIFG1
    case USCI_I2C_UCTXIFG1:  break;         // Vector 20: TXIFG1
    case USCI_I2C_UCRXIFG0:                 // Vector 22: RXIFG0
        rx_val = UCB2RXBUF;
        if (RXByteCtr)
        {
          ReceiveBuffer[ReceiveIndex++] = rx_val;
          RXByteCtr--;
        }

        if (RXByteCtr == 1)
        {
          UCB2CTLW0 |= UCTXSTP;
        }
        else if (RXByteCtr == 0)
        {
          UCB2IE &= ~UCRXIE;
          MasterMode = IDLE_MODE;
          __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
        }
        break;
    case USCI_I2C_UCTXIFG0:                 // Vector 24: TXIFG0
        switch (MasterMode)
        {
          case TX_REG_ADDRESS_MODE:
              UCB2TXBUF = TransmitRegAddr;
              if (RXByteCtr)
                  MasterMode = SWITCH_TO_RX_MODE;   // Need to start receiving now
              else
                  MasterMode = TX_DATA_MODE;        // Continue to transmision with the data in Transmit Buffer
              break;

          case SWITCH_TO_RX_MODE:
              UCB2IE |= UCRXIE;              // Enable RX interrupt
              UCB2IE &= ~UCTXIE;             // Disable TX interrupt
              UCB2CTLW0 &= ~UCTR;            // Switch to receiver
              MasterMode = RX_DATA_MODE;    // State state is to receive data
              UCB2CTLW0 |= UCTXSTT;          // Send repeated start
              if (RXByteCtr == 1)
              {
                  //Must send stop since this is the N-1 byte
                  while((UCB2CTLW0 & UCTXSTT));
                  UCB2CTLW0 |= UCTXSTP;      // Send stop condition
              }
              break;

          case TX_DATA_MODE:
              if (TXByteCtr)
              {
                  UCB2TXBUF = TransmitBuffer[TransmitIndex++];
                  TXByteCtr--;
              }
              else
              {
                  //Done with transmission
                  UCB2CTLW0 |= UCTXSTP;     // Send stop condition
                  MasterMode = IDLE_MODE;
                  UCB2IE &= ~UCTXIE;                       // disable TX interrupt
                  __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
              }
              break;

          default:
              __no_operation();
              break;
        }
        break;
    default: break;
  }
}
