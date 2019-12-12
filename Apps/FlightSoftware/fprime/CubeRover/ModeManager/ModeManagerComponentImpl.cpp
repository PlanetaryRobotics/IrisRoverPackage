// ======================================================================
// \title  ModeManagerComponentImpl.cpp
// \author raewynduvall
// \brief  cpp file for ModeManager component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/ModeManager/ModeManagerComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  ModeManagerComponentImpl ::
#if FW_OBJECT_NAMES == 1
    ModeManagerComponentImpl(
        const char *const compName
    ) :
      ModeManagerComponentBase(compName)
#else
    ModeManagerComponentImpl(void)
#endif
  {

  }

  void ModeManagerComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    ModeManagerComponentBase::init(queueDepth, instance);
  }

  ModeManagerComponentImpl ::
    ~ModeManagerComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void ModeManagerComponentImpl ::
    FatalReceive_handler(
        const NATIVE_INT_TYPE portNum,
        FwEventIdType Id
    )
  {
    // TODO
  }

  void ModeManagerComponentImpl ::
    pingIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    // TODO
  }

  void ModeManagerComponentImpl ::
    controlMode_handler(
        const NATIVE_INT_TYPE portNum,
        OpMode mode
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void ModeManagerComponentImpl ::
    GOTO_MODE_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        OpModeCmd mode_cmd
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace CubeRover
