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

namespace CubeRover {

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

      //! Handler implementation for cmdUplink
      //!
      void cmdUplink_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );
      
      
        // User defined methods, memebers, and structs
        
        void updateTelemetry();
      
        uint8_t  m_uplinkSeq, m_downlinkSeq;                // TLM0, TLM1
        uint32_t m_packetsRx, m_packetsTx,                  // TLM2, TLM3
                 m_tlmItemsReceived m_tlmItemsDownlinked,   // TLM4, TLM5
                 m_logsReceived, m_logsDownlinked,          // TLM6, TLM7
                 m_cmdsUplinked, m_cmdsSent, m_cmdErrs,     // TLM8, TLM9, TLM10
                 m_appBytesReceived, m_appBytesDownlinked;  // TLM11, TLM 12
      
        struct FswPacketHeader {
            uint8_t seq;
            uint16_t checksum;
            uint16_t length;
            union {
               uint8_t startByte;
               struct FswCommand command;
               struct FswTelemetry telemetry;
               struct Fswlog log;
            } payload1;
            // Additional telemetry or logs (but not commands) can follow the first
            // object in the payload, payload1, up until the end of the packet determined
            // by length.
        };
        
        struct FswCommand {
            uint8_t magic;
            uint8_t component;
            uint8_t command;
            uint8_t arguments_start;
        };
        
        struct FswTelemetry {
            uint8_t magic;
            uint8_t comBuffer[ComBuffer::SERIALIZED_SIZE];  // The maximum possible size for an comBuffer Object
        };
        
        struct FswLog {
            uint8_t magic;
            uint8_t comBuffer[ComBuffer::SERIALIZED_SIZE];  // The maximum possible size for an comBuffer Object
        };
        

    };

} // end namespace CubeRover

#endif
