// ======================================================================
// \title  MotorControlComponentImpl.cpp
// \author cedric
// \brief  cpp file for MotorControl component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "CubeRover/MotorControl/MotorControlComponentImpl.hpp"
#include <stdlib.h>
#include <string.h>

#include "Fw/Types/BasicTypes.hpp"
#include "i2c.h"
#include "Include/CubeRoverConfig.hpp"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  /**
   * @brief      Constructs a new instance.
   *
   * @param[in]  compName  The component name
   */
  MotorControlComponentImpl ::
#if FW_OBJECT_NAMES == 1
    MotorControlComponentImpl(
        const char *const compName
    ) :
      MotorControlComponentBase(compName)
#else
    MotorControlComponentImpl(void)
#endif
  {
      m_encoderTickToCmRatio = 0;
      m_fwDist = 0;
      m_fwSpeed = 0;
      m_leftSpeed = 0;
      m_reDist = 0;
      m_reSpeed = 0;
      m_rightAngle = 0;
      m_leftAngle = 0;
      m_rightSpeed = 0;
  }


  /**
   * @brief      Initialize the motor control component
   *
   * @param[in]  queueDepth  The queue depth
   * @param[in]  instance    The instance
   */
  void MotorControlComponentImpl :: init(const NATIVE_INT_TYPE queueDepth, const NATIVE_INT_TYPE instance){
    MotorControlComponentBase::init(queueDepth, instance);

    // Create look-up table for I2C communications with motor controller board
    generateChecksumTable(CRC8);

    // left and right turn parameters
    m_rightSpeed = 0;
    m_leftSpeed = 0;
    m_rightAngle = 0;
    m_leftAngle = 0;

    // forward and reverse parameters
    m_fwDist = 0;
    m_reDist = 0;
    m_fwSpeed = 0;
    m_reSpeed = 0;

    // Initialize the encoder tick to cm ratio
    m_encoderTickToCmRatio = (PI * CUBEROVER_WHEEL_DIAMETER_CM) / (MOTOR_NB_PAIR_POLES * MOTOR_GEAR_BOX_REDUCTION * 6.0);
  }


  /**
   * @brief      Destroys the object.
   */
  MotorControlComponentImpl :: ~MotorControlComponentImpl(void)
  {

  }

  /**
   * @brief      Handler implementations for user-defined typed input ports
   *
   * @param[in]  portNum  The port number
   * @param[in]  context  The context
   */
  void MotorControlComponentImpl :: schedIn_handler( const NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context){
    // TODO
  }

  /**
   * @brief      Command handler implementations
   *
   * @param[in]  opCode                The operation code
   * @param[in]  cmdSeq                The command sequence
   * @param[in]  CommandConfiguration  The command configuration
   * @param[in]  Parameter             The parameter
   * @param[in]  Value                 The value
   */
  void MotorControlComponentImpl :: MC_DrivingConfiguration_cmdHandler( const FwOpcodeType opCode,
                                                                        const U32 cmdSeq,
                                                                        CommandList CommandConfiguration,
                                                                        ParameterList Parameter,
                                                                        U8 Value){
    switch(CommandConfiguration){
      case FORWARD_CFG:
        switch(Parameter){
          case DISTANCE:
            m_fwDist = Value;
            break;
          case SPEED:
            m_fwSpeed = Value;
            break;
          default:
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }
        break;
      case REVERSE_CFG:
        switch(Parameter){
          case DISTANCE:
            m_reDist = Value;
            break;
          case SPEED:
            m_reSpeed = Value;
            break;
          default:
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }
        break;
      case LEFT_CFG:
        switch(Parameter){
          case SPEED:
            m_leftSpeed = Value;
            break;
          case ANGLE:
            m_leftAngle = Value;
            break;
          default:
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }
        break;
      case RIGHT_CFG:
        switch(Parameter){
          case SPEED:
            m_rightSpeed = Value;
            break;
          case ANGLE:
            m_rightAngle = Value;
            break;
          default:
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }

        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
        break;

    }
  }

  
  /**
   * @brief      Convert cm to motor encoder ticks
   *
   * @param[in]  dist  The distance
   */
  inline Motor_tick MotorControlComponentImpl :: cmToMotorTicks(const Distance_cm dist){
    return dist / m_encoderTickToCmRatio;
  }


  /**
   * @brief      Enables the drivers.
   *
   * @return     Motor controller error
   */
  MCError MotorControlComponentImpl :: enableDrivers(){
    // @TODO
    return MC_NO_ERROR;
  }


  /**
   * @brief      Disables the drivers.
   *
   * @return     Motor controller error
   */
  MCError MotorControlComponentImpl :: disableDrivers(){
    MCError err = MC_NO_ERROR;

    // Disable driver for the front left wheel.
    err = writeMotorControlRegister( MOTOR_CONTROL_I2CREG, 
                                     MotorControllerI2C::I2C_DISABLE_DRIVER, 
                                     FRONT_LEFT_MC_I2C_ADDR,
                                     0xff /*dummy write*/);
    if(err != MC_NO_ERROR)
      return err;

    // Disable driver for the rear left wheel.
    err = writeMotorControlRegister( MOTOR_CONTROL_I2CREG, 
                                     MotorControllerI2C::I2C_DISABLE_DRIVER, 
                                     REAR_LEFT_MC_I2C_ADDR,
                                     0xff /*dummy write*/);
    if(err != MC_NO_ERROR)
      return err;

    // Disable driver for the front right wheel.
    err = writeMotorControlRegister( MOTOR_CONTROL_I2CREG, 
                                     MotorControllerI2C::I2C_DISABLE_DRIVER, 
                                     FRONT_RIGHT_MC_I2C_ADDR,
                                     0xff /*dummy write*/);
    if(err != MC_NO_ERROR)
      return err;

    // Disable driver for the rear right wheel.
    err = writeMotorControlRegister( MOTOR_CONTROL_I2CREG, 
                                     MotorControllerI2C::I2C_DISABLE_DRIVER, 
                                     REAR_RIGHT_MC_I2C_ADDR,
                                     0xff /*dummy write*/);
    return err;
  }


  /**
   * @brief      Execute commands related to driving functions
   *
   * @param[in]  opCode          The operation code
   * @param[in]  cmdSeq          The command sequence
   * @param[in]  DrivingCommand  The driving command
   */
  void MotorControlComponentImpl :: MC_ExecuteDriving_cmdHandler( const FwOpcodeType opCode,
                                                                  const U32 cmdSeq,
                                                                  DriveCommandList DrivingCommand){
    MCError err = MC_NO_ERROR;
    Motor_tick targetTick = cmToMotorTicks(m_fwDist);

    switch(DrivingCommand){
      case GO_FORWARD:
      case GO_REVERSE:

        // Set target position in ticks for the front left wheel
        err = writeMotorControlRegister( MOTOR_CONTROL_I2CREG, 
                                         MotorControllerI2C::I2C_RELATIVE_TARGET_POSITION, 
                                         FRONT_LEFT_MC_I2C_ADDR,
                                         targetTick);
        if(err != MC_NO_ERROR){
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;
        }

        // Set target position in ticks for the front right wheel
        err = writeMotorControlRegister( MOTOR_CONTROL_I2CREG,
                                         MotorControllerI2C::I2C_RELATIVE_TARGET_POSITION,
                                         FRONT_RIGHT_MC_I2C_ADDR,
                                         targetTick);
        if(err != MC_NO_ERROR){
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;
        }

        err = writeMotorControlRegister( MOTOR_CONTROL_I2CREG,
                                         MotorControllerI2C::I2C_RELATIVE_TARGET_POSITION,
                                         REAR_LEFT_MC_I2C_ADDR,
                                         targetTick);
        if(err != MC_NO_ERROR){
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        err = writeMotorControlRegister( MOTOR_CONTROL_I2CREG,
                                         MotorControllerI2C::I2C_RELATIVE_TARGET_POSITION,
                                         REAR_RIGHT_MC_I2C_ADDR,
                                         targetTick);
        if(err != MC_NO_ERROR){
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }
        
        // Set target speed in percentage
        err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                               MotorControllerI2C::I2C_TARGET_SPEED,
                               FRONT_LEFT_MC_I2C_ADDR,
                               targetTick);
        if(err != MC_NO_ERROR){
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                               MotorControllerI2C::I2C_TARGET_SPEED,
                               FRONT_RIGHT_MC_I2C_ADDR,
                               targetTick);
        if(err != MC_NO_ERROR){
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                               MotorControllerI2C::I2C_TARGET_SPEED,
                               REAR_LEFT_MC_I2C_ADDR,
                               targetTick);
        if(err != MC_NO_ERROR){
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        err = writeMotorControlRegister( MOTOR_CONTROL_I2CREG,
                                MotorControllerI2C::I2C_TARGET_SPEED,
                                REAR_RIGHT_MC_I2C_ADDR,
                                targetTick);
        if(err != MC_NO_ERROR){
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }    
        break;
      case TURN_LEFT:

        break;
      case TURN_RIGHT:

        break;
      default:
        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
        return;
    }

    // enable all motor controller drivers
    enableDrivers();

    // send command to all motor controllers to execute driving command
    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                    MotorControllerI2C::I2C_EXECUTE_CMD,
                                    FRONT_LEFT_MC_I2C_ADDR,
                                    DrivingCommand);
    if(err != MC_NO_ERROR){
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
      return;          
    }

    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                    MotorControllerI2C::I2C_EXECUTE_CMD,
                                    FRONT_RIGHT_MC_I2C_ADDR,
                                    DrivingCommand);
    if(err != MC_NO_ERROR){
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
      return;          
    }  

    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                    MotorControllerI2C::I2C_EXECUTE_CMD,
                                    REAR_LEFT_MC_I2C_ADDR,
                                    DrivingCommand);
    if(err != MC_NO_ERROR){
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
      return;          
    }  

    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                    MotorControllerI2C::I2C_EXECUTE_CMD,
                                    REAR_RIGHT_MC_I2C_ADDR,
                                    DrivingCommand);
    if(err != MC_NO_ERROR){
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
      return;          
    }  

    // if the code reaches  
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Handle commands that adjust motor tuning parameters
   *
   * @param[in]  opCode           The operation code
   * @param[in]  cmdSeq           The command sequence
   * @param[in]  TuningParameter  The tuning parameter
   * @param[in]  Value            The value
   */
  void MotorControlComponentImpl :: MC_TuningParameters_cmdHandler( const FwOpcodeType opCode,
                                                                    const U32 cmdSeq,
                                                                    tuningParameterList TuningParameter,
                                                                    U16 Value){
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }


  /**
   * @brief      Handles commands that adjust acceleration profiles of the rover
   *
   * @param[in]  opCode                 The operation code
   * @param[in]  cmdSeq                 The command sequence
   * @param[in]  AccelerationParameter  The acceleration parameter
   * @param[in]  Value                  The value
   */
  void MotorControlComponentImpl :: MC_AccelerationProfiles_cmdHandler( const FwOpcodeType opCode,
                                                                        const U32 cmdSeq,
                                                                        accelerationParameterList AccelerationParameter,
                                                                        U16 Value){
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }


  /**
   * @brief      Handles commands that handles stall detection enabling / disabling
   *
   * @param[in]  opCode            The operation code
   * @param[in]  cmdSeq            The command sequence
   * @param[in]  motorStallEnable  The motor stall enable
   */
  void MotorControlComponentImpl :: MC_StallDetection_cmdHandler( const FwOpcodeType opCode,
                                                                  const U32 cmdSeq,
                                                                  motorStallEnableList motorStallEnable){
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }


  /**
   * @brief      Handles commands that handles ticks position counter value
   *
   * @param[in]  opCode                The operation code
   * @param[in]  cmdSeq                The command sequence
   * @param[in]  ResetPositionCounter  The reset position counter
   */
  void MotorControlComponentImpl :: MC_PositionCounterReset_cmdHandler( const FwOpcodeType opCode,
                                                                        const U32 cmdSeq,
                                                                        U8 ResetPositionCounter){
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Gets the size data.
   *
   * @param[in]  id    The identifier
   *
   * @return     The size data.
   */
  uint32_t MotorControlComponentImpl :: getSizeData(const MotorControllerI2C::I2cRegisterId id){
    switch(id){
        case MotorControllerI2C::I2C_ADDRESS:
        case MotorControllerI2C::I2C_DIRECTION:
        case MotorControllerI2C::I2C_TARGET_SPEED:
        case MotorControllerI2C::I2C_EXECUTE_CMD:
        case MotorControllerI2C::I2C_ENABLE_DRIVER:
        case MotorControllerI2C::I2C_DISABLE_DRIVER:
        case MotorControllerI2C::I2C_RESET_CONTROLLER:
        case MotorControllerI2C::I2C_FAULT_REGISTER:
        case MotorControllerI2C::I2C_CLEAR_FAULT:
        case MotorControllerI2C::I2C_STATUS_REGISTER:
        case MotorControllerI2C::I2C_POSITION_SENSOR_CURRENT_COMBINATION:
          return 1;
        case MotorControllerI2C::I2C_MOTOR_CURRENT:
        case MotorControllerI2C::I2C_P_CURRENT:
        case MotorControllerI2C::I2C_I_CURRENT:
        case MotorControllerI2C::I2C_P_VELOCITY:
        case MotorControllerI2C::I2C_I_VELOCITY:
        case MotorControllerI2C::I2C_D_VELOCITY:
        case MotorControllerI2C::I2C_P_POSITION:
        case MotorControllerI2C::I2C_I_POSITION:
        case MotorControllerI2C::I2C_D_POSITION:
        case MotorControllerI2C::I2C_ACCELERATION:
        case MotorControllerI2C::I2C_DECELERATION:
        case MotorControllerI2C::I2C_CURRENT_VELOCITY:
          return 2;
        case MotorControllerI2C::I2C_RELATIVE_TARGET_POSITION:
        case MotorControllerI2C::I2C_CURRENT_POSITION:
          return 4;
        case MotorControllerI2C::I2C_MC_MAX_NUM_OF_ELEMENTS:
        default:
          return 0;
      }   
  }


  /**
   * @brief      Gets the header size.
   *
   * @return     The header size.
   */
  uint32_t MotorControlComponentImpl :: getHeaderSize(){
    return sizeof(MotorControllerI2C::MotorControlI2cRegId_t) + 1 /* byte that contains data size */;
  }


  /**
   * @brief      Gets the checksum size.
   *
   * @return     The checksum size.
   */
  uint32_t MotorControlComponentImpl :: getChecksumSize(){
    return sizeof(MotorControlChecksum);
  }


  /**
   * @brief      Generate look-up table to speed calculation of checksum
   *             calculation
   *
   * @param[in]  polynomial  The polynomial
   */
  void MotorControlComponentImpl :: generateChecksumTable(const ChecksumType polynomial){
  
    for(int32_t i=0; i<sizeof(checksumLookUpTable); ++i){
  
      int32_t curr=i;
  
      for(int32_t j=0; j<8; ++j){
        if((curr & 0x80) != 0){
          curr = (curr << 1) ^ (int)polynomial;
        } 
        else{
          curr <<= 1;
        }
      }
      
      checksumLookUpTable[i] = (uint8_t) curr;
    }
  }



   /**
    * @brief      Calculates the checksum 8.
    *
    * @param      data          The data
    * @param[in]  bufferLength  The buffer length
    * @param      checksum      The checksum
    *
    * @return     The checksum 8.
    */
  MCError MotorControlComponentImpl :: computeChecksum8( uint8_t *data,
                                                         const uint32_t bufferLength,
                                                         MotorControlChecksum *checksum){
    MotorControlChecksum sum = 0;

    if(data == NULL)
      return MC_UNEXPECTED_ERROR;


    for(uint32_t i=0; i<bufferLength; i++)
      sum = checksumLookUpTable[sum ^ data[i]];

    *checksum = sum;

    return MC_NO_ERROR;
  }


  /**
   * @brief      Pack data for a transmission buffer
   *
   * @param[in]  id          The identifier
   * @param[in]  data        The data
   * @param[in]  dataLength  The data length
   *
   * @return     Motor Controller error
   */
  MCError MotorControlComponentImpl :: packTransmitBuffer(const MotorControllerI2C::I2cRegisterId id,
                                                          const uint32_t data,
                                                          const uint32_t dataLength){

    uint32_t packetLength = getHeaderSize() + dataLength + getChecksumSize();

    if(packetLength <= 0 || packetLength > MC_BUFFER_MAX_SIZE){
      return MC_UNEXPECTED_ERROR;
    }

    txData[0] = id;
    txData[1] = dataLength;
    memcpy(txData+2, &data, sizeof(data));
    computeChecksum8(txData, packetLength-1, txData+packetLength-1 );

    return MC_NO_ERROR;
  }


  /**
   * @brief      Writes a register.
   *
   * @param      i2c   I 2 c
   * @param[in]  id    The identifier
   * @param[in]  add   The add
   * @param[in]  data  The data
   *
   * @return     Motor Controller error
   */
  MCError MotorControlComponentImpl :: writeMotorControlRegister( i2cBASE_t *i2c,
                                                                  const MotorControllerI2C::I2cRegisterId id,
                                                                  const MotorControllerI2C::I2cSlaveAddress add,
                                                                  uint32_t data){
      MCError ret = MC_NO_ERROR;
      uint32_t dataLength = getSizeData(id);

      if(dataLength <= 0) 
        return MC_UNEXPECTED_ERROR;

      if(i2c == NULL)
        return MC_UNEXPECTED_ERROR;

      if((ret = packTransmitBuffer(id, data, dataLength)) != MC_NO_ERROR)
        return ret;

      // Send command to all motor controllers
      if((ret = i2cMasterTransmit(i2c, add, dataLength, txData)) != MC_NO_ERROR)
        return ret;

      return ret;
  }


  /**
   * @brief      I2c master transmit
   *
   * @param      i2c     I 2 c
   * @param[in]  sadd    The slave address
   * @param[in]  length  The length
   * @param[in]  data    The data
   *
   * @return     Motor Controller error
   */
  MCError MotorControlComponentImpl :: i2cMasterTransmit(i2cBASE_t *i2c,
                                                      const MotorControllerI2C::I2cSlaveAddress sadd,
                                                      const uint32_t length,
                                                      uint8_t * data){
    if(i2c == NULL)
      return MC_UNEXPECTED_ERROR;

    if(data == NULL)
      return MC_UNEXPECTED_ERROR;

    /* Configure address of Slave to talk to */
    i2cSetSlaveAdd(i2c, sadd);

    /* Set direction to Transmitter */
    i2cSetDirection(i2c, I2C_TRANSMITTER);

    /* Configure Data count */
    i2cSetCount(i2c, length);

    /* Set mode as Master */
    i2cSetMode(i2c, I2C_MASTER);

    /* Set Stop after programmed Count */
    i2cSetStop(i2c);

    /* Transmit Start Condition */
    i2cSetStart(i2c);

    /* Transmit DATA_COUNT number of data in Polling mode */
    i2cSend(i2c, length, data);

    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(i2c) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(i2c) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(i2c);

    return MC_NO_ERROR;
  }


  /**
   * @brief      I2C master receive
   *
   * @param      i2c     I 2 c
   * @param[in]  sadd    The slave address
   * @param[in]  length  The length
   * @param[in]  data    The data
   *
   * @return     Motor controller error
   */
  MCError MotorControlComponentImpl :: i2cMasterReceive(i2cBASE_t *i2c,
                                                     const MotorControllerI2C::I2cSlaveAddress sadd,
                                                     const uint32_t length,
                                                     uint8_t * data){
    if(i2c == NULL)
      return MC_UNEXPECTED_ERROR;

    if(data == NULL)
      return MC_UNEXPECTED_ERROR;

    /* Configure address of Slave to talk to */
    i2cSetSlaveAdd(i2c, sadd);

    /* Set direction to receiver */
    i2cSetDirection(i2c, I2C_RECEIVER);

    /* Configure Data count */
    i2cSetCount(i2c, length);

    /* Set mode as Master */
    i2cSetMode(i2c, I2C_MASTER);

    /* Set Stop after programmed Count */
    i2cSetStop(i2c);

    /* Transmit Start Condition */
    i2cSetStart(i2c);

    /* Transmit DATA_COUNT number of data in Polling mode */
    i2cReceive(i2c, length, data);

    /* Wait until Bus Busy is cleared */
    // TODO add timeout
    while(i2cIsBusBusy(i2c) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(i2c) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(i2c);

    return MC_NO_ERROR;
  }

} // end namespace CubeRover
