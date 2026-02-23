// ======================================================================
// \title  GroundInterfaceComponentImpl.hpp
// \author justin
// \brief  hpp file for GroundInterface component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef GroundInterface_HPP
#define GroundInterface_HPP

#include "CubeRover/GroundInterface/GroundInterfaceComponentAc.hpp"
#include "CubeRover/NetworkManager/NetworkManager.hpp" // ! TODO: FIXME Hacky patch connection to steal NM data
#include <Include/FswPacket.hpp>
#include "NamesAndMessagesSender.hpp"

namespace CubeRover
{

    extern int8_t m_persistent_state;

    class GroundInterfaceComponentImpl : public GroundInterfaceComponentBase,
                                         public ::IrisNames::NamesAndMessageSender
    {

    public:
        // ----------------------------------------------------------------------
        // Construction, initialization, and destruction
        // ----------------------------------------------------------------------

        //! Construct object GroundInterface
        //!
        GroundInterfaceComponentImpl(
#if FW_OBJECT_NAMES == 1
            const char *const compName /*!< The component name*/
#else
            void
#endif
        );

        //! Initialize object GroundInterface
        //!
        void init(
            const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
        );

        //! Destroy object GroundInterface
        //!
        ~GroundInterfaceComponentImpl(void);

        PRIVATE :

            // ----------------------------------------------------------------------
            // Handler implementations for user-defined typed input ports
            // ----------------------------------------------------------------------

            //! Handler for input port schedIn
            //
            void
            schedIn_handler(
                NATIVE_INT_TYPE portNum, /*!< The port number*/
                NATIVE_UINT_TYPE context /*!< The call order*/
            );

        //! Handler implementation for tlmDownlink
        //!
        void tlmDownlink_handler(
            const NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::ComBuffer &data,           /*!< Buffer containing packet data*/
            U32 context                    /*!< Call context value; meaning chosen by user*/
        );

        //! Handler for input port logDirectDownlink
        //
        void logDirectDownlink_handler(
            NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::ComBuffer &data,     /*!< Buffer containing packet data*/
            U32 context              /*!< Call context value; meaning chosen by user*/
        );

        //! Handler implementation for logDownlink
        //!
        void logDownlink_handler(
            const NATIVE_INT_TYPE portNum, /*!< The port number*/
            FwEventIdType id,              /*!< Log ID*/
            Fw::Time &timeTag,             /*!< Time Tag*/
            Fw::LogSeverity severity,      /*!< The severity argument*/
            Fw::LogBuffer &args            /*!< Buffer containing serialized log entry*/
        );

        //! Handler implementation for appDownlink
        //!
        void appDownlink_handler(
            const NATIVE_INT_TYPE portNum, /*!< The port number*/
            U16 callbackId,                /*!< Metadata Field: Unique Id to map this file to the command that generated it*/
            U32 fileGroupCreateTime,       /*!< When the File Group this file comes from was created (i.e. image capture time -  each image LINE is a "File", so an image is a "File Group") */
            U16 fileGroupLineNumber,       /*!< Which "line" in the File Group this file is (i.e. image line number - each image LINE is a "File", so an image is a "File Group"). 0-indexed. */
            U16 fileGroupTotalLines,       /*!< Total number of lines in the File Group (image) */
            bool forceMetadataBlock,       /*!< Force inclusion of file metadata block in this block. Otherwise it's only included in some lines. */
            Fw::Buffer &fwBuffer           /*!< Buffer containing the data*/
        );

        //! Handler implementation for cmdUplink
        //!
        void cmdUplink_handler(
            const NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::Buffer &fwBuffer);

        PRIVATE :

            // ----------------------------------------------------------------------
            // Command handler implementations
            // ----------------------------------------------------------------------

            //! Implementation for Set_Primary_Interface command handler
            //! Sets the primary interface.
            void
            Set_Primary_Interface_cmdHandler(
                const FwOpcodeType opCode, /*!< The opcode*/
                const U32 cmdSeq,          /*!< The command sequence number*/
                PrimaryInterface primary_interface);

        //! Implementation for Set_GroundInterface_Telemetry_Level command handler
        //! Sets the telemetry level to emit for this component.
        void Set_GroundInterface_Telemetry_Level_cmdHandler(
            const FwOpcodeType opCode, /*!< The opcode*/
            const U32 cmdSeq,          /*!< The command sequence number*/
            TelemetryLevel telemetry_level);

        //! Handler for command SetInterfaceAutoSwitch
        /* Turn ON/OFF whether Hercules is allowed to automatically switch its
        primary network interface based on connection status. Default is ON. */
        void SetInterfaceAutoSwitch_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            bool on);

        //! Handler for command RollCredits
        /* Turn ON/OFF whether names and messages should be downlinked. Default is ON. */
        void RollCredits_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            bool on);

        //! Handler for command Set_NameAndMessage_Period
        /* Set how many seconds (minimum) should occur between each name/message downlink. Min is 1. */
        void Set_NameAndMessage_Period_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            U16 seconds);

        // User defined methods, members, and structs

    public:
        static uint16_t hashTime(uint32_t time); // Used for files to get unique Id for parallel downlinks

        PRIVATE :

            void
            downlinkFileMetadata(uint16_t fileGroupId, uint16_t fileGroupLineNumber, uint16_t fileGroupTotalLines, uint8_t totalBlocks, uint16_t callbackId, uint32_t timestamp_ms);
        void downlinkBufferWrite(void *_data, uint16_t size, downlinkPacketType from);
        void flushTlmDownlinkBuffer();
        void downlink(void *_data, uint16_t size);
        void updateTelemetry();

        FswPacket::Seq_t m_uplinkSeq, m_downlinkSeq;  // TLM0, TLM1
        uint32_t m_packetsRx, m_packetsTx,            // TLM2, TLM3
            m_tlmItemsReceived, m_tlmItemsDownlinked, // TLM4, TLM5
            m_logsReceived, m_logsDownlinked,         // TLM6, TLM7
            m_cmdsUplinked, m_cmdsSent, m_cmdErrs,    // TLM8, TLM9, TLM10
            m_appBytesReceived, m_appBytesDownlinked; // TLM11, TLM 12

        uint8_t m_tlmDownlinkBuffer[WF121_UDP_MAX_PAYLOAD];
        uint8_t m_fileDownlinkBuffer[NUM_APPS_USE_FILE_DOWNLINK][WF121_UDP_MAX_PAYLOAD];
        uint8_t *m_tlmDownlinkBufferPos;
        uint16_t m_downlink_objects_size; // Maximum usable buffer space for the current network interface
        uint16_t m_tlmDownlinkBufferSpaceAvailable;
        PrimaryInterface m_interface_port_num;
        PrimaryInterface m_temp_interface_port_num;
        TelemetryLevel m_telemetry_level;

        // Whether or not automatically switching downlink mode is allowed:
        bool m_auto_switch_allowed = true;

        PRIVATE : void Set_Interface_Auto_Switch(bool on);
        void Switch_Primary_Interface(PrimaryInterface primary_interface);
        void downlinkNameCoreImpl(const char *pHead);
        void downlinkMessageCoreImpl(const char *pMessagerString, const char *pMessageString);
    };

} // end namespace CubeRover

#endif
