// ======================================================================
// \title  UdpInterfaceComponentImpl.hpp
// \author cedric
// \brief  hpp file for UdpInterface component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef UdpInterface_HPP
#define UdpInterface_HPP

#include "CubeRover/UdpInterface/UdpInterfaceComponentAc.hpp"
#include "CubeRover/CubeRoverSocket/SocketCommon.hpp"
#include <Include/FswPacket.hpp>    // PrimaryFlightController/FlightMCU

namespace CubeRover {

  class UdpInterfaceComponentImpl :
    public UdpInterfaceComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object UdpInterface
      //!
      UdpInterfaceComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object UdpInterface
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object UdpInterface
      //!
      ~UdpInterfaceComponentImpl(void);

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

      //! Handler implementation for downlinkData
      //!
      void downlinkData_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );
      
    void updateTelemetry();

    int32_t recvfrom(   int sockfd,
                        void *buf,
                        uint32_t len,
                        Socket::SocketFlag flags,
                        struct sockaddr *src_addr,
                        Socket::SockLength *addrlen);

    void open(const char* port);

    class UdpSerialBuffer : public Fw::SerializeBufferBase{
        public:
           NATIVE_UINT_TYPE getBuffCapacity(void) const {
            return sizeof(m_buff);
          }

          // Get the max number of bytes that can be serialized
          NATIVE_UINT_TYPE getBuffSerLeft(void) const {
            const NATIVE_UINT_TYPE size  = getBuffCapacity();
            const NATIVE_UINT_TYPE loc = getBuffLength();

            if (loc >= (size-1) ) {
                return 0;
            }
            else {
                return (size - loc - 1);
            }
          }

          U8* getBuffAddr(void) {
            return m_buff;
          }

          const U8* getBuffAddr(void) const {
            return m_buff;
          }

        private:
          // Should be the max of all the input ports serialized sizes...
          U8 m_buff[Socket::UDP_RECEIVER_MSG_SIZE];
    };
    
    struct UdpHeader {
        uint16_t src_port;
        uint16_t dest_port;
        uint16_t length;
        uint16_t checksum;
    } __attribute__ ((packed));
    
    struct UdpPacket {
        struct UdpHeader header;
        uint8_t byte0;
    } __attribute__ ((packed));
    
    UdpSerialBuffer m_recvBuff;
    //UdpSerialBuffer m_portBuff; //!< working buffer for decoding packets
    uint32_t m_packetsSent;
    uint32_t m_bytesSent;     // All received packet lengths (and dropped too)
    uint32_t m_packetsReceived;
    uint32_t m_bytesReceived;     // All received packet lengths (and dropped too)
    uint32_t m_packetsDropped;
    uint32_t m_decodeErrors;
    uint32_t m_firstSeq;
    uint32_t m_currSeq;
    uint32_t m_fd;
    
    };

} // end namespace CubeRover

#endif
