// ======================================================================
// \title  i2c.h
// \author cedric
// \edited by Jonathan
// \brief  controls MSP side of i2c interface between MSP430 motor controller
//         (this device) and the Hercules microcontroller (Hercules is master)
//         Hercules can read from or write to registers on this device
//
//          USES INTERRUPTS
// ======================================================================

#include "i2c.h"

uint8_t g_rxBuffer[I2C_RX_BUFFER_MAX_SIZE];
uint8_t g_txBuffer[I2C_TX_BUFFER_MAX_SIZE];
uint8_t g_rxBufferIdx;
uint8_t g_txBufferIdx;
uint8_t g_rxByteCtr;
uint8_t g_txByteCtr;
I2cMode g_slaveMode;
uint8_t g_i2cSlaveAddress;
uint8_t g_readRegAddr;
uint8_t g_i2cCmdLength[MAX_NB_CMDS];
//bool g_sendTelem = false;

/**
 * @brief      Disables i 2 c receive interrupt.
 */
inline void disableI2cRxInterrupt(void){
  UCB0IE &= ~UCRXIE;
//    return;
}


/**
 * @brief      Enables i 2 c receive interrupt.
 */
inline void enableI2cRxInterrupt(void){
  UCB0IE |= UCRXIE;
}


/**
 * @brief      Disables i 2 c transmit interrupt.
 */
inline void disableI2cTxInterrupt(void){
  UCB0IE &= ~UCTXIE;
//    return;
}


/**
 * @brief      Enables i 2 c transmit interrupt.
 */
inline void enableI2cTxInterrupt(void){
  UCB0IE |= UCTXIE;
}


/**
 * @brief      Copy an array of data 
 *
 * @param      source  The source
 * @param      dest    The destination
 * @param[in]  size    The size
 */
void copyArray(uint8_t *source, uint8_t *dest, int size){
    int copyIndex = 0;
    for (copyIndex = 0; copyIndex < size; copyIndex++){
        dest[copyIndex] = source[copyIndex];
    }
}


/**
 * @brief      Process the command received from i2c
 *
 * @param[in]  cmd   The command
 */
inline void i2cSlaveProcessCmd(const uint8_t cmd)
{
      g_rxBufferIdx = 0;
      g_txBufferIdx = 0;
      g_rxByteCtr = 0;
      // uint8_t * txData;

  switch(cmd)
  {
      //-----------------------------------------------------------------
      // Commands requesting to send data to master
      //-----------------------------------------------------------------
      case MC_REG_I2C_ADDRESS:
          // txData = (uint8_t*)&g_i2cSlaveAddress;
          g_slaveMode = TX_DATA_MODE;
          g_txByteCtr = g_i2cCmdLength[cmd];
          //Fill out the TransmitBuffer
          copyArray((uint8_t*)&g_i2cSlaveAddress, (uint8_t*)g_txBuffer, g_txByteCtr);
          disableI2cRxInterrupt();
          enableI2cTxInterrupt();
        break;
      case MC_REG_CURRENT_POSITION:
          // txData = (uint8_t*)&g_currentPosition;
          g_slaveMode = TX_DATA_MODE;
          g_txByteCtr = g_i2cCmdLength[cmd];
          //Fill out the TransmitBuffer
          copyArray((uint8_t*)&g_currentPosition, (uint8_t*)g_txBuffer, g_txByteCtr);
          disableI2cRxInterrupt();
          enableI2cTxInterrupt();
        break;
      case MC_REG_CURRENT_SPEED:
      {
          int16_t speed_info = (int16_t)(g_currentSpeed >> 7); // 7 LSBs are 0s, 16 MSBs are too
          // txData = (uint8_t*)&speed_info;
          g_slaveMode = TX_DATA_MODE;
          g_txByteCtr = g_i2cCmdLength[cmd];
          //Fill out the TransmitBuffer
          copyArray((uint8_t*)&speed_info, (uint8_t*)g_txBuffer, g_txByteCtr);
          disableI2cRxInterrupt();
          enableI2cTxInterrupt();
        break;
      }
      case MC_REG_MOTOR_CURRENT:
          // txData = (uint8_t*)&g_piCur.Fbk;
          g_slaveMode = TX_DATA_MODE;
          g_txByteCtr = g_i2cCmdLength[cmd];
          //Fill out the TransmitBuffer
          copyArray((uint8_t*)&g_piCur.Fbk, (uint8_t*)g_txBuffer, g_txByteCtr);
          disableI2cRxInterrupt();
          enableI2cTxInterrupt();
        break;       
      case MC_REG_MC_STATUS:
          // txData = (uint8_t*)&g_statusRegister;
          g_slaveMode = TX_DATA_MODE;
          g_txByteCtr = g_i2cCmdLength[cmd];
          //Fill out the TransmitBuffer
          copyArray((uint8_t*)&g_statusRegister, (uint8_t*)g_txBuffer, g_txByteCtr);
          disableI2cRxInterrupt();
          enableI2cTxInterrupt();
        break;
      case MC_REG_MC_FAULT:
          // txData = (uint8_t*)&g_faultRegister;
          g_slaveMode = TX_DATA_MODE;
          g_txByteCtr = g_i2cCmdLength[cmd];

          //Fill out the TransmitBuffer
          copyArray((uint8_t*)&g_faultRegister, (uint8_t*)g_txBuffer, g_txByteCtr);
          disableI2cRxInterrupt();
          enableI2cTxInterrupt();
        break;        
      //-----------------------------------------------------------------
      // Commands requesting to process some more data from master
      //-----------------------------------------------------------------
      case TARGET_POSITION:
      case TARGET_SPEED:
      case P_CURRENT:
      case I_CURRENT:
      case P_SPEED:
      case I_SPEED:
      case ACC_RATE:
      case DEC_RATE:
      case CONTROL_REGISTER:
          g_slaveMode = RX_DATA_MODE;
          g_rxByteCtr = g_i2cCmdLength[cmd];
          enableI2cRxInterrupt();
          disableI2cTxInterrupt();
          return;
      default:
          g_faultRegister |= MC_FAULT_I2C_ERROR;
          __no_operation();
          return;
  }
}


