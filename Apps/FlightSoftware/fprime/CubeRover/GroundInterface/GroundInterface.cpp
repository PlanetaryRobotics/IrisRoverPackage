// ======================================================================
// \title  GroundInterfaceComponentImpl.cpp
// \author justin
// \brief  cpp file for GroundInterface component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/GroundInterface/GroundInterface.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <cstring>

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  GroundInterfaceComponentImpl ::
#if FW_OBJECT_NAMES == 1
    GroundInterfaceComponentImpl(
        const char *const compName
    ) :
      GroundInterfaceComponentBase(compName)
#else
    GroundInterfaceComponentImpl(void)
#endif
  {
      m_uplinkSeq = 0; m_downlinkSeq = 0;
      m_packetsRx = 0; m_packetsTx = 0;
      m_tlmItemsReceived = 0; m_tlmItemsDownlinked = 0;
      m_logsReceived = 0; m_logsDownlinked = 0;
      m_cmdsUplinked = 0; m_cmdsSent = 0; m_cmdErrs = 0;
      m_appBytesReceived = 0; m_appBytesDownlinked = 0;
      m_downlinkBufferPos = m_downlinkBuffer;
      m_downlinkBufferSpaceAvailable = DOWNLINK_BUFFER_SIZE;
  }

  void GroundInterfaceComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    GroundInterfaceComponentBase::init(instance);
  }

  GroundInterfaceComponentImpl ::
    ~GroundInterfaceComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void GroundInterfaceComponentImpl ::
    tlmDownlink_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
    m_tlmItemsReceived++;
    const U8 *tlmData = data.getBuffAddr();
    uint16_t length = static_cast<uint16_t>(data.getBuffLength());
    downlinkBufferWrite(tlmData, length, DownlinkTelemetry);
    m_tlmItemsDownlinked++;
    updateTelemetry();
  }

  void GroundInterfaceComponentImpl ::
    logDownlink_handler(
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    // TODO
  }

  void GroundInterfaceComponentImpl ::
    appDownlink_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    // TODO
  }

  void GroundInterfaceComponentImpl ::
    cmdUplink_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    
    m_packetsRx++;

    struct FswPacket *packet = reinterpret_cast<struct FswPacket *>(fwBuffer.getdata());
    U32 buffer_size = fwBuffer.getsize();
    if (buffer_size != packet->header.length) {
        m_cmdErrs++;
        log_WARNING_HI_GI_UplinkedPacketError(MISMATCHED_LENGTH, packet->header.length,
                static_cast<U16>(buffer_size));
    }
    
    if (packet->header.seq != m_uplinkSeq + 1) {
        // Out of sequence packet! Drop!
    }
    m_uplinkSeq = packet->header.seq;
    
    // TODO: Compute checksum
    
    // ASSUME ONE COMMAND PER PACKET
    if (packet->payload1.command.magic != COMMAND_MAGIC) {
        // Uplinked packet not a recognized command! Drop!
    }
    
    m_cmdsUplinked++;
    log_ACTIVITY_HI_GI_CommandReceived(packet->header.seq, packet->header.length);
    Fw::ComBuffer command(reinterpret_cast<uint8_t *>(packet), packet->header.length);
    m_cmdsSent++;
    
    cmdDispatch_out(0, command, 0);        // TODO: Arg 3 Context?
    
    updateTelemetry();
  }
  
    void GroundInterfaceComponentImpl::downlinkBufferWrite(const uint8_t *data, uint16_t size, downlinkPacketType from) {
        FW_ASSERT(data);
        FW_ASSERT(size < DOWNLINK_BUFFER_SIZE);
        bool flushOnWrite = false;
        if (size > m_downlinkBufferSpaceAvailable) {
            flushDownlinkBuffer();
        } else if (size == m_downlinkBufferSpaceAvailable) {
            flushOnWrite = true;
        }
        
        memcpy(m_downlinkBufferPos, data, size);
        m_downlinkBufferPos += size;
        m_downlinkBufferSpaceAvailable -= size;
        
        log_DIAGNOSTIC_GI_DownlinkedItem(m_downlinkSeq, from);
        
        if (flushOnWrite)
            flushDownlinkBuffer();
        
    }
    
    void GroundInterfaceComponentImpl::flushDownlinkBuffer() {
        U16 checksum;   // TODO
        U16 length = static_cast<U16>(m_downlinkBufferPos - m_downlinkBuffer);
        Fw::Buffer buffer(0, 0, reinterpret_cast<U64>(m_downlinkBuffer), length);
        log_ACTIVITY_LO_GI_DownlinkedPacket(m_downlinkSeq, checksum, length);
        downlinkBufferSend_out(0, buffer);
        m_packetsTx++;
        
        m_downlinkBufferPos = m_downlinkBuffer;
        m_downlinkBufferSpaceAvailable = DOWNLINK_BUFFER_SIZE;
    }
  
    void GroundInterfaceComponentImpl::updateTelemetry() {
        tlmWrite_GI_UplinkSeqNum(m_uplinkSeq);
        tlmWrite_GI_DownlinkSeqNum(m_downlinkSeq);
        tlmWrite_GI_PacketsReceived(m_packetsRx);
        tlmWrite_GI_PacketsTransmitted(m_packetsTx);
        tlmWrite_GI_TlmItemsReceived(m_tlmItemsReceived);
        tlmWrite_GI_TlmItemsDownlinked(m_tlmItemsDownlinked);
        tlmWrite_GI_LogsReceived(m_logsReceived);
        tlmWrite_GI_LogsDownlinked(m_logsDownlinked);
        tlmWrite_GI_CmdsUplinked(m_cmdsUplinked);
        tlmWrite_GI_CmdsSent(m_cmdsSent);
        tlmWrite_GI_UplinkPktErrs(m_cmdErrs);
        tlmWrite_GI_AppBytesReceived(m_appBytesReceived);
        tlmWrite_GI_AppBytesDownlinked(m_appBytesDownlinked);
    }

} // end namespace CubeRover
