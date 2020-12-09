// ======================================================================
// \title  MotorControlComponentImpl.hpp
// \author cedric
// \editted by Michael
// \brief  hpp file for MotorControl component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef MotorControl_HPP
#define MotorControl_HPP

#include "CubeRover/MotorControl/MotorControlComponentAc.hpp"
#include "i2c.h"

namespace CubeRover {

  #define MC_BUFFER_MAX_SIZE      16 // Maximum size of I2C buffer
  #define PI                      3.14159265

  namespace MotorControllerI2C
  {
    typedef uint8_t MotorControlI2cRegId_t;

    typedef enum 
    {
      I2C_ADDRESS = 0,
      RELATIVE_TARGET_POSITION = 1,
      TARGET_SPEED = 2,
      CURRENT_POSITION = 3,
      CURRENT_SPEED = 4,
      MOTOR_CURRENT = 5,
      P_CURRENT = 6,
      I_CURRENT = 7,
      P_SPEED = 8,
      I_SPEED = 9,
      ACC_RATE = 10,
      DEC_RATE = 11,
      CONTROL_REGISTER = 12,
      STATUS_REGISTER = 13,
      FAULT_REGISTER = 14,
      EXECUTE_CMD = 15,
      ENABLE_DRIVER = 16,
      DISABLE_DRIVER = 17,
      RESET_CONTROLLER = 18,
      CLEAR_FAULT = 19,
      MAX_NB_CMDS = 20,
      UNSET = 99

      // Previous implement
      /*
      I2C_ADDRESS = 0x00, // This was 1
      I2C_RELATIVE_TARGET_POSITION,
      I2C_DIRECTION, // DELETE
      I2C_TARGET_SPEED,
      I2C_CURRENT_POSITION,
      // I2C_Current_Speed
      I2C_MOTOR_CURRENT,
      I2C_P_CURRENT,
      I2C_I_CURRENT,
      I2C_P_VELOCITY,
      I2C_I_VELOCITY,
      I2C_D_VELOCITY, // DELTETE
      I2C_P_POSITION, // DELETE ALL
      I2C_I_POSITION,
      I2C_D_POSITION, // TO HERE
      I2C_ACCELERATION,
      I2C_DECELERATION,
      I2C_EXECUTE_CMD,
      I2C_CURRENT_VELOCITY,
      I2C_ENABLE_DRIVER,
      I2C_DISABLE_DRIVER,
      I2C_RESET_CONTROLLER,
      I2C_FAULT_REGISTER,
      I2C_CLEAR_FAULT,
      I2C_STATUS_REGISTER,
      I2C_POSITION_SENSOR_CURRENT_COMBINATION,
      I2C_MC_MAX_NUM_OF_ELEMENTS*/
    }I2cRegisterId;

    typedef uint8_t I2cSlaveAddress;
  } // end of MotorControllerI2C namespace

  typedef enum 
  {
      MC_NO_ERROR,
      MC_UNEXPECTED_ERROR
  }MCError;

  typedef enum 
  {
    CRC8              = 0xD5,
    CRC8_CCITT        = 0x07,
    CRC8_DALLAS_MAXIM = 0x31,
    CRC8_SAE_J1850    = 0x1D,
    CRC_8_WCDMA       = 0x9B
  }ChecksumType;

  typedef uint8_t MotorControlChecksum;
  typedef int32_t Distance_cm;
  typedef int32_t Motor_tick;
  typedef uint8_t Speed_percent;

  class MotorControlComponentImpl :
    public MotorControlComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object MotorControl
      //!
      MotorControlComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object MotorControl
      //!
      void init(const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/);

