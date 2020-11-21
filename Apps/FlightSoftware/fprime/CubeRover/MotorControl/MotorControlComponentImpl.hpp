// ======================================================================
// \title  MotorControlComponentImpl.hpp
// \author michael
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
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object MotorControl
      //!
      ~MotorControlComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for PingIn
      //!
      void PingIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

      //! Handler implementation for motorDataOut
      //!
      void motorDataOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          bool movement_in_progress, /*!<  A simple boolean to show if the MoC is driving the motors */
          I32 encoder_dist_motor_0, /*!<  In units of encoder counts */
          I32 encoder_dist_motor_1, /*!<  In units of encoder counts */
          I32 encoder_dist_motor_2, /*!<  In units of encoder counts */
          I32 encoder_dist_motor_3 /*!<  In units of encoder counts */
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for MC_DrivingConfiguration command handler
      //! Configure movement command
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

      //! Implementation for MC_UpdateTelemetry command handler
      //! Requests an update from each of the motor controllers
      void MC_UpdateTelemetry_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );


    };

} // end namespace CubeRover

#endif
