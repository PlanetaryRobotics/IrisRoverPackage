// ====================================================================== 
// \title  PullCmdImpl.cpp
// \author sofiahurtado
// \brief  cpp file for PullCmd component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 


#include <CubeRoverGround/PullCmd/PullCmdComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRoverGround {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  PullCmdComponentImpl ::
#if FW_OBJECT_NAMES == 1
    PullCmdComponentImpl(
        const char *const compName
    ) :
      PullCmdComponentBase(compName)
#else
    PullCmdImpl(void)
#endif
  {

  }

  void PullCmdComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    ) 
  {
    PullCmdComponentBase::init(queueDepth, instance);
  }

  PullCmdComponentImpl ::
    ~PullCmdComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void PullCmdComponentImpl ::
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

  void PullCmdComponentImpl ::
    SignalGen_Settings_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U32 Frequency,
        U32 Amplitude,
        U32 Phase
    )
  {
    // TODO
  }

  void PullCmdComponentImpl ::
    SignalGen_Toggle_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
  }

  void PullCmdComponentImpl ::
    SignalGen_Skip_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
  }

} // end namespace CubeRoverGround
