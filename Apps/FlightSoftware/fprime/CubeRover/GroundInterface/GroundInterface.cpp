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

#define DOWNLINK_BUFFER_SIZE UDP_MAX_PAYLOAD - sizeof(struct FswPacket::FswPacketHeader)

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
      m_downlinkPacket = reinterpret_cast<struct FswPacket::FswPacketHeader *>(m_downlinkBuffer + 8);  // 8 byte UDP header
      m_downlinkBufferPos = m_downlinkBuffer + sizeof(struct FswPacket::FswPacketHeader);
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
    uint8_t *tlmData = reinterpret_cast<uint8_t *>(data.getBuffAddr());
    FswPacket::Length_t length = static_cast<FswPacket::Length_t>(data.getBuffLength());
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
        U16 callbackId,
        U32 createTime,
        Fw::Buffer &fwBuffer
    )
  {
    uint8_t *data = reinterpret_cast<uint8_t *>(fwBuffer.getdata());
    U32 dataSize = fwBuffer.getsize();
    U32 singlePacketSize = dataSize + sizeof(struct FswPacket::FswFileHeader);
    m_appBytesReceived += dataSize;
    
    Fw::Time _txStart = getTime();
    // FW_ASSERT(_txStart.getTimeBase() != TB_NONE);   // Assert time port is connected
    uint32_t txStart = static_cast<uint32_t>(_txStart.get_time_ms());
    uint16_t hashedId = hashTime(txStart);
    
    if (singlePacketSize <= DOWNLINK_BUFFER_SIZE) {
        uint8_t downlinkBuffer[singlePacketSize];
        struct FswPacket::FswPacket *packet = reinterpret_cast<struct FswPacket::FswPacket*>(downlinkBuffer);
        packet->payload0.file.header.magic = FSW_FILE_MAGIC;
        packet->payload0.file.header.totalBlocks = 1;
        packet->payload0.file.header.blockNumber = 1;
        packet->payload0.file.header.length = static_cast<FswPacket::FileLength_t>(dataSize);
        memcpy(&packet->payload0.file.file.byte0, data, dataSize);
        downlinkFileMetadata(hashedId, 1, static_cast<uint16_t>(callbackId), static_cast<uint32_t>(createTime));
        downlinkBufferWrite(downlinkBuffer, static_cast<FswPacket::Length_t>(singlePacketSize), DownlinkFile);
        m_appBytesDownlinked += singlePacketSize;
    } else {    // Send file fragments
        flushDownlinkBuffer();  // Flush first to get new seq
        int numBlocks = dataSize % (UDP_MAX_PAYLOAD - sizeof(struct FswPacket::FswPacketHeader) - sizeof(struct FswPacket::FswFileHeader));
        downlinkFileMetadata(hashedId, numBlocks, static_cast<uint16_t>(callbackId), static_cast<uint32_t>(createTime));
        int readStride = static_cast<int>(dataSize) / numBlocks;
        uint8_t downlinkBuffer[UDP_MAX_PAYLOAD];
        struct FswPacket::FswPacket *packet = reinterpret_cast<struct FswPacket::FswPacket*>(downlinkBuffer);
        for (int blockNum = 1; blockNum <= numBlocks; ++blockNum) {
            packet->payload0.file.header.magic = FSW_FILE_MAGIC;
            packet->payload0.file.header.totalBlocks = numBlocks;
            packet->payload0.file.header.blockNumber = blockNum;
            FswPacket::FileLength_t blockLength;
            if (blockNum < numBlocks) {     // Send full datagram fragment
                blockLength = readStride;
                dataSize -= readStride;
                packet->payload0.file.header.length = blockLength;
                memcpy(&packet->payload0.file.file.byte0, data, blockLength);
                FswPacket::Length_t datagramLength = 8 + sizeof(struct FswPacket::FswPacketHeader) + sizeof(struct FswPacket::FswFileHeader) + blockLength;
                log_DIAGNOSTIC_GI_DownlinkedItem(m_downlinkSeq, DownlinkFile);
                downlink(packet, datagramLength);
                data += datagramLength;
            } else {        // Final Fragment is written to the member buffer to downlink with other objects
                FW_ASSERT(dataSize > 0);
                blockLength = static_cast<FswPacket::FileLength_t>(dataSize);
                packet->payload0.file.header.length = blockLength;
                memcpy(&packet->payload0.file.file.byte0, data, blockLength);
                downlinkBufferWrite(&packet->payload0.file, sizeof(struct FswPacket::FswFileHeader) + blockLength, DownlinkFile);
            }
            m_appBytesDownlinked += blockLength;
        }
    }
    updateTelemetry();
  }

  void GroundInterfaceComponentImpl ::
    cmdUplink_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    
    m_packetsRx++;

    struct FswPacket::FswPacket *packet = reinterpret_cast<struct FswPacket::FswPacket *>(fwBuffer.getdata());
    U32 buffer_size = fwBuffer.getsize();
    if (buffer_size != packet->header.length) {
        m_cmdErrs++;
        log_WARNING_HI_GI_UplinkedPacketError(MISMATCHED_LENGTH, packet->header.length,
                static_cast<U16>(buffer_size));
    }
    
    if (packet->header.seq != m_uplinkSeq + 1) {
        m_cmdErrs++;
        return;
    }
    m_uplinkSeq = packet->header.seq;
    
    // TODO: Compute checksum
    
    // ASSUME ONE COMMAND PER PACKET
    if (packet->payload0.command.magic != COMMAND_MAGIC) {
        // Uplinked packet not a recognized command! Drop!
    }
    
    m_cmdsUplinked++;
    log_ACTIVITY_HI_GI_CommandReceived(packet->header.seq, packet->header.length);
    Fw::ComBuffer command(reinterpret_cast<uint8_t *>(packet), packet->header.length);
    m_cmdsSent++;
    
    // TODO: Any parsing or decoding required?
    cmdDispatch_out(0, command, 0);        // TODO: Arg 3 Context?
    
    updateTelemetry();
  }
  
    void GroundInterfaceComponentImpl::downlinkBufferWrite(void *_data, FswPacket::Length_t size, downlinkPacketType from) {
        FW_ASSERT(_data);
        FW_ASSERT(size <= DOWNLINK_BUFFER_SIZE);
        uint8_t *data = reinterpret_cast<uint8_t *>(_data);
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
        FswPacket::Length_t length = static_cast<FswPacket::Length_t>(m_downlinkBufferPos - m_downlinkBuffer);
        downlink(m_downlinkBuffer, length);
        m_downlinkBufferPos = m_downlinkBuffer + sizeof(struct FswPacket::FswPacketHeader);
        m_downlinkBufferSpaceAvailable = DOWNLINK_BUFFER_SIZE;
    }
    
    void GroundInterfaceComponentImpl::downlink(void *_data, FswPacket::Length_t size) {
        FW_ASSERT(_data);
        FswPacket::Checksum_t checksum;   // TODO
        uint8_t *data = reinterpret_cast<uint8_t *>(_data);
        struct FswPacket::FswPacketHeader *packetHeader = reinterpret_cast<struct FswPacket::FswPacketHeader *>(data + 8);   // 8 byte UDP header
        packetHeader->seq = m_downlinkSeq;
        packetHeader->checksum = checksum;
        packetHeader->length = size;
        FW_ASSERT(size < UDP_MAX_PAYLOAD);
        Fw::Buffer buffer(0, 0, reinterpret_cast<U64>(data), size);
        log_ACTIVITY_LO_GI_DownlinkedPacket(m_downlinkSeq, checksum, size);
        downlinkBufferSend_out(0, buffer);
        m_downlinkSeq++;
        m_packetsTx++;
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
    
    uint16_t GroundInterfaceComponentImpl::hashTime(uint32_t time) {
        uint16_t hash = 0;
        uint16_t *_time = reinterpret_cast<uint16_t *>(&time);
        hash += *_time;
        _time++;
        hash += *_time;
        return ~hash;
    }
    
    void GroundInterfaceComponentImpl::downlinkFileMetadata(uint16_t hashedId, uint8_t totalBlocks, uint16_t callbackId, uint32_t timestamp_ms) {
        struct FswPacket::FswFile metadata = {0};
        metadata.header.magic = FSW_FILE_MAGIC;
        metadata.header.hashedId = hashedId;
        metadata.header.totalBlocks = totalBlocks;
        metadata.header.blockNumber = 0;
        metadata.header.length = sizeof(struct FswPacket::FswFileMetadata);
        metadata.file.metadata.callbackId = callbackId;
        metadata.file.metadata.timestamp = timestamp_ms;
        downlinkBufferWrite(&metadata, static_cast<FswPacket::Length_t>(sizeof(metadata)), DownlinkFile);
    }

} // end namespace CubeRover
