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

#include "Fw/Types/BasicTypes.hpp"
#include "CubeRover/MotorControl/MotorControlComponentAc.hpp"
#include "MotorController_i2c.h"
#include "MotorController.hpp"

#define FRONT_LEFT_MC_I2C_ADDR 0x48
#define FRONT_RIGHT_MC_I2C_ADDR 0x49
#define REAR_RIGHT_MC_I2C_ADDR 0x4B
#define REAR_LEFT_MC_I2C_ADDR 0x4A

#define MC_BUFFER_MAX_SIZE 16 // Maximum size of I2C buffer

namespace CubeRover
{

    class MotorControlComponentImpl : public MotorControlComponentBase
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
        void motorCommandIn_handler(const NATIVE_INT_TYPE portNum,                 /*!< The port number*/
                                    CubeRoverPorts::MC_CommandType command_type,   /*!<  Selector for which command should be called */
                                    CubeRoverPorts::MC_MovementType movement_type, /*!<  Selector how movement should be interpreted */
                                    U8 Distance,
                                    U8 Speed);

        // ----------------------------------------------------------------------
        // Command handler implementations
        // ----------------------------------------------------------------------

        //! Implementation for MC_Current_PID command handler
        //!
        void MC_Current_PID_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                       const U32 cmdSeq,          /*!< The command sequence number*/
                                       U8 Motor_ID,               /*!< The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR */
                                       U32 PI_Values);

        //! Implementation for MC_Speed_PID command handler
        //!
        void MC_Speed_PID_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                     const U32 cmdSeq,          /*!< The command sequence number*/
                                     U8 Motor_ID,               /*!< The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR */
                                     U32 PID_Values);

        //! Implementation for MC_Acceleration command handler
        //!
        void MC_Acceleration_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                        const U32 cmdSeq,          /*!< The command sequence number*/
                                        U8 Motor_ID,               /*!< The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR */
                                        U32 Rate_Values);

        //! Implementation for MC_StallDetection command handler
        //!
        void MC_StallDetection_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                          const U32 cmdSeq,          /*!< The command sequence number*/
                                          U8 Motor_ID,               /*!< The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR */
                                          U8 Value /*!< 0x00 is disabled, 0xFF is enabled */);

        //! Implementation for MC_ResetPosition command handler
        //!
        void MC_ResetPosition_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                         const U32 cmdSeq,          /*!< The command sequence number*/
                                         U8 Motor_ID /*!< The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR */);

        //! Implementation for MC_Spin command handler
        //!
        void MC_Spin_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                const U32 cmdSeq,          /*!< The command sequence number*/
                                U8 Motor_ID,               /*!< The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR */
                                U32 Raw_Ticks);

        //! Implementation for MC_PowerBoost command handler
        //!
        void MC_PowerBoost_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                      const U32 cmdSeq,          /*!< The command sequence number*/
                                      U8 Motor_ID,               /*!< The motor id from 0 - 4 Motor 0 is all motors, Motorsss 1 - 4 are FL, FR, BL, BR */
                                      U8 Value /*!< 0x00 is On, 0xFF is Off */);

        //! Implementation for MC_SetParameter command handler
        //!
        void MC_SetParameter_cmdHandler(const FwOpcodeType opCode,   /*!< The opcode*/
                                        const U32 cmdSeq,            /*!< The command sequence number*/
                                        MC_ParameterSelection Value, /*!<  Change internal parameters of the module */
                                        U32 New_Value /*!<  The new value to be used in place */);

        //! Implementation for MC_UpdateTelemetry command handler
        //! Requests an update from each of the motor controllers
        void MC_UpdateTelemetry_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                           const U32 cmdSeq /*!< The command sequence number*/);

        /* Implementation specific declarations */


        /* -------------------------------------------------------
         * MC DEFINITIONS
         * Author: RAD
         * -------------------------------------------------------
         */

    private:

        typedef enum
        {
            MOTOR_A = 0,
            MOTOR_B = 1,
            MOTOR_C = 2,
            MOTOR_D = 3,
            NUM_MOTORS = 4
        }MCMotorID_t;

    public:

        // --- MC Error Types
        typedef enum
        {
            MC_NO_ERROR,
            MC_I2C_TIMEOUT_ERROR,
            MC_UNEXPECTED_ERROR,
            MC_BAD_COMMAND_INPUT
        } MCError_t;

        // mc state
        typedef enum
        {
            IDLE,           // Updating Parameters w/Drivers Disabled
            ENABLED,        // Drivers Enabled
            ARMED,          // Execute Drive Sent
            RUNNING,        // Driving
            TARGET_REACHED, // Target Reached
            DISABLED,       // Powered Off (theoretically)
            FAULT           // Fault
        } MCState_t;







        /* --- FUNCTIONS --- */
        void initMotorControllers();

        MCError_t updateMotorControllers(const RegisterAddress_t id, void *_data);


        /* --- Private Variables --- */
        MCState_t motorControllerState;
        MotorController motors[NUM_MOTORS];




















        /* ------------------------------------------
         * OBSOLETE MC DEFINITIONS
         * ------------------------------------------
         */

        // Tightly coupled to *_ADDR and *_ID defines
        static const uint8_t motorIdAddressMap[NUM_MOTORS];

        i2cBASE_t *m_i2c;

        MCError_t sendAllMotorsData(const RegisterAddress_t id, void *_data);

        MCError_t motorControlTransfer(I2cSlaveAddress_t add,
                                       RegisterAddress_t reg,
                                       void *_data);

        uint32_t regSizeMap(RegisterAddress_t reg);

        bool checkMotorsStatus();
        bool startMotorMovement();
        MCError_t moveAllMotorsStraight(int32_t distance, int16_t speed);
        MCError_t rotateAllMotors(int16_t angle, int16_t speed);
        MCError_t spinMotors(bool forward);

        bool updateTelemetry();

        bool pollStatus();

        // Member items
        uint32_t tick_count = 0;

        uint16_t m_ticksToRotation;

        // Encoder Converting values
//        float m_encoderTickToCMRatio;

        // Angular distance converting value
        float m_angularToLinear;

        // Stall detection
        bool m_stallDetectectionEnabled[4];

        StatusRegister_t m_currStatus[NUM_MOTORS];

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
