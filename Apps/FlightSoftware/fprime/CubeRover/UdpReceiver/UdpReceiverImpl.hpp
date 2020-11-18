// ======================================================================
// \title  UdpReceiverComponentImpl.hpp
// \author justin
// \brief  hpp file for UdpReceiver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef UdpReceiver_HPP
#define UdpReceiver_HPP

#include "CubeRover/UdpReceiver/UdpReceiverComponentAc.hpp"

namespace CubeRover {

  class UdpReceiverComponentImpl :
    public UdpReceiverComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object UdpReceiver
      //!
      UdpReceiverComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object UdpReceiver
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object UdpReceiver
      //!
      ~UdpReceiverComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for uplinkData
      //!
      void uplinkData_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );


    };

} // end namespace CubeRover

#endif
