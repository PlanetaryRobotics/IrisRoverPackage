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
    
   const uint8_t MotorControlComponentImpl::motorIdAddressMap[NUM_MOTORS] = {
       FRONT_LEFT_MC_I2C_ADDR, FRONT_RIGHT_MC_I2C_ADDR,
       REAR_LEFT_MC_I2C_ADDR, REAR_RIGHT_MC_I2C_ADDR
   };

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
    m_i2c = MOTOR_CONTROL_I2CREG;
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
    m_openloop_mode = false;
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
   * @param[in]  command_type   Movement or telemetry
   * @param[in]  movement_type  Type of movement
   * @param[in]  Distance       Distance
   * @param[in]  Speed          Speed
   */
  void MotorControlComponentImpl :: motorCommandIn_handler(const NATIVE_INT_TYPE portNum,
                                                           CubeRoverPorts::MC_CommandType command_type,
                                                           CubeRoverPorts::MC_MovementType movement_type,
                                                           U8 Distance,
                                                           U8 Speed)
  {
    MCError_t err;
    if (command_type == CubeRoverPorts::MC_DrivingConfiguration) {
        switch (movement_type) {
          case CubeRoverPorts::MC_Forward:
              err = moveAllMotorsStraight(Distance, Speed);
              log_COMMAND_MC_moveStarted();
              break;
          case CubeRoverPorts::MC_Backward:
              err = moveAllMotorsStraight(-Distance, Speed);
              log_COMMAND_MC_moveStarted();
              break;
          case CubeRoverPorts::MC_Left:
              err = rotateAllMotors(Distance, Speed);
              log_COMMAND_MC_moveStarted();
              break;
          case CubeRoverPorts::MC_Right:
              err = rotateAllMotors(-Distance, Speed);
              log_COMMAND_MC_moveStarted();
              break;
          case CubeRoverPorts::MC_Stop:
              err = moveAllMotorsStraight(0, 0);
              break;
          default:
              return;
        }
        if (err != MC_NO_ERROR) {
            // TODO: Should stop right?
            log_WARNING_HI_MC_MSPNotResponding();
        } else {
            // Only Poll for status if the movement was successful
            // TODO: PollStatus or checkMotorStatus in a Loop?? The latter is preferred
            pollStatus();
        }
    }
    else if (command_type == CubeRoverPorts::MC_UpdateTelemetry) {
        updateTelemetry();
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
    uint16_t P_Value = (uint16_t)((PI_Values & (uint32_t)0x00ff) >> 0 );
    uint16_t I_Value = (uint16_t)((PI_Values & (uint32_t)0xff00) >> 16);

    if (Motor_ID == ALL_MOTOR_ID) {
        err = sendAllMotorsData(REG_P_CURRENT, &P_Value);
        if (err != MC_NO_ERROR) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }

        err = sendAllMotorsData(REG_I_CURRENT, &I_Value);
        if (err != MC_NO_ERROR) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }
    }
    else {
        err = motorControlTransfer(motorIdAddressMap[Motor_ID], REG_P_CURRENT, &P_Value);
        if (err != MC_NO_ERROR) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;          
        }

        err = motorControlTransfer(motorIdAddressMap[Motor_ID], REG_I_CURRENT, &I_Value);
        if (err != MC_NO_ERROR) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;          
        }
    }
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
                                                            U32 PID_Values)
  {
    MCError_t err;
    uint16_t P_Value = (uint16_t)((PID_Values & (uint32_t)0x00ff) >> 0 );
    uint16_t I_Value = (uint16_t)((PID_Values & (uint32_t)0xff00) >> 16);
    
    if (Motor_ID == ALL_MOTOR_ID) {
        err = sendAllMotorsData(REG_P_SPEED, &P_Value);
        if (err != MC_NO_ERROR) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }

        err = sendAllMotorsData(REG_I_SPEED, &I_Value);
        if (err != MC_NO_ERROR) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }
    }
    else {
        err = motorControlTransfer(motorIdAddressMap[Motor_ID], REG_P_SPEED, &P_Value);
        if (err != MC_NO_ERROR) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;          
        }

        err = motorControlTransfer(motorIdAddressMap[Motor_ID], REG_I_SPEED, &I_Value);
        if (err != MC_NO_ERROR) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;          
        }
    }
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
   * @param[in]  Rate_Values     Acceleration and Decelleration rates
   */
  void MotorControlComponentImpl :: MC_Acceleration_cmdHandler(const FwOpcodeType opCode,
                                                               const U32 cmdSeq,
                                                               U8 Motor_ID,
                                                               U32 Rate_Values)
  {
    MCError_t err;
    uint16_t accel = (uint16_t)((Rate_Values & (uint32_t)0x00ff) >> 0 );
    uint16_t decel = (uint16_t)((Rate_Values & (uint32_t)0xff00) >> 16);
    
    if (Motor_ID == ALL_MOTOR_ID) {
        err = sendAllMotorsData(REG_ACC_RATE, &accel);
        if (err != MC_NO_ERROR) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }

        err = sendAllMotorsData(REG_DEC_RATE, &decel);
        if (err != MC_NO_ERROR) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }
    }
    else {
        err = motorControlTransfer(motorIdAddressMap[Motor_ID], REG_ACC_RATE, &accel);
        if (err != MC_NO_ERROR) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;          
        }

        err = motorControlTransfer(motorIdAddressMap[Motor_ID], REG_DEC_RATE, &decel);
        if (err != MC_NO_ERROR) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;          
        }
    }
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
    // TODO: KEEP OR DEPRACATE? What does this do
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
    // TODO: DEPRACATE THIS? What does this actually do?
    switch (Motor_ID)
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
   * @brief      Command handler implementation to manually spin the motors at full speed
   * 
   * @note       This skips any conversion from ground units to Motor Control units. The received
   *             value is directly forwarded to the requested motor controller.
   *
   * @param[in]  opCode          The operation code
   * @param[in]  cmdSeq          The command sequence
   * @param[in]  Motor_ID        The motor(s) ID
   * @param[in]  Raw_Ticks       Raw ticks to send to the motor controllers
   */
  void MotorControlComponentImpl :: MC_Spin_cmdHandler(const FwOpcodeType opCode,
                                                       const U32 cmdSeq,
                                                       U8 Motor_ID,
                                                       U32 Raw_Ticks)
  {
    // TODO: Should this force open loop control as well?
    MCError_t err;
    uint8_t speed = MAX_SPEED;
    if (Motor_ID == ALL_MOTOR_ID) {
        err = sendAllMotorsData(REG_TARGET_SPEED, &speed);
        if (err != MC_NO_ERROR) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }

        err = sendAllMotorsData(REG_RELATIVE_TARGET_POSITION, &Raw_Ticks);
        if (err != MC_NO_ERROR) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }
    }
    else {
        err = motorControlTransfer(motorIdAddressMap[Motor_ID], REG_TARGET_SPEED, &speed);
        if (err != MC_NO_ERROR) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;          
        }

        err = motorControlTransfer(motorIdAddressMap[Motor_ID], REG_RELATIVE_TARGET_POSITION, &Raw_Ticks);
        if (err != MC_NO_ERROR) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;          
        }
    }
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
    // m_forward_is_positive
    // consts to convert ground units to motor controller units
    // angular to linear conversion
    // status registers
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
  

