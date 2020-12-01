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

#define DOWNLINK_OBJECTS_SIZE UDP_MAX_PAYLOAD - sizeof(struct FswPacket::FswPacketHeader)

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
      m_downlinkPacket = reinterpret_cast<struct FswPacket::FswPacketHeader *>(m_downlinkBuffer + 8);   // 8byte UDP header
      m_downlinkBufferPos = m_downlinkPacket + sizeof(struct FswPacket::FswPacketHeader);
      m_downlinkBufferSpaceAvailable = DOWNLINK_OBJECTS_SIZE;
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
    U32 singleFileObjectSize = dataSize + sizeof(struct FswPacket::FswFileHeader);
    m_appBytesReceived += dataSize;
    
    Fw::Time _txStart = getTime();
    // FW_ASSERT(_txStart.getTimeBase() != TB_NONE);   // Assert time port is connected
    uint32_t txStart = static_cast<uint32_t>(_txStart.get_time_ms());
    uint16_t hashedId = hashTime(txStart);
    
    if (singleFileObjectSize <= DOWNLINK_OBJECTS_SIZE) {
        uint8_t downlinkBuffer[singleFileObjectSize];
        struct FswPacket::FswPacket *obj = reinterpret_cast<struct FswFile file *>(downlinkBuffer);
        obj->header.magic = FSW_FILE_MAGIC;
        obj->header.totalBlocks = 1;
        obj->header.blockNumber = 1;
        obj->header.length = static_cast<FswPacket::FileLength_t>(dataSize);
        memcpy(&obj->file.byte0, data, dataSize);
        downlinkFileMetadata(hashedId, 1, static_cast<uint16_t>(callbackId), static_cast<uint32_t>(createTime));
        downlinkBufferWrite(downlinkBuffer, static_cast<FswPacket::Length_t>(singleFileObjectSize), DownlinkFile);
        m_appBytesDownlinked += singleFileObjectSize;
    } else {    // Send file fragments
        flushDownlinkBuffer();  // Flush first to get new seq
        int numBlocks = static_cast<int>(dataSize) / (DOWNLINK_OBJECTS_SIZE - sizeof(struct FswPacket::FswFileHeader));
        if (static_cast<int>(dataSize) % (DOWNLINK_OBJECTS_SIZE - sizeof(struct FswPacket::FswFileHeader)) > 0)
            numBlocks++;
        downlinkFileMetadata(hashedId, numBlocks, static_cast<uint16_t>(callbackId), static_cast<uint32_t>(createTime));
        int readStride = static_cast<int>(dataSize) / numBlocks;
        uint8_t downlinkBuffer[UDP_MAX_PAYLOAD + 8];    // 8byte UDP header
        struct FswPacket::FswPacket *packet = reinterpret_cast<struct FswPacket::FswPacket*>(downlinkBuffer + 8);
        for (int blockNum = 1; blockNum <= numBlocks; ++blockNum) {
            packet->payload0.file.header.magic = FSW_FILE_MAGIC;
            packet->payload0.file.header.hashedId = hashedId;
            packet->payload0.file.header.totalBlocks = numBlocks;
            packet->payload0.file.header.blockNumber = blockNum;
            if (blockNum < numBlocks) {     // Send full datagram fragment
                dataSize -= readStride;
                packet->payload0.file.header.length = readStride;
                memcpy(&packet->payload0.file.file.byte0, data, readStride);
                FswPacket::Length_t datagramLength = 8 + sizeof(struct FswPacket::FswPacketHeader) + sizeof(struct FswPacket::FswFileHeader) + readStride;
                log_DIAGNOSTIC_GI_DownlinkedItem(m_downlinkSeq, DownlinkFile);
                downlink(packet, datagramLength);
                data += datagramLength;
            } else {        // Final Fragment is written to the member buffer to downlink with other objects
                FW_ASSERT(dataSize > 0);
                FileLength_t blockLength = static_cast<FswPacket::FileLength_t>(dataSize);
                packet->payload0.file.header.length = blockLength;
                memcpy(&packet->payload0.file.file.byte0, data, blockLength);
                downlinkBufferWrite(&packet->payload0.file,
                                    8 + sizeof(struct FswPacket::FswPacketHeader) + sizeof(struct FswPacket::FswFileHeader) + blockLength, 
                                    DownlinkFile);
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
  
  
    /*
     * @brief Write a packet to the downlink buffer.
     * 
     * Multiple downlink objects can be written to the downlink buffer to be packed into a single packet for downlink.
     * If the given object exceeds the space available in the buffer, the buffer will be flushed (downlinked) first then
     * the object will be written to the now empty buffer. If the packet fits exactly into the remining space to fill
     * the MTU, the buffer will be immediately flushed with the given object.
     * 
     * @param _data Pointer to the object to be downlinked. It should start with one of the FSWPacket object headers (TODO: const?)
     * @param size  Size of the object to be downlinked in bytes
     * @param from  Where the downlinked object is from (used for logging)
     * 
     */
    void GroundInterfaceComponentImpl::downlinkBufferWrite(void *_data, FswPacket::Length_t size, downlinkPacketType from) {
        FW_ASSERT(_data);
        FW_ASSERT(size <= DOWNLINK_OBJECTS_SIZE);
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
    
    /*
     * @brief Downlink contents of the downlink buffer
     * 
     */
    void GroundInterfaceComponentImpl::flushDownlinkBuffer() {
        FswPacket::Length_t length = static_cast<FswPacket::Length_t>(m_downlinkBufferPos - m_downlinkBuffer);
        downlink(m_downlinkBuffer, length);
        m_downlinkBufferPos = m_downlinkPacket + sizeof(struct FswPacket::FswPacketHeader);
        m_downlinkBufferSpaceAvailable = DOWNLINK_OBJECTS_SIZE;
    }
    
    /*
     * @brief Downlink
     * 
     * Downlink data at the given buffer and size. The buffer must point to a packet formatted as a UDP datagram
     * encapsulating a FSWPacket. The UDP header will be filled out at the transport layer (either UDP sender or
     * WF121). This method will fill the FSWPacket fields and update this object respectively.
     * 
     * @param _data Pointer to the UDP Buffer
     * @param size  Size of the object in the UDP buffer including the size of the UDP and FSWPacket headers
     * 
     */
    void GroundInterfaceComponentImpl::downlink(void *_data, FswPacket::Length_t size) {
        FW_ASSERT(_data);
        FswPacket::Checksum_t checksum = 0x8008;   // TODO
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
    
    /*
     * @brief Hash the downlink time
     * 
     * To support parallel file downlinks (ie downlink a navigation image while a science image is being downlinked),
     * a field, hashedId, is required to differentiate between the two files being downlinked. A 16bit hash of the
     * 32bit timestamp of when the transfer initiated is used.
     * 
     * @param time Time the transfer started
     * @return The 16bit hashed timestamp
     * 
     */
    uint16_t GroundInterfaceComponentImpl::hashTime(uint32_t time) {
        uint16_t hash = 0;
        uint16_t *_time = reinterpret_cast<uint16_t *>(&time);
        hash += *_time;
        _time++;
        hash += *_time;
        return ~hash;
    }
    
    /*
     * 
     * @brief Downlink the metadata object for a file
     * 
     * Files being downlinked can also downlink additional metadata which would be redundant to be included in every
     * file block being downlinked. The metadata object is downlinked via the downlink buffer, so it is possible that
     * the metadata object could be sent during or after a file downlink.
     * 
     * @param hashedId    The hashed timestamp of when the transfer initiated so the metadata field can share the value
     * @param totalBlocks The total number of blocks in the file (used to match the file header of the downlinked file)
     * @param callbackId  The callback if of the file (the unique ID assigned to the function which generated this file)
     * @param timestamp_ms  The time the file was created
     * 
     */
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
