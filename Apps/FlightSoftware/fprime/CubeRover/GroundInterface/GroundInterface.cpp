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
#include <CubeRover/NetworkManager/NetworkManager.hpp> // ! TODO: FIXME Hacky patch connection to steal NM data
#include "Fw/Types/BasicTypes.hpp"
#include <string.h>

extern CubeRover::NetworkManagerComponentImpl networkManager; // ! TODO: FIXME Hacky patch connection to steal NM data

namespace CubeRover
{

#pragma PERSISTENT
    extern int8_t m_persistent_state = -1;

    static inline FswPacket::Checksum_t computeChecksum(const void *_data, FswPacket::Length_t length)
    {
        const uint8_t *data = static_cast<const uint8_t *>(_data);
        FswPacket::Checksum_t checksum = 0;
        for (; length; length--)
        {
            checksum += *data;
            data++;
        }
        return ~checksum;
    }

    // Fetches whether (NetworkManager thinks) the rover is connected to a
    // network (and capable of wireless downlink).
    // Wrapper for extern NetworkManager connection.
    static bool isNetworkConnected()
    {
        Wf121::DirectMessage::RadioSwState radio_state = networkManager.m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getRadioState();
        return (radio_state == Wf121::DirectMessage::RadioSwState::UDP_CONNECTED);
    }

    // Fetches how many packets have been sent to the Radio for downlink.
    // Wrapper for extern NetworkManager connection.
    static uint32_t getRadioTxPacketCount()
    {
        return networkManager.m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getUdpTxPacketCount();
    }

    // Fetches how many queue spots remain in the NetworkInterface UDP downlink
    // queue.
    static uint8_t getRadioTxDownlinkQueueRoom()
    {
        return networkManager.m_pRadioDriver->m_networkInterface.udpTxQueueRoom();
    }

    // Determines whether there's room in the UDP TX Downlink Queue for an
    // image line (we can choose the threshold to set here and this makes
    // sure it's applied consistently.
    static bool isQueueRoomForImageLine()
    {
        return (getUdpTxDownlinkQueueRoom() >= 1);
    }

    // Shitty bodge to wait until the current packet is sent through the
    // NetworkInterface and there is room for another packet (used to prevent
    // File downlinks from swamping the interface and dropping lines).
    // Halts the idle thread that include normal GI servicing until this is
    // done. Big problem is GI is guarded so it also prevents other threads
    // from using it either. Generally this is just not the place to be doing
    // this but it works and that does the trick for now (RC8/9).
    //
    // Also aborts the wait if network connectivity is lost if
    // `quitOnNetworkLoss` (so it doesn't halt forever - other important things
    // probably need to happen even in the idle task if network connectivity
    // is lost).
    static void haltIdleUntilPacketDownlinkComplete(bool quitOnNetworkLoss = true)
    {
        // ! TODO: FIXME
        // !Forcibly halt the idle thread until Wf121TxTask sends the packet (tx count goes up):
        // ! (do this to avoid maxing out the radio Tx queue):
        uint32_t startUdpTxCount = getRadioTxPacketCount();
        while (startUdpTxCount == getRadioTxPacketCount() &&
               !isQueueRoomForImageLine() &&
               !(quitOnNetworkLoss && !isNetworkConnected()))
        {
            vTaskDelay(10 / portTICK_PERIOD_MS); // Check back in 10ms
        }
    }

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    GroundInterfaceComponentImpl ::
#if FW_OBJECT_NAMES == 1
        GroundInterfaceComponentImpl(
            const char *const compName) : GroundInterfaceComponentBase(compName)
#else
        GroundInterfaceComponentImpl(void)
#endif
    {
        m_uplinkSeq = 0;
        m_downlinkSeq = 0;
        m_packetsRx = 0;
        m_packetsTx = 0;
        m_tlmItemsReceived = 0;
        m_tlmItemsDownlinked = 0;
        m_logsReceived = 0;
        m_logsDownlinked = 0;
        m_cmdsUplinked = 0;
        m_cmdsSent = 0;
        m_cmdErrs = 0;
        m_appBytesReceived = 0;
        m_appBytesDownlinked = 0;
        m_tlmDownlinkBufferPos = m_tlmDownlinkBuffer + sizeof(struct FswPacket::FswPacketHeader);
        // Use INITIAL_PRIMARY_NETWORK_INTERFACE to change the persistent variable m_persistent_state to the correct value
        if (m_persistent_state == -1)
        {
            switch (INITIAL_PRIMARY_NETWORK_INTERFACE)
            {
            case WATCHDOG:
                m_persistent_state = 0;
                break;
            case WF121: // Default to WF121
            default:
                m_persistent_state = 1;
            }
        }
        switch (m_persistent_state)
        {
        case WF121:
            m_downlink_objects_size = WF121_UDP_MAX_PAYLOAD - sizeof(struct FswPacket::FswPacketHeader);
            m_temp_interface_port_num = WF121;
            break;
        case WATCHDOG: // Default to smallest buffer size for safety even if we were using WF121
        default:
            m_downlink_objects_size = WATCHDOG_MAX_PAYLOAD - sizeof(struct FswPacket::FswPacketHeader);
            m_temp_interface_port_num = WATCHDOG;
        }
        m_tlmDownlinkBufferSpaceAvailable = m_downlink_objects_size;
        m_interface_port_num = m_temp_interface_port_num;
        m_telemetry_level = CRITICAL;
    }

