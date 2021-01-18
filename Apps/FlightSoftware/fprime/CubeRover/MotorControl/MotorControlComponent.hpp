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
      I2C_ADDRESS = 0,              // TESTED // Back address  / 1 Byte
      RELATIVE_TARGET_POSITION = 1, // TESTED // In Motor Ticks (9750 ticks per rotation) / 4 Bytes
      TARGET_SPEED = 2,             // TESTED // In Normalized speed of? // 2 Bytes
      CURRENT_POSITION = 3,         // TESTED // In Motor Ticks (9750 ticks per rotation) // 2 Bytes
      CURRENT_SPEED = 4,            // TESTED // Weird values. Normalized version of tick rate. See Spreadsheet labeled "Motor tests" // 4 Bytes
      MOTOR_CURRENT = 5,            // TESTED // Returns Milliamps in IQ format // 4 Bytes
      P_CURRENT = 6,                // TESTED Nominally 0.95 // 4 Bytes
      I_CURRENT = 7,                // ASSUMED WORKING // By testing P_Current // 4 Bytes (Really 2 lower bytes)
      P_SPEED = 8,                  // TESTED Nominally 1 // 4 Bytes
      I_SPEED = 9,                  // ASSUMED WORKING // By testing P_Current // 4 Bytes (Really 2 lower bytes)
      ACC_RATE = 10,                // WILL NOT WORK
      DEC_RATE = 11,                // WILL NOT WORK
      CONTROL_REGISTER = 12,        // NOT MEANINGFUL
      STATUS_REGISTER = 13,         // NOT MEANINGFUL
      FAULT_REGISTER = 14,          // NOT MEANINGFUL
      EXECUTE_CMD = 15,             // NOT A THING
      ENABLE_DRIVER = 16,           // NOT A THING
      DISABLE_DRIVER = 17,          // NOT A THING
      RESET_CONTROLLER = 18,        // NOT A THING
      CLEAR_FAULT = 19,             // WILL NOT WORK
      MAX_NB_CMDS = 20,             // Not a command
      UNSET = 99                    // Not a command

    }I2cRegisterId;

    typedef uint8_t I2cSlaveAddress;
  } // end of MotorControllerI2C namespace

  typedef enum 
  {
      MC_NO_ERROR,
      MC_I2C_TIMEOUT_ERROR,
      MC_UNEXPECTED_ERROR
  }MCError;

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

      //! Handler implementation for motorCommandIn
      //!
      void motorCommandIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                  CubeRoverPorts::MC_CommandType command_type, /*!<  Selector for which command should be called */
                                  CubeRoverPorts::MC_MovementType movement_type, /*!<  Selector how movement should be interpreted */
                                  U8 Distance, 
                                  U8 Speed);

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for MC_Current_PID command handler
      //! 
      void MC_Current_PID_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                     const U32 cmdSeq, /*!< The command sequence number*/
                                     U8 Motor_ID, /*!< The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors */
                                     U32 PI_Values);

      //! Implementation for MC_Speed_PID command handler
      //! 
      void MC_Speed_PID_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                   const U32 cmdSeq, /*!< The command sequence number*/
                                   U8 Motor_ID, /*!< The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors */
                                   U64 PID_Values);

      //! Implementation for MC_Position_PID command handler
      //! 
      void MC_Position_PID_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                      const U32 cmdSeq, /*!< The command sequence number*/
                                      U8 Motor_ID, /*!< The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors */
                                      U64 PID_Values);

      //! Implementation for MC_Acceleration command handler
      //! 
      void MC_Acceleration_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                      const U32 cmdSeq, /*!< The command sequence number*/
                                      U8 Motor_ID, /*!< The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors */
                                      U32 Rate_Values);

      //! Implementation for MC_StallDetection command handler
      //! 
      void MC_StallDetection_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                        const U32 cmdSeq, /*!< The command sequence number*/
                                        U8 Motor_ID, /*!< The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors */
                                        U8 Value /*!< 0x00 is disabled, 0xFF is enabled */);

      //! Implementation for MC_ResetPosition command handler
      //! 
      void MC_ResetPosition_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                       const U32 cmdSeq, /*!< The command sequence number*/
                                       U8 Motor_ID /*!< The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors */);

      //! Implementation for MC_Spin command handler
      //! 
      void MC_Spin_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                              const U32 cmdSeq, /*!< The command sequence number*/
                              U8 Motor_ID, /*!< The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors */
                              U8 Spin_Type);

      //! Implementation for MC_PowerBoost command handler
      //! 
      void MC_PowerBoost_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                    const U32 cmdSeq, /*!< The command sequence number*/
                                    U8 Motor_ID, /*!< The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors */
                                    U8 Value /*!< 0x00 is On, 0xFF is Off */);

      //! Implementation for MC_SetParameter command handler
      //! 
      void MC_SetParameter_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                      const U32 cmdSeq, /*!< The command sequence number*/
                                      MC_ParameterSelection Value, /*!<  Change internal parameters of the module */
                                      U32 New_Value /*!<  The new value to be used in place */);

      //! Implementation for MC_GetParameter command handler
      //! Returns all current parameters of the module
      void MC_GetParameters_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                      const U32 cmdSeq /*!< The command sequence number*/);

      //! Implementation for MC_UpdateTelemetry command handler
      //! Requests an update from each of the motor controllers
      void MC_UpdateTelemetry_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                         const U32 cmdSeq /*!< The command sequence number*/);

      //! Implementation for MC_DriveTest command handler
      //! Allows the direct commanding of moves bypassing Nav if things are incorrect
      void MC_DriveTest_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                   const U32 cmdSeq, /*!< The command sequence number*/
                                   I64 Distance, 
                                   I8 MoveType);

      //! Implementation for MC_SelfTest command handler
      //! Runs through a routine to confirms the MSP are functioning properly
      void MC_SelfTest_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                  const U32 cmdSeq /*!< The command sequence number*/);
      
    private:
      uint8_t txData[MC_BUFFER_MAX_SIZE];
      uint8_t rxData[MC_BUFFER_MAX_SIZE];

      MCError sendAllMotorsData(i2cBASE_t *i2c,
                                const MotorControllerI2C::I2cRegisterId id,
                                uint8_t * data);

      MCError writeMotorControlRegister(i2cBASE_t *i2c,
                                        const MotorControllerI2C::I2cRegisterId id,
                                        const MotorControllerI2C::I2cSlaveAddress add,
                                        uint8_t * data);

      MCError i2cMasterTransmit(i2cBASE_t *i2c,
                                const MotorControllerI2C::I2cSlaveAddress sadd,
                                const uint32_t length,
                                uint8_t * data);

      MCError i2cMasterReceive(i2cBASE_t *i2c,
                               const MotorControllerI2C :: I2cSlaveAddress sadd,
                               const uint32_t length,
                               uint8_t * data);

      uint32_t getSizeData(const MotorControllerI2C::I2cRegisterId id);
      uint8_t setIDBuffer(const MotorControllerI2C::I2cRegisterId id);
      bool expectingReturnMessage(const MotorControllerI2C::I2cRegisterId id);

      MCError moveAllMotorsStraight(int32_t distance, int16_t speed);
      MCError rotateAllMotors(int16_t angle, int16_t speed);
      MCError spinMotors(bool forward);

      MCError enableDrivers();
      MCError disableDrivers();
      void resetMotorControllers();

      Motor_tick groundCMToMotorTicks(int16_t dist);
      Speed_percent groundSpeedToSpeedPrecent(int16_t speed);

      bool updateTelemetry();
      bool updateSpeed();
      bool updateCurrent();
      bool updateEncoder();

      void delayForI2C();

      // Member items
      uint32_t tick_count = 0;

      uint16_t m_ticksToRotation;

      // Encoder Converting values
      float m_encoderTickToCMRatio;

      // Angular distance converting value
      float m_angularToLinear;

      // Stall detection
      bool m_stallDetectectionEnabled[4];

      // Shortcut to rotate the wheels accordingly
      bool m_clockwise_is_positive = true;

      bool m_Round_Robin_Telemetry;
      uint8_t m_Robin_Number = 0;

      // Front left (FL), Front right (FR), Rear right (RR), Rear left (RL) tick counts
      // Internal tick counter
      int32_t m_FL_Encoder_Count;
      int32_t m_FR_Encoder_Count;
      int32_t m_RR_Encoder_Count;
      int32_t m_RL_Encoder_Count;

      // Offset for resetting tick count
      int32_t m_FR_Encoder_Count_Offset;
      int32_t m_FL_Encoder_Count_Offset;
      int32_t m_RL_Encoder_Count_Offset;
      int32_t m_RR_Encoder_Count_Offset;

      // Timeout for I2C communication
      uint16_t m_i2c_timeout_threshold = 1350;
    };

} // end namespace CubeRover

#endif
