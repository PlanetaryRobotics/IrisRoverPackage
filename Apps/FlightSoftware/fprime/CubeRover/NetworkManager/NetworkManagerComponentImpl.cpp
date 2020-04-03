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
    readData_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
      uint16_t bytesRead = 0;
      const uint16_t headerSize = 10;
      //m_networkManager.ReceiveUdpData(fwBuffer, headerSize, &bytesRead, UdpReadMode::WAIT_UNTIL_READY | UdpReadMode::PEEK_READ, 10);
    // TODO
  }

  void NetworkManagerComponentImpl ::
    writeData_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
      U8 * testData;
      NATIVE_UINT_TYPE size = 10;
      Fw::SerializeStatus stat = fwBuffer.deserialize(testData, &size);
      FW_ASSERT(stat == Fw::FW_SERIALIZE_OK,static_cast<NATIVE_INT_TYPE>(stat));

      //m_networkManager.SendUdpData(testData, 24, 10000);
  }

} // end namespace CubeRover
