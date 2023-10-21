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

        for (int i =0; i < NUM_MOTORS; i++) {
            m_motor_controllers[i] = MotorControllerStruct();
            m_motor_controllers[i].msp430_McRegStruct = MC_ICD_RegStruct();
            m_motor_controllers[i].herc_McRegStruct = MC_ICD_RegStruct();
        }
        // m_motor_controllers[0] = MotorControllerStruct();
        // m_motor_controllers[0].msp430_McRegStruct = MC_ICD_RegStruct();
        // m_motor_controllers[0].herc_McRegStruct = MC_ICD_RegStruct();
        // m_motor_controllers[1] = MotorControllerStruct();
        // m_motor_controllers[1].msp430_McRegStruct = MC_ICD_RegStruct();
        // m_motor_controllers[1].herc_McRegStruct = MC_ICD_RegStruct();
        // m_motor_controllers[2] = MotorControllerStruct();
        // m_motor_controllers[2].msp430_McRegStruct = MC_ICD_RegStruct();
        // m_motor_controllers[2].herc_McRegStruct = MC_ICD_RegStruct();
        // m_motor_controllers[3] = MotorControllerStruct();
        // m_motor_controllers[3].msp430_McRegStruct = MC_ICD_RegStruct();
        // m_motor_controllers[3].herc_McRegStruct = MC_ICD_RegStruct();
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
            U16 MotorD_Decel_Rate)
    {
        mc_mutex.lock();

        MotorControllerStruct *mc = &m_motor_controllers[MOTOR_A];
        mc->target_pos = MotorA_TargetPosition;
        mc->target_dir = MotorA_TargetDir;
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
        mc->target_dir = MotorB_TargetDir;
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
        mc->target_dir = MotorC_TargetDir;
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
        mc->target_dir = MotorD_TargetDir;
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
            U8 Dir,
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
            U8 Dir,
            U32 Raw_Ticks,
            U8 Percent_speed)
    {
        MCError_t err;
        int32_t distance = (int32_t) Raw_Ticks;

//        mc_mutex.lock();
        if (containsMotorID(Motor_ID, MOTOR_A)) {
//            if (containsMotorID(Dir, MOTOR_A)) {
//                distance = -1 * Raw_Ticks;
//            }
            setTargetPos(&m_motor_controllers[MOTOR_A], Raw_Ticks);
            setTargetSpeed(&m_motor_controllers[MOTOR_A], Percent_speed);

            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
        }
        if (containsMotorID(Motor_ID, MOTOR_B)) {
//            if (containsMotorID(Dir, MOTOR_B)) {
//                distance = -1 * Raw_Ticks;
//            }
            setTargetPos(&m_motor_controllers[MOTOR_B], Raw_Ticks);
            setTargetSpeed(&m_motor_controllers[MOTOR_B], Percent_speed);

            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
        }
        if (containsMotorID(Motor_ID, MOTOR_C)) {
//            if (containsMotorID(Dir, MOTOR_C)) {
//                distance = -1 * Raw_Ticks;
//            }
            setTargetPos(&m_motor_controllers[MOTOR_C], Raw_Ticks);
            setTargetSpeed(&m_motor_controllers[MOTOR_C], Percent_speed);

            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
        }
        if (containsMotorID(Motor_ID, MOTOR_D)) {
//            if (containsMotorID(Dir, MOTOR_D)) {
//                distance = -1 * Raw_Ticks;
//            }
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

    void MotorControlComponentImpl::checkFaults()
    {
//        FaultRegister_t fault_test[NUM_MOTORS];
//        MC_ERR_t err_test[NUM_MOTORS];
//
//        mc_mutex.lock();
//        for (int i = 0; i < NUM_MOTORS; i++)
//        {
//            err_test[i] = getMcRegFault(&m_motor_controllers[i]);
//            fault_test[i] = m_motor_controllers[i].faultReg;
//        }
//        mc_mutex.unLock();
        return;
    }

    void MotorControlComponentImpl::checkStates()
    {
//        StateRegister_t state_test[NUM_MOTORS];
//        MC_ERR_t err_test[NUM_MOTORS];
//
//        mc_mutex.lock();
//        for (int i = 0; i < NUM_MOTORS; i++)
//        {
//            err_test[i] = getMcRegFault(&m_motor_controllers[i]);
//            state_test[i] = m_motor_controllers[i].stateReg;
//        }
//        mc_mutex.unLock();
        return;
    }







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
        for (int i = 0; i < NUM_MOTORS; ++i)
        {
            MCError_t err = motorControlTransfer(motorIdAddressMap[i], reg, _data);
            if (err != MC_NO_ERROR)
                return err;
        }
        // TODO: What if one latched up? Should we check status here and issue STOP?

        return MC_NO_ERROR;
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

} // end namespace CubeRover
