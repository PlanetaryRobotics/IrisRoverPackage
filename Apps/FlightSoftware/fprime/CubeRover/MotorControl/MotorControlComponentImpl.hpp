// ======================================================================
// \title  MotorControlComponentImpl.hpp
// \author cedric
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

  namespace MotorControllerI2C{
  typedef uint8_t MotorControlI2cRegId_t;

    typedef enum {
      I2C_ADDRESS = 0x01,
      I2C_RELATIVE_TARGET_POSITION,
      I2C_DIRECTION,
      I2C_TARGET_SPEED,
      I2C_CURRENT_POSITION,
      I2C_MOTOR_CURRENT,
      I2C_P_CURRENT,
      I2C_I_CURRENT,
      I2C_P_VELOCITY,
      I2C_I_VELOCITY,
      I2C_D_VELOCITY,
      I2C_P_POSITION,
      I2C_I_POSITION,
      I2C_D_POSITION,
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
      I2C_MC_MAX_NUM_OF_ELEMENTS
    }I2cRegisterId;

    typedef uint8_t I2cSlaveAddress;
  } // end of MotorControllerI2C namespace

  typedef enum {
      MC_NO_ERROR,
      MC_UNEXPECTED_ERROR
  }MCError;

  typedef enum {
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
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object MotorControl
      //!
      ~MotorControlComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for schedIn
      //!
      void schedIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for MC_DrivingConfiguration command handler
      //! Configure forward command
      void MC_DrivingConfiguration_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          CommandList CommandConfiguration, 
          ParameterList Parameter, 
          U8 Value 
      );

      //! Implementation for MC_ExecuteDriving command handler
      //! Execute one configured command
      void MC_ExecuteDriving_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          DriveCommandList DrivingCommand 
      );

      //! Implementation for MC_TuningParameters command handler
      //! Contains all motor control parameters
      void MC_TuningParameters_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          tuningParameterList TuningParameter, 
          U16 Value 
      );

      //! Implementation for MC_AccelerationProfiles command handler
      //! Contains acceleration and deceleration profile
      void MC_AccelerationProfiles_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          accelerationParameterList AccelerationParameter, 
          U16 Value 
      );

      //! Implementation for MC_StallDetection command handler
      //! Enable / disable detection of motor stall
      void MC_StallDetection_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          motorStallEnableList motorStallEnable 
      );

      //! Implementation for MC_PositionCounterReset command handler
      //! Reset position counter
      void MC_PositionCounterReset_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U8 ResetPositionCounter 
      );

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

      uint8_t checksumLookUpTable[256];

      Motor_tick cmToMotorTicks(const Distance_cm dist);
      MCError enableDrivers();
      MCError disableDrivers();

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

      float m_encoderTickToCmRatio;
    };

} // end namespace CubeRover

#endif
