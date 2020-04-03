// ======================================================================
// \title  NetworkManagerComponentImpl.hpp
// \author cedric
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
#include "CubeRover/NetworkManager/NetworkManager.hpp"

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
      CubeRoverNetworkManager m_networkManager;

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for run
      //!
      void run_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

      //! Handler implementation for readPoll
      //!
      void readData_handler(
            const NATIVE_INT_TYPE portNum,
            Fw::Buffer &fwBuffer
        );

      //! Handler implementation for writeData
      //!
      void writeData_handler(
            const NATIVE_INT_TYPE portNum,
            Fw::Buffer &fwBuffer
        );
    };

} // end namespace CubeRover

#endif
