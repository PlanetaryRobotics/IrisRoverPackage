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


#include <CubeRover/UdpReceiver/UdpReceiverComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

#include "lin.h"

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
    ,m_fd((uint32_t)(LIN_REG))
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


  /**
   * @brief      Handler for scheduled event
   *
   * @param[in]  portNum  The port number
   * @param[in]  context  The context
   */
  void UdpReceiverComponentImpl ::
    Sched_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    this->tlmWrite_UR_BytesReceived(this->m_bytesReceived);
    this->tlmWrite_UR_PacketsReceived(this->m_packetsReceived);
    this->tlmWrite_UR_PacketsDropped(this->m_packetsDropped);
  }


  /**
   * @brief      Reads poll handler.
   *
   * @param[in]  portNum   The port number
   * @param      fwBuffer  The connected buffer
   */
  void UdpReceiverComponentImpl ::
    readPoll_handler(const NATIVE_INT_TYPE portNum,
                    Fw::Buffer &fwBuffer)
  {
    int32_t psize = recvfrom(m_fd,
                             m_recvBuff.getBuffAddr(),
                             m_recvBuff.getBuffCapacity(),
                             Socket::MSG_WAITALL,
                             0,
                             0);
    if(psize < 0){
      this->log_WARNING_HI_UR_RecvError(psize);
      return ;
    }

    // reset buffer for deserialization
    Fw::SerializeStatus stat = m_recvBuff.setBuffLen(psize);
    FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, stat);

    // get sequence number
    uint8_t seqNum;
    stat = m_recvBuff.deserialize(seqNum);

    // check for deserialization error or port number too high
    if(stat != Fw::FW_SERIALIZE_OK) {
      this->log_WARNING_HI_UR_DecodeError(DECODE_SEQ, stat);
      this->m_decodeErrors++;
      return;
    }

    // track sequence number
    if(this->m_firstSeq) {
      // first time, set tracked sequence number equal to the one received
      this->m_currSeq = seqNum;
      this->m_firstSeq = false;
    }else {
      // make sure sequence number has gone up by one
      if (seqNum != ++this->m_currSeq) {
        // will only be right if it rolls over only once, but better than
        // nothing
        U8 diff = seqNum - this->m_currSeq;
        this->m_packetsDropped += diff;
        // send EVR
        this->log_WARNING_HI_UR_DroppedPacket(diff);
        // reset to current sequence
        this->m_currSeq = seqNum;
      }
    }

    // get port number
    uint8_t port;
    stat = m_recvBuff.deserialize(port);
    // check for de-serialization error or port number too high
    if (stat != Fw::FW_SERIALIZE_OK || port > this->getNum_readPoll_InputPorts()) {
      this->log_WARNING_HI_UR_DecodeError(DECODE_PORT, stat);
      this->m_decodeErrors++;
      return;
    }

    // get buffer for port
    stat = m_recvBuff.deserialize(fwBuffer);
    if (stat != Fw::FW_SERIALIZE_OK) {
      this->log_WARNING_HI_UR_DecodeError(DECODE_BUFFER,stat);
      this->m_decodeErrors++;
      return;
    }

    this->m_packetsReceived++;
    this->m_bytesReceived += psize;
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
    status = linTxRxError((volatile linBASE_t *)sockfd);

    // Check for frame error, overrun and parity
    if((status & LIN_FE_INT) || (status & LIN_OE_INT) || (status & LIN_PE_INT)){
      return -1;
    }

    if(flags & Socket::MSG_WAITALL){
      // blocking read
      linGetData((volatile linBASE_t *)sockfd, (uint8_t *)buf);
      return (uint32)((uint32)(lin->FORMAT & 0x00070000U) >> 16U); // Send the calculated size of the data (copied from lin.h)
    }
    else{
      return Socket::EINVAL; // no supported by current driver
    }

    return 0;
  }
} // end namespace CubeRover
