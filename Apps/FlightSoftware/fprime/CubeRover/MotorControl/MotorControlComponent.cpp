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
    m_i2c = i2cREG1;
    m_stallDetectectionEnabled[0] = true;
    m_stallDetectectionEnabled[1] = true;
    m_stallDetectectionEnabled[2] = true;
    m_stallDetectectionEnabled[3] = true;
    m_FL_Encoder_Count_Offset = 0;
    m_FR_Encoder_Count_Offset = 0;
    m_RR_Encoder_Count_Offset = 0;
    m_RL_Encoder_Count_Offset = 0;
    m_FL_Encoder_Count = 0;
    m_FR_Encoder_Count = 0;
    m_RR_Encoder_Count = 0;
    m_RL_Encoder_Count = 0;
  }

  /**
   * @brief      Initialize the motor control component
   *
   * @param[in]  instance    The instance
   */
  void MotorControlComponentImpl :: init( const NATIVE_INT_TYPE instance)
  {
    MotorControlComponentBase::init(instance);

    // Initalized the ticks per rotation
    m_ticksToRotation = 9750;

    // Initialize the encoder tick to cm ratio
    m_encoderTickToCMRatio = m_ticksToRotation / ( PI * CUBEROVER_WHEEL_DIAMETER_CM); //(PI * CUBEROVER_WHEEL_DIAMETER_CM) / (MOTOR_NB_PAIR_POLES * MOTOR_GEAR_BOX_REDUCTION * 6.0);

    // Initalize the converting values
    m_angularToLinear = CUBEROVER_COM_TO_WHEEL_CIRC_CM/360; 
           // This should be the circumference from the COM of the rover to the wheel.

    // Should only 1 of current/encoder/speed be updated at any one time?
    m_Round_Robin_Telemetry = false;
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
    MCError_t err;
    switch(command_type)
    {
      // We actively want to be moving
      case CubeRoverPorts::MC_DrivingConfiguration:
        switch(movement_type)
        {
          case CubeRoverPorts::MC_Forward:
            err = moveAllMotorsStraight(Distance, Speed);
            if (err != MC_NO_ERROR)
            {
              log_WARNING_HI_MC_MSPNotResponding();
            }
            log_COMMAND_MC_moveStarted();
            break;

          case CubeRoverPorts::MC_Backward:
            err = moveAllMotorsStraight(-Distance, Speed);
            if (err != MC_NO_ERROR)
            {
              log_WARNING_HI_MC_MSPNotResponding();
            }
            log_COMMAND_MC_moveStarted();
            break;

          case CubeRoverPorts::MC_Left:
            err = rotateAllMotors(Distance, Speed);
            if (err != MC_NO_ERROR)
            {
              log_WARNING_HI_MC_MSPNotResponding();
            }
            log_COMMAND_MC_moveStarted();
            break;

          case CubeRoverPorts::MC_Right:
            err = rotateAllMotors(-Distance, Speed);
            if (err != MC_NO_ERROR)
            {
              log_WARNING_HI_MC_MSPNotResponding();
            }
            log_COMMAND_MC_moveStarted();
            break;

          // Stopping the system
          case CubeRoverPorts::MC_Stop:
            err = moveAllMotorsStraight(0, 0);
            if (err != MC_NO_ERROR)
            {
              log_WARNING_HI_MC_MSPNotResponding();
            }
            break;

          // Not a valid option, just leave
          default:
            return;
        }
        break;

      // Constant heartbeat to keep updating ground telemetry
      case CubeRoverPorts::MC_UpdateTelemetry:
        updateTelemetry();
        break;

      // Not a valid option, just leave
      default:
        return;
    }
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
    MCError_t err;
    uint16_t P_Value, I_Value;
    // TODO TEST THIS!
    P_Value = (uint16_t) PI_Values;
    I_Value = (uint16_t) (PI_Values << 16);         // FIXME: Justin: highly doubt this si correct

    switch(Motor_ID)
    {
      // FL Motor
      case 0:
        // TODO again, need to find out how to pass int8*
        err = writeMotorControlRegister(REG_P_CURRENT,
                                        FRONT_LEFT_MC_I2C_ADDR,
                                        (uint8_t*) &P_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        err = writeMotorControlRegister(REG_I_CURRENT,
                                        FRONT_LEFT_MC_I2C_ADDR,
                                        (uint8_t*) &I_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }
        break;

      // FR Motor
      case 1:
        // TODO again, need to find out how to pass int8*
        err = writeMotorControlRegister(REG_P_CURRENT,
                                        FRONT_RIGHT_MC_I2C_ADDR,
                                        (uint8_t*) &P_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        err = writeMotorControlRegister(REG_I_CURRENT,
                                        FRONT_LEFT_MC_I2C_ADDR,
                                        (uint8_t*) &I_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }
        break;

      // RR Motor
      case 2:
        // TODO again, need to find out how to pass int8*
        err = writeMotorControlRegister(REG_P_CURRENT,
                                        REAR_RIGHT_MC_I2C_ADDR,
                                        (uint8_t*) &P_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        err = writeMotorControlRegister(REG_I_CURRENT,
                                        REAR_RIGHT_MC_I2C_ADDR,
                                        (uint8_t*) &I_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }
        break;

      // RL Motor
      case 3:
        // TODO again, need to find out how to pass int8*
        err = writeMotorControlRegister(REG_P_CURRENT,
                                        REAR_LEFT_MC_I2C_ADDR,
                                        (uint8_t*) &P_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        err = writeMotorControlRegister(REG_I_CURRENT,
                                        REAR_LEFT_MC_I2C_ADDR,
                                        (uint8_t*) &I_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }
        break;

      case 4:
        // TODO again, need to find out how to pass int8*
        err = sendAllMotorsData(REG_P_CURRENT,
                                (uint8_t*) &P_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        err = sendAllMotorsData(REG_I_CURRENT,
                                (uint8_t*) &I_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }
        break;

      default:
        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
        return;
     }

    // If all else goes well, we succeeded
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
    MCError_t err;
    uint16_t P_Value, I_Value;
    // TODO TEST THIS!
    P_Value = (uint16_t) PID_Values;
    I_Value = (uint16_t) (PID_Values << 16);
    // D's don't exist

    switch(Motor_ID)
    {
      // FL Motor
      case 0:
        // TODO again, need to find out how to pass int8*
        err = writeMotorControlRegister(REG_P_SPEED,
                                        FRONT_LEFT_MC_I2C_ADDR,
                                        (uint8_t*) &P_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        err = writeMotorControlRegister(REG_I_SPEED,
                                        FRONT_LEFT_MC_I2C_ADDR,
                                        (uint8_t*) &I_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }
        break;

      // FR Motor
      case 1:
        // TODO again, need to find out how to pass int8*
        err = writeMotorControlRegister(REG_P_SPEED,
                                        FRONT_RIGHT_MC_I2C_ADDR,
                                        (uint8_t*) &P_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        err = writeMotorControlRegister(REG_I_SPEED,
                                        FRONT_LEFT_MC_I2C_ADDR,
                                        (uint8_t*) &I_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }
        break;

      // RR Motor
      case 2:
        // TODO again, need to find out how to pass int8*
        err = writeMotorControlRegister(REG_P_SPEED,
                                        REAR_RIGHT_MC_I2C_ADDR,
                                        (uint8_t*) &P_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        err = writeMotorControlRegister(REG_I_SPEED,
                                        REAR_RIGHT_MC_I2C_ADDR,
                                        (uint8_t*) &I_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }
        break;

      // RL Motor
      case 3:
        // TODO again, need to find out how to pass int8*
        err = writeMotorControlRegister(REG_P_SPEED,
                                        REAR_LEFT_MC_I2C_ADDR,
                                        (uint8_t*) &P_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        err = writeMotorControlRegister(REG_I_SPEED,
                                        REAR_LEFT_MC_I2C_ADDR,
                                        (uint8_t*) &I_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }
        break;

      case 4:
        // TODO again, need to find out how to pass int8*
        err = sendAllMotorsData(REG_P_SPEED,
                                (uint8_t*) &P_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }

        err = sendAllMotorsData(REG_I_SPEED,
                                (uint8_t*) &I_Value);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;          
        }
        break;

      default:
        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
        return;
     }

    // If all else goes well, we succeeded
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
    // This function doesn't do anything. Position is not a PID factor we have control over
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
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
    // This function doesn't do anything. Acceleration is not something we have control over
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
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
    if ((Value != 0x00 && Value != 0xFF ) | Motor_ID > 4)
    {
      // Not a valid option
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
      return;
    }

    if (Value == 4)
    {
      for (int i = 0; i < 4; i++)
      {
        if (Value == 0xFF)
          m_stallDetectectionEnabled[i] = true;
        else
          m_stallDetectectionEnabled[i] = false;
      }
    }

    else
    {
      if (Value == 0xFF)
        m_stallDetectectionEnabled[Motor_ID] = true;
      else
        m_stallDetectectionEnabled[Motor_ID] = false;
    }

    // If all else goes well, we succeeded
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
    switch(Motor_ID)
    {
      // Motor 0 (FL)
      case 0:
        m_FL_Encoder_Count_Offset = -m_FL_Encoder_Count;
        break;

      // Motor 1 (FR)
      case 1:
        m_FR_Encoder_Count_Offset = -m_FR_Encoder_Count;
        break;

      // Motor 2 (RR)
      case 2:
        m_RR_Encoder_Count_Offset = -m_RR_Encoder_Count;
        break;

      // Motor 3 (RL)
      case 3:
        m_RL_Encoder_Count_Offset = -m_RL_Encoder_Count;
        break;

      // All motors
      case 4:
        m_FL_Encoder_Count_Offset = -m_FL_Encoder_Count;
        m_FR_Encoder_Count_Offset = -m_FR_Encoder_Count;
        m_RR_Encoder_Count_Offset = -m_RR_Encoder_Count;
        m_RL_Encoder_Count_Offset = -m_RL_Encoder_Count;
        break;

      // Not a valid option
      default:
        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
        return;
    }

     // If all else goes well, we succeeded 
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
    MCError_t err;
    switch(Spin_Type)
    {
      // Forward Spin
      case 0:
        err = moveAllMotorsStraight(MAX_SPIN_DISTANCE, 0);
        if (err != MC_NO_ERROR) 
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;
        }
        break;

      // Backwards Spin
      case 1:
        err = moveAllMotorsStraight(-1*MAX_SPIN_DISTANCE, 0);
        if (err != MC_NO_ERROR) 
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;
        }
        break;

      // Stop
      case 2:
        err = moveAllMotorsStraight(0, 0);
        if (err != MC_NO_ERROR) 
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;
        }
        break;

      // Not a valid option
      default:
        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
        return;

    }

    // If all else goes well, we succeeded 
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
    if (updateTelemetry())
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);

    else
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
  }

  /**
   * @brief      Command handler implementation to force update telemetry
   *
   * @param[in]  opCode          The operation code
   * @param[in]  cmdSeq          The command sequence
   */
  void MotorControlComponentImpl :: MC_DriveTest_cmdHandler(const FwOpcodeType opCode,
                                                            const U32 cmdSeq,
                                                            I64 Distance,
                                                            I8 MoveType)
  {
    MCError_t err;

    switch(MoveType)
    {
      // Forward or backwards
      case 0:
        err = moveAllMotorsStraight(Distance,0);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;
        }
        break;

      // Rotating clockwise, or counter clockwise
      case 1:
        err = rotateAllMotors(Distance,0);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;
        }

      // Stop
      case 2:
        err = moveAllMotorsStraight(0,0);
        if (err != MC_NO_ERROR)
        {
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
          return;
        }
        break;
    }

    // All else succeeds, then I guess we did too.
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Command handler implementation to force update telemetry
   *
   * @param[in]  opCode          The operation code
   * @param[in]  cmdSeq          The command sequence
   */
  void MotorControlComponentImpl :: MC_SelfTest_cmdHandler(const FwOpcodeType opCode,
                                                           const U32 cmdSeq)
  {
    // TODO IMPLEMENT SELF TEST!
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

    /**
     * @brief      Gets the size data.
     *
     * @param[in]  id    The identifier
     *
     * @return     The size data.
     */
    uint32_t MotorControlComponentImpl::getSizeData(const RegisterAddress_t id)
    {
      switch(id)
      {
          case REG_I2C_ADDRESS:
          case DEPRACATE_ENABLE_DRIVER:
          case DEPRACATE_DISABLE_DRIVER:
          case DEPRACATE_RESET_CONTROLLER:
          case REG_FAULT:
          case REG_CLR_FAULT:
          case REG_STATUS:
            return 1;
          case REG_MOTOR_CURRENT:
          case REG_P_CURRENT:
          case REG_I_CURRENT:
          case REG_P_SPEED:
          case REG_I_SPEED:
          case REG_ACC_RATE:
          case REG_DEC_RATE:
          case REG_CURRENT_POSITION:
          case REG_TARGET_SPEED:
            return 2;
          case REG_RELATIVE_TARGET_POSITION:
          case REG_CURRENT_SPEED:
            return 4;
          case NUM_REGS:
          default:
            return 0;
        }   
    }
 
    /**
     * @brief      Sends the same data to every motor register, returning any errors found
     *
     * @param      i2c   I 2 c
     * @param[in]  reg    The slave register to write to
     * @param[in]  data  The data
     *
     * @return     Motor Controller error
     */
    MotorControlComponentImpl::MCError_t
    MotorControlComponentImpl::sendAllMotorsData(const RegisterAddress_t reg, uint8_t* data)
    {
        MCError_t err = MC_NO_ERROR;
        // Send command to all motor controllers
        err = writeMotorControlRegister(reg,
                                        FRONT_LEFT_MC_I2C_ADDR,
                                        data);
        if (err != MC_NO_ERROR)
          return err;          
       
        err = writeMotorControlRegister(reg,
                                        FRONT_RIGHT_MC_I2C_ADDR,
                                        data);
        if (err != MC_NO_ERROR)
          return err;          
       
        err = writeMotorControlRegister(reg,
                                        REAR_LEFT_MC_I2C_ADDR,
                                        data);
        if (err != MC_NO_ERROR)
          return err;          
       
        err = writeMotorControlRegister(reg,
                                        REAR_RIGHT_MC_I2C_ADDR,
                                        data);
        if (err != MC_NO_ERROR)
          return err;          
       
        return MC_NO_ERROR;  
    }
 
    /**
     * @brief      Helper function to move all motors simultaneously
     *
     * @param[in]  Distance       The distance to travel in motor ticks
     * @param[in]  Speed          The speed to travel in normalized speed
     */
    MotorControlComponentImpl::MCError_t
    MotorControlComponentImpl::moveAllMotorsStraight(int32_t distance, int16_t speed)
    {
        MotorTick_t Right_Wheels_Relative_ticks, Left_Wheels_Relative_ticks, Relative_ticks;
        // Error preset
        MCError_t err;
       
        Throttle_t motor_speed;
       
        if (speed > 0)
        {
          motor_speed = groundSpeedToSpeedPrecent(speed);
       
          // Send the speed to all the motors
          // Required to send this before the setpoint (or else the MC will start spinning before speed was set)
          err = sendAllMotorsData(REG_TARGET_SPEED,
                                  (uint8_t*) &motor_speed);
          if (err != MC_NO_ERROR)
            return err;
        }      
       
        // Convert from cm to motor ticks
        Relative_ticks = groundCMToMotorTicks(distance);
       
        // Ensure the sides are traveling the right direction
        if (m_forward_is_positive)
        {
          Right_Wheels_Relative_ticks = Relative_ticks;
          Left_Wheels_Relative_ticks = -1*Relative_ticks;
        }
       
        else
        {
          Right_Wheels_Relative_ticks = -1*Relative_ticks;
          Left_Wheels_Relative_ticks = Relative_ticks;
        }    
       
        // FIXME: XXX: CRITICAL SECTION REQUIRED
        
        err = writeMotorControlRegister(REG_RELATIVE_TARGET_POSITION,
                                        FRONT_LEFT_MC_I2C_ADDR,
                                        (uint8_t*) &Left_Wheels_Relative_ticks);
        if (err != MC_NO_ERROR)
          return err;  
       
        err = writeMotorControlRegister(REG_RELATIVE_TARGET_POSITION,
                                        FRONT_RIGHT_MC_I2C_ADDR,
                                        (uint8_t*) &Right_Wheels_Relative_ticks);
        if (err != MC_NO_ERROR)
          return err;  
       
        err = writeMotorControlRegister(REG_RELATIVE_TARGET_POSITION,
                                        REAR_RIGHT_MC_I2C_ADDR,
                                        (uint8_t*) &Right_Wheels_Relative_ticks);
        if (err != MC_NO_ERROR)
          return err;  
       
        err = writeMotorControlRegister(REG_RELATIVE_TARGET_POSITION,
                                        REAR_LEFT_MC_I2C_ADDR,
                                        (uint8_t*) &Left_Wheels_Relative_ticks);
        
        // FIXME: XXX: CRITICAL SECTION REQUIRED
        
        return err;  
    }
 
    /**
    * @brief      Helper function to rotate all motors simultaneously
    *
    * @param[in]  Distance       ???
    * @param[in]  Speed          ???
    */
    MotorControlComponentImpl::MCError_t
    MotorControlComponentImpl::rotateAllMotors(int16_t distance, int16_t speed)
    {
        // Error preset
        MCError_t err;
       
        if (speed > 0)
        {
          // TODO: Need to correct the container to pass an int8_t
          Throttle_t motor_speed = m_angularToLinear*groundSpeedToSpeedPrecent(speed);
       
        // // FIXME: These need to be updated to use the correct register (TARGET)
          // Send the speed to all the motors
          err = sendAllMotorsData(REG_CURRENT_SPEED,
                                  (uint8_t*) &motor_speed);
          if (err != MC_NO_ERROR)
            return err;  
        }
       
        MotorTick_t Relative_ticks = m_angularToLinear*groundCMToMotorTicks(distance);
       
        // TODO: Need to correct the container to pass an int8_t
        err = writeMotorControlRegister(REG_CURRENT_POSITION, 
                                        FRONT_LEFT_MC_I2C_ADDR,
                                        (uint8_t*) &Relative_ticks);
        if (err != MC_NO_ERROR)
          return err;  
       
        err = writeMotorControlRegister(REG_CURRENT_POSITION, 
                                        FRONT_RIGHT_MC_I2C_ADDR,
                                        (uint8_t*) &Relative_ticks);
        if (err != MC_NO_ERROR)
          return err;  
       
        err = writeMotorControlRegister(REG_CURRENT_POSITION, 
                                        REAR_RIGHT_MC_I2C_ADDR,
                                        (uint8_t*) &Relative_ticks);
        if (err != MC_NO_ERROR)
          return err;  
       
        err = writeMotorControlRegister(REG_CURRENT_POSITION, 
                                        REAR_LEFT_MC_I2C_ADDR,
                                        (uint8_t*) &Relative_ticks);
        return err;
    }
 
    MotorControlComponentImpl::MCError_t
    MotorControlComponentImpl::spinMotors(bool forward)
    {
        MotorTick_t Right_Wheels_Relative_ticks, Left_Wheels_Relative_ticks, Relative_ticks;
        // Error preset
        MCError_t err;
       
        // Convert from cm to motor ticks
        Relative_ticks = MAX_SPIN_DISTANCE;
       
        // Ensure the sides are traveling the right direction
        if (m_forward_is_positive)
        {
          Right_Wheels_Relative_ticks = Relative_ticks;
          Left_Wheels_Relative_ticks = -1*Relative_ticks;
        }
       
        else
        {
          Right_Wheels_Relative_ticks = -1*Relative_ticks;
          Left_Wheels_Relative_ticks = Relative_ticks;
        } 
       
        err = writeMotorControlRegister(REG_CURRENT_POSITION, 
                                        FRONT_LEFT_MC_I2C_ADDR,
                                        (uint8_t*) &Left_Wheels_Relative_ticks);
        if (err != MC_NO_ERROR)
          return err;  
       
        err = writeMotorControlRegister(REG_CURRENT_POSITION, 
                                        FRONT_RIGHT_MC_I2C_ADDR,
                                        (uint8_t*) &Right_Wheels_Relative_ticks);
        if (err != MC_NO_ERROR)
          return err;  
       
        err = writeMotorControlRegister(REG_CURRENT_POSITION, 
                                        REAR_RIGHT_MC_I2C_ADDR,
                                        (uint8_t*) &Right_Wheels_Relative_ticks);
        if (err != MC_NO_ERROR)
          return err;  
       
        err = writeMotorControlRegister(REG_CURRENT_POSITION, 
                                        REAR_LEFT_MC_I2C_ADDR,
                                        (uint8_t*) &Left_Wheels_Relative_ticks);
        return err;
    }
 
    /**
     * @brief      Writes a register.
     *
     * @param      i2c   I 2 c
     * @param[in]  addr  Register address
     * @param[in]  addr   Device address
     * @param[in]  data  The data
     *
     * @return     Motor Controller error
     */
    MotorControlComponentImpl::MCError_t
    MotorControlComponentImpl::writeMotorControlRegister(const RegisterAddress_t reg,
                                                         const I2cSlaveAddress_t addr,
                                                         uint8_t * data)
    {
        // TODO: Cleanup ret, what are we going to return from I2C functions
        MCError_t ret = MC_NO_ERROR;
        uint32_t dataLength = getSizeData(reg);
        uint8_t reg_buffer[1];
        reg_buffer[0] = (uint8_t)reg;
 
        if (dataLength <= 0) 
          return MC_UNEXPECTED_ERROR;
 
        // Inform the MSP of the desired register
        i2cMasterTransmit(m_i2c, addr, 1, reg_buffer);
        // TODO: If error on above (currently returns void) return
 
        // If we want something, receive
        if (expectingReturnMessage(reg))
          i2cMasterReceive(m_i2c, addr, dataLength, data);
 
        // Else, tell the data you have
        else
          i2cMasterTransmit(m_i2c, addr, dataLength, data);
 
        return ret;
    }
 
    /**
    * @brief      Enables all motors
    *
    * @return     Motor controller error
    */
    MotorControlComponentImpl::MCError_t MotorControlComponentImpl::enableDrivers()
    {
        WatchdogCommandOut_out(0, CubeRoverPorts::motorsOn);
        return MC_NO_ERROR;
    }
 
    /**
    * @brief      Disable all motors
    *
    * @return     Motor controller error
    */
    MotorControlComponentImpl::MCError_t MotorControlComponentImpl::disableDrivers()
    {
        WatchdogCommandOut_out(0, CubeRoverPorts::motorsOff);
        return MC_NO_ERROR;
    }
 
    /**
    * @brief      Resets all motors
    *
    * @return     None
    */
    void MotorControlComponentImpl::resetMotorControllers()
    {
      WatchdogCommandOut_out(0, CubeRoverPorts::motorsReset);
    }
 
    /**
    * @brief      Converts cm to motor ticks
    *
    * @param[in]  Distance the system wants to travel in cm    
    *
    * @return     Motor ticks to rotate
    */
    MotorControlComponentImpl::MotorTick_t
    MotorControlComponentImpl::groundCMToMotorTicks(int16_t dist)
    {
        // FIXME: Jonathan to develop this function
        return 0;
    }
 
    /**
    * @brief      Converts from ground speed to motor normalized speed
    *
    * @param[in]  The speed in ground version of cm/s (scaled from 0x00 - 0x0A)
    *              meaning 0-10cm/s
    *
    * @return     Precentage of the speed in motor control terms
    */
    MotorControlComponentImpl::Throttle_t
    MotorControlComponentImpl::groundSpeedToSpeedPrecent(int16_t speed)
    {
        // FIXME: Jonathan to develop this function
        return 0;
    }
 
    /**
    * @brief      Delays for 1050 ticks slow enough for slave sides
    *
    */
    bool MotorControlComponentImpl :: updateTelemetry()
    {
        bool Update_Success = true;
       
        // Update only a single type of data at a time
        if (m_Round_Robin_Telemetry)
        {
          switch(++m_Robin_Number)
          {
            case 0:
              Update_Success = updateSpeed();
              break;
       
            case 1:
              Update_Success = updateCurrent();
              break;
       
            case 2:
              Update_Success = updateEncoder();
              m_Robin_Number = 0;
              break;
       
            default:
              m_Robin_Number = 0;
              break;
          }
        }
       
        // Update all of them
        else
        {
          if (!updateSpeed())
            return false;
          if (!updateCurrent()) 
            return false;
          if (updateEncoder())
            return false;
        }
       
        return Update_Success;
    }
 
    /**
    * @brief      Delays for 1050 ticks slow enough for slave sides
    *
    */
    bool MotorControlComponentImpl::updateSpeed()
    {
        // FIXME: REG_CURRENT_SPEED reads (#ticks / #seconds) -> Fixed-point 17MSB . 15LSB 
        // BUT JONATHAN WILL UPDATE 
        
        MCError_t err = MC_NO_ERROR;
        uint8_t Speed_buffer[MC_BUFFER_MAX_SIZE];
       
        // Get FL Current Speed
        err = writeMotorControlRegister(REG_CURRENT_SPEED,
                                        FRONT_LEFT_MC_I2C_ADDR,
                                        &Speed_buffer[0]);
       
        // Make sure everything is going well
        if (err != MC_NO_ERROR)
        {
          resetMotorControllers();
          log_WARNING_HI_MC_MSPNotResponding();
          return false;
        }
       
        // Get FR Current Speed
        err = writeMotorControlRegister(REG_CURRENT_SPEED,
                                        FRONT_RIGHT_MC_I2C_ADDR,
                                        &Speed_buffer[4]);
       
        // Make sure everything is going well
        if (err != MC_NO_ERROR)
        {
          resetMotorControllers();
          log_WARNING_HI_MC_MSPNotResponding();
          return false;
        }
       
        // Get RR Current Speed
        err = writeMotorControlRegister(REG_CURRENT_SPEED,
                                        REAR_RIGHT_MC_I2C_ADDR,
                                        &Speed_buffer[8]);
       
        // Make sure everything is going well
        if (err != MC_NO_ERROR)
        {
          resetMotorControllers();
          log_WARNING_HI_MC_MSPNotResponding();
          return false;
        }
       
        // Get RL Current Speed
        err = writeMotorControlRegister(REG_CURRENT_SPEED,
                                        REAR_LEFT_MC_I2C_ADDR,
                                        &Speed_buffer[12]);
       
        // Make sure everything is going well
        if (err != MC_NO_ERROR)
        {
          resetMotorControllers();
          log_WARNING_HI_MC_MSPNotResponding();
          return false;
        }
       
        // If we got all the values we need, then we can update telemetry
        tlmWrite_MC_FL_Speed((uint32_t) Speed_buffer[0]);
        tlmWrite_MC_FR_Speed((uint32_t) Speed_buffer[4]);
        tlmWrite_MC_RR_Speed((uint32_t) Speed_buffer[8]);
        tlmWrite_MC_RL_Speed((uint32_t) Speed_buffer[12]);
        return true;
    }
 
    /**
    * @brief      Delays for 1050 ticks slow enough for slave sides
    *
    */
    bool MotorControlComponentImpl::updateCurrent()
    {
        MCError_t err = MC_NO_ERROR;
        uint8_t Current_buffer[MC_BUFFER_MAX_SIZE];
       
        // Get FL Current 
        err = writeMotorControlRegister(REG_MOTOR_CURRENT,
                                        FRONT_LEFT_MC_I2C_ADDR,
                                        &Current_buffer[0]);
       
        // Make sure everything is going well
        if (err != MC_NO_ERROR)
        {
          resetMotorControllers();
          log_WARNING_HI_MC_MSPNotResponding();
          return false;
        }
       
        // Get FR Current 
        err = writeMotorControlRegister(REG_MOTOR_CURRENT,
                                        FRONT_RIGHT_MC_I2C_ADDR,
                                        &Current_buffer[4]);
       
        // Make sure everything is going well
        if (err != MC_NO_ERROR)
        {
          resetMotorControllers();
          log_WARNING_HI_MC_MSPNotResponding();
          return false;
        }
       
        // Get RR Current 
        err = writeMotorControlRegister(REG_MOTOR_CURRENT,
                                        REAR_RIGHT_MC_I2C_ADDR,
                                        &Current_buffer[8]);
       
        // Make sure everything is going well
        if (err != MC_NO_ERROR)
        {
          resetMotorControllers();
          log_WARNING_HI_MC_MSPNotResponding();
          return false;
        }
       
        // Get RL Current
        err = writeMotorControlRegister(REG_MOTOR_CURRENT,
                                        REAR_LEFT_MC_I2C_ADDR,
                                        &Current_buffer[12]);
       
        // Make sure everything is going well
        if (err != MC_NO_ERROR)
        {
          resetMotorControllers();
          log_WARNING_HI_MC_MSPNotResponding();
          return false;
        }
       
        // If we got all the values we need, then we can update telemetry
        tlmWrite_MC_FL_Current((uint32_t) Current_buffer[0]);
        tlmWrite_MC_FR_Current((uint32_t) Current_buffer[4]);
        tlmWrite_MC_RR_Current((uint32_t) Current_buffer[8]);
        tlmWrite_MC_RL_Current((uint32_t) Current_buffer[12]);
        return true;
    }
 
    /**
    * @brief      Delays for 1050 ticks slow enough for slave sides
    *
    */
    bool MotorControlComponentImpl::updateEncoder()
    {
        MCError_t err = MC_NO_ERROR;
        uint8_t Encoder_buffer[MC_BUFFER_MAX_SIZE];
       
        // Get FL Current 
        err = writeMotorControlRegister(REG_MOTOR_CURRENT,
                                        FRONT_LEFT_MC_I2C_ADDR,
                                        &Encoder_buffer[0]);
       
        // Make sure everything is going well
        if (err != MC_NO_ERROR)
        {
          resetMotorControllers();
          log_WARNING_HI_MC_MSPNotResponding();
          return false;
        }
       
        // Get FR Current 
        err = writeMotorControlRegister(REG_MOTOR_CURRENT,
                                        FRONT_RIGHT_MC_I2C_ADDR,
                                        &Encoder_buffer[4]);
       
        // Make sure everything is going well
        if (err != MC_NO_ERROR)
        {
          resetMotorControllers();
          log_WARNING_HI_MC_MSPNotResponding();
          return false;
        }
       
        // Get RR Current 
        err = writeMotorControlRegister(REG_MOTOR_CURRENT,
                                        REAR_RIGHT_MC_I2C_ADDR,
                                        &Encoder_buffer[8]);
       
        // Make sure everything is going well
        if (err != MC_NO_ERROR)
        {
          resetMotorControllers();
          log_WARNING_HI_MC_MSPNotResponding();
          return false;
        }
       
        // Get RL Current Encoder value
        err = writeMotorControlRegister(REG_MOTOR_CURRENT,
                                        REAR_LEFT_MC_I2C_ADDR,
                                        &Encoder_buffer[12]);
       
        // Make sure everything is going well
        if (err != MC_NO_ERROR)
        {
          resetMotorControllers();
          log_WARNING_HI_MC_MSPNotResponding();
          return false;
        }
       
        // If we got all the values we need, then we can update telemetry
        m_FL_Encoder_Count += (uint16_t) Encoder_buffer[0];
        m_FR_Encoder_Count += (uint16_t) Encoder_buffer[4];
        m_RR_Encoder_Count += (uint16_t) Encoder_buffer[8];
        m_RL_Encoder_Count += (uint16_t) Encoder_buffer[12];
        tlmWrite_MC_FL_Encoder_Dist(m_FL_Encoder_Count + m_FR_Encoder_Count_Offset);
        tlmWrite_MC_FR_Encoder_Dist(m_FR_Encoder_Count + m_FL_Encoder_Count_Offset);
        tlmWrite_MC_RR_Encoder_Dist(m_RR_Encoder_Count + m_RL_Encoder_Count_Offset);
        tlmWrite_MC_RL_Encoder_Dist(m_RL_Encoder_Count + m_RR_Encoder_Count_Offset);
        return true;
    }
 
    /**
    * @brief      Converts from ground speed to motor normalized speed
    *
    * @param[in]  The speed in ground version of cm/s (scaled from 0x00 - 0x0A)
    *              meaning 0-10cm/s
    *
    * @return     Precentage of the speed in motor control terms
    */
    bool MotorControlComponentImpl::expectingReturnMessage(const RegisterAddress_t id)
    {
        switch(id)
        {
          case REG_I2C_ADDRESS:
          case REG_CURRENT_POSITION:
          case REG_CURRENT_SPEED:
          case REG_MOTOR_CURRENT:
            return true;
       
          default:
            return false;
        } 
    }

} // end namespace CubeRover
