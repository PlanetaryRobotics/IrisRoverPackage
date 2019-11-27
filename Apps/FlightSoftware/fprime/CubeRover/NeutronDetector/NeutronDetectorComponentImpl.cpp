// ======================================================================
// \title  NeutronDetectorComponentImpl.cpp
// \author cedric
// \brief  cpp file for NeutronDetector component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/NeutronDetector/NeutronDetectorComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  NeutronDetectorComponentImpl ::
#if FW_OBJECT_NAMES == 1
    NeutronDetectorComponentImpl(
        const char *const compName
    ) :
      NeutronDetectorComponentBase(compName)
#else
    NeutronDetectorComponentImpl(void)
#endif
  {

  }

  void NeutronDetectorComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    NeutronDetectorComponentBase::init(queueDepth, instance);
  }

  NeutronDetectorComponentImpl ::
    ~NeutronDetectorComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void NeutronDetectorComponentImpl ::
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

  void NeutronDetectorComponentImpl ::
    ND_CollectData_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace CubeRover
