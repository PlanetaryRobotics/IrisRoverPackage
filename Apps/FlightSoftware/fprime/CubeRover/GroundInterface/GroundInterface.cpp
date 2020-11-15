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


#include <CubeRover/GroundInterface/GroundInterfaceComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <CubeRoverConfig.hpp>

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  GroundInterfaceComponentImpl ::
#if FW_OBJECT_NAMES == 1
    GroundInterfaceComponentImpl(
        const char *const compName
    ) :
      GroundInterfaceComponentBase(compName),
      m_uplinkSeq(0), m_downlinkSeq(0)
#else
    GroundInterfaceComponentImpl(void)
#endif
  {

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
    // TODO
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

    struct FswPacketHeader *packet = reinterpret_cast<struct FswPacketHeader *>(fwBuffer.getdata());
    U32 buffer_size = fwBuffer.getsize();
    if (buffer_size != packet->length) {
        m_cmdErrs++;
        log_WARNING_HI_GI_UplinkedPacketError(MISMATCHED_LENGTH,
                packet->length, static_cast<U16>(buffer_size));
    }
    
    if (packet->seq != m_uplinkSeq + 1) {
        // Out of sequence packet! Drop!
    }
    m_uplinkSeq = packet->seq;
    
    // TODO: Compute checksum
    
    // ASSUME ONE COMMAND PER PACKET
    if (packet->payload1.command.magic != COMMAND_MAGIC) {
        // Uplinked packet not a recognized command! Drop!
    }
    
    m_cmdsUplinked++;
    log_ACTIVITY_HI_GI_CommandReceived(packet->seq, packet->length);
    Fw::ComBuffer command(reinterpret_cast<uint8_t *>(packet), packet->length);
    m_cmdsSent++;
    
    cmdDispatch_out(0, command, 0);        // TODO: Arg 3 Context?
    
    updateTelemetry();
    
  }
  
    void updateTelemetry() {
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
