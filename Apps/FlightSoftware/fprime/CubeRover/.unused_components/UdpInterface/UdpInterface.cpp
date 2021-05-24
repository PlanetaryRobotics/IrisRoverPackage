// ======================================================================
// \title  UdpInterfaceComponentImpl.cpp
// \author justin
// \brief  cpp file for UdpInterface component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/UdpInterface/UdpInterface.hpp>
#include "Fw/Types/BasicTypes.hpp"

#include "sci.h"
#include <cstring>

// Wired (RS422 via WatchDog) Connection Parameters
#define WIRED_UDP_PORT_ROVER    8080
#define WIRED_UDP_PORT_LANDER   8080

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------


  /**
   * @brief      Constructs a new instance.
   *
   * @param[in]  compName  The component name
   */
  UdpInterfaceComponentImpl ::
#if FW_OBJECT_NAMES == 1
    UdpInterfaceComponentImpl(
        const char *const compName
    ) :
      UdpInterfaceComponentBase(compName)
#else
    UdpInterfaceComponentImpl(void)
#endif
    //,m_fd((uint32_t)(SCI_REG))
    ,m_packetsSent(0)
    ,m_bytesSent(0)
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
  void UdpInterfaceComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    UdpInterfaceComponentBase::init(instance);
  }


  /**
   * @brief      Destroys the object.
   */
  UdpInterfaceComponentImpl ::
    ~UdpInterfaceComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------
  
  void UdpInterfaceComponentImpl ::
    uplinkData_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
      
    if (fwBuffer.getsize() < sizeof(struct UdpHeader)) {    // Expects a datagram
        this->log_WARNING_HI_UR_RecvError(static_cast<I32>(fwBuffer.getsize()));
        this->m_packetsDropped++;
        updateTelemetry();
        return;
    }
    
    struct UdpHeader *header = reinterpret_cast<struct UdpHeader *>(fwBuffer.getdata());
    
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
    
    if (static_cast<uint8_t>(fwBuffer.getsize()) != header->length) {
        this->log_WARNING_HI_UR_DecodeError(DECODE_PAYLOAD, static_cast<I32>(header->length));
        this->log_WARNING_HI_UR_DroppedPacket(static_cast<U32>(header->checksum));
        this->m_decodeErrors++;
        this->m_packetsDropped++;
        updateTelemetry();
        return;
    }
    
    uint8_t computedPayloadLen = static_cast<uint8_t>(fwBuffer.getsize()) - sizeof(*header);
    U64 payloadStart = reinterpret_cast<U64>(reinterpret_cast<uint8_t *>(fwBuffer.getdata()) + sizeof(*header));
    Fw::Buffer payloadBuffer(0, 0, payloadStart, header->length - sizeof(*header)); // XXX: Set buffer manager to 0xfff...?
    forwardDatagramPayload_out(0, payloadBuffer);
    
    m_packetsReceived++;
    m_bytesReceived += fwBuffer.getsize();  // Note: Datagram size not payload size
    updateTelemetry();
  }
  
  void UdpInterfaceComponentImpl ::
    downlinkDatagram_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    struct UdpPacket *packet = reinterpret_cast<struct UdpPacket *>(fwBuffer.getdata());
    packet->header.src_port = static_cast<uint16_t>(WIRED_UDP_PORT_ROVER);
    packet->header.dest_port = static_cast<uint16_t>(WIRED_UDP_PORT_LANDER);
    packet->header.length = static_cast<uint16_t>(fwBuffer.getsize());
    // ASSERT length is less than MTU
    // TODO: Compute checksum
    
    forwardDatagram_out(0, fwBuffer);
    m_packetsSent++;
    m_bytesSent += fwBuffer.getsize(); 
    updateTelemetry();
  }

    void UdpInterfaceComponentImpl::updateTelemetry() {
        this->tlmWrite_UR_BytesSent(this->m_bytesSent);
        this->tlmWrite_UR_PacketsSent(this->m_packetsSent);
        this->tlmWrite_UR_BytesReceived(this->m_bytesReceived);
        this->tlmWrite_UR_PacketsReceived(this->m_packetsReceived);
        this->tlmWrite_UR_PacketsDropped(this->m_packetsDropped);
        this->tlmWrite_UR_DecodeErrors(this->m_decodeErrors);
    }

  // -------------------------------------------------------------------------
  // Implementation of custom functions of UDP interface
  // -------------------------------------------------------------------------


  /**
   * @brief      Open the communication port
   *
   * @param[in]  port  The port
   */
  void UdpInterfaceComponentImpl::open(
          const char* port
          ){

  }

  int32_t UdpInterfaceComponentImpl :: recvfrom( int sockfd,
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
