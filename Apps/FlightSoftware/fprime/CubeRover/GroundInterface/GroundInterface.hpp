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
#include <Include/FswPacket.hpp>

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
      
        // User defined methods, members, and structs
            
        void downlinkBufferWrite(void *_data, uint16_t size, downlinkPacketType from);
        void flushDownlinkBuffer();
        void downlink(void *_data, uint16_t size);
        void updateTelemetry();
      
        FswPacket::Seq_t  m_uplinkSeq, m_downlinkSeq;                // TLM0, TLM1
        uint32_t m_packetsRx, m_packetsTx,                  // TLM2, TLM3
                 m_tlmItemsReceived, m_tlmItemsDownlinked,  // TLM4, TLM5
                 m_logsReceived, m_logsDownlinked,          // TLM6, TLM7
                 m_cmdsUplinked, m_cmdsSent, m_cmdErrs,     // TLM8, TLM9, TLM10
                 m_appBytesReceived, m_appBytesDownlinked;  // TLM11, TLM 12
        
        uint8_t m_downlinkBuffer[UDP_MAX_PAYLOAD];  // Entire datagram. UdpSender will complete Udp header
        struct FswPacket::FswPacketHeader *m_downlinkPacket;   // Start of FswPacket in datagram
        uint8_t *m_downlinkBufferPos;
        uint16_t m_downlinkBufferSpaceAvailable;
      

    };

} // end namespace CubeRover

#endif