      //! Destroy object MotorControl
      //!
      ~MotorControlComponentImpl(void);

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for PingIn
      //!
      void PingIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                          U32 key /*!< Value to return to pinger*/);

      //! Handler implementation for schedIn
      //!
      void schedIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                           NATIVE_UINT_TYPE context /*!< The call order*/);

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for MC_MotorConfiguration command handler
      //! Configure motors command
      void MC_MotorConfiguration_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                            const U32 cmdSeq, /*!< The command sequence number*/
                                            U8 Motor_ID, /*!< The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors */
                                            MP_CommandList MotorParameter, 
                                            U32 Value);

      //! Implementation for MC_DrivingConfiguration command handler
      //! Configure movement command
      void MC_DrivingConfiguration_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                              const U32 cmdSeq, /*!< The command sequence number*/
                                              CC_CommandList CommandConfiguration, 
                                              ParameterList Parameter, 
                                              U8 Value);

      //! Implementation for MC_ExecuteDriving command handler
      //! Execute one configured command
      void MC_ExecuteDriving_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                        const U32 cmdSeq, /*!< The command sequence number*/
                                        DriveCommandList DrivingCommand);

      //! Implementation for MC_StallDetection command handler
      //! Enable / disable detection of motor stall
      void MC_StallDetection_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                        const U32 cmdSeq, /*!< The command sequence number*/
                                        motorStallEnableList motorStallEnable);

      //! Implementation for MC_PositionCounterReset command handler
      //! Reset position counter
      void MC_PositionCounterReset_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                              const U32 cmdSeq, /*!< The command sequence number*/
                                              U8 ResetPositionCounter);

      //! Implementation for MC_UpdateTelemetry command handler
      //! Requests an update from each of the motor controllers
      void MC_UpdateTelemetry_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                         const U32 cmdSeq /*!< The command sequence number*/);

    private:
      uint8_t txData[MC_BUFFER_MAX_SIZE];
      uint8_t rxData[MC_BUFFER_MAX_SIZE];

      MCError writeMotorControlRegister(i2cBASE_t *i2c,
                                        const MotorControllerI2C::I2cRegisterId id,
                                        const MotorControllerI2C::I2cSlaveAddress address,
                                        uint32_t data);

      MCError i2cMasterTransmit(i2cBASE_t *i2c,
                                const MotorControllerI2C::I2cSlaveAddress sadd,
                                const uint32_t length,
                                uint8_t * data);

      MCError i2cMasterReceive(i2cBASE_t *i2c,
                               const MotorControllerI2C :: I2cSlaveAddress sadd,
                               const uint32_t length,
                               uint8_t * data);

      uint32_t getSizeData(const MotorControllerI2C::I2cRegisterId id);

      uint32_t getHeaderSize();

      uint32_t getChecksumSize();

      void generateChecksumTable(const ChecksumType polynomial);

      MCError computeChecksum8(uint8_t *data,
                               const uint32_t bufferLength,
                               MotorControlChecksum *checksum);

      MCError packTransmitBuffer(const MotorControllerI2C::I2cRegisterId id,
                                 const uint32_t data,
                                 const uint32_t dataLength);

      MCError sendAllMotorsData(i2cBASE_t *i2c,
                                const MotorControllerI2C::I2cRegisterId id,
                                uint32_t data);

      uint8_t checksumLookUpTable[256];

      Motor_tick cmToMotorTicks(const Distance_cm dist);
      MCError enableDrivers();
      MCError disableDrivers();

      void MotorControlComponentImpl :: Move_all_motors(uint32_t distance);

      //Debug functions/items
      uint32_t tick_count = 0;

      uint8_t rotations_to_ticks;

      // left and right turn parameters
      uint8_t m_rightSpeed;
      uint8_t m_leftSpeed;
      uint8_t m_rightAngle;
      uint8_t m_leftAngle;

      // forward and reverse parameters
      Distance_cm m_fwDist;
      Distance_cm m_reDist;
      uint8_t m_fwSpeed;
      uint8_t m_reSpeed;

      // Encoder Converting value
      float m_encoderTickToCmRatio;

      // Stall detection
      bool m_stallDetectectionEnabled = false;

      // Front left (FL), Front right (FR), Rear right (RR), Rear left (RL) tick counts
      // Internal tick counter
      uint32_t FL_Encoder_Count;
      uint32_t FR_Encoder_Count;
      uint32_t RR_Encoder_Count;
      uint32_t RL_Encoder_Count;

      // Offset for resetting tick count
      uint32_t FL_Encoder_Count_Offset = 0;
      uint32_t FR_Encoder_Count_Offset = 0;
      uint32_t RR_Encoder_Count_Offset = 0;
      uint32_t RL_Encoder_Count_Offset = 0;
    };

} // end namespace CubeRover

#endif
