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
        const NATIVE_INT_TYPE instance
    )
  {
    NavigationComponentBase::init(instance);
  }

  NavigationComponentImpl ::
    ~NavigationComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void NavigationComponentImpl ::
    run_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // TODO
  }

  void NavigationComponentImpl ::
    cmdIn_handler(
        const NATIVE_INT_TYPE portNum,
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CmdArgBuffer &args
    )
  {
    // TODO
  }

} // end namespace CubeRover
