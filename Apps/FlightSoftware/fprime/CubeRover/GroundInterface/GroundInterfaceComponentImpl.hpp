// ======================================================================
// \title  GroundInterfaceComponentImpl.hpp
// \author justin
// \brief  hpp file for GroundInterface component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef GroundInterface_HPP
#define GroundInterface_HPP

#include "CubeRover/GroundInterface/GroundInterfaceComponentAc.hpp"

namespace Svc {

  class GroundInterfaceComponentImpl :
    public GroundInterfaceComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object GroundInterface
      //!
      GroundInterfaceComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object GroundInterface
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object GroundInterface
      //!
      ~GroundInterfaceComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for tlmDownlink
      //!
      void tlmDownlink_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::ComBuffer &data, /*!< Buffer containing packet data*/
          U32 context /*!< Call context value; meaning chosen by user*/
      );

      //! Handler implementation for logDownlink
      //!
      void logDownlink_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwEventIdType id, /*!< Log ID*/
          Fw::Time &timeTag, /*!< Time Tag*/
          Fw::LogSeverity severity, /*!< The severity argument*/
          Fw::LogBuffer &args /*!< Buffer containing serialized log entry*/
      );

      //! Handler implementation for appDownlink
      //!
      void appDownlink_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Handler implementation for uplinkStorageBufferGet
      //!
      void uplinkStorageBufferGet_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 size 
      );

      //! Handler implementation for downlinkBufferSend
      //!
      void downlinkBufferSend_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );


    };

} // end namespace Svc

#endif
