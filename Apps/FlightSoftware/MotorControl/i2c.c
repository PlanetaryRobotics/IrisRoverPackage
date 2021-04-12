#include "i2c.h"

volatile uint8_t g_rxBuffer[I2C_RX_BUFFER_MAX_SIZE];
volatile uint8_t g_txBuffer[I2C_TX_BUFFER_MAX_SIZE];
volatile uint8_t g_rxBufferIdx;
volatile uint8_t g_txBufferIdx;
volatile uint8_t g_rxByteCtr;
volatile uint8_t g_txByteCtr;
volatile I2cMode g_slaveMode;
volatile uint8_t g_i2cSlaveAddress;
volatile uint8_t g_readRegAddr;


extern volatile _iq g_currentSpeed;
extern volatile int32_t g_currentPosition;
extern volatile int32_t g_targetPosition;
extern volatile PI_CONTROLLER g_piSpd;
extern volatile PI_CONTROLLER g_piCur;
extern volatile uint16_t g_maxSpeed;
extern uint8_t g_statusRegister;
extern uint8_t g_controlRegister;
extern uint8_t g_faultRegister;

int16_t g_transmitCurrentPosition;

volatile uint8_t g_i2cCmdLength[MAX_NB_CMDS];

extern CmdState g_cmdState;
extern bool driveOpenLoop;


/**
 * @brief      Disables i 2 c receive interrupt.
 */
