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

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for fileDownlinkBufferSendIn
      //!
      void downlink_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Handler implementation for schedIn
      //!
      void schedIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );
      
      
        // User defined methods, members, and structs
       
        CubeRoverNetworkManager::CubeRoverNetworkManager m_crnm;
        CubeRoverNetworkManager::CubeRoverNetworkStateMachine m_current_state;
        U8 m_fileUplinkBuffer[UDP_MAX_PAYLOAD];
        void update();    // Behavior of periodic status update
        void getUplinkDatagram();

    };

} // end namespace CubeRover

#endif
