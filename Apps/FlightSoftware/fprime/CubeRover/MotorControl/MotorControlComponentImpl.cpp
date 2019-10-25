// ======================================================================
// \title  MotorControlComponentImpl.cpp
// \author cedric
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
#include "gio.h"

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
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    MotorControlComponentBase::init(queueDepth, instance);
  }

  MotorControlComponentImpl ::
    ~MotorControlComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void MotorControlComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void MotorControlComponentImpl ::
    MC_LedCtrl_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        controlState control
    )
  {
    switch(control){
      case TURN_ON:
        gioSetBit(gioPORTB, 1, 1);
        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
      break;
      case TURN_OFF:
        gioSetBit(gioPORTB, 1, 0);
        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
      break;
      default:
       this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_VALIDATION_ERROR);
       return;
    }
  }

} // end namespace CubeRover
