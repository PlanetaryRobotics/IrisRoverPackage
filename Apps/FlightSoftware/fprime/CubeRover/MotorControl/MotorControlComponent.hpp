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
#include "CubeRover/WatchDogInterface/WatchDogInterface.hpp" // need the `reset_values_possible` enum definitions

#define MOTOR_A_IDX 0
#define MOTOR_A (1 << MOTOR_A_IDX)
#define MOTOR_B_IDX 1
#define MOTOR_B (1 << MOTOR_B_IDX)
#define MOTOR_C_IDX 2
#define MOTOR_C (1 << MOTOR_C_IDX)
#define MOTOR_D_IDX 3
#define MOTOR_D (1 << MOTOR_D_IDX)

#define FRONT_LEFT_MC_I2C_ADDR 0x48
#define FRONT_RIGHT_MC_I2C_ADDR 0x49
#define REAR_RIGHT_MC_I2C_ADDR 0x4B
#define REAR_LEFT_MC_I2C_ADDR 0x4A

// How often to emit a scheduler heartbeat:
#define MC_SCHED_HEARTBEAT_INTERVAL_MS 5000

namespace CubeRover
{

        class MotorControlComponentImpl : public MotorControlComponentBase,  public WatchDogInterfaceComponentBaseFriend
        {

                ::Os::Mutex mc_mutex;

                inline uint8_t idToMask(uint8_t motor_id)
                {
                        return 1 << motor_id;
                }

                //    inline uint8_t containsMotorID(uint8_t motor_ids_mask, uint8_t motor_id)
                //    {
                //        return motor_ids_mask & (1 << motor_id);
                //    }

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
                void PingIn_handler(
                    const NATIVE_INT_TYPE portNum, /*!< The port number*/
                    U32 key                        /*!< Value to return to pinger*/
                );

                //! Handler implementation for schedIn
                //!
                void schedIn_handler(
                    const NATIVE_INT_TYPE portNum, /*!< The port number*/
                    NATIVE_UINT_TYPE context       /*!< The call order*/
                );

                //! Handler implementation for motorCommandIn
                //!
                void motorCommandIn_handler(
                    const NATIVE_INT_TYPE portNum,                 /*!< The port number*/
                    CubeRoverPorts::MC_CommandType command_type,   /*!<  Selector for which command should be called */
                    CubeRoverPorts::MC_MovementType movement_type, /*!<  Selector how movement should be interpreted */
                    U8 Distance,
                    U8 Speed);

                // ----------------------------------------------------------------------
                // Command handler implementations
                // ----------------------------------------------------------------------

                //! Handler for command MC_UpdateTelemetry
                /* Forces a telemetry update from the motors */
                void MC_UpdateTelemetry_cmdHandler(
                    const FwOpcodeType opCode, /*!< The opcode*/
                    const U32 cmdSeq           /*!< The command sequence number*/
                );

                //! Handler for command MC_SetAllParams
                /* Sets ALL RW params for EVERY motor.
                   (Note: REG_MC_CTRL = MC_CMD_UPDATE_CONFIG ) */
                void MC_SetAllParams_cmdHandler(
                    const FwOpcodeType opCode, /*!< The opcode*/
                    const U32 cmdSeq,          /*!< The command sequence number*/
                    U8 MotorA_TargetDir,
                    U32 MotorA_TargetPosition,
                    U8 MotorA_TargetSpeed,
                    U16 MotorA_Current_P_Val,
                    U16 MotorA_Current_I_Val,
                    U16 MotorA_Speed_P_Val,
                    U16 MotorA_Speed_I_Val,
                    U16 MotorA_Accel_Rate,
                    U16 MotorA_Decel_Rate,
                    U8 MotorB_TargetDir,
                    U32 MotorB_TargetPosition,
                    U8 MotorB_TargetSpeed,
                    U16 MotorB_Current_P_Val,
                    U16 MotorB_Current_I_Val,
                    U16 MotorB_Speed_P_Val,
                    U16 MotorB_Speed_I_Val,
                    U16 MotorB_Accel_Rate,
                    U16 MotorB_Decel_Rate,
                    U8 MotorC_TargetDir,
                    U32 MotorC_TargetPosition,
                    U8 MotorC_TargetSpeed,
                    U16 MotorC_Current_P_Val,
                    U16 MotorC_Current_I_Val,
                    U16 MotorC_Speed_P_Val,
                    U16 MotorC_Speed_I_Val,
                    U16 MotorC_Accel_Rate,
                    U16 MotorC_Decel_Rate,
                    U8 MotorD_TargetDir,
                    U32 MotorD_TargetPosition,
                    U8 MotorD_TargetSpeed,
                    U16 MotorD_Current_P_Val,
                    U16 MotorD_Current_I_Val,
                    U16 MotorD_Speed_P_Val,
                    U16 MotorD_Speed_I_Val,
                    U16 MotorD_Accel_Rate,
                    U16 MotorD_Decel_Rate);

