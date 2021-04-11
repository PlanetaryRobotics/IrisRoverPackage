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
#include "MotorController_i2c.h"


#define MC_BUFFER_MAX_SIZE      16 // Maximum size of I2C buffer
#define PI                      3.14159265


namespace CubeRover {

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
      
      
    /* Implementation specific declarations */
    private:
        typedef enum  {
            REG_I2C_ADDRESS = 0,              // DEVELOPMENT ONLY // READ-ONLY
            REG_RELATIVE_TARGET_POSITION = 1, // Write-only
            REG_TARGET_SPEED = 2,             // Write-only
            REG_CURRENT_POSITION = 3,         // Read-only
            REG_CURRENT_SPEED = 4,            // Read-only  TODO: Micheal's original comment, look into this "Weird values. Normalized version of tick rate. See Spreadsheet labeled "Motor tests""
            REG_MOTOR_CURRENT = 5,            // Read-only
            REG_P_CURRENT = 6,                // Write-only
            REG_I_CURRENT = 7,                // Write-only
            REG_P_SPEED = 8,                  // Write-only
            REG_I_SPEED = 9,                  // Write-only
            REG_ACC_RATE = 10,                // Write-only
            REG_DEC_RATE = 11,                // Write-only
            REG_CTRL = 12,                    // Write-only TODO: Check
            REG_STATUS = 13,                  // Read-only  TODO: Check
            REG_FAULT = 14,                   // Read-only
            REG_CLR_FAULT = 15,               // Write-only  TODO:: Jonathan to hold over this test option to flight
            NUM_REGS = 16,
            // TODO: Everything past here is depracated
            // EXECUTE_CMD = 16,             // NOT A THING
            DEPRACATE_ENABLE_DRIVER = 17,           // NOT A THING  TODO: Watchdog???
            DEPRACATE_DISABLE_DRIVER = 18,          // NOT A THING  TODO: Watchdog???
            DEPRACATE_RESET_CONTROLLER = 19,        // NOT A THING  TODO: Watchdog to handle this???
            // UNSET = 99                    // Not a command
        } RegisterAddress_t;

        typedef enum {
            MC_NO_ERROR,
            MC_I2C_TIMEOUT_ERROR,
            MC_UNEXPECTED_ERROR
        } MCError_t;
        
        typedef int32_t Distance_cm_t;
        typedef int32_t MotorTick_t;
        typedef uint8_t Throttle_t;
      
        uint8_t txData[MC_BUFFER_MAX_SIZE];
        uint8_t rxData[MC_BUFFER_MAX_SIZE];
        
        i2cBASE_t *m_i2c;

        MCError_t sendAllMotorsData(const RegisterAddress_t id, uint8_t * data);

        MCError_t writeMotorControlRegister(const RegisterAddress_t id,
                                            const I2cSlaveAddress_t add,
                                            uint8_t * data);

        uint32_t getSizeData(const RegisterAddress_t reg);
        bool expectingReturnMessage(const RegisterAddress_t reg);

        MCError_t moveAllMotorsStraight(int32_t distance, int16_t speed);
        MCError_t rotateAllMotors(int16_t angle, int16_t speed);
        MCError_t spinMotors(bool forward);

        MCError_t enableDrivers();
        MCError_t disableDrivers();
        void resetMotorControllers();

        MotorTick_t groundCMToMotorTicks(int16_t dist);
        Throttle_t groundSpeedToSpeedPrecent(int16_t speed);

        bool updateTelemetry();
        bool updateSpeed();
        bool updateCurrent();
        bool updateEncoder();

        // Member items
        uint32_t tick_count = 0;

        uint16_t m_ticksToRotation;

        // Encoder Converting values
        float m_encoderTickToCMRatio;

        // Angular distance converting value
        float m_angularToLinear;

        // Stall detection
        bool m_stallDetectectionEnabled[4];

        // Does a positive setpoint drive the rover forward or backwards
        // Set this flag to rotate the wheels accordingly
        bool m_forward_is_positive = true;

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
    };

} // end namespace CubeRover

#endif