/**
 * @brief      This function takes care the copying data recevied from i2c to
 *             variables
 *
 * @param[in]  cmd   The command
 */
inline void i2cSlaveTransactionDone(const uint8_t cmd){
    switch(cmd)
    {
      case MC_REG_I2C_ADDRESS:
      case MC_REG_CURRENT_POSITION:
      case MC_REG_CURRENT_SPEED:
      case MC_REG_MOTOR_CURRENT:
      case STATUS_REGISTER:
      case FAULT_REGISTER:
        break;
      case MC_REG_TARGET_POSITION:
        copyArray((uint8_t*)g_rxBuffer,
                  (uint8_t*)&(mcRegStruct.mc_target_pos),
                  sizeof(mcRegStruct.mc_target_pos));
        setParamUpdateFlag(cmd);
        break;
      case MC_REG_TARGET_SPEED:
        copyArray((uint8_t*)g_rxBuffer,
                  (uint8_t*)&(mcRegStruct.mc_target_speed),
                  sizeof(mcRegStruct.mc_target_speed));
        setParamUpdateFlag(cmd);
        break;
      case MC_REG_P_CURRENT:
         // This type conversion (casting _IQ(15) to uint8_t) has been verified to be working as of 5-1-2021
        copyArray((uint8_t*)g_rxBuffer,
                        (uint8_t*)&(mcRegStruct.mc_piCurKp),
                        sizeof(mcRegStruct.mc_piCurKp));
        setParamUpdateFlag(cmd);
        break;
      case MC_REG_I_CURRENT:
        copyArray((uint8_t*)g_rxBuffer,
                  (uint8_t*)&(mcRegStruct.mc_piCurKi),
                  sizeof(mcRegStruct.mc_piCurKi));
        setParamUpdateFlag(cmd);
        break;
      case MC_REG_P_SPEED:
        copyArray((uint8_t*)g_rxBuffer,
                  (uint8_t*)&(mcRegStruct.mc_piSpdKp),
                  sizeof(mcRegStruct.mc_piSpdKp));
        setParamUpdateFlag(cmd);
        break;
      case MC_REG_I_SPEED:
        copyArray((uint8_t*)g_rxBuffer,
                  (uint8_t*)&(mcRegStruct.mc_piSpdKi),
                  sizeof(mcRegStruct.mc_piSpdKi));
        setParamUpdateFlag(cmd);
        break;
      case MC_REG_MAX_CURRENT:
          copyArray((uint8_t*)g_rxBuffer,
                    (uint8_t*)&(mcRegStruct.mc_maxCurrent),
                    sizeof(mcRegStruct.mc_maxCurrent));
          setParamUpdateFlag(cmd);
          break;
      case MC_REG_DISABLE_FAULT_MASK:
          copyArray((uint8_t*)g_rxBuffer,
                    (uint8_t*)&(mcRegStruct.mc_ignoreFaults),
                    sizeof(mcRegStruct.mc_ignoreFaults));
          setParamUpdateFlag(cmd);
          break;
      case CONTROL_REGISTER:
        copyArray((uint8_t*)g_rxBuffer,
                  (uint8_t*)&g_controlRegister,
                   sizeof(g_controlRegister));
        // E-STOP CHECK
        if(g_controlRegister & MC_CMD_DISABLE_DRIVER)
        {
            g_cmdState = DISABLE;
            disableGateDriver();
        }


        break;
      default:
        break;  
    }
}

/**
 * @brief      Initializes the command length
 *              (expected # of bytes to read/write for each register)
 */
void initializeCmdLength()
{
  g_i2cCmdLength[I2C_ADDRESS] = 1;
  g_i2cCmdLength[TARGET_POSITION] = 4;
  g_i2cCmdLength[TARGET_SPEED] = 1;
  g_i2cCmdLength[CURRENT_POSITION] = 4;
  g_i2cCmdLength[CURRENT_SPEED] = 2;
  g_i2cCmdLength[MOTOR_CURRENT] = 4;
  g_i2cCmdLength[P_CURRENT] = 2;
  g_i2cCmdLength[I_CURRENT] = 2;
  g_i2cCmdLength[P_SPEED] = 2;
  g_i2cCmdLength[I_SPEED] = 2;
//  g_i2cCmdLength[ACC_RATE] = 2;
//  g_i2cCmdLength[DEC_RATE] = 2;
  g_i2cCmdLength[CONTROL_REGISTER] = 1;
  g_i2cCmdLength[STATUS_REGISTER] = 1;
  g_i2cCmdLength[FAULT_REGISTER] = 1;
}