                //! Handler for command MC_SetParameter
                /* Update single parameter of selected motor controller(s). */
                void MC_SetParameter_cmdHandler(
                    const FwOpcodeType opCode, /*!< The opcode*/
                    const U32 cmdSeq,          /*!< The command sequence number*/
                    U8 Motor_ID,               /*!< Bitmask of motors to update.*/
                    U8 Param_RegAddr,          /*!< The register address of the parameter to set.*/
                    U32 Param_NewValue         /*!<  The new value of the parameter.*/
                );

                //! Handler for command MC_Spin
                /* The new value of the parameter. */
                void MC_Spin_cmdHandler(
                    const FwOpcodeType opCode, /*!< The opcode*/
                    const U32 cmdSeq,          /*!< The command sequence number*/
                    U8 Motor_ID,               /*!< One or All motors to update.
                                      MotorA:0x00 , MotorB:0x01 , MotorC:0x02 , MotorB:0x03 , AllMotors:0xFF*/
                    U8 Dir,                    /*!< Direction motor movement
                                                       Positive Ticks : 0 , Negative Ticks : 1*/
                    U32 Raw_Ticks              /*!< Distance to spin in ticks. (Default = 20000)*/
                );

                //! Handler for command MC_Spin_Configured
                /* The new value of the parameter. */
                void MC_Spin_Configured_cmdHandler(
                    const FwOpcodeType opCode, /*!< The opcode*/
                    const U32 cmdSeq,          /*!< The command sequence number*/
                    U8 Motor_ID,               /*!< Bitmask of motors to update.
                                      MotorA:0x01 , MotorB:0x02 , MotorC:0x04 , MotorD:0x08 , AllMotors:0x0F*/
                    U8 Dir,                    /*!< Direction motor movement
                                                       Positive Ticks : 0 , Negative Ticks : 1*/
                    U32 Raw_Ticks,             /*!< Distance to spin in ticks. (Default = 20000)*/
                    U8 Percent_Speed           /*!< Speed set by percent of MAX_SPEED. (Default = 70)*/
                );

                void MC_Spin_Full_Custom_cmdHandler(
                    const FwOpcodeType opCode, /*!< The opcode*/
                    const U32 cmdSeq,          /*!< The command sequence number*/
                    U8 MotorA_Dir,
                    U32 MotorA_Ticks,
                    U8 MotorA_Speed,
                    U8 MotorB_Dir,
                    U32 MotorB_Ticks,
                    U8 MotorB_Speed,
                    U8 MotorC_Dir,
                    U32 MotorC_Ticks,
                    U8 MotorC_Speed,
                    U8 MotorD_Dir,
                    U32 MotorD_Ticks,
                    U8 MotorD_Speed);

                // ----------------------------------------------------------------------
                // OBSOLETE Command handler implementations
                //      To be used as Helper Functions
                // ----------------------------------------------------------------------

                //! Implementation for MC_Current_PID command handler
                //!
                //        void MC_Current_PID_cmdHandler(
                void MC_Current_PID_set(
                    const FwOpcodeType opCode, /*!< The opcode*/
                    const U32 cmdSeq,          /*!< The command sequence number*/
                    U8 Motor_ID,
                    U32 PI_Values);

                //! Implementation for MC_Speed_PID command handler
                //!
                //        void MC_Speed_PID_cmdHandler(
                void MC_Speed_PID_set(
                    const FwOpcodeType opCode, /*!< The opcode*/
                    const U32 cmdSeq,          /*!< The command sequence number*/
                    U8 Motor_ID,
                    U32 PID_Values);

