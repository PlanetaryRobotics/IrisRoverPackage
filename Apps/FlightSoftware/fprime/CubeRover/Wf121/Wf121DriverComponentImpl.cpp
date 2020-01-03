// ======================================================================
// \title  Wf121DriverComponentImpl.cpp
// \author cedric
// \brief  cpp file for Wf121Driver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/Wf121/Wf121DriverComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  Wf121DriverComponentImpl ::
#if FW_OBJECT_NAMES == 1
    Wf121DriverComponentImpl(
        const char *const compName
    ) :
      Wf121DriverComponentBase(compName)
#else
    Wf121DriverComponentImpl(void)
#endif
  {

  }

  void Wf121DriverComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    Wf121DriverComponentBase::init(instance);
  }

  Wf121DriverComponentImpl ::
    ~Wf121DriverComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void Wf121DriverComponentImpl ::
    Wf121ReadWrite_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &writeBuffer,
        Fw::Buffer &readBuffer
    )
  {
    // TODO
  }

} // end namespace CubeRover
