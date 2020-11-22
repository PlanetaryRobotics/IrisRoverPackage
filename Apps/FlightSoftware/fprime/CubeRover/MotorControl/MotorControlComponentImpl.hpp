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

      //! Implementation for MC_DrvingCommand command handler
      //! 
      void MC_DrvingCommand_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U8 Motor_ID, /*!< The motor id from 0 - 3 (Motor 0 is Front Left motor and goes counter clockwise) */
          CommandList Command /*!< The motor id from 0 - 3 (Motor 0 is Front Left motor and goes counter clockwise) */
      );


    };

} // end namespace CubeRover

#endif
