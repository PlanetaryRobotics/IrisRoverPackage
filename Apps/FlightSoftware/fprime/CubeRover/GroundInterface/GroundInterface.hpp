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
#include <Include/CubeRoverConfig.hpp>

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
        // TODO: component/field length should be sourced from Config.hpp
      
        struct FswPacketHeader {
            uint8_t seq;
            uint16_t length;
            uint16_t checksum;
        } __attribute__((packed));
        
        struct FswCommand {
            uint32_t magic;
            uint8_t component;      // This field along with command make up the
            uint8_t opcode;         // id when concatenated resulting in a U16
            uint8_t byte0;
        } __attribute__((packed));
        
        struct FswCommandResponse { // This is downlinked via the file (app downlink port)
            uint32_t magic;
            uint8_t component;      // Same as command
            uint8_t opcode;         // Same as command
            uint8_t errorcode;
            uint16_t errorinfo;
        } __attribute__((packed));
        
        struct FswFile {
            uint32_t magic;
            uint8_t totalBlocks;
            uint8_t blockNumber;    // This is 1-indexed
            uint16_t length;        // This is the size of the following data **not including this header**
            uint8_t byte0;
        } __attribute__((packed));
            
        struct FswPacket {
            struct FswPacketHeader header;
            union {
               uint8_t startByte;
               uint32_t magic0;     // Magic of first packet
               struct FswCommand command;
               struct FswCommandResponse cmdResp;
               struct FswFile file;
               // struct FswTelemetry telemetry;
               // struct FswLog log;
            } payload0;
            // Additional telemetry or logs (but not commands) can follow the first
            // object in the payload, payload0, up until the end of the packet determined
            // by length.
        };
            
        void downlinkBufferWrite(void *_data, uint16_t size, downlinkPacketType from);
        void flushDownlinkBuffer();
        void downlink(void *_data, uint16_t size);
        void updateTelemetry();
      
        uint8_t  m_uplinkSeq, m_downlinkSeq;                // TLM0, TLM1
        uint32_t m_packetsRx, m_packetsTx,                  // TLM2, TLM3
                 m_tlmItemsReceived, m_tlmItemsDownlinked,  // TLM4, TLM5
                 m_logsReceived, m_logsDownlinked,          // TLM6, TLM7
                 m_cmdsUplinked, m_cmdsSent, m_cmdErrs,     // TLM8, TLM9, TLM10
                 m_appBytesReceived, m_appBytesDownlinked;  // TLM11, TLM 12
        
        uint8_t m_downlinkBuffer[UDP_MAX_PAYLOAD];  // Entire datagram. UdpSender will complete Udp header
        struct FswPacketHeader *m_downlinkPacket;   // Start of FswPacket in datagram
        uint8_t *m_downlinkBufferPos;
        uint16_t m_downlinkBufferSpaceAvailable;
      

    };

} // end namespace CubeRover

#endif
