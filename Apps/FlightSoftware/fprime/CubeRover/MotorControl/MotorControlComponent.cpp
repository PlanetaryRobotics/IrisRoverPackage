// ======================================================================
// \title  MotorControlComponentImpl.cpp
// \author cedric
// \editted by Michael
// \brief  cpp file for MotorControl component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <CubeRover/MotorControl/MotorControlComponent.hpp>
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
   * @param[in]  instance    The instance
   */
  void MotorControlComponentImpl :: init( const NATIVE_INT_TYPE instance)
  {
    MotorControlComponentBase::init(instance);

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

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  /**
   * @brief      Handler implementation for ping (health)
   *
   * @param[in]  portNum  The port number
   * @param[in]  key      Value to return to pinger
   */
  void MotorControlComponentImpl :: PingIn_handler(const NATIVE_INT_TYPE portNum, U32 key)
  {
    this->PingOut_out(portNum, key);
  }

  /**
   * @brief      Handler implementation for ping (health)
   *
   * @param[in]  portNum  The port number
   * @param[in]  context  ???
   */
  /*void MotorControlComponentImpl :: schedIn_handler(const NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context)
  {

      MCError err;
      err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                      MotorControllerI2C::RELATIVE_TARGET_POSITION,
                                      0x48,
                                      0);

      uint8_t stop_dummy_buffer[2] = {0x0,0x0};
      uint8_t dummy_buffer[2] = {0x00,0xFF};
      i2cMasterTransmit(MOTOR_CONTROL_I2CREG, 0x48, 2, dummy_buffer);

      i2cMasterTransmit(MOTOR_CONTROL_I2CREG, 0x48, 2, stop_dummy_buffer);

      uint8_t data_buffer[] = {0,0,0,0};
      err = i2cMasterReceive(MOTOR_CONTROL_I2CREG,
                             0x48,
                             4,
                             data_buffer);*/
//    if(i2cIsTxReady(MOTOR_CONTROL_I2CREG))
//    {
//      /* Configure address of Slave to talk to */
//      i2cSetSlaveAdd(MOTOR_CONTROL_I2CREG, 0x48);
//
//      /* Set direction to Transmitter */
//      i2cSetDirection(MOTOR_CONTROL_I2CREG, I2C_TRANSMITTER);
//
//      /* Configure Data count */
//      i2cSetCount(MOTOR_CONTROL_I2CREG, 8);
//
//      /* Set mode as Master */
//      i2cSetMode(MOTOR_CONTROL_I2CREG, I2C_MASTER);
//
//      /* Set Stop after programmed Count */
//      i2cSetStop(MOTOR_CONTROL_I2CREG);
//
//      /* Transmit Start Condition */
//      i2cSetStart(MOTOR_CONTROL_I2CREG);
//
//      /* Transmit DATA_COUNT number of data in Polling mode */
//      i2cSendByte(MOTOR_CONTROL_I2CREG, 0x85);
//
//      /* Wait until Bus Busy is cleared */
//      while(i2cIsBusBusy(MOTOR_CONTROL_I2CREG) == true);
//
//      /* Wait until Stop is detected */
//      while(i2cIsStopDetected(MOTOR_CONTROL_I2CREG) == 0);
//
//      /* Clear the Stop condition */
//      i2cClearSCD(MOTOR_CONTROL_I2CREG);
//    }
//    else
//    {
//      // TX is not ready
//      //Move_all_motors(3*rotations_to_ticks);
//    }
    
//  }

  /**
   * @brief      Handler implementation for motorCommand port (move command from Nav)
   *
   * @param[in]  portNum        The port number
   * @param[in]  command_type   ???
   * @param[in]  movement_type  The port number
   * @param[in]  Distance       ???
   * @param[in]  Speed          ???
   */
  void MotorControlComponentImpl :: motorCommandIn_handler(const NATIVE_INT_TYPE portNum,
                                                           CubeRoverPorts::MC_CommandType command_type,
                                                           CubeRoverPorts::MC_MovementType movement_type,
                                                           U8 Distance,
                                                           U8 Speed)
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  /**
   * @brief      Command handler implementation to change PI values
   *
   * @param[in]  opCode          The operation code
   * @param[in]  cmdSeq          The command sequence
   * @param[in]  Motor_ID        The motor(s) ID
   * @param[in]  PI_Values       The new PI values 
   */
  void MotorControlComponentImpl :: MC_Current_PID_cmdHandler(const FwOpcodeType opCode,
                                                             const U32 cmdSeq,
                                                             U8 Motor_ID,
                                                             U32 PI_Values)
  {
    // TODO
    // Not a valid option!
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Command handler implementation to change PID values
   *
   * @param[in]  opCode          The operation code
   * @param[in]  cmdSeq          The command sequence
   * @param[in]  Motor_ID        The motor(s) ID
   * @param[in]  PID_Values      The new PID values 
   */
  void MotorControlComponentImpl :: MC_Speed_PID_cmdHandler(const FwOpcodeType opCode,
                                                            const U32 cmdSeq,
                                                            U8 Motor_ID,
                                                            U64 PID_Values)
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Command handler implementation to change PID values
   *
   * @param[in]  opCode          The operation code
   * @param[in]  cmdSeq          The command sequence
   * @param[in]  Motor_ID        The motor(s) ID
   * @param[in]  PID_Values      The new PID values 
   */
  void MotorControlComponentImpl :: MC_Position_PID_cmdHandler(const FwOpcodeType opCode,
                                                               const U32 cmdSeq,
                                                               U8 Motor_ID,
                                                               U64 PID_Values)
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Command handler implementation to change accel/deceleration
   *                  This function does nothing since the MC doesn't have
   *                  this code
   *
   * @param[in]  opCode          The operation code
   * @param[in]  cmdSeq          The command sequence
   * @param[in]  Motor_ID        The motor(s) ID
   * @param[in]  Rate_Values     A value to enable or disable powerbost
   */
  void MotorControlComponentImpl :: MC_Acceleration_cmdHandler(const FwOpcodeType opCode,
                                                               const U32 cmdSeq,
                                                               U8 Motor_ID,
                                                               U32 Rate_Values)
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Command handler implementation to control stall detection
   *
   * @param[in]  opCode          The operation code
   * @param[in]  cmdSeq          The command sequence
   * @param[in]  Motor_ID        The motor(s) ID
   * @param[in]  Spin_Type       A value to enable or disable powerbost
   */
  void MotorControlComponentImpl :: MC_StallDetection_cmdHandler(const FwOpcodeType opCode,
                                                                 const U32 cmdSeq,
                                                                 U8 Motor_ID,
                                                                 U8 Value)
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Command handler implementation to reset encoder counts
   *
   * @param[in]  opCode          The operation code
   * @param[in]  cmdSeq          The command sequence
   * @param[in]  Motor_ID        The motor(s) ID
   */
  void MotorControlComponentImpl :: MC_ResetPosition_cmdHandler(const FwOpcodeType opCode,
                                                                const U32 cmdSeq,
                                                                U8 Motor_ID)
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Command handler implementation to manually spin (or stop)
   *
   * @param[in]  opCode          The operation code
   * @param[in]  cmdSeq          The command sequence
   * @param[in]  Motor_ID        The motor(s) ID
   * @param[in]  Spin_Type       A value to enable or disable powerbost
   */
  void MotorControlComponentImpl :: MC_Spin_cmdHandler(const FwOpcodeType opCode,
                                                       const U32 cmdSeq,
                                                       U8 Motor_ID,
                                                       U8 Spin_Type)
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Command handler implementation to control power limits
   *
   * @param[in]  opCode          The operation code
   * @param[in]  cmdSeq          The command sequence
   * @param[in]  Motor_ID        The motor(s) ID
   * @param[in]  Value           A value to enable or disable powerbost
   */
  void MotorControlComponentImpl :: MC_PowerBoost_cmdHandler(const FwOpcodeType opCode,
                                                             const U32 cmdSeq,
                                                             U8 Motor_ID,
                                                             U8 Value)
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Command handler implementation to set a specific parameter
   *
   * @param[in]  opCode          The operation code
   * @param[in]  cmdSeq          The command sequence
   * @param[in]  ParamSelect     The selected parameter
   * @param[in]  New_Value       The new parameter value
   */
  void MotorControlComponentImpl :: MC_SetParameter_cmdHandler(const FwOpcodeType opCode,
                                                               const U32 cmdSeq,
                                                               MC_ParameterSelection ParamSelect,
                                                               U32 New_Value)
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Command handler implementation to retrieve all parameters
   *
   * @param[in]  opCode          The operation code
   * @param[in]  cmdSeq          The command sequence
   */
  void MotorControlComponentImpl :: MC_GetParameters_cmdHandler(const FwOpcodeType opCode,
                                                               const U32 cmdSeq)
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Command handler implementation to force update telemetry
   *
   * @param[in]  opCode          The operation code
   * @param[in]  cmdSeq          The command sequence
   */
  void MotorControlComponentImpl :: MC_UpdateTelemetry_cmdHandler(const FwOpcodeType opCode,
                                                                  const U32 cmdSeq)
  {
    // FIRST CHECK STATUS!

    MCError err = MC_NO_ERROR;
    uint8_t Data_buffer[MC_BUFFER_MAX_SIZE];

    uint32_t Data_size = getSizeData(MotorControllerI2C::CURRENT_POSITION);


    // Requesting Front Left Tick count
    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                    MotorControllerI2C::CURRENT_POSITION,
                                    FRONT_LEFT_MC_I2C_ADDR,
                                    0);
    if(err != MC_NO_ERROR)
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
      return;          
    }

    // Polling front left tick count
    err = i2cMasterReceive(MOTOR_CONTROL_I2CREG,
                           FRONT_LEFT_MC_I2C_ADDR,
                           Data_size, 
                           Data_buffer);

    if(err != MC_NO_ERROR)
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
      return;          
    }

    // Copy from the buffer
    FL_Encoder_Count = Data_buffer[0];

    // Requesting Front right Tick count
    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                    MotorControllerI2C::CURRENT_POSITION,
                                    FRONT_RIGHT_MC_I2C_ADDR,
                                    0);
    if(err != MC_NO_ERROR)
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
      return;          
    }

    // Polling Front right tick count
    err = i2cMasterReceive(MOTOR_CONTROL_I2CREG,
                           FRONT_RIGHT_MC_I2C_ADDR,
                           Data_size, 
                           Data_buffer);

    if(err != MC_NO_ERROR)
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
      return;          
    }

    // Copy from the buffer
    FR_Encoder_Count = Data_buffer[0];

    // Requesting Rear right Tick count
    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                    MotorControllerI2C::CURRENT_POSITION,
                                    REAR_RIGHT_MC_I2C_ADDR,
                                    0);
    if(err != MC_NO_ERROR)
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
      return;          
    }

    // Polling Rear right tick count
    err = i2cMasterReceive(MOTOR_CONTROL_I2CREG,
                           REAR_RIGHT_MC_I2C_ADDR,
                           Data_size, 
                           Data_buffer);

    if(err != MC_NO_ERROR)
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
      return;          
    }

    // Copy from the buffer
    RR_Encoder_Count = Data_buffer[0];

    // Requesting Rear left Tick count
    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                    MotorControllerI2C::CURRENT_POSITION,
                                    REAR_LEFT_MC_I2C_ADDR,
                                    0);
    if(err != MC_NO_ERROR)
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
      return;          
    }

    // Polling Rear left tick count
    err = i2cMasterReceive(MOTOR_CONTROL_I2CREG,
                           REAR_LEFT_MC_I2C_ADDR,
                           Data_size, 
                           Data_buffer);

    if(err != MC_NO_ERROR)
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
      return;          
    }

    // Copy from the buffer
    RL_Encoder_Count = Data_buffer[0];

    // NOW WE UPDATE TLM but we also need current and speed!
    //tlmWrite_X_Acc(accX);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Motor configuration command handler implementation
   *
   * @param[in]  opCode          The operation code
   * @param[in]  cmdSeq          The command sequence
   * @param[in]  Motor_ID        The selected motor (4 means all motors)
   * @param[in]  MotorParameter  The targeted parameter to change
   * @param[in]  Value           The new value
   */
  void MotorControlComponentImpl :: MC_MotorConfiguration_cmdHandler(const FwOpcodeType opCode,
                                                                     const U32 cmdSeq,
                                                                     U8 Motor_ID,
                                                                     MP_CommandList MotorParameter,
                                                                     U32 Value)
  {


    // Not a valid option!
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);

    // If we reached here we succeded.
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Driving configuration command handler implementation
   *
   * @param[in]  opCode                The operation code
   * @param[in]  cmdSeq                The command sequence
   * @param[in]  CommandConfiguration  The command configuration
   * @param[in]  Parameter             The parameter
   * @param[in]  Value                 The value
   */
  void MotorControlComponentImpl :: MC_DrivingConfiguration_cmdHandler(const FwOpcodeType opCode,
                                                                       const U32 cmdSeq,
                                                                       CC_CommandList CommandConfiguration,
                                                                       ParameterList Parameter,
                                                                       U8 Value)
  {
    switch(CommandConfiguration)
    {
      // Moving forward command configuration
      case FORWARD_CFG:
        switch(Parameter)
        {
          // Setting the distance to travel
          case DISTANCE:
            m_fwDist = Value;
            break;

          // Setting the speed to travel
          case SPEED:
            m_fwSpeed = Value;
            break;

          // Not a valid option
          default:
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }
        break;

      // Moving backward command configuration
      case REVERSE_CFG:
        switch(Parameter)
        {
          // Setting the distance to travel
          case DISTANCE:
            m_reDist = Value;
            break;

          // Setting the speed to travel
          case SPEED:
            m_reSpeed = Value;
            break;

          // Not a valid option
          default:
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }
        break;

      // Rotating to the left (counter clockwise) command configuration
      case LEFT_CFG:
        switch(Parameter)
        {
          // Setting the distance to travel
          case ANGLE:
            m_leftAngle = Value;
            break;

          // Setting the speed to travel
          case SPEED:
            m_leftSpeed = Value;
            break;

          // Not a valid option
          default:
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }
        break;

      // Rotating to the right (clockwise) command configuration
      case RIGHT_CFG:
        switch(Parameter)
        {
          // Setting the distance to travel
          case ANGLE:
            m_rightAngle = Value;
            break;

          // Setting the speed to travel
          case SPEED:
            m_rightSpeed = Value;
            break;

          // Not a valid option
          default:
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }

        // Command configuration was successful!
        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
        break;

        // Not a valid option
        default:
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;
    }
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
                                                                  DriveCommandList DrivingCommand)
  {
    MCError err = MC_NO_ERROR;
    Motor_tick targetTick = cmToMotorTicks(m_fwDist);

    switch(DrivingCommand)
    {
      case GO_REVERSE:
      case GO_FORWARD:

        // Set target position in ticks for the front left wheel
        err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG, 
                                        MotorControllerI2C::RELATIVE_TARGET_POSITION, 
                                        FRONT_LEFT_MC_I2C_ADDR,
                                        targetTick);
        if(err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;
        }

        // Set target position in ticks for the front right wheel
        err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                        MotorControllerI2C::RELATIVE_TARGET_POSITION,
                                        FRONT_RIGHT_MC_I2C_ADDR,
                                        targetTick);
        if(err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;
        }

        // Set target position in ticks for the rear right wheel
        err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                        MotorControllerI2C::RELATIVE_TARGET_POSITION,
                                        REAR_RIGHT_MC_I2C_ADDR,
                                        targetTick);
        if(err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        // Set target position in ticks for the rear left wheel
        err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                        MotorControllerI2C::RELATIVE_TARGET_POSITION,
                                        REAR_LEFT_MC_I2C_ADDR,
                                        targetTick);
        if(err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }
        
        // Set target speed in percentage for front left motor
        err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                        MotorControllerI2C::TARGET_SPEED,
                                        FRONT_LEFT_MC_I2C_ADDR,
                                        targetTick);
        if(err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        // Set target speed in percentage for front right motor
        err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                        MotorControllerI2C::TARGET_SPEED,
                                        FRONT_RIGHT_MC_I2C_ADDR,
                                        targetTick);
        if(err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        // Set target speed in percentage for rear right motor
        err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                        MotorControllerI2C::TARGET_SPEED,
                                        REAR_RIGHT_MC_I2C_ADDR,
                                        targetTick);
        if(err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }  

        // Set target speed in percentage for rear left motor
        err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                        MotorControllerI2C::TARGET_SPEED,
                                        REAR_LEFT_MC_I2C_ADDR,
                                        targetTick);
        if(err != MC_NO_ERROR)
        {
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
    }// End Switch statement

    // Enable all motor controller drivers
    enableDrivers();

    // Send command to all motor controllers to execute driving command
    err = sendAllMotorsData(MOTOR_CONTROL_I2CREG,
                            MotorControllerI2C::EXECUTE_CMD,
                            DrivingCommand);

    if(err != MC_NO_ERROR)
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
      return;          
    }

    else
    {
      // Everything was successful if we reached here
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
    }
  }

  /**
   * @brief      Handle commands that adjust acceleration profiles of the rover
   *
   * @param[in]  opCode                 The operation code
   * @param[in]  cmdSeq                 The command sequence
   * @param[in]  motorStallEnable       The desired new state of stall detection (ENABLED or DISABLED)
   */
  void MotorControlComponentImpl :: MC_StallDetection_cmdHandler(const FwOpcodeType opCode,
                                                                 const U32 cmdSeq,
                                                                 motorStallEnableList motorStallEnable)
  {
    switch(motorStallEnable)
    {
      // Enable stall detection
      case ENABLED:
        m_stallDetectectionEnabled = true;
        break;

      // Disable stall detection
      case DISABLED:
        m_stallDetectectionEnabled = false;
        break;

      // Not a valid configuration
      default:
        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
        return;
    }

    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Handle commands that adjust acceleration profiles of the rover
   *
   * @param[in]  opCode                 The operation code
   * @param[in]  cmdSeq                 The command sequence
   * @param[in]  ResetPositionCounter   Bit mask selecting which motor(s) to reset
   */
  void MotorControlComponentImpl :: MC_PositionCounterReset_cmdHandler(const FwOpcodeType opCode,
                                                                       const U32 cmdSeq,
                                                                       U8 ResetPositionCounter)
  {
    // The "first" 4 bits are the desired encoders to reset
    // Follows the mentality that motor 0 (bit 0) if FL, and moves in the clockwise formation

    // Motor 0 (FL)
    if(00000001 & ResetPositionCounter != 0)
    {
      FL_Encoder_Count_Offset = -FL_Encoder_Count;
    }

    // Motor 1 (FR)
    if(00000010 & ResetPositionCounter != 0)
    {
      FR_Encoder_Count_Offset = -FR_Encoder_Count;
    }

    // Motor 2 (RR)
    if(00000100 & ResetPositionCounter != 0)
    {
      RR_Encoder_Count_Offset = -RR_Encoder_Count;
    }

    // Motor 3 (RL)
    if(00001000 & ResetPositionCounter != 0)
    {
      RL_Encoder_Count_Offset = -RL_Encoder_Count;
    }

    // If none of the bits were set, the operator is calling a meaningless command, so raise an error
    if(00001111 & ResetPositionCounter != 0)
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
      return;
    }

    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

/**
   * @brief      Gets the size data.
   *
   * @param[in]  id    The identifier
   *
   * @return     The size data.
   */
  uint32_t MotorControlComponentImpl :: getSizeData(const MotorControllerI2C::I2cRegisterId id)
  {
    switch(id)
    {
        case MotorControllerI2C::I2C_ADDRESS:
        case MotorControllerI2C::ENABLE_DRIVER:
        case MotorControllerI2C::DISABLE_DRIVER:
        case MotorControllerI2C::RESET_CONTROLLER:
        case MotorControllerI2C::FAULT_REGISTER:
        case MotorControllerI2C::CLEAR_FAULT:
        case MotorControllerI2C::STATUS_REGISTER:
          return 1;
        case MotorControllerI2C::MOTOR_CURRENT:
        case MotorControllerI2C::P_CURRENT:
        case MotorControllerI2C::I_CURRENT:
        case MotorControllerI2C::P_SPEED:
        case MotorControllerI2C::I_SPEED:
        case MotorControllerI2C::ACC_RATE:
        case MotorControllerI2C::DEC_RATE:
        case MotorControllerI2C::CURRENT_POSITION:
        case MotorControllerI2C::TARGET_SPEED:
          return 2;
        case MotorControllerI2C::RELATIVE_TARGET_POSITION:
        case MotorControllerI2C::CURRENT_SPEED:
          return 4;
        case MotorControllerI2C::MAX_NB_CMDS:
        default:
          return 0;
      }   
  }

  /**
   * @brief      Gets the header size.
   *
   * @return     The header size.
   */
  uint32_t MotorControlComponentImpl :: getHeaderSize()
  {
    return sizeof(MotorControllerI2C::MotorControlI2cRegId_t) + 1 /* byte that contains data size */;
  }


  /**
   * @brief      Gets the checksum size.
   *
   * @return     The checksum size.
   */
  uint32_t MotorControlComponentImpl :: getChecksumSize()
  {
    return sizeof(MotorControlChecksum);
  }


  /**
   * @brief      Generate look-up table to speed calculation of checksum
   *             calculation
   *
   *             NOTE: NOTE IMPLEMENTED
   *
   * @param[in]  polynomial  The polynomial
   */
  void MotorControlComponentImpl :: generateChecksumTable(const ChecksumType polynomial){
  
    for(int32_t i=0; i<sizeof(checksumLookUpTable); ++i)
    {
      int32_t curr=i;
  
      for(int32_t j=0; j<8; ++j){

        if((curr & 0x80) != 0)
        {
          curr = (curr << 1) ^ (int)polynomial;
        } 

        else
        {
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
    *             NOTE: NOTE IMPLEMENTED
    *
    * @return     The checksum 8.
    */
  MCError MotorControlComponentImpl :: computeChecksum8(uint8_t *data,
                                                        const uint32_t bufferLength,
                                                        MotorControlChecksum *checksum){
    MotorControlChecksum sum = 0;

    if(data == NULL)
      return MC_UNEXPECTED_ERROR;


    for(uint32_t i=0; i<bufferLength; i++)
    {
      sum = checksumLookUpTable[sum ^ data[i]];
    }

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

    if(packetLength <= 0 || packetLength > MC_BUFFER_MAX_SIZE)
      return MC_UNEXPECTED_ERROR;

    txData[0] = id;
    txData[1] = dataLength;
    memcpy(txData+2, &data, sizeof(data));
    computeChecksum8(txData, packetLength-1, txData+packetLength-1 );

    return MC_NO_ERROR;
  }

  /**
   * @brief      Sends the same data to every motor register, returning any errors found
   *
   * @param      i2c   I 2 c
   * @param[in]  id    The identifier
   * @param[in]  data  The data
   *
   * @return     Motor Controller error
   */

  MCError MotorControlComponentImpl :: sendAllMotorsData(i2cBASE_t *i2c,
                                                         const MotorControllerI2C::I2cRegisterId id,
                                                         uint32_t data)
  {
    MCError err = MC_NO_ERROR;
    // Send command to all motor controllers
    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                    id,
                                    FRONT_LEFT_MC_I2C_ADDR,
                                    data);
    if(err != MC_NO_ERROR)
    {
      return err;          
    }

    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                    id,
                                    FRONT_RIGHT_MC_I2C_ADDR,
                                    data);
    if(err != MC_NO_ERROR)
    {
      return err;          
    }  

    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                    id,
                                    REAR_LEFT_MC_I2C_ADDR,
                                    data);
    if(err != MC_NO_ERROR)
    {
      return err;          
    }  

    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG,
                                    id,
                                    REAR_RIGHT_MC_I2C_ADDR,
                                    data);
    if(err != MC_NO_ERROR)
    {
      return err;          
    }

    return MC_NO_ERROR;  
  }

  /**
   * @brief      Helper function to move all motors simultaneously
   *
   * @param[in]  Distance       The distance to travel in motor ticks
   * @param[in]  Speed          The speed to travel in normalized speed
   */
  MCError moveAllMotorsStraight(uint32_t distance, uint16_t speed)
  {
    // Error preset
    MCError err;
    err = sendAllMotorsData(MOTOR_CONTROL_I2CREG, 
                            MotorControllerI2C::CURRENT_SPEED,
                            &speed)
    if(err != MC_NO_ERROR)
      return err;          

    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG, 
                                    MotorControllerI2C::CURRENT_POSITION, 
                                    FRONT_LEFT_MC_I2C_ADDR,
                                    &distance);
    if(err != MC_NO_ERROR)
      return err;  

    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG, 
                                    MotorControllerI2C::CURRENT_POSITION, 
                                    FRONT_RIGHT_MC_I2C_ADDR,
                                    &distance);
    if(err != MC_NO_ERROR)
      return err;  

    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG, 
                                    MotorControllerI2C::CURRENT_POSITION, 
                                    REAR_RIGHT_MC_I2C_ADDR,
                                    &distance);
    if(err != MC_NO_ERROR)
      return err;  

    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG, 
                                    MotorControllerI2C::CURRENT_POSITION, 
                                    REAR_LEFT_MC_I2C_ADDR,
                                    &distance);
    return err;  
  }

  /**
  * @brief      Helper function to rotate all motors simultaneously
  *
  * @param[in]  Distance       ???
  * @param[in]  Speed          ???
  */
  MCError rotateAllMotors(uint8_t distance, uint8_t speed)
  {
    // Error preset
    MCError err;
    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG, 
                                    MotorControllerI2C::CURRENT_POSITION, 
                                    FRONT_LEFT_MC_I2C_ADDR,
                                    distance);

    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG, 
                                    MotorControllerI2C::CURRENT_POSITION, 
                                    FRONT_RIGHT_MC_I2C_ADDR,
                                    distance);

    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG, 
                                    MotorControllerI2C::CURRENT_POSITION, 
                                    REAR_RIGHT_MC_I2C_ADDR,
                                    distance);

    err = writeMotorControlRegister(MOTOR_CONTROL_I2CREG, 
                                    MotorControllerI2C::CURRENT_POSITION, 
                                    REAR_LEFT_MC_I2C_ADDR,
                                    distance);
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
  MCError MotorControlComponentImpl :: writeMotorControlRegister(i2cBASE_t *i2c,
                                                                 const MotorControllerI2C::I2cRegisterId id,
                                                                 const MotorControllerI2C::I2cSlaveAddress add,
                                                                 uint8_t * data)
  {
      MCError ret = MC_NO_ERROR;
      uint32_t dataLength = getSizeData(id);

      if(dataLength <= 0) 
        return MC_UNEXPECTED_ERROR;

      if(i2c == NULL)
        return MC_UNEXPECTED_ERROR;

      // Inform the MSP of the desired register
      if((ret = i2cMasterTransmit(i2c, add, 1, id)) != MC_NO_ERROR)
        return ret;

      // If we want something, receive
      if(expectingReturnMessage(id))
        ret = i2cMasterReceive(i2c, add, dataLength, data) != MC_NO_ERROR

      // Else, tell the data you have
      else
        ret = i2cMasterReceive(i2c, add, dataLength, data) != MC_NO_ERROR

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
                                                         uint8_t * data)
  {
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
    uint16_t timeouter = 0;
    while(i2cIsBusBusy(i2c) == true)
    {
      if(++timeouter > timeout_threshold)
        return MC_I2C_TIMEOUT_ERROR;
    }   

    /* Wait until Stop is detected */
    timeouter = 0;
    while(i2cIsStopDetected(i2c) == 0)
    {
      if(++timeouter > timeout_threshold)
        return MC_I2C_TIMEOUT_ERROR;
    }   

    /* Clear the Stop condition */
    i2cClearSCD(i2c);

    /* Delay long enough for the slave to be ready */
    delayForI2C();

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
                                                        uint8_t * data)
  {
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
    uint16_t timeouter = 0;
    while(i2cIsBusBusy(i2c) == true)
    {
      if(++timeouter > timeout_threshold)
        return MC_I2C_TIMEOUT_ERROR;
    }   

    /* Wait until Stop is detected */
    timeouter = 0;
    while(i2cIsStopDetected(i2c) == 0)
    {
      if(++timeouter > timeout_threshold)
        return MC_I2C_TIMEOUT_ERROR;
    }

    /* Clear the Stop condition */
    i2cClearSCD(i2c);

    /* Delay long enough for the slave to be ready */
    delayForI2C();

    return MC_NO_ERROR;
  }

  /**
  * @brief      eEnables all motors
  *
  * @return     Motor controller error
  */
  MCError MotorControlComponentImpl :: enableDrivers()
  {
    return MC_NO_ERROR;
  }

  /**
  * @brief      Converts cm to motor ticks
  *
  * @param[in]  Distance the system wants to travel in cm    
  *
  * @return     Motor controller error
  */
  Motor_tick MotorControlComponentImpl :: cmToMotorTicks(const Distance_cm dist)
  {
    return 0;
  }

  /**
  * @brief      Delays for 1050 ticks slow enough for slave sides
  *
  */
  void MotorControlComponentImpl :: delayForI2C()
  {
    // for (unsigned i = 180000000; i; --i); ~= 13.5s
    for (unsigned i = 900; i; --i);
  }

  bool MotorControlComponentImpl :: expectingReturnMessage(const MotorControllerI2C::I2cRegisterId id)
  {
    switch(id)
    {
      case MotorControllerI2C::I2C_ADDRESS:
      case MotorControllerI2C::CURRENT_POSITION:
      case MotorControllerI2C::CURRENT_SPEED:
      case MotorControllerI2C::MOTOR_CURRENT:
        return true;

      default:
        return false;
    } 
  }

} // end namespace CubeRover
