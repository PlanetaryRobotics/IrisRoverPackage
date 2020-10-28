// ======================================================================
// \title  NetworkManagerComponentImpl.hpp
// \author justin
// \brief  hpp file for NetworkManager component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef NetworkManager_HPP
#define NetworkManager_HPP

#include "CubeRover/NetworkManager/NetworkManagerComponentAc.hpp"
#include "CubeRover/NetworkManager/CubeRoverNetworkManager.hpp"

namespace CubeRover {

  class NetworkManagerComponentImpl :
    public NetworkManagerComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object NetworkManager
      //!
      NetworkManagerComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object NetworkManager
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object NetworkManager
      //!
      ~NetworkManagerComponentImpl(void);

    PRIVATE:

      CubeRoverNetworkManager::CubeRoverNetworkManager m_crnm;
      CubeRoverNetworkManager::CubeRoverNetworkStateMachine m_current_state;
      U8 m_fileUplinkBuffer[MAX_SIZE_PAYLOAD];    // This is not thread-safe. Use and discard immed

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for fileUplinkBufferGet
      //!
      Fw::Buffer fileUplinkBufferGet_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 size 
      );

      //! Handler implementation for fileDownlinkBufferSendIn
      //!
      void fileDownlinkBufferSendIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      void update();    // Behavior of periodic status update

      //! Handler implementation for schedIn
      //!
      void schedIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );


    };

} // end namespace CubeRover

#endif
