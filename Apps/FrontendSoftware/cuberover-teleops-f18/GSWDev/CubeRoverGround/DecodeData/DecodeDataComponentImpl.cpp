// ====================================================================== 
// \title  DecodeDataImpl.cpp
// \author raewynd
// \brief  cpp file for DecodeData component implementation class
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


#include <CubeRoverGround/DecodeData/DecodeDataComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRoverGround {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  DecodeDataComponentImpl ::
#if FW_OBJECT_NAMES == 1
    DecodeDataComponentImpl(
        const char *const compName
    ) :
      DecodeDataComponentBase(compName)
#else
    DecodeDataImpl(void)
#endif
  {

  }

  void DecodeDataComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    ) 
  {
    DecodeDataComponentBase::init(queueDepth, instance);
  }

  DecodeDataComponentImpl ::
    ~DecodeDataComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void DecodeDataComponentImpl ::
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

  void DecodeDataComponentImpl ::
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

  void DecodeDataComponentImpl ::
    SignalGen_Toggle_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
  }

  void DecodeDataComponentImpl ::
    SignalGen_Skip_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
  }

} // end namespace CubeRoverGround