    void GroundInterfaceComponentImpl ::
        init(
            const NATIVE_INT_TYPE instance)
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

    //! Handler for input port schedIn
    //
    void GroundInterfaceComponentImpl::schedIn_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        NATIVE_UINT_TYPE context /*!< The call order*/
    )
    {
        // Downlink a new Name or Message if allowed (and it's time):
        this->downlinkNameOrMessageIfAllowed(getTime().get_time_ms());

        // Automatically switch network mode if allowed:
        if (m_auto_switch_allowed)
        {
            bool network_connected = isNetworkConnected();
            if (m_interface_port_num != WF121 && network_connected)
            {
                log_ACTIVITY_HI_InterfaceAutoSwitch(static_cast<FromInterface>(m_interface_port_num), static_cast<ToInterface>(WF121));
                Switch_Primary_Interface(WF121);
            }
            else if (m_interface_port_num != WATCHDOG && !network_connected)
            {
                log_ACTIVITY_HI_InterfaceAutoSwitch(static_cast<FromInterface>(m_interface_port_num), static_cast<ToInterface>(WATCHDOG));
                Switch_Primary_Interface(WATCHDOG);
            }
        }
    }

    void GroundInterfaceComponentImpl ::
        tlmDownlink_handler(
            const NATIVE_INT_TYPE portNum,
            Fw::ComBuffer &data,
            U32 context)
    {
        m_tlmItemsReceived++;
        uint8_t *tlmData = reinterpret_cast<uint8_t *>(data.getBuffAddr());
        FswPacket::Length_t length = static_cast<FswPacket::Length_t>(data.getBuffLength());
        downlinkBufferWrite(tlmData, length, DownlinkTelemetry);
        m_tlmItemsDownlinked++;

        // Avoid emitting GI telemetry here since downlinking telemetry would cause >1
        // telemetry item to be emitted, which could create unbounded growth where each telemetry item
        // creates > 1 telemetry item and suddenly all telemetry is GI telemetry.
        // ! updateTelemetry();
    }

    void GroundInterfaceComponentImpl ::
        logDirectDownlink_handler(
            const NATIVE_INT_TYPE portNum,
            Fw::ComBuffer &data,
            U32 context)
    {
        // In the new topology, all logs go to ActiveLogger, which passes some of them (e.g. severe or fatal) to
        // GroundInterface for immediate downlinking, based on filters (cfg'able from ground), and all them to
        // ComLogger (TBD). ComLogger can then be commanded to pull any of them from the FileSystem based on timestamp
        // and pass those to GroundInterface as well (see logDownlink_handler below for more info).
        //
        // NOTE: Serialization into a packet (LogPacket) has already happened by this point. See `Primer.md` for more
        // details about how this works.
        m_logsReceived++;
        uint8_t *logData = reinterpret_cast<uint8_t *>(data.getBuffAddr());
        FswPacket::Length_t length = static_cast<FswPacket::Length_t>(data.getBuffLength());
        // NOTE: These go into the telem downlink buffer, right along side telemetry (intentionally).
        downlinkBufferWrite(logData, length, DownlinkLog);
        m_logsDownlinked++;

        // Update only relevant telemetry:
        tlmWrite_GI_LogsDownlinked(m_logsDownlinked);
        tlmWrite_GI_LogsReceived(m_logsReceived);
    }

    void GroundInterfaceComponentImpl ::
        logDownlink_handler(
            const NATIVE_INT_TYPE portNum,
            FwEventIdType id,
            Fw::Time &timeTag,
            Fw::LogSeverity severity,
            Fw::LogBuffer &args)
    {
        // This is where logs from ComLogger go, after they're pulled from the FileSystem. See `logDirectDownlink` above for where logs from ActiveLogger are handled.
        // TODO: Based on ComLogger
    }

    void GroundInterfaceComponentImpl ::
        appDownlink_handler(
            const NATIVE_INT_TYPE portNum,
            U16 callbackId,
            U32 createTime,
            Fw::Buffer &fwBuffer)
    {
        uint8_t *data = reinterpret_cast<uint8_t *>(fwBuffer.getdata());
        U32 dataSize = fwBuffer.getsize();
        U32 singleFileObjectSize = dataSize + sizeof(struct FswPacket::FswFileHeader);
        m_appBytesReceived += dataSize;

        Fw::Time _txStart = getTime();
        // FW_ASSERT(_txStart.getTimeBase() != TB_NONE);   // Assert time port is connected
        uint32_t txStart = static_cast<uint32_t>(_txStart.get_time_ms());
        uint16_t hashedId = hashTime(txStart);
        uint8_t *downlinkBuffer = m_fileDownlinkBuffer[portNum];
        if (singleFileObjectSize <= m_downlink_objects_size)
        {
            struct FswPacket::FswFile *obj = reinterpret_cast<struct FswPacket::FswFile *>(downlinkBuffer);
            obj->header.magic = FSW_FILE_MAGIC;
            obj->header.hashedId = hashedId;
            obj->header.totalBlocks = 1;
            obj->header.blockNumber = 1;
            obj->header.length = static_cast<FswPacket::FileLength_t>(dataSize);
            memcpy(&obj->file.byte0, data, dataSize);
            downlinkFileMetadata(hashedId, 1, static_cast<uint16_t>(callbackId), static_cast<uint32_t>(createTime));
            downlinkBufferWrite(downlinkBuffer, static_cast<FswPacket::Length_t>(singleFileObjectSize), DownlinkFile);
            m_appBytesDownlinked += singleFileObjectSize;
            // ! TODO: FIXME
            // !Forcibly halt the idle thread until Wf121TxTask sends the packet (tx count goes up):
            // ! (do this to avoid maxing out the radio Tx queue):
            flushTlmDownlinkBuffer(); // FLUSH BUFFER TO GET PACKET OUT
            haltIdleUntilPacketDownlinkComplete();
        }
        else
        { // Send file fragments
            // TESTING don't intersperse telemetry or logs with file!!! flushTlmDownlinkBuffer();  // Flush first to get new seq
            int numBlocks = static_cast<int>(dataSize) / (m_downlink_objects_size - sizeof(struct FswPacket::FswFileHeader));
            if (static_cast<int>(dataSize) % (m_downlink_objects_size - sizeof(struct FswPacket::FswFileHeader)) > 0)
                numBlocks++;
            downlinkFileMetadata(hashedId, numBlocks, static_cast<uint16_t>(callbackId), static_cast<uint32_t>(createTime));
            flushTlmDownlinkBuffer(); // TESTING!! DOWNLINK METADATA PRIOR TO FILE DOWNLINK
            int readStride = static_cast<int>(dataSize) / numBlocks;
            struct FswPacket::FswPacket *packet = reinterpret_cast<struct FswPacket::FswPacket *>(downlinkBuffer);
            for (int blockNum = 1; blockNum <= numBlocks; ++blockNum)
            {
                packet->payload0.file.header.magic = FSW_FILE_MAGIC;
                packet->payload0.file.header.hashedId = hashedId;
                packet->payload0.file.header.totalBlocks = numBlocks;
                packet->payload0.file.header.blockNumber = blockNum;
                FswPacket::FileLength_t blockLength;
                if (blockNum < numBlocks)
                { // Send full datagram fragment
                    blockLength = readStride;
                    dataSize -= blockLength;
                    packet->payload0.file.header.length = blockLength;
                    memcpy(&packet->payload0.file.file.byte0, data, blockLength);
                    FswPacket::Length_t datagramLength = sizeof(struct FswPacket::FswPacketHeader) + sizeof(struct FswPacket::FswFileHeader) + blockLength;
                    log_DIAGNOSTIC_GI_DownlinkedItem(m_downlinkSeq, DownlinkFile);
                    downlink(downlinkBuffer, datagramLength);
                    data += blockLength;
                    haltIdleUntilPacketDownlinkComplete();
                }
                else
                { // Final Fragment is written to the member buffer to downlink with other objects
                    FW_ASSERT(dataSize > 0);
                    blockLength = static_cast<FswPacket::FileLength_t>(dataSize);
                    packet->payload0.file.header.length = blockLength;
                    memcpy(&packet->payload0.file.file.byte0, data, blockLength);
                    downlinkBufferWrite(&packet->payload0.file, sizeof(struct FswPacket::FswFileHeader) + blockLength, DownlinkFile);
                    flushTlmDownlinkBuffer(); // TESTING!! DOWNLINK FINAL BLOCK WITHOUT INTERRUPTION
                }
                m_appBytesDownlinked += blockLength;
            }
        }
        updateTelemetry();
    }

    // ASSUME ONE COMMAND PER UPLINK PACKET
    void GroundInterfaceComponentImpl ::
        cmdUplink_handler(
            const NATIVE_INT_TYPE portNum,
            Fw::Buffer &fwBuffer)
    {
        // TODO: Use portnumber to increment some internal variable for which packet came from where

        m_packetsRx++;

        struct FswPacket::FswPacket *packet = reinterpret_cast<struct FswPacket::FswPacket *>(fwBuffer.getdata());
        U32 buffer_size = fwBuffer.getsize();
        if (buffer_size != packet->header.length + sizeof(struct FswPacket::FswPacketHeader))
        {
            m_cmdErrs++;
            log_WARNING_HI_GI_UplinkedPacketError(MISMATCHED_LENGTH, packet->header.length, static_cast<U16>(buffer_size));
            return;
        }

        if (packet->header.seq != 0 && packet->header.seq != (m_uplinkSeq + 1))
        {
            // 0 is a special uplinkSeq we use to say we don't care about the uplinkSeq (in case of sync-loss)
            m_cmdErrs++;
            log_WARNING_HI_GI_UplinkedPacketError(uplinkedPacketError::OUT_OF_SEQUENCE, (m_uplinkSeq + 1), packet->header.seq);
            return;
        }

        if (packet->payload0.command.magic != COMMAND_MAGIC && packet->payload0.command.magic != FSW_RADIO_COMMAND_MAGIC)
        {
            m_cmdErrs++;
            log_WARNING_LO_GI_UplinkedBadMagic((U32)packet->payload0.command.magic);
            return;
        }

        if (computeChecksum(packet, packet->header.length + sizeof(struct FswPacket::FswPacketHeader)))
        { // computeChecksum returns 0 if correct
            m_cmdErrs++;
            log_WARNING_HI_GI_UplinkedPacketError(BAD_CHECKSUM, 0, static_cast<U16>(packet->header.checksum));
            return;
        }

        // Set new uplinkSeq. NOTE: by design, if seq_num==0, this will reset uplinkSeq to 0 (where it starts on boot).
        m_uplinkSeq = packet->header.seq;
        m_cmdsUplinked++;
        log_ACTIVITY_HI_GI_CommandReceived(packet->header.seq, packet->header.length);
        Fw::ComBuffer command(reinterpret_cast<uint8_t *>(&packet->payload0.command), packet->header.length);

        // NOTE: since we don't use the `seqCmdStatus` arg3 (U32 context) isn't
        // used in and doesn't go anywhere from CommandDispatcher, so we can
        // safely ignore it.
        cmdDispatch_out(0, command, 0);
        m_cmdsSent++;

        updateTelemetry();
    }

    void GroundInterfaceComponentImpl ::
        Switch_Primary_Interface(PrimaryInterface primary_interface)
    {
        flushTlmDownlinkBuffer();
        // TODO: Should probably flush file downlink buffers too
        switch (primary_interface)
        {
        case WF121:
            m_downlink_objects_size = WF121_UDP_MAX_PAYLOAD - sizeof(struct FswPacket::FswPacketHeader);
            m_persistent_state = 1;
            break;
        case WATCHDOG: // Default to smallest buffer size for safety even if we were using WF121
        default:
            m_downlink_objects_size = WATCHDOG_MAX_PAYLOAD - sizeof(struct FswPacket::FswPacketHeader);
            m_persistent_state = 0;
        }
        m_tlmDownlinkBufferSpaceAvailable = m_downlink_objects_size;
        m_interface_port_num = primary_interface;
    }

    void GroundInterfaceComponentImpl::Set_Interface_Auto_Switch(bool on)
    {
        m_auto_switch_allowed = on;
        log_ACTIVITY_HI_InterfaceAutoSwitchChanged(on);
    }

    // ----------------------------------------------------------------------
    // Command handler implementations
    // ----------------------------------------------------------------------

    void GroundInterfaceComponentImpl ::
        Set_Primary_Interface_cmdHandler(
            const FwOpcodeType opCode,
            const U32 cmdSeq,
            PrimaryInterface primary_interface)
    {
        // If the primary interface is being manually changed, turn off
        // auto-switch (and do this before the interface change so the event
        // goes out on the old interface).
        Set_Interface_Auto_Switch(false);
        // Change interface:
        Switch_Primary_Interface(primary_interface);
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

    //! Handler for command SetInterfaceAutoSwitch
    /* Turn ON/OFF whether Hercules is allowed to automatically switch its
    primary network interface based on connection status. Default is ON. */
    void GroundInterfaceComponentImpl::SetInterfaceAutoSwitch_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        bool on)
    {
        Set_Interface_Auto_Switch(on);
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

    void GroundInterfaceComponentImpl ::
        Set_GroundInterface_Telemetry_Level_cmdHandler(
            const FwOpcodeType opCode,
            const U32 cmdSeq,
            TelemetryLevel telemetry_level)
    {
        m_telemetry_level = telemetry_level;
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

    //! Handler for command RollCredits
    /* Turn ON/OFF whether names and messages should be downlinked. Default is ON. */
    void GroundInterfaceComponentImpl::RollCredits_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        bool on)
    {
        // Set allowed state:
        m_namesAndMessagesAllowed = on;
        // Reset awaiting status:
        m_awaitingNameOrMessageDownlink = false;
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

    //! Handler for command Set_NameAndMessage_Period
    /* Set how many seconds (minimum) should occur between each name/message downlink. Min is 1. */
    void GroundInterfaceComponentImpl::Set_NameAndMessage_Period_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        U16 seconds)
    {
        m_nameOrMessageDownlinkPeriod_ms = seconds * 1000;
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

    /*
     * @brief Write a packet to the downlink buffer.
     *
     * Multiple downlink objects can be written to the downlink buffer to be packed into a single packet for downlink.
     * If the given object exceeds the space available in the buffer, the buffer will be flushed (downlinked) first then
     * the object will be written to the now empty buffer. If the packet fits exactly into the remaining space to fill
     * the MTU, the buffer will be immediately flushed with the given object.
     *
     * @param _data Pointer to the object to be downlinked. It should start with one of the FSWPacket object headers (TODO: const?)
     * @param size  Size of the object to be downlinked in bytes
     * @param from  Where the downlinked object is from (used for logging)
     *
     */
    void GroundInterfaceComponentImpl::downlinkBufferWrite(void *_data, FswPacket::Length_t size, downlinkPacketType from)
    {
        FW_ASSERT(_data);
        FW_ASSERT(size <= m_downlink_objects_size);
        uint8_t *data = reinterpret_cast<uint8_t *>(_data);
        bool flushOnWrite = false;
        if (size > m_tlmDownlinkBufferSpaceAvailable)
        {
            flushTlmDownlinkBuffer();
        }
        else if (size == m_tlmDownlinkBufferSpaceAvailable)
        {
            flushOnWrite = true;
        }

        memcpy(m_tlmDownlinkBufferPos, data, size);
        m_tlmDownlinkBufferPos += size;
        m_tlmDownlinkBufferSpaceAvailable -= size;

        // log_DIAGNOSTIC_GI_DownlinkedItem(m_downlinkSeq, from); // This is bad b/c it causes a race condition where every log will generate >=1 log

        if (flushOnWrite)
            flushTlmDownlinkBuffer();
    }

    /*
     * @brief Downlink contents of the downlink buffer
     *
     */
    void GroundInterfaceComponentImpl::flushTlmDownlinkBuffer()
    {
        // TODO: Check on mode manager wired MTU is 255B
        FswPacket::Length_t length = static_cast<FswPacket::Length_t>(m_tlmDownlinkBufferPos - m_tlmDownlinkBuffer);
        downlink(m_tlmDownlinkBuffer, length);
        m_tlmDownlinkBufferPos = m_tlmDownlinkBuffer + sizeof(struct FswPacket::FswPacketHeader);
        m_tlmDownlinkBufferSpaceAvailable = m_downlink_objects_size;
        // Update all GI telemetry now (starts off a new packet):
        updateTelemetry();
        // Update the name / message head pointer:
        advanceNameOrMessageHeadIfNeeded();
    }

    /*
     * @brief Downlink
     *
     * Downlink data at the given buffer and size. The buffer must point to a packet formatted as a FSWPacket.
     * This method will fill the FSWPacket fields and update this object respectively.
     *
     * @param _data Pointer to the FSWPacket Buffer
     * @param size  Size of the object in the UDP buffer including the size of the FSWPacket headers
     *
     */
    void GroundInterfaceComponentImpl::downlink(void *_data, FswPacket::Length_t size)
    {
        FW_ASSERT(_data);
        FW_ASSERT(size <= WF121_UDP_MAX_PAYLOAD);
        uint8_t *data = reinterpret_cast<uint8_t *>(_data); // Start of the ddatagram
        struct FswPacket::FswPacketHeader *packetHeader = reinterpret_cast<struct FswPacket::FswPacketHeader *>(data);
        packetHeader->seq = m_downlinkSeq;
        packetHeader->length = size - sizeof(struct FswPacket::FswPacketHeader);
        packetHeader->checksum = 0;
        packetHeader->checksum = computeChecksum(_data, size);
        Fw::Buffer buffer(0, 0, reinterpret_cast<U64>(data), size);
        log_ACTIVITY_LO_GI_DownlinkedPacket(m_downlinkSeq, packetHeader->checksum, size);
        downlinkBufferSend_out((int)m_interface_port_num, buffer);
        m_downlinkSeq++;
        m_packetsTx++;
    }

    void GroundInterfaceComponentImpl::updateTelemetry()
    {
        switch (m_telemetry_level)
        {
        case ALL:
        case IMPORTANT:
        case CRITICAL:
        default:
            /* TODO: THESE SHOULD ONLY UPDATE ONCE PER TELEMETRY DOWNLINK NOT ON THE RATE GROUP ITS TOO MUCH */
            tlmWrite_GI_DownlinkSeqNum(m_downlinkSeq);
            tlmWrite_GI_TlmItemsDownlinked(m_tlmItemsDownlinked);
            tlmWrite_GI_LogsDownlinked(m_logsDownlinked);
            tlmWrite_GI_AppBytesDownlinked(m_appBytesDownlinked);
            tlmWrite_GI_CmdsUplinked(m_cmdsUplinked);
            tlmWrite_GI_UplinkPktErrs(m_cmdErrs);
            tlmWrite_GI_TlmItemsReceived(m_tlmItemsReceived);
            tlmWrite_GI_LogsReceived(m_logsReceived);
            tlmWrite_GI_AppBytesReceived(m_appBytesReceived);
            tlmWrite_GI_CmdsSent(m_cmdsSent);
            tlmWrite_GI_UplinkSeqNum(m_uplinkSeq);
            tlmWrite_GI_PacketsReceived(m_packetsRx);
            tlmWrite_GI_PacketsTransmitted(m_packetsTx);
        }
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
    uint16_t GroundInterfaceComponentImpl::hashTime(uint32_t time)
    {
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
    void GroundInterfaceComponentImpl::downlinkFileMetadata(uint16_t hashedId, uint8_t totalBlocks, uint16_t callbackId, uint32_t timestamp_ms)
    {
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

    void GroundInterfaceComponentImpl::downlinkNameCoreImpl(const char *pHead)
    {
        // Convert to FW string and emit the log:
        Fw::LogStringArg name(pHead);
        this->log_ACTIVITY_LO_BroughtToYouBy(name);
    }

    void GroundInterfaceComponentImpl::downlinkMessageCoreImpl(const char *pMessagerString, const char *pMessageString)
    {
        // Convert to FW strings and emit the log:
        Fw::LogStringArg messager(pMessagerString);
        Fw::LogStringArg message(pMessageString);
        this->log_ACTIVITY_LO_SpecialMessage(messager, message);
    }

} // end namespace CubeRover