/**
 * @brief      Initializes i2c module.
 */
McI2cAddr_t initializeI2cModule()
{
    g_rxBufferIdx = 0;
    g_txBufferIdx = 0;
    g_slaveMode = RX_REG_ADDRESS_MODE;
    g_readRegAddr = 0;

    initializeCmdLength();

    // Configure I2C interface for slave interface
    EUSCI_B_I2C_initSlaveParam param = {0};
    param.slaveAddress = MC_SLAVE_I2C_ADDR_BASE;
    param.slaveAddress |= (READ_ADDR1) ? 0x01 : 0x00;
    param.slaveAddress |= (READ_ADDR2) ? 0x02 : 0x00;
    g_i2cSlaveAddress = param.slaveAddress; // Possibly Debug
    param.slaveAddressOffset = EUSCI_B_I2C_OWN_ADDRESS_OFFSET0;
    param.slaveOwnAddressEnable = EUSCI_B_I2C_OWN_ADDRESS_ENABLE;
    EUSCI_B_I2C_initSlave(EUSCI_B0_BASE, &param);

    EUSCI_B_I2C_enable(EUSCI_B0_BASE);

    EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE,
                             EUSCI_B_I2C_RECEIVE_INTERRUPT0 +
                             EUSCI_B_I2C_STOP_INTERRUPT);

    EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE,
                              EUSCI_B_I2C_RECEIVE_INTERRUPT0 +
                              EUSCI_B_I2C_STOP_INTERRUPT);

    return g_i2cSlaveAddress;
}


/**
 * @brief      I2C interrupt routine
 */
#pragma CODE_SECTION(USCI_B0_ISR, ".TI.ramfunc")
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void){
  uint8_t rxBuf = 0;

  switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG)){
    case USCI_NONE:          break;         // Vector 0: No interrupts
    case USCI_I2C_UCALIFG:   break;         // Vector 2: ALIFG
    case USCI_I2C_UCNACKIFG: break;         // Vector 4: NACKIFG
    case USCI_I2C_UCSTTIFG:  break;         // Vector 6: STTIFG
    case USCI_I2C_UCSTPIFG:                 // Vector 8: STPIFG
    {
      UCB0IFG &= ~(UCTXIFG0);
      break;
    }
    case USCI_I2C_UCRXIFG3:  break;         // Vector 10: RXIFG3
    case USCI_I2C_UCTXIFG3:  break;         // Vector 12: TXIFG3
    case USCI_I2C_UCRXIFG2:  break;         // Vector 14: RXIFG2
    case USCI_I2C_UCTXIFG2:  break;         // Vector 16: TXIFG2
    case USCI_I2C_UCRXIFG1:  break;         // Vector 18: RXIFG1
    case USCI_I2C_UCTXIFG1:  break;         // Vector 20: TXIFG1
    case USCI_I2C_UCRXIFG0:                 // Vector 22: RXIFG0
    {
      rxBuf = UCB0RXBUF;
      if(g_slaveMode == RX_REG_ADDRESS_MODE){ // recieving register address that master wants to interact with
        TB0CCTL0 = 0x0000; // Turn off timer interrupt
        g_readRegAddr = rxBuf;
        i2cSlaveProcessCmd(g_readRegAddr);
      } else if(g_slaveMode == RX_DATA_MODE){ // master is writing to a register (specified by g_readRegAddr)
        g_rxBuffer[g_rxBufferIdx++] = rxBuf;
        g_rxByteCtr--;
        if(g_rxByteCtr == 0){
          //Done Receiving MSG
          g_slaveMode = RX_REG_ADDRESS_MODE;
          disableI2cTxInterrupt();
          enableI2cRxInterrupt();
          i2cSlaveTransactionDone(g_readRegAddr);
          TB0CCTL0 = CCIE; // turn timer interrupt back on
        }
      }
    } // end case USCI_I2C_ICRXIFG0
    case USCI_I2C_UCTXIFG0:                 // Vector 24: TXIFG0
      if(g_slaveMode == TX_DATA_MODE){
          __delay_cycles(1000);
          // master is reading bytes from us
          UCB0TXBUF = g_txBuffer[g_txBufferIdx++];
          g_txByteCtr--;
          if(g_txByteCtr == 0){
            g_slaveMode = RX_REG_ADDRESS_MODE;
            disableI2cTxInterrupt();
            enableI2cRxInterrupt();
            i2cSlaveTransactionDone(g_readRegAddr);
            TB0CCTL0 = CCIE; // turn timer interrupt back on
          }
      } // end case USCI_I2C_UCTXIFG0
      break;         
    default:
        break;
  } // end statement switching on USCI_I2C_UCBIT9IFG
}
