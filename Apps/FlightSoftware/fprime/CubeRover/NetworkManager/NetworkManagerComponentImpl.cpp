// ======================================================================
// \title  NetworkManagerComponentImpl.cpp
// \author cedric
// \brief  cpp file for NetworkManager component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "NetworkManager.hpp"
#include <CubeRover/NetworkManager/NetworkManagerComponentImpl.hpp>
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
    run_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    if(m_networkManager.UpdateNetworkManager() != NO_ERROR){

    }
  }

  void NetworkManagerComponentImpl ::
    readPoll_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    // TODO
  }

  void NetworkManagerComponentImpl ::
    write_handler(
        const NATIVE_INT_TYPE portNum,
        U32 size
    )
  {
    // TODO
  }

} // end namespace CubeRover
