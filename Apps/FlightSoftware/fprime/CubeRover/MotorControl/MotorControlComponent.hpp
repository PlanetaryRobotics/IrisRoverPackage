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


#define MOTOR_CONTROL_I2CREG     	i2cREG1
#define ALL_MOTOR_ADDR			    0x00
#define ALL_MOTOR_ID			    0x00
// TODO: MAKE MOTOR IDS ENUMS IN FPRIME XML
#define FRONT_LEFT_MC_I2C_ADDR      0x48
#define FRONT_LEFT_MC_I2C_ID        0x01
#define FRONT_RIGHT_MC_I2C_ADDR     0x49
#define FRONT_RIGHT_MC_I2C_ID       0x02
#define REAR_LEFT_MC_I2C_ADDR       0x4A
#define REAR_LEFT_MC_I2C_ID         0x03
#define REAR_RIGHT_MC_I2C_ADDR      0x4B
#define REAR_RIGHT_MC_I2C_ID        0x04
#define NUM_MOTORS                  4

#define MAX_SPEED                   100
#define CUBEROVER_WHEEL_DIAMETER_CM			20.0f
#define CUBEROVER_COM_TO_WHEEL_CIRC_CM	78.54f
#define MOTOR_NB_PAIR_POLES							1.0f
#define MOTOR_GEAR_BOX_REDUCTION				5.0f
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
                                     U8 Motor_ID, /*!< The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR */
                                     U32 PI_Values);

      //! Implementation for MC_Speed_PID command handler
      //! 
      void MC_Speed_PID_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                   const U32 cmdSeq, /*!< The command sequence number*/
                                   U8 Motor_ID, /*!< The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR */
                                   U32 PID_Values);

      //! Implementation for MC_Acceleration command handler
      //! 
      void MC_Acceleration_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                      const U32 cmdSeq, /*!< The command sequence number*/
                                      U8 Motor_ID, /*!< The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR */
                                      U32 Rate_Values);

      //! Implementation for MC_StallDetection command handler
      //! 
      void MC_StallDetection_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                        const U32 cmdSeq, /*!< The command sequence number*/
                                        U8 Motor_ID, /*!< The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR */
                                        U8 Value /*!< 0x00 is disabled, 0xFF is enabled */);

      //! Implementation for MC_ResetPosition command handler
      //! 
      void MC_ResetPosition_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                       const U32 cmdSeq, /*!< The command sequence number*/
                                       U8 Motor_ID /*!< The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR */);

      //! Implementation for MC_Spin command handler
      //! 
      void MC_Spin_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                              const U32 cmdSeq, /*!< The command sequence number*/
                              U8 Motor_ID, /*!< The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR */
                              U32 Raw_Ticks);

      //! Implementation for MC_PowerBoost command handler
      //! 
      void MC_PowerBoost_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                    const U32 cmdSeq, /*!< The command sequence number*/
                                    U8 Motor_ID, /*!< The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR */
                                    U8 Value /*!< 0x00 is On, 0xFF is Off */);

      //! Implementation for MC_SetParameter command handler
      //! 
      void MC_SetParameter_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                      const U32 cmdSeq, /*!< The command sequence number*/
                                      MC_ParameterSelection Value, /*!<  Change internal parameters of the module */
                                      U32 New_Value /*!<  The new value to be used in place */);

      //! Implementation for MC_UpdateTelemetry command handler
      //! Requests an update from each of the motor controllers
      void MC_UpdateTelemetry_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                         const U32 cmdSeq /*!< The command sequence number*/);

      
    /* Implementation specific declarations */
    private:
        typedef enum  {
            REG_I2C_ADDRESS = 0,              // DEVELOPMENT ONLY // READ-ONLY
            REG_RELATIVE_TARGET_POSITION = 1, // Write-only
            REG_TARGET_SPEED = 2,             // Write-only
            REG_CURRENT_POSITION = 3,         // Read-only
            REG_MOTOR_CURRENT = 5,            // Read-only
            REG_P_CURRENT = 6,                // Write-only
            REG_I_CURRENT = 7,                // Write-only
            REG_P_SPEED = 8,                  // Write-only
            REG_I_SPEED = 9,                  // Write-only
            REG_ACC_RATE = 10,                // Write-only
            REG_DEC_RATE = 11,                // Write-only
            REG_CTRL = 12,                    // Write-only
            e_REG_STATUS = 13,                // Read-only
            REG_FAULT = 14,                   // Read-only
            NUM_REGS = 16,
        } RegisterAddress_t;
        
        /* Motor Control Interface w.r.t. PR #51 & PR#52 */
        typedef union {
            uint8_t value;
            struct {
                uint8_t open_loop   : 1;
                uint8_t clear_fault : 1;
                uint8_t fsm_disable : 1;
                uint8_t fsm_run     : 1;
                uint8_t override_fault_detection : 1;
                uint8_t unused      : 3;
            } bits;
        } ControlRegister_t;
        
        typedef union {
            uint8_t value;
            struct {
                uint8_t open_loop   : 1;
                uint8_t clear_fault : 1;
                uint8_t fsm_disable : 1;
                uint8_t position_converged : 1;
                uint8_t controller_error   : 1;
                uint8_t unused             : 3;
            } bits;
        } StatusRegister_t;
        
        typedef union {
            uint8_t value;
            struct {
                uint8_t driver_fault        : 1;
                uint8_t position_no_change  : 1;
                uint8_t driving_wrong_direction : 1;
                uint8_t unused              : 5;
            } bits;
        } FaultRegister_t;

        typedef enum {
            MC_NO_ERROR,
            MC_I2C_TIMEOUT_ERROR,
            MC_UNEXPECTED_ERROR
        } MCError_t;
        
        typedef int32_t Distance_cm_t;
        typedef int32_t MotorTick_t;
        typedef uint8_t Throttle_t;
        
        // Tightly coupled to *_ADDR and *_ID defines
        static const uint8_t motorIdAddressMap[NUM_MOTORS];
      
        i2cBASE_t *m_i2c;

        MCError_t sendAllMotorsData(const RegisterAddress_t id, void *_data);

        MCError_t motorControlTransfer(I2cSlaveAddress_t add,
                                       RegisterAddress_t reg,
                                       void *_data);

        uint32_t regSizeMap(RegisterAddress_t reg);

        MCError_t moveAllMotorsStraight(int32_t distance, int16_t speed);
        MCError_t rotateAllMotors(int16_t angle, int16_t speed);
        MCError_t spinMotors(bool forward);

        MotorTick_t groundCMToMotorTicks(int16_t dist);
        Throttle_t groundSpeedToSpeedPrecent(int16_t speed);

        bool updateTelemetry();

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
