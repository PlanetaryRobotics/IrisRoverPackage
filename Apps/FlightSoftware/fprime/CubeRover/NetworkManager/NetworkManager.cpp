// ======================================================================
// \title  NetworkManagerComponentImpl.cpp
// \author justin
// \brief  cpp file for NetworkManager component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/NetworkManager/NetworkManager.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  NetworkManagerComponentImpl ::
#if FW_OBJECT_NAMES == 1
    NetworkManagerComponentImpl(
        const char *const compName
    ) :
      NetworkManagerComponentBase(compName)
#else
    NetworkManagerComponentImpl(void)
#endif
  {

  }

  void NetworkManagerComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    NetworkManagerComponentBase::init(instance);
  }

  NetworkManagerComponentImpl ::
    ~NetworkManagerComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void NetworkManagerComponentImpl ::
    fileUplinkBufferGet_handler(
        const NATIVE_INT_TYPE portNum,
        U32 size
    )
  {
    // TODO
  }

  void NetworkManagerComponentImpl ::
    fileDownlinkBufferSendIn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    // TODO
  }

  void NetworkManagerComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // TODO
  }

} // end namespace CubeRover
