// ======================================================================
// \title  FreeRtosSerialDriverComponentImpl.hpp
// \author cedric
// \brief  hpp file for FreeRtosSerialDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FreeRtosSerialDriver_HPP
#define FreeRtosSerialDriver_HPP

#include "Drv/FreeRtosSerialDriver/FreeRtosSerialDriverComponentAc.hpp"

namespace Drv {

  class FreeRtosSerialDriverComponentImpl :
    public FreeRtosSerialDriverComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object FreeRtosSerialDriver
      //!
      FreeRtosSerialDriverComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object FreeRtosSerialDriver
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object FreeRtosSerialDriver
      //!
      ~FreeRtosSerialDriverComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for readBufferSend
      //!
      void readBufferSend_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Handler implementation for serialSend
      //!
      void serialSend_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &serBuffer 
      );


    };

} // end namespace Drv

#endif