uint32_t MotorControlComponentImpl::regSizeMap(RegisterAddress_t reg) {
    switch (reg) {
        case REG_I2C_ADDRESS:
        case REG_TARGET_SPEED:
        case REG_CTRL:
        case REG_FAULT:
        case e_REG_STATUS:
            return 1;
        case REG_P_CURRENT:
        case REG_I_CURRENT:
        case REG_P_SPEED:
        case REG_I_SPEED:
        case REG_ACC_RATE:
        case REG_DEC_RATE:
            return 2;
        case REG_RELATIVE_TARGET_POSITION:
        case REG_CURRENT_POSITION:
        case REG_MOTOR_CURRENT:  // TODO: CHeck if this retuns 2 byts or 4!? 2 from thismethod 4 from micheal's update current tlm code
            return 4;
        default:
            return 0;
    }   
}
 
MotorControlComponentImpl::MCError_t
MotorControlComponentImpl::sendAllMotorsData(const RegisterAddress_t reg, void *_data) {
    uint8_t *data = static_cast<uint8_t *>(_data);
    
    for (int i = 0; i < NUM_MOTORS; ++i) {
        MCError_t err = motorControlTransfer(motorIdAddressMap[i], reg, data);
        if (err != MC_NO_ERROR)
            return err;
    }
    // TODO: What if one latched up? Should we check status here and issue STOP?

    return MC_NO_ERROR;  
}

