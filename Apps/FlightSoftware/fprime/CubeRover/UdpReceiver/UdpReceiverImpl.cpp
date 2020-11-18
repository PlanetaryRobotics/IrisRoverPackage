// ======================================================================
// \title  UdpReceiverComponentImpl.cpp
// \author justin
// \brief  cpp file for UdpReceiver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/UdpReceiver/UdpReceiverComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  UdpReceiverComponentImpl ::
#if FW_OBJECT_NAMES == 1
    UdpReceiverComponentImpl(
        const char *const compName
    ) :
      UdpReceiverComponentBase(compName)
#else
    UdpReceiverComponentImpl(void)
#endif
  {

  }

  void UdpReceiverComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    UdpReceiverComponentBase::init(instance);
  }

  UdpReceiverComponentImpl ::
    ~UdpReceiverComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void UdpReceiverComponentImpl ::
    uplinkData_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    // TODO
  }

} // end namespace CubeRover
