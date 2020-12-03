// ======================================================================
// \title  NavigationComponentImpl.cpp
// \author michael
// \brief  cpp file for Navigation component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/Navigation/NavigationComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  NavigationComponentImpl ::
#if FW_OBJECT_NAMES == 1
    NavigationComponentImpl(
        const char *const compName
    ) :
      NavigationComponentBase(compName)
#else
    NavigationComponentImpl(void)
#endif
  {

  }

  void NavigationComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    NavigationComponentBase::init(queueDepth, instance);
  }

  NavigationComponentImpl ::
    ~NavigationComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void NavigationComponentImpl ::
    PingIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    // TODO
  }

  void NavigationComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // TODO
  }

  void NavigationComponentImpl ::
    MotorDataOut_handler(
        const NATIVE_INT_TYPE portNum,
        bool movement_finished
    )
  {
    // TODO
  }

  void NavigationComponentImpl ::
    IMUDataOut_handler(
        const NATIVE_INT_TYPE portNum,
        I16 x_acc,
        I16 y_acc,
        I16 z_acc,
        I16 x_gyro,
        I16 y_gyro,
        I16 z_gyro
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void NavigationComponentImpl ::
    Nav_DriveCommand_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        NAV_COMMANDLIST Command,
        U8 Distance,
        U8 Speed
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace CubeRover
