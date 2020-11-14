// ======================================================================
// \title  UdpReceiverComponentImpl.cpp
// \author cedric
// \brief  cpp file for UdpReceiver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/UdpReceiver/UdpReceiver.hpp>
#include "Fw/Types/BasicTypes.hpp"

#include "sci.h"
#include <cstring>

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------


  /**
   * @brief      Constructs a new instance.
   *
   * @param[in]  compName  The component name
   */
  UdpReceiverComponentImpl ::
#if FW_OBJECT_NAMES == 1
    UdpReceiverComponentImpl(
        const char *const compName
    ) :
      UdpReceiverComponentBase(compName)
#else
    UdpReceiverComponentImpl(void)
#endif
    ,m_fd((uint32_t)(SCI_REG))
    ,m_packetsReceived(0)
    ,m_bytesReceived(0)
    ,m_packetsDropped(0)
    ,m_decodeErrors(0)
    ,m_firstSeq(true)
    ,m_currSeq(0)
  {

  }


  /**
   * @brief      Initializes the given instance.
   *
   * @param[in]  instance  The instance
   */
  void UdpReceiverComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    UdpReceiverComponentBase::init(instance);
  }


  /**
   * @brief      Destroys the object.
   */
  UdpReceiverComponentImpl ::
    ~UdpReceiverComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------
  
  void UdpReceiverComponentImpl ::
    uplinkData_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    
    struct udpHeader {
        uint8_t src_port;
        uint8_t dest_port;
        uint8_t length;
        uint8_t checksum;
    };
  
    if (fwBuffer.getsize() < sizeof(struct udpHeader)) {    // Expects a datagram
        this->log_WARNING_HI_UR_RecvError(static_cast<I32>(fwBuffer.getsize()));
        this->m_packetsDropped++;
        updateTelemetry();
        return;
    }
    
    struct udpHeader *header = reinterpret_cast<struct udpHeader *>(fwBuffer.getdata());
    
    // TODO: CHECK SRC & DEST PORTS
    // TODO: CHECK CHECKSUM
    if (false) {
        // TODO: this->log_WARNING_HI_UR_DecodeError(DECODE_HEADER, I32 <received port/checksum>);
        this->log_WARNING_HI_UR_DroppedPacket(static_cast<U32>(header->checksum));
        this->m_decodeErrors++;
        this->m_packetsDropped++;
        updateTelemetry();
        return;
    }
    
    uint8_t computedPayloadLen = static_cast<uint8_t>(fwBuffer.getsize()) - sizeof(*header);
    if (computedPayloadLen != header->length) {
        this->log_WARNING_HI_UR_DecodeError(DECODE_PAYLOAD, static_cast<I32>(computedPayloadLen));
        this->log_WARNING_HI_UR_DroppedPacket(static_cast<U32>(header->checksum));
        this->m_decodeErrors++;
        this->m_packetsDropped++;
        updateTelemetry();
        return;
    }
    
    Fw::Buffer payloadBuffer = getReceivedDatagramBuffer_out(0, header->length);
    memcpy(reinterpret_cast<uint8_t *>(payloadBuffer.getdata()),
           reinterpret_cast<uint8_t *>(fwBuffer.getdata()) + sizeof(*header), header->length);
    
    m_packetsReceived++;
    m_bytesReceived += fwBuffer.getsize();  // Note: Datagram size not payload size
    updateTelemetry();
  }

    void UdpReceiverComponentImpl::updateTelemetry() {
        this->tlmWrite_UR_BytesReceived(this->m_bytesReceived);
        this->tlmWrite_UR_PacketsReceived(this->m_packetsReceived);
        this->tlmWrite_UR_PacketsDropped(this->m_packetsDropped);
        this->tlmWrite_UR_DecodeErrors(this->m_decodeErrors);
    }


  // -------------------------------------------------------------------------
  // Implementation of custom functions of UDP receiver
  // -------------------------------------------------------------------------


  /**
   * @brief      Open the communication port
   *
   * @param[in]  port  The port
   */
  void UdpReceiverComponentImpl::open(
          const char* port
          ){

  }

  int32_t UdpReceiverComponentImpl :: recvfrom( int sockfd,
                                              void *buf,
                                              uint32_t len,
                                              Socket::SocketFlag flags,
                                              struct sockaddr *src_addr,
                                              Socket::SockLength *addrlen){

    uint32_t status;

    // First check for receiving buffer error
    status = sciRxError((volatile sciBASE_t *)sockfd);

    // Check for frame error, overrun and parity
    if((status & SCI_FE_INT) || (status & SCI_OE_INT) || (status & SCI_PE_INT)){
      return -1;
    }

    if(flags & Socket::MSG_WAITALL){
      // blocking read
      sciReceive((volatile sciBASE_t *)sockfd, len, (uint8_t *)buf);
      return len; // at that point all expected data has been received
    }
    else{
      return Socket::EINVAL; // no supported by current driver
    }

    return 0;
  }
} // end namespace CubeRover
