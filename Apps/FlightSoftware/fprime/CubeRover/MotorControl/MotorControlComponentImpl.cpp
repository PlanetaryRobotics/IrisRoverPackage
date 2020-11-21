// ======================================================================
// \title  MotorControlComponentImpl.cpp
// \author michael
// \brief  cpp file for MotorControl component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/MotorControl/MotorControlComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

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

  }

  void MotorControlComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    MotorControlComponentBase::init(instance);
  }

  MotorControlComponentImpl ::
    ~MotorControlComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void MotorControlComponentImpl ::
    PingIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    // TODO
  }

  void MotorControlComponentImpl ::
    motorDataOut_handler(
        const NATIVE_INT_TYPE portNum,
        bool movement_in_progress,
        I32 encoder_dist_motor_0,
        I32 encoder_dist_motor_1,
        I32 encoder_dist_motor_2,
        I32 encoder_dist_motor_3
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void MotorControlComponentImpl ::
    MC_DrivingConfiguration_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        CommandList CommandConfiguration,
        ParameterList Parameter,
        U8 Value
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void MotorControlComponentImpl ::
    MC_ExecuteDriving_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        DriveCommandList DrivingCommand
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void MotorControlComponentImpl ::
    MC_TuningParameters_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        tuningParameterList TuningParameter,
        U16 Value
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void MotorControlComponentImpl ::
    MC_AccelerationProfiles_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        accelerationParameterList AccelerationParameter,
        U16 Value
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void MotorControlComponentImpl ::
    MC_StallDetection_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        motorStallEnableList motorStallEnable
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void MotorControlComponentImpl ::
    MC_PositionCounterReset_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U8 ResetPositionCounter
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void MotorControlComponentImpl ::
    MC_UpdateTelemetry_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace CubeRover