bool MotorControlComponentImpl::checkMotorsStatus() {
    MCError_t err;
    for (int i = 0; i < NUM_MOTORS; ++i) {
        err = motorControlTransfer(motorIdAddressMap[i], e_REG_STATUS, &m_currStatus[i].value);
        if (err != MC_NO_ERROR) {
            // I2C Communication Error
            watchdogResetRequest_out(0, CubeRoverPorts::motorsReset);
            // TODO: Reset our I2C too
            return false;
        } else if (m_currStatus[i].bits.controller_error) {
            // TODO: Send STOP general call
            // TODO: Need to check mappping between resetting one motor and which one is connected to watchdog
            // TODO: Check status again after reset
            return false;
            watchdogResetRequest_out(0, CubeRoverPorts::motorsReset);
            // XXX: Do we need to update our tlm counter?
        } else if (!m_currStatus[i].bits.position_converged) {
            // TODO: Do we... wait?
            return false;
        }
    }
    return true;
}

/**
 * @brief      Helper function to move all motors simultaneously
 *
 * @param[in]  Distance       The distance to travel in motor ticks
 * @param[in]  Speed          The speed to travel in normalized speed
 */
MotorControlComponentImpl::MCError_t
MotorControlComponentImpl::moveAllMotorsStraight(int32_t distance, int16_t speed) {
    MCError_t err;

    checkMotorsStatus();
   
    Throttle_t motor_speed;
    // Enforce speed always positive. Direction set by distance
    if (speed >= 0) {
        motor_speed = groundSpeedToSpeedPrecent(speed);
       
        // Send the speed to all the motors
        // Required to send this before the setpoint (or else the MC will start spinning before speed was set)
        err = sendAllMotorsData(REG_TARGET_SPEED, &motor_speed);
        if (err != MC_NO_ERROR)
            return err;
    } else {
        return MC_UNEXPECTED_ERROR;     // Initialize???
    }
   
    MotorTick_t Right_Wheels_Relative_ticks, Left_Wheels_Relative_ticks, Relative_ticks;
    Relative_ticks = groundCMToMotorTicks(distance);
    // Ensure the sides are traveling the right direction
    if (m_forward_is_positive) {
        Right_Wheels_Relative_ticks = Relative_ticks;
        Left_Wheels_Relative_ticks = -1*Relative_ticks;
    } else {
        Right_Wheels_Relative_ticks = -1*Relative_ticks;
        Left_Wheels_Relative_ticks = Relative_ticks;
    }    
    
    err = motorControlTransfer(FRONT_LEFT_MC_I2C_ADDR, REG_RELATIVE_TARGET_POSITION, &Left_Wheels_Relative_ticks);
    if (err != MC_NO_ERROR)
        return err;  
   
    err = motorControlTransfer(FRONT_RIGHT_MC_I2C_ADDR, REG_RELATIVE_TARGET_POSITION, &Right_Wheels_Relative_ticks);
    if (err != MC_NO_ERROR)
        return err;  
   
    err = motorControlTransfer(REAR_RIGHT_MC_I2C_ADDR, REG_RELATIVE_TARGET_POSITION, &Right_Wheels_Relative_ticks);
    if (err != MC_NO_ERROR)
        return err;  
   
    err = motorControlTransfer(REAR_LEFT_MC_I2C_ADDR, REG_RELATIVE_TARGET_POSITION, &Left_Wheels_Relative_ticks);
    if (err != MC_NO_ERROR)
        return err;

    // FIXME: XXX: CRITICAL SECTION REQUIRED
    uint8_t execute_cmd = (1 << 5);
    if (m_openloop_mode)
        execute_cmd |= (1 << 0);
    err = sendAllMotorsData(REG_CTRL, &execute_cmd);

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
MotorControlComponentImpl::rotateAllMotors(int16_t distance, int16_t speed) {
    MCError_t err;

    checkMotorsStatus();
   
    // Enforce speed always positive. Direction set by distance
    if (speed > 0) {
        Throttle_t motor_speed = groundSpeedToSpeedPrecent(speed);
       
        // Send the speed to all the motors
        // Required to send this before the setpoint (or else the MC will start spinning before speed was set)
        err = sendAllMotorsData(REG_TARGET_SPEED, &motor_speed);
        if (err != MC_NO_ERROR)
            return err;  
    } else {
        return err;
    }
   
    MotorTick_t Relative_ticks = m_angularToLinear*groundCMToMotorTicks(distance);
    
    StatusRegister_t status;
    err = motorControlTransfer(FRONT_LEFT_MC_I2C_ADDR, e_REG_STATUS, &status.value);
    err = motorControlTransfer(FRONT_RIGHT_MC_I2C_ADDR, e_REG_STATUS, &status.value);
    err = motorControlTransfer(REAR_LEFT_MC_I2C_ADDR, e_REG_STATUS, &status.value);
    err = motorControlTransfer(REAR_RIGHT_MC_I2C_ADDR, e_REG_STATUS, &status.value);
   
    err = motorControlTransfer(FRONT_LEFT_MC_I2C_ADDR, REG_RELATIVE_TARGET_POSITION, &Relative_ticks);
    if (err != MC_NO_ERROR)
        return err;  
   
    err = motorControlTransfer(FRONT_RIGHT_MC_I2C_ADDR, REG_RELATIVE_TARGET_POSITION, &Relative_ticks);
    if (err != MC_NO_ERROR)
        return err;  
   
    err = motorControlTransfer(REAR_RIGHT_MC_I2C_ADDR, REG_RELATIVE_TARGET_POSITION, &Relative_ticks);
    if (err != MC_NO_ERROR)
        return err;  
   
    err = motorControlTransfer(REAR_LEFT_MC_I2C_ADDR, REG_RELATIVE_TARGET_POSITION, &Relative_ticks);
    if (err != MC_NO_ERROR)
        return err;

    // FIXME: XXX: CRITICAL SECTION REQUIRED
    uint8_t execute_cmd = (1 << 5);
    if (m_openloop_mode)
        execute_cmd |= (1 << 0);
    err = sendAllMotorsData(REG_CTRL, &execute_cmd);
    // FIXME: XXX: CRITICAL SECTION REQUIRED
    
    return err;
}

MotorControlComponentImpl::MCError_t
MotorControlComponentImpl::motorControlTransfer(I2cSlaveAddress_t addr,
                                                     RegisterAddress_t reg,
                                                     void *_data)
{
    uint8_t *data = static_cast<uint8_t *>(_data);
    uint32_t dataLength = regSizeMap(reg);

    if (dataLength <= 0) 
      return MC_UNEXPECTED_ERROR;

    uint8_t reg_buffer = static_cast<uint8_t>(reg);
    if (!i2cMasterTransmit(m_i2c, addr, 1, &reg_buffer)) {
        // TODO: Check response below ERROR OCCURRED
        return MC_I2C_TIMEOUT_ERROR;
    }

    if (reg == REG_I2C_ADDRESS      ||
        reg == REG_CURRENT_POSITION ||
        reg == REG_CURRENT_POSITION ||
        reg == REG_MOTOR_CURRENT    ||
        reg == e_REG_STATUS         ||
        reg == REG_FAULT) {
            if (i2cMasterReceive(m_i2c, addr, dataLength, data))
                return MC_NO_ERROR;
            else
                return MC_I2C_TIMEOUT_ERROR;
    } else {
            if (i2cMasterTransmit(m_i2c, addr, dataLength, data))
                return MC_NO_ERROR;
            else
                return MC_I2C_TIMEOUT_ERROR;
    }
}

// Convert ground units to motor control native units
MotorControlComponentImpl::MotorTick_t
MotorControlComponentImpl::groundCMToMotorTicks(int16_t dist) {
    // TODO: Make this constant editable
    return (int)(158.343f * (float(dist)));
}

// Convert ground units to motor control native units
MotorControlComponentImpl::Throttle_t
MotorControlComponentImpl::groundSpeedToSpeedPrecent(int16_t speed) {
    // FIXME: Jonathan to develop this function
    // TODO: If no constant multiple by operator editable 1
    return speed;
}

bool MotorControlComponentImpl::updateTelemetry() {
    MCError_t err = MC_NO_ERROR;
    uint32_t buffer[NUM_MOTORS];
   
    for (int i = 0; i < NUM_MOTORS; ++i) { 
        err = motorControlTransfer(motorIdAddressMap[i], REG_MOTOR_CURRENT, &buffer[i]);
        if (err != MC_NO_ERROR) {
            // resetMotorControllers();
            // TODO: THIS LOG REALLY SHOULD INDICATE WHICH FAILED??
            log_WARNING_HI_MC_MSPNotResponding();
        }
    }
    
    tlmWrite_MC_FL_Current((uint32_t) buffer[0]);
    tlmWrite_MC_FR_Current((uint32_t) buffer[1]);
    tlmWrite_MC_RR_Current((uint32_t) buffer[2]);
    tlmWrite_MC_RL_Current((uint32_t) buffer[3]);
    
    if (err != MC_NO_ERROR)
        asm (" nop");
        // resetMotorControllers();
   
    for (int i = 0; i < NUM_MOTORS; ++i) { 
        err = motorControlTransfer(motorIdAddressMap[i], REG_CURRENT_POSITION, &buffer[i]);
        if (err != MC_NO_ERROR) {
            // resetMotorControllers();
            // TODO: THIS LOG REALLY SHOULD INDICATE WHICH FAILED??
            log_WARNING_HI_MC_MSPNotResponding();
        }
    }
   
    // If we got all the values we need, then we can update telemetry
    m_FL_Encoder_Count += (uint16_t) buffer[0];
    m_FR_Encoder_Count += (uint16_t) buffer[1];
    m_RR_Encoder_Count += (uint16_t) buffer[2];
    m_RL_Encoder_Count += (uint16_t) buffer[3];
    tlmWrite_MC_FL_Encoder_Ticks(m_FL_Encoder_Count + m_FR_Encoder_Count_Offset);
    tlmWrite_MC_FR_Encoder_Ticks(m_FR_Encoder_Count + m_FL_Encoder_Count_Offset);
    tlmWrite_MC_RR_Encoder_Ticks(m_RR_Encoder_Count + m_RL_Encoder_Count_Offset);
    tlmWrite_MC_RL_Encoder_Ticks(m_RL_Encoder_Count + m_RR_Encoder_Count_Offset);
    
    if (err != MC_NO_ERROR)
        asm (" nop");
        // resetMotorControllers();
    
    return true;
}

bool MotorControlComponentImpl::pollStatus() {
    StatusRegister_t status[NUM_MOTORS];
    do {
        unsigned delay = 500000;
        while (delay)    // Delay 0.5s to give the motors a chance to converge. 0.5 / (1/ 110e6)
            delay--;

        uint8_t reg = e_REG_STATUS;
        for (int i = 0; i < 4; ++i) {
            for (int j = 10000; j; j--);
            i2cMasterTransmit(m_i2c, FRONT_LEFT_MC_I2C_ADDR+i, 1, &reg);
            for (int j = 10000; j; j--);
            i2cMasterReceive(m_i2c, FRONT_LEFT_MC_I2C_ADDR+i, 1, &(status[i].value));
        }
    } while (!(status[0].bits.position_converged &&
               status[1].bits.position_converged &&
               status[2].bits.position_converged &&
               status[3].bits.position_converged));        // FIXME: Potential infinite loop (Should reset after 2.5 min)

    return true;
}


} // end namespace CubeRover