inline void disableI2cRxInterrupt(void){
  UCB0IE &= ~UCRXIE;
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
inline void copyArray(uint8_t *source, uint8_t *dest, uint8_t size){
    uint8_t copyIndex = 0;
    for (copyIndex = 0; copyIndex < size; copyIndex++){
        dest[copyIndex] = source[copyIndex];
    }
}


/**
 * @brief      Process the command received from i2c
 *
 * @param[in]  cmd   The command
 */
inline void i2cSlaveProcessCmd(const uint8_t cmd){
  g_rxBufferIdx = 0;
  g_txBufferIdx = 0;
  g_rxByteCtr = 0;

  switch(cmd){
      //-----------------------------------------------------------------
      // Commands requesting to send data to master
      //-----------------------------------------------------------------
      case I2C_ADDRESS:                        
        g_slaveMode = TX_DATA_MODE;
        g_txByteCtr = g_i2cCmdLength[cmd];
        //Fill out the TransmitBuffer
        copyArray((uint8_t*)&g_i2cSlaveAddress, (uint8_t*)g_txBuffer, g_txByteCtr);
        disableI2cRxInterrupt();
        enableI2cTxInterrupt();
        break;
      case CURRENT_POSITION:                 
        g_slaveMode = TX_DATA_MODE;
        g_txByteCtr = g_i2cCmdLength[cmd];
        //Fill out the TransmitBuffer
        g_transmitCurrentPosition = (int16_t) g_currentPosition;
        copyArray((uint8_t*)&g_transmitCurrentPosition, (uint8_t*)g_txBuffer, g_txByteCtr);
        disableI2cRxInterrupt();
        enableI2cTxInterrupt();
        break;
      case CURRENT_SPEED:
        g_slaveMode = TX_DATA_MODE;
        g_txByteCtr = g_i2cCmdLength[cmd];
        int16_t speed_info = (int16_t)(g_currentSpeed >> 7); // 7 LSBs are 0s, 16 MSBs are too
        //Fill out the TransmitBuffer
        copyArray((uint8_t*)&speed_info, (uint8_t*)g_txBuffer, g_txByteCtr);
        disableI2cRxInterrupt();
        enableI2cTxInterrupt();
        break;
      case MOTOR_CURRENT:
        g_slaveMode = TX_DATA_MODE;
        g_txByteCtr = g_i2cCmdLength[cmd];
        //Fill out the TransmitBuffer
        int16_t current_info = (int16_t)g_piCur.Fbk; // top 16 MSBs empty
        copyArray((uint8_t*)&current_info, (uint8_t*)g_txBuffer, g_txByteCtr);
        disableI2cRxInterrupt();
        enableI2cTxInterrupt();
        break;       
      case STATUS_REGISTER:
        g_slaveMode = TX_DATA_MODE;
        g_txByteCtr = g_i2cCmdLength[cmd];
        //Fill out the TransmitBuffer
        copyArray((uint8_t*)&g_statusRegister, (uint8_t*)g_txBuffer, g_txByteCtr);
        disableI2cRxInterrupt();
        enableI2cTxInterrupt();    
        break;
      case FAULT_REGISTER:
        g_slaveMode = TX_DATA_MODE;
        g_txByteCtr = g_i2cCmdLength[cmd];
        //update g_faultRegister
        if(read_driver_fault())
            g_faultRegister |= DRIVER_FAULT;
        else
            g_faultRegister &= ~DRIVER_FAULT;

        //Fill out the TransmitBuffer
        copyArray((uint8_t*)&g_faultRegister, (uint8_t*)g_txBuffer, g_txByteCtr);
        disableI2cRxInterrupt();
        enableI2cTxInterrupt();    
        break;        
      //-----------------------------------------------------------------
      // Commands requesting to process some more data from master
      //-----------------------------------------------------------------
      case RELATIVE_TARGET_POSITION:
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
          break;
      default:
          __no_operation();
          break;
  }
}


/**
 * @brief      This function takes care the copying data recevied from i2c to
 *             variables
 *
 * @param[in]  cmd   The command
 */
inline void i2cSlaveTransactionDone(const uint8_t cmd){
    switch(cmd){
      case I2C_ADDRESS:
      case CURRENT_POSITION:
      case MOTOR_CURRENT:
      case STATUS_REGISTER:
      case FAULT_REGISTER:
        break;
      case RELATIVE_TARGET_POSITION:
        copyArray((uint8_t*)g_rxBuffer,
                  (uint8_t*)&g_targetPosition,
                  sizeof(g_targetPosition));
        g_currentPosition = 0; // reset current position
        break;
      case TARGET_SPEED:
        copyArray((uint8_t*)g_rxBuffer,
                  (uint8_t*)&g_maxSpeed,
                  sizeof(g_maxSpeed));
        if(g_maxSpeed > MAX_TARGET_SPEED) g_maxSpeed = MAX_TARGET_SPEED;
        break;
      case P_CURRENT:
        copyArray((uint8_t*)g_rxBuffer,
                  (uint8_t*)&g_piCur.Kp,
                  sizeof(g_piCur.Kp));
        break;
      case I_CURRENT:
        copyArray((uint8_t*)g_rxBuffer,
                  (uint8_t*)&g_piCur.Ki,
                  sizeof(g_piCur.Ki));
        break;
      case P_SPEED:
        copyArray((uint8_t*)g_rxBuffer,
                  (uint8_t*)&g_piSpd.Kp,
                  sizeof(g_piSpd.Kp));
        break;
      case I_SPEED:
        copyArray((uint8_t*)g_rxBuffer,
                  (uint8_t*)&g_piSpd.Ki,
                  sizeof(g_piSpd.Ki));
        break;
      case CONTROL_REGISTER:
        copyArray((uint8_t*)g_rxBuffer,
                  (uint8_t*)&g_controlRegister,
                   sizeof(g_controlRegister));

        // update status register if told to drive in open loop
        if(g_controlRegister & DRIVE_OPEN_LOOP){
            g_statusRegister |= DRIVE_OPEN_LOOP;
        }

        if(g_controlRegister & CLEAR_DRIVER_FAULT){
            clear_driver_fault();
            g_statusRegister |= CLEAR_DRIVER_FAULT; // indicates an attempt to clear fault was made
        }
        // update state machine if requested
        if(g_controlRegister & STATE_MACHINE_DISABLE){
            g_cmdState = DISABLE;
            updateStateMachine();
            g_statusRegister |= STATE_MACHINE_DISABLE; // status reg bit 3: 1 if in disable state, 0 if not
        } else if (g_controlRegister & STATE_MACHINE_RUN){
            g_cmdState = RUN;
            updateStateMachine();
            g_statusRegister &= ~STATE_MACHINE_DISABLE;
        }

        break;


        break;     
      case ACC_RATE:
      case DEC_RATE:
      default:
        break;  
    }
}


/**
 * @brief      Initializes the command length.
 */
void initializeCmdLength(){
  g_i2cCmdLength[I2C_ADDRESS] = 1;
  g_i2cCmdLength[RELATIVE_TARGET_POSITION] = 4;
  g_i2cCmdLength[TARGET_SPEED] = 1;
  g_i2cCmdLength[CURRENT_POSITION] = 4;
  g_i2cCmdLength[CURRENT_SPEED] = 2;
  g_i2cCmdLength[MOTOR_CURRENT] = 2;
  g_i2cCmdLength[P_CURRENT] = 4;
  g_i2cCmdLength[I_CURRENT] = 2;
  g_i2cCmdLength[P_SPEED] = 4;
  g_i2cCmdLength[I_SPEED] = 2;
  g_i2cCmdLength[ACC_RATE] = 2;
  g_i2cCmdLength[DEC_RATE] = 2;
  g_i2cCmdLength[CONTROL_REGISTER] = 1;
  g_i2cCmdLength[STATUS_REGISTER] = 1;
  g_i2cCmdLength[FAULT_REGISTER] = 1;
}


/**
 * @brief      Initializes i2c module.
 */
void initializeI2cModule(){
  g_rxBufferIdx = 0;
  g_txBufferIdx = 0;
  g_slaveMode = RX_REG_ADDRESS_MODE;
  g_readRegAddr = 0;

  initializeCmdLength();

  // Configure I2C interface for slave interface
  EUSCI_B_I2C_initSlaveParam param = {0};
  param.slaveAddress = I2C_SLAVE_ADDRESS;
  param.slaveAddress |= (READ_ADDR1) ? 0x01 : 0x00;
  param.slaveAddress |= (READ_ADDR2) ? 0x02 : 0x00;
  g_i2cSlaveAddress = param.slaveAddress; // [DEBUG]
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
    case USCI_I2C_UCNACKIFG:                // Vector 4: NACKIFG
      break;
    case USCI_I2C_UCSTTIFG:  break;         // Vector 6: STTIFG
    case USCI_I2C_UCSTPIFG:
      UCB0IFG &= ~(UCTXIFG0);
      break;                                // Vector 8: STPIFG
    case USCI_I2C_UCRXIFG3:  break;         // Vector 10: RXIFG3
    case USCI_I2C_UCTXIFG3:  break;         // Vector 12: TXIFG3
    case USCI_I2C_UCRXIFG2:  break;         // Vector 14: RXIFG2
    case USCI_I2C_UCTXIFG2:  break;         // Vector 16: TXIFG2
    case USCI_I2C_UCRXIFG1:  break;         // Vector 18: RXIFG1
    case USCI_I2C_UCTXIFG1:  break;         // Vector 20: TXIFG1
    case USCI_I2C_UCRXIFG0:                 // Vector 22: RXIFG0
      rxBuf = UCB0RXBUF;
      switch(g_slaveMode){
        case RX_REG_ADDRESS_MODE:
          g_readRegAddr = rxBuf;
          i2cSlaveProcessCmd(g_readRegAddr);
          break;
        case RX_DATA_MODE:
          g_rxBuffer[g_rxBufferIdx++] = rxBuf;
          g_rxByteCtr--;
          if(g_rxByteCtr == 0){
            //Done Receiving MSG
            g_slaveMode = RX_REG_ADDRESS_MODE;
            disableI2cTxInterrupt();
            enableI2cRxInterrupt();
            i2cSlaveTransactionDone(g_readRegAddr);
          }
          break;
      default:
          break;
      }
      break;  
    case USCI_I2C_UCTXIFG0:                 // Vector 24: TXIFG0
      switch(g_slaveMode){
        case TX_DATA_MODE:
          UCB0TXBUF = g_txBuffer[g_txBufferIdx++];
          g_txByteCtr--;
          if(g_txByteCtr == 0){
            g_slaveMode = RX_REG_ADDRESS_MODE;
            disableI2cTxInterrupt();
            enableI2cRxInterrupt();
            i2cSlaveTransactionDone(g_readRegAddr);
          }
          break;
        default:
          break;
      }
      break;         
    default: break;
  }
}
