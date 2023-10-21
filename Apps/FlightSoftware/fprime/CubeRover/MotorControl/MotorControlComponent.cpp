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

#include "FreeRTOS.h"
#include "os_task.h"

#include "Fw/Types/BasicTypes.hpp"
#include "Include/CubeRoverConfig.hpp"

namespace CubeRover
{

    const uint8_t MotorControlComponentImpl::motorIdAddressMap[NUM_MOTORS] = {
        FRONT_LEFT_MC_I2C_ADDR, FRONT_RIGHT_MC_I2C_ADDR,
        REAR_LEFT_MC_I2C_ADDR, REAR_RIGHT_MC_I2C_ADDR};

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
            const char *const compName) : MotorControlComponentBase(compName)
#else
        MotorControlComponentImpl(void)
#endif
    {
        m_i2c = MOTOR_CONTROL_I2CREG;
        m_updateParams = false;
        m_motor_controllers[0] = MotorControllerStruct();
        m_motor_controllers[1] = MotorControllerStruct();
        m_motor_controllers[2] = MotorControllerStruct();
        m_motor_controllers[3] = MotorControllerStruct();
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
    void MotorControlComponentImpl ::init(const NATIVE_INT_TYPE instance)
    {
        MotorControlComponentBase::init(instance);
        initMotorControllers();
    }

    /**
     * @brief      Destroys the object.
     */
    MotorControlComponentImpl ::~MotorControlComponentImpl(void)
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
    void MotorControlComponentImpl ::PingIn_handler(const NATIVE_INT_TYPE portNum, U32 key)
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
    /* --- NAV COMPONENT MOVE COMMANDS --- */
    void MotorControlComponentImpl ::motorCommandIn_handler(const NATIVE_INT_TYPE portNum,
                                                            CubeRoverPorts::MC_CommandType command_type,
                                                            CubeRoverPorts::MC_MovementType movement_type,
                                                            U8 Distance,
                                                            U8 Speed)
    {

    }

    // ----------------------------------------------------------------------
    // Command handler implementations
    // ----------------------------------------------------------------------

    /**
     * @brief      Command handler implementation to force update telemetry
     *
     * @param[in]  opCode          The operation code
     * @param[in]  cmdSeq          The command sequence
     */
    void MotorControlComponentImpl ::MC_UpdateTelemetry_cmdHandler(
            const FwOpcodeType opCode,
            const U32 cmdSeq)
    {
        if (updateTelemetry())
            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
        else
            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
    }

    /**
     * @brief      Command handler implementation to set ALL parameters for EVERY motor;
     *
     * @param[in]  opCode          The operation code
     * @param[in]  cmdSeq          The command sequence
     * @param[in]  values          All the parameter values
     */
    void MotorControlComponentImpl ::MC_SetAllParams_cmdHandler(
            const FwOpcodeType opCode, /*!< The opcode*/
            const U32 cmdSeq, /*!< The command sequence number*/
            U32 MotorA_TargetPosition,
            U8 MotorA_TargetSpeed,
            U16 MotorA_Current_P_Val,
            U16 MotorA_Current_I_Val,
            U16 MotorA_Speed_P_Val,
            U16 MotorA_Speed_I_Val,
            U16 MotorA_Accel_Rate,
            U16 MotorA_Decel_Rate,
            U32 MotorB_TargetPosition,
            U8 MotorB_TargetSpeed,
            U16 MotorB_Current_P_Val,
            U16 MotorB_Current_I_Val,
            U16 MotorB_Speed_P_Val,
            U16 MotorB_Speed_I_Val,
            U16 MotorB_Accel_Rate,
            U16 MotorB_Decel_Rate,
            U32 MotorC_TargetPosition,
            U8 MotorC_TargetSpeed,
            U16 MotorC_Current_P_Val,
            U16 MotorC_Current_I_Val,
            U16 MotorC_Speed_P_Val,
            U16 MotorC_Speed_I_Val,
            U16 MotorC_Accel_Rate,
            U16 MotorC_Decel_Rate,
            U32 MotorD_TargetPosition,
            U8 MotorD_TargetSpeed,
            U16 MotorD_Current_P_Val,
            U16 MotorD_Current_I_Val,
            U16 MotorD_Speed_P_Val,
            U16 MotorD_Speed_I_Val,
            U16 MotorD_Accel_Rate,
            U16 MotorD_Decel_Rate)
    {
        mc_mutex.lock();

        MotorControllerStruct *mc = &m_motor_controllers[MOTOR_A];
        mc->target_pos = MotorA_TargetPosition;
        mc->target_speed = MotorA_TargetSpeed;
        mc->current_p_val = MotorA_Current_P_Val;
        mc->current_i_val = MotorA_Current_I_Val;
        mc->speed_p_val = MotorA_Speed_P_Val;
        mc->speed_p_val = MotorA_Speed_I_Val;
        mc->acc_val = MotorA_Accel_Rate;
        mc->dec_val = MotorA_Decel_Rate;
//        mc->up_to_date = 0xFF;
        mc->ctrlReg = MC_CMD_OVERRIDE_PROTECTED;

        mc = &m_motor_controllers[MOTOR_B];
        mc->target_pos = MotorB_TargetPosition;
        mc->target_speed = MotorB_TargetSpeed;
        mc->current_p_val = MotorB_Current_P_Val;
        mc->current_i_val = MotorB_Current_I_Val;
        mc->speed_p_val = MotorB_Speed_P_Val;
        mc->speed_p_val = MotorB_Speed_I_Val;
        mc->acc_val = MotorB_Accel_Rate;
        mc->dec_val = MotorB_Decel_Rate;
//        mc->up_to_date = 0xFF;
        mc->ctrlReg = MC_CMD_OVERRIDE_PROTECTED;

        mc = &m_motor_controllers[MOTOR_C];
        mc->target_pos = MotorC_TargetPosition;
        mc->target_speed = MotorC_TargetSpeed;
        mc->current_p_val = MotorC_Current_P_Val;
        mc->current_i_val = MotorC_Current_I_Val;
        mc->speed_p_val = MotorC_Speed_P_Val;
        mc->speed_p_val = MotorC_Speed_I_Val;
        mc->acc_val = MotorC_Accel_Rate;
        mc->dec_val = MotorC_Decel_Rate;
//        mc->up_to_date = 0xFF;
        mc->ctrlReg = MC_CMD_OVERRIDE_PROTECTED;

        mc = &m_motor_controllers[MOTOR_D];
        mc->target_pos = MotorD_TargetPosition;
        mc->target_speed = MotorD_TargetSpeed;
        mc->current_p_val = MotorD_Current_P_Val;
        mc->current_i_val = MotorD_Current_I_Val;
        mc->speed_p_val = MotorD_Speed_P_Val;
        mc->speed_p_val = MotorD_Speed_I_Val;
        mc->acc_val = MotorD_Accel_Rate;
        mc->dec_val = MotorD_Decel_Rate;
//        mc->up_to_date = 0xFF;
        mc->ctrlReg = MC_CMD_OVERRIDE_PROTECTED;

        m_updateParams = true;
        mc_mutex.unLock();

        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

    /**
     * @brief      Command handler implementation to set a specific parameter
     *
     * @param[in]  opCode          The operation code
     * @param[in]  cmdSeq          The command sequence
     * @param[in]  ParamSelect     The selected parameter
     * @param[in]  New_Value       The new parameter value
     */
    void MotorControlComponentImpl ::MC_SetParameter_cmdHandler(const FwOpcodeType opCode,
                                                                const U32 cmdSeq,
                                                                U8 Motor_ID,
                                                                U8 Param_RegAddr,
                                                                U32 Param_NewValue)
    {
        if (containsMotorID(Motor_ID, MOTOR_A)) {
            setTargetPos(&m_motor_controllers[MOTOR_A], Param_NewValue);
        }
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
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
    void MotorControlComponentImpl ::MC_Spin_cmdHandler(
            const FwOpcodeType opCode,
            const U32 cmdSeq,
            U8 Motor_ID,
            U32 Raw_Ticks)
    {

        // TODO: Should this force open loop control as well?
        MCError_t err;
        uint8_t speed = MAX_SPEED;
        uint32_t ticks = Raw_Ticks;

        if (Motor_ID == ALL_MOTOR_ID)
        {
            err = sendAllMotorsData(MC_REG_TARGET_SPEED, &speed);
            if (err != MC_NO_ERROR)
            {
                this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
                return;
            }

            err = sendAllMotorsData(MC_REG_TARGET_POSITION, &Raw_Ticks);
            if (err != MC_NO_ERROR)
            {
                this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
                return;
            }
        }
        else
        {
            err = motorControlTransfer(motorIdAddressMap[Motor_ID], MC_REG_TARGET_SPEED, &speed);
            if (err != MC_NO_ERROR)
            {
                this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
                return;
            }

            err = motorControlTransfer(motorIdAddressMap[Motor_ID], MC_REG_TARGET_POSITION, &Raw_Ticks);
            if (err != MC_NO_ERROR)
            {
                this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
                return;
            }
        }

        checkStates();
        checkFaults();

        if (!startMotorMovement())
        {
            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
            return;
        }

        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
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
    void MotorControlComponentImpl ::MC_Spin_Configured_cmdHandler(
            const FwOpcodeType opCode,
            const U32 cmdSeq,
            U8 Motor_ID,
            U32 Raw_Ticks,
            U8 Percent_speed)
    {
        MCError_t err;

        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);

//        mc_mutex.lock();
        if (containsMotorID(Motor_ID, MOTOR_A)) {
            setTargetPos(&m_motor_controllers[MOTOR_A], Raw_Ticks);
            setTargetSpeed(&m_motor_controllers[MOTOR_A], Percent_speed);

            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
        }
        if (containsMotorID(Motor_ID, MOTOR_B)) {
            setTargetPos(&m_motor_controllers[MOTOR_B], Raw_Ticks);
            setTargetSpeed(&m_motor_controllers[MOTOR_B], Percent_speed);

            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
        }
        if (containsMotorID(Motor_ID, MOTOR_C)) {
            setTargetPos(&m_motor_controllers[MOTOR_C], Raw_Ticks);
            setTargetSpeed(&m_motor_controllers[MOTOR_C], Percent_speed);

            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
        }
        if (containsMotorID(Motor_ID, MOTOR_D)) {
            setTargetPos(&m_motor_controllers[MOTOR_D], Raw_Ticks);
            setTargetSpeed(&m_motor_controllers[MOTOR_D], Percent_speed);

            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
        }

        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
        m_updateParams = true;
//        mc_mutex.unLock();

        testSpin();

        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }










    // ----------------------------------------------------------------------
    // UNSAFE TEST functions
    // ----------------------------------------------------------------------

    MotorControlComponentImpl::MCError_t
    MotorControlComponentImpl::testSpin()
    {
        MCError_t err;

        for (int i=0; i<NUM_MOTORS; i++)
        {
            mcTestSetSpeed(&m_motor_controllers[i]);
        }

        for (int i=0; i<NUM_MOTORS; i++)
        {
            mcTestSetPos(&m_motor_controllers[i]);
        }

        for (int i=0; i<NUM_MOTORS; i++)
        {
            mcTestDrive(&m_motor_controllers[i]);
        }

        return err;
    }











    // ----------------------------------------------------------------------
    // OBSOLETE Command handler implementations
    //      To be used as Helper Functions
    // ----------------------------------------------------------------------

    /**
     * @brief      Command handler implementation to change PI values
     *
     * @param[in]  opCode          The operation code
     * @param[in]  cmdSeq          The command sequence
     * @param[in]  Motor_ID        The motor(s) ID
     * @param[in]  PI_Values       The new PI values
     */
//    void MotorControlComponentImpl ::MC_Current_PID_cmdHandler(
    void MotorControlComponentImpl ::MC_Current_PID_set(
            const FwOpcodeType opCode,
            const U32 cmdSeq,
            U8 Motor_ID,
            U32 PI_Values)
    {
        MCError_t err;
        uint16_t P_Value = (uint16_t)((PI_Values & (uint32_t)0x00ff) >> 0);
        uint16_t I_Value = (uint16_t)((PI_Values & (uint32_t)0xff00) >> 16);

        if (Motor_ID == ALL_MOTOR_ID)
        {
            err = sendAllMotorsData(MC_REG_P_CURRENT, &P_Value);
            if (err != MC_NO_ERROR)
            {
                this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
                return;
            }

            err = sendAllMotorsData(MC_REG_I_CURRENT, &I_Value);
            if (err != MC_NO_ERROR)
            {
                this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
                return;
            }
        }
        else
        {
            err = motorControlTransfer(motorIdAddressMap[Motor_ID], MC_REG_P_CURRENT, &P_Value);
            if (err != MC_NO_ERROR)
            {
                this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
                return;
            }

            err = motorControlTransfer(motorIdAddressMap[Motor_ID], MC_REG_I_CURRENT, &I_Value);
            if (err != MC_NO_ERROR)
            {
                this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
                return;
            }
        }
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

    /**
     * @brief      Command handler implementation to change PID values
     *
     * @param[in]  opCode          The operation code
     * @param[in]  cmdSeq          The command sequence
     * @param[in]  Motor_ID        The motor(s) ID
     * @param[in]  PID_Values      The new PID values
     */
//    void MotorControlComponentImpl ::MC_Speed_PID_cmdHandler(
    void MotorControlComponentImpl ::MC_Speed_PID_set(
            const FwOpcodeType opCode,
            const U32 cmdSeq,
            U8 Motor_ID,
            U32 PID_Values)
    {
        MCError_t err;
        uint16_t P_Value = (uint16_t)((PID_Values & (uint32_t)0x00ff) >> 0);
        uint16_t I_Value = (uint16_t)((PID_Values & (uint32_t)0xff00) >> 16);

        if (Motor_ID == ALL_MOTOR_ID)
        {
            err = sendAllMotorsData(MC_REG_P_SPEED, &P_Value);

            if (err != MC_NO_ERROR)
            {
                this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
                return;
            }

            err = sendAllMotorsData(MC_REG_I_SPEED, &I_Value);
            if (err != MC_NO_ERROR)
            {
                this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
                return;
            }
        }
        else
        {
            err = motorControlTransfer(motorIdAddressMap[Motor_ID], MC_REG_P_SPEED, &P_Value);
            if (err != MC_NO_ERROR)
            {
                this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
                return;
            }

            err = motorControlTransfer(motorIdAddressMap[Motor_ID], MC_REG_I_SPEED, &I_Value);
            if (err != MC_NO_ERROR)
            {
                this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
                return;
            }
        }
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
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
//    void MotorControlComponentImpl ::MC_Acceleration_cmdHandler(
    void MotorControlComponentImpl ::MC_Acceleration_set(
            const FwOpcodeType opCode,
            const U32 cmdSeq,
            U8 Motor_ID,
            U32 Rate_Values)
    {
        MCError_t err;
        uint16_t accel = (uint16_t)((Rate_Values & (uint32_t)0x00ff) >> 0);
        uint16_t decel = (uint16_t)((Rate_Values & (uint32_t)0xff00) >> 16);

        if (Motor_ID == ALL_MOTOR_ID)
        {
            err = sendAllMotorsData(MC_REG_ACC_RATE, &accel);
            if (err != MC_NO_ERROR)
            {
                this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
                return;
            }

            err = sendAllMotorsData(MC_REG_DEC_RATE, &decel);
            if (err != MC_NO_ERROR)
            {
                this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
                return;
            }
        }
        else
        {
            err = motorControlTransfer(motorIdAddressMap[Motor_ID], MC_REG_ACC_RATE, &accel);
            if (err != MC_NO_ERROR)
            {
                this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
                return;
            }

            err = motorControlTransfer(motorIdAddressMap[Motor_ID], MC_REG_DEC_RATE, &decel);
            if (err != MC_NO_ERROR)
            {
                this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
                return;
            }
        }
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

    /**
     * @brief      Command handler implementation to control stall detection
     *
     * @param[in]  opCode          The operation code
     * @param[in]  cmdSeq          The command sequence
     * @param[in]  Motor_ID        The motor(s) ID
     * @param[in]  Spin_Type       A value to enable or disable powerbost
     */
//    void MotorControlComponentImpl ::MC_StallDetection_cmdHandler(
    void MotorControlComponentImpl ::MC_StallDetection_set(
            const FwOpcodeType opCode,
            const U32 cmdSeq,
            U8 Motor_ID,
            U8 Value)
    {
        // TODO: KEEP OR DEPRACATE? What does this do
        if ((Value != 0x00 && Value != 0xFF) | Motor_ID > 4)
        {
            // Not a valid option
            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
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

        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

    /**
     * @brief      Command handler implementation to reset encoder counts
     *
     * @param[in]  opCode          The operation code
     * @param[in]  cmdSeq          The command sequence
     * @param[in]  Motor_ID        The motor(s) ID
     */
//    void MotorControlComponentImpl ::MC_ResetPosition_cmdHandler(
    void MotorControlComponentImpl ::MC_ResetPosition_set(
            const FwOpcodeType opCode,
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
            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
            return;
        }

        // If all else goes well, we succeeded
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

    /**
     * @brief      Command handler implementation to control power limits
     *
     * @param[in]  opCode          The operation code
     * @param[in]  cmdSeq          The command sequence
     * @param[in]  Motor_ID        The motor(s) ID
     * @param[in]  Value           A value to enable or disable powerbost
     */
//    void MotorControlComponentImpl ::MC_PowerBoost_cmdHandler(
    void MotorControlComponentImpl ::MC_PowerBoost_set(
            const FwOpcodeType opCode,
            const U32 cmdSeq,
            U8 Motor_ID,
            U8 Value)
    {
        // TODO
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }



















    // ----------------------------------------------------------------------
    // User-defined helper functions
    // ----------------------------------------------------------------------

    void MotorControlComponentImpl::initMotorControllers()
    {
        mc_mutex.lock();
        m_motorControllerState = DISABLED;
        for (int i = 0; i < NUM_MOTORS; i++)
        {
            initMotorController(&m_motor_controllers[i], i);
        }
        mc_mutex.unLock();
    }
//
    void MotorControlComponentImpl::checkFaults()
    {
        FaultRegister_t fault_test[NUM_MOTORS];
        MC_ERR_t err_test[NUM_MOTORS];

        mc_mutex.lock();
        for (int i = 0; i < NUM_MOTORS; i++)
        {
            err_test[i] = getMcRegFault(&m_motor_controllers[i]);
            fault_test[i] = m_motor_controllers[i].faultReg;
        }
        mc_mutex.unLock();
        return;
    }

    void MotorControlComponentImpl::checkStates()
    {
        StateRegister_t state_test[NUM_MOTORS];
        MC_ERR_t err_test[NUM_MOTORS];

        mc_mutex.lock();
        for (int i = 0; i < NUM_MOTORS; i++)
        {
            err_test[i] = getMcRegFault(&m_motor_controllers[i]);
            state_test[i] = m_motor_controllers[i].stateReg;
        }
        mc_mutex.unLock();
        return;
    }

    //    void MotorControlComponentImpl::runMcStateMachine()
    //    {
    //
    //        switch (m_motorControllerState)
    //        {
    //        case DISABLED:
    //            return;
    //        case IDLE:
    //            checkFaults();
    //            return;
    //        case ENABLED:
    //        case ARMED:
    //        case RUNNING:
    //        case TARGET_REACHED:
    //        case FAULT:
    //        default:
    //            return;
    //        }
    //    }







    // ----------------------------------------------------------------------
    // OBSOLETE User-defined helper functions
    // ----------------------------------------------------------------------

    uint32_t MotorControlComponentImpl::regSizeMap(MC_ICD_RegAddr reg)
    {
        switch (reg)
        {
            case MC_REG_I2C_ADDRESS:
            case MC_REG_TARGET_SPEED:
            case MC_REG_MC_CTRL:
            case MC_REG_MC_FAULT:
            case MC_REG_MC_STATUS:
                return 1;
            case MC_REG_P_CURRENT:
            case MC_REG_I_CURRENT:
            case MC_REG_P_SPEED:
            case MC_REG_I_SPEED:
            case MC_REG_ACC_RATE:
            case MC_REG_DEC_RATE:
                return 2;
            case MC_REG_TARGET_POSITION:
            case MC_REG_CURRENT_POSITION:
            case MC_REG_MOTOR_CURRENT: // TODO: CHeck if this retuns 2 byts or 4!? 2 from thismethod 4 from micheal's update current tlm code
                return 4;
            default:
                return 0;
        }
    }



    MotorControlComponentImpl::MCError_t
    MotorControlComponentImpl::sendAllMotorsData(const MC_ICD_RegAddr reg, void *_data)
    {
        uint8_t *data = static_cast<uint8_t *>(_data);

        for (int i = 0; i < NUM_MOTORS; ++i)
        {
            MCError_t err = motorControlTransfer(motorIdAddressMap[i], reg, data);
            if (err != MC_NO_ERROR)
                return err;
        }
        // TODO: What if one latched up? Should we check status here and issue STOP?

        return MC_NO_ERROR;
    }

    bool MotorControlComponentImpl::checkMotorsStatus()
    {
        MCError_t err;
        for (int i = 0; i < NUM_MOTORS; ++i)
        {
            err = motorControlTransfer(motorIdAddressMap[i], MC_REG_MC_STATUS, &m_currStatus[i].value);
            if (err != MC_NO_ERROR)
            {
                // I2C Communication Error
                // TODO: Replace this function with whatever the new working one that gets made is. Reference NetworkManager.cpp for how to do this
                watchdogResetRequest_out(0, CubeRoverPorts::motorsReset);
                // TODO: Reset our I2C too
                return false;
            }
            else if (m_currStatus[i].bits.controller_error)
            {
                // TODO: Send STOP general call
                // TODO: Need to check mappping between resetting one motor and which one is connected to watchdog
                // TODO: Check status again after reset
                // TODO: Replace this function with whatever the new working one that gets made is. Reference NetworkManager.cpp for how to do this
                watchdogResetRequest_out(0, CubeRoverPorts::motorsReset);
                return false;
                // XXX: Do we need to update our tlm counter?
            }
            else if (!m_currStatus[i].bits.position_converged)
            {
                // TODO: Do we... wait?
                return false;
            }
        }
        return true;
    }

    bool MotorControlComponentImpl::startMotorMovement()
    {
        MCError_t err;
        char motorStartValue = START_MOTORS;
        for (int i = 0; i < NUM_MOTORS; ++i)
        {
            err = motorControlTransfer(motorIdAddressMap[i], MC_REG_MC_CTRL, &motorStartValue);
            if (err != MC_NO_ERROR)
            {
                // I2C Communication Error
                // watchdogResetRequest_out(0, CubeRoverPorts::motorsReset);
                // TODO: Reset our I2C too
                return false;
            }
        }
        return true;
    }

    /**
     * @brief
     *
     *
     *
     * @param[in]  Distance       The distance to travel in motor ticks
     * @param[in]  Speed          The speed to travel in normalized speed
     */
    MotorControlComponentImpl::MCError_t
    MotorControlComponentImpl::moveAllMotorsStraight(int32_t distance, int16_t speed)
    {
        MCError_t err;

        checkMotorsStatus(); // TODO: se the return value here ...

        // Enforce speed always positive. Direction set by distance
        if (speed > 0)
        {
//          uint8_t motor_speed = groundSpeedToSpeedPrecent(speed);

            // Send the speed to all the motors
            // Required to send this before the setpoint (or else the MC will start spinning before speed was set)
            err = sendAllMotorsData(MC_REG_TARGET_SPEED, &speed);
            if (err != MC_NO_ERROR)
                return err;
        }
        else
        {
            return MC_BAD_COMMAND_INPUT;
        }

        int32_t Right_Wheels_Relative_ticks, Left_Wheels_Relative_ticks, Relative_ticks;
        Relative_ticks = distance;
        // Ensure the sides are traveling the right direction
        if (m_forward_is_positive)
        {
            Right_Wheels_Relative_ticks = Relative_ticks;
            Left_Wheels_Relative_ticks = -1 * Relative_ticks;
        }
        else
        {
            Right_Wheels_Relative_ticks = -1 * Relative_ticks;
            Left_Wheels_Relative_ticks = Relative_ticks;
        }

        taskENTER_CRITICAL();
        err = motorControlTransfer(FRONT_LEFT_MC_I2C_ADDR, MC_REG_TARGET_POSITION, &Left_Wheels_Relative_ticks);
        if (err != MC_NO_ERROR)
        {
            taskEXIT_CRITICAL();
            return err;
        }

        err = motorControlTransfer(FRONT_RIGHT_MC_I2C_ADDR, MC_REG_TARGET_POSITION, &Right_Wheels_Relative_ticks);
        if (err != MC_NO_ERROR)
        {
            taskEXIT_CRITICAL();
            return err;
        }

        err = motorControlTransfer(REAR_RIGHT_MC_I2C_ADDR, MC_REG_TARGET_POSITION, &Right_Wheels_Relative_ticks);
        if (err != MC_NO_ERROR)
        {
            taskEXIT_CRITICAL();
            return err;
        }

        err = motorControlTransfer(REAR_LEFT_MC_I2C_ADDR, MC_REG_TARGET_POSITION, &Left_Wheels_Relative_ticks);
        if (err != MC_NO_ERROR)
        {
            taskEXIT_CRITICAL();
            return err;
        }

        taskEXIT_CRITICAL();
        if (!startMotorMovement())
        {
            return MC_UNEXPECTED_ERROR;
        }

        return err;
    }

    /**
     * @brief      Helper function to rotate all motors simultaneously
     *
     * @param[in]  Distance       The distance to travel in motor ticks
     * @param[in]  Speed          The speed to travel in normalized speed
     */
    MotorControlComponentImpl::MCError_t
    MotorControlComponentImpl::rotateAllMotors(int16_t distance, int16_t speed)
    {
        MCError_t err;

        checkMotorsStatus();

        // Enforce speed always positive. Direction set by distance
        if (speed > 0)
        {
//            uint8_t motor_speed = m_angularToLinear * groundSpeedToSpeedPrecent(speed);

            // Send the speed to all the motors
            // Required to send this before the setpoint (or else the MC will start spinning before speed was set)
            err = sendAllMotorsData(MC_REG_TARGET_SPEED, &speed);
            if (err != MC_NO_ERROR)
                return err;
        }
        else
        {
            return err;
        }

        int32_t Relative_ticks = m_angularToLinear * distance;

        taskENTER_CRITICAL();
        StatusRegister_t status;
        err = motorControlTransfer(FRONT_LEFT_MC_I2C_ADDR, MC_REG_MC_STATUS, &status.value);
        err = motorControlTransfer(FRONT_RIGHT_MC_I2C_ADDR, MC_REG_MC_STATUS, &status.value);
        err = motorControlTransfer(REAR_LEFT_MC_I2C_ADDR, MC_REG_MC_STATUS, &status.value);
        err = motorControlTransfer(REAR_RIGHT_MC_I2C_ADDR, MC_REG_MC_STATUS, &status.value);

        err = motorControlTransfer(FRONT_LEFT_MC_I2C_ADDR, MC_REG_TARGET_POSITION, &Relative_ticks);
        if (err != MC_NO_ERROR)
        {
            taskEXIT_CRITICAL();
            return err;
        }

        err = motorControlTransfer(FRONT_RIGHT_MC_I2C_ADDR, MC_REG_TARGET_POSITION, &Relative_ticks);
        if (err != MC_NO_ERROR)
        {
            taskEXIT_CRITICAL();
            return err;
        }

        err = motorControlTransfer(REAR_RIGHT_MC_I2C_ADDR, MC_REG_TARGET_POSITION, &Relative_ticks);
        if (err != MC_NO_ERROR)
        {
            taskEXIT_CRITICAL();
            return err;
        }

        err = motorControlTransfer(REAR_LEFT_MC_I2C_ADDR, MC_REG_TARGET_POSITION, &Relative_ticks);
        taskEXIT_CRITICAL();

        return err;
    }

    MotorControlComponentImpl::MCError_t
    MotorControlComponentImpl::motorControlTransfer(I2cSlaveAddress_t addr,
                                                    MC_ICD_RegAddr reg,
                                                    void *_data)
    {
        uint8_t *data = static_cast<uint8_t *>(_data);
        uint32_t dataLength = regSizeMap(reg);

        if (dataLength <= 0)
            return MC_UNEXPECTED_ERROR;

        uint8_t reg_buffer = static_cast<uint8_t>(reg);

        if (reg == MC_REG_I2C_ADDRESS ||
            reg == MC_REG_CURRENT_POSITION ||
            reg == MC_REG_CURRENT_POSITION ||
            reg == MC_REG_MOTOR_CURRENT ||
            reg == MC_REG_MC_STATUS ||
            reg == MC_REG_MC_FAULT)
        {
            taskENTER_CRITICAL();
            if (i2cMasterReadData(m_i2c, addr, reg_buffer, dataLength, data))
            {
                taskEXIT_CRITICAL();
                return MC_NO_ERROR;
            }
            else
            {
                taskEXIT_CRITICAL();
                return MC_I2C_TIMEOUT_ERROR;
            }
        }
        else
        {
            taskENTER_CRITICAL();
            if (i2cMasterTransmit(m_i2c, addr, reg_buffer, dataLength, data))
            {
                taskEXIT_CRITICAL();
                return MC_NO_ERROR;
            }
            else
            {
                taskEXIT_CRITICAL();
                return MC_I2C_TIMEOUT_ERROR;
            }
        }
    }

    bool MotorControlComponentImpl::updateTelemetry()
    {
        MCError_t err = MC_NO_ERROR;
        uint32_t buffer[NUM_MOTORS];

        for (int i = 0; i < NUM_MOTORS; ++i)
        {
            err = motorControlTransfer(motorIdAddressMap[i], MC_REG_MOTOR_CURRENT, &buffer[i]);
            if (err != MC_NO_ERROR)
            {
                // resetMotorControllers();
                // TODO: THIS LOG REALLY SHOULD INDICATE WHICH FAILED??
                log_WARNING_HI_MC_MSPNotResponding();
            }
        }

        tlmWrite_MC_FL_Current((uint32_t)buffer[0]);
        tlmWrite_MC_FR_Current((uint32_t)buffer[1]);
        tlmWrite_MC_RR_Current((uint32_t)buffer[2]);
        tlmWrite_MC_RL_Current((uint32_t)buffer[3]);

        if (err != MC_NO_ERROR)
            asm(" nop");
        // resetMotorControllers();

        for (int i = 0; i < NUM_MOTORS; ++i)
        {
            err = motorControlTransfer(motorIdAddressMap[i], MC_REG_CURRENT_POSITION, &buffer[i]);
            if (err != MC_NO_ERROR)
            {
                // resetMotorControllers();
                // TODO: THIS LOG REALLY SHOULD INDICATE WHICH FAILED??
                log_WARNING_HI_MC_MSPNotResponding();
            }
        }

        // If we got all the values we need, then we can update telemetry
        m_FL_Encoder_Count += (uint16_t)buffer[0];
        m_FR_Encoder_Count += (uint16_t)buffer[1];
        m_RR_Encoder_Count += (uint16_t)buffer[2];
        m_RL_Encoder_Count += (uint16_t)buffer[3];
        tlmWrite_MC_FL_Encoder_Ticks(m_FL_Encoder_Count + m_FR_Encoder_Count_Offset);
        tlmWrite_MC_FR_Encoder_Ticks(m_FR_Encoder_Count + m_FL_Encoder_Count_Offset);
        tlmWrite_MC_RR_Encoder_Ticks(m_RR_Encoder_Count + m_RL_Encoder_Count_Offset);
        tlmWrite_MC_RL_Encoder_Ticks(m_RL_Encoder_Count + m_RR_Encoder_Count_Offset);

        if (err != MC_NO_ERROR)
            asm(" nop");
        // resetMotorControllers();

        return true;
    }

    bool MotorControlComponentImpl::pollStatus()
    {
        StatusRegister_t status;
        status.value = 0xff;
        int loop_count = 10;
        do
        {
            unsigned delay = 55000000;
            while (delay) // Delay 0.5s to give the motors a chance to converge. 0.5 / (1/ 110e6)
                delay--;

            uint8_t reg = MC_REG_MC_STATUS;
            for (int i = 0; i < 4; ++i)
            {
                StatusRegister_t this_status;
                i2cMasterReadData(m_i2c, FRONT_LEFT_MC_I2C_ADDR + i, reg, 1, &this_status.value);
                status.value &= this_status.value;
            }
            loop_count--;
            if (loop_count <= 0)
                return false;
        } while (!(status.bits.position_converged));

        return true;
    }

} // end namespace CubeRover
