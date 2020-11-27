// ======================================================================
// \title  UdpReceiverComponentImpl.hpp
// \author cedric
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
#include "CubeRover/CubeRoverSocket/SocketCommon.hpp"

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

      //! Handler implementation for Sched
      //!
      void Sched_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

      //! Handler implementation for readPoll
      //!
      void readPoll_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Handler implementation for uplinkData
      //!
      void uplinkData_handler(
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

      UdpSerialBuffer m_recvBuff;
      //UdpSerialBuffer m_portBuff; //!< working buffer for decoding packets
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