                //! Implementation for MC_Acceleration command handler
                //!
                //        void MC_Acceleration_cmdHandler(
                void MC_Acceleration_set(
                    const FwOpcodeType opCode, /*!< The opcode*/
                    const U32 cmdSeq,          /*!< The command sequence number*/
                    U8 Motor_ID,
                    U32 Rate_Values);

                //! Implementation for MC_StallDetection command handler
                //!
                //        void MC_StallDetection_cmdHandler(
                void MC_StallDetection_set(
                    const FwOpcodeType opCode, /*!< The opcode*/
                    const U32 cmdSeq,          /*!< The command sequence number*/
                    U8 Motor_ID,
                    U8 Value /*!< 0x00 is disabled, 0xFF is enabled */
                );

                //! Implementation for MC_ResetPosition command handler
                //!
                //        void MC_ResetPosition_cmdHandler(
                void MC_ResetPosition_set(
                    const FwOpcodeType opCode, /*!< The opcode*/
                    const U32 cmdSeq,          /*!< The command sequence number*/
                    U8 Motor_ID);

                //! Implementation for MC_PowerBoost command handler
                //!
                //        void MC_PowerBoost_cmdHandler(
                void MC_PowerBoost_set(
                    const FwOpcodeType opCode, /*!< The opcode*/
                    const U32 cmdSeq,          /*!< The command sequence number*/
                    U8 Motor_ID,
                    U8 Value /*!< 0x00 is On, 0xFF is Off */
                );

                //! Handler for command MC_Emergency_Stop
                /* Powers down the motor bus (stopping all actions) immediately.
                        May have to reset Hercules to regain I2C comms after this. */
                void MC_Emergency_Stop_cmdHandler(
                    FwOpcodeType opCode, /*!< The opcode*/
                    U32 cmdSeq /*!< The command sequence number*/
                );

                /* Implementation specific declarations */

                /* -------------------------------------------------------
                 * MC DEFINITIONS
                 * Author: RAD
                 * -------------------------------------------------------
                 */

        private:
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
                void checkFaults();
                void checkStates();

                /* --- Private Variables --- */

                // PROTECTED -- Requires mc_mutex to access
                MotorControllerStruct m_motor_controllers[NUM_MOTORS];
                MCState_t m_motorControllerState;

                // UNPROTECTED
                bool m_updateParams;

                /* ------------------------------------------
                 * UNSAFE TEST functions
                 * ------------------------------------------
                 */
                MCError_t testSpin();
                bool setSpinParams(uint8_t motor_mask, uint8_t dir_mask,
                                   int32_t dist, uint8_t speed);

                /* ------------------------------------------
                 * OBSOLETE MC DEFINITIONS
                 * ------------------------------------------
                 */

                bool OG_Spin_Handler(const FwOpcodeType opCode, const U32 cmdSeq,
                                     U8 Motor_ID, U8 Dir, U32 Raw_Ticks);

                // Tightly coupled to *_ADDR and *_ID defines
                static const uint8_t motorIdAddressMap[NUM_MOTORS];

                MCError_t sendAllMotorsData(const MC_ICD_RegAddr id, void *_data);

                MCError_t motorControlTransfer(I2cSlaveAddress_t add,
                                               MC_ICD_RegAddr reg,
                                               void *_data);

                uint32_t regSizeMap(MC_ICD_RegAddr reg);

                bool checkMotorsStatus();
                bool startMotorMovement();
                MCError_t moveAllMotorsStraight(int32_t distance, int16_t speed);
                MCError_t rotateAllMotors(int16_t angle, int16_t speed);
                MCError_t spinMotors(bool forward);

                uint8_t updateTelemSpecific(MC_ICD_RegAddr regID, uint32_t *buffer);
                bool updateTelemetry();

                bool pollStatus();

                void powerOffMotors(uint32_t wait_for_power_off_ms=0);

                void powerOnMotors(uint32_t wait_for_power_on_ms=10);

                // Member items
                uint32_t tick_count = 0;

                uint16_t m_ticksToRotation;

                // Encoder Converting values
                //        float m_encoderTickToCMRatio;

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
