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

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for MC_MotorConfiguration command handler
      //! 
      void MC_MotorConfiguration_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U8 Motor_ID, /*!< The motor id from 0 - 4 (Motor 0 is Front Left motor and goes counter clockwise) 4 is all motors */
          MP_CommandList MotorParameter, 
          U32 Value 
      );

      //! Implementation for MC_DrivingConfiguration command handler
      //! Configure movement command
      void MC_DrivingConfiguration_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          CC_CommandList CommandConfiguration, 
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

      //! Implementation for MC_StallDetection command handler
      //! Enable / disable detection of motor stall
      void MC_StallDetection_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          motorStallEnableList motorStallEnable 
      );

      //! Implementation for MC_PositionCounterReset command handler
      //! Reset the position counter of specific motor(s)
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
