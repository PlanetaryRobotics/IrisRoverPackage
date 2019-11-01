// ======================================================================
// \title  FreeRtosSerialDriverComponentImpl.cpp
// \author cedric
// \brief  cpp file for FreeRtosSerialDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Drv/FreeRtosSerialDriver/FreeRtosSerialDriverComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  FreeRtosSerialDriverComponentImpl ::
#if FW_OBJECT_NAMES == 1
    FreeRtosSerialDriverComponentImpl(
        const char *const compName
    ) :
      FreeRtosSerialDriverComponentBase(compName)
#else
    FreeRtosSerialDriverComponentImpl(void)
#endif
  {

  }

  void FreeRtosSerialDriverComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    FreeRtosSerialDriverComponentBase::init(instance);
  }

  FreeRtosSerialDriverComponentImpl ::
    ~FreeRtosSerialDriverComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void FreeRtosSerialDriverComponentImpl ::
    readBufferSend_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    // TODO
  }

  void FreeRtosSerialDriverComponentImpl ::
    serialSend_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &serBuffer
    )
  {
    // TODO
  }

} // end namespace Drv
