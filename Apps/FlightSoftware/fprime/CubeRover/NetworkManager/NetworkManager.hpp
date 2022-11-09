// ======================================================================
// \title  NetworkManagerComponentImpl.hpp
// \author Connor Colombo (colombo@cmu.edu)
// \brief  hpp file for NetworkManager component implementation class.
//
// NOTE: This is pretty much just a pass-through class to `RadioDriver` now
// since actual network management activities are handled by the Radio's
// internal MCU itself. All this does is pass data into and pull data out of
// the `RadioDriver` and handle unresolvable Radio faults (mainly by resetting
// the Radio).
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef NetworkManager_HPP
#define NetworkManager_HPP

#include "CubeRover/NetworkManager/NetworkManagerComponentAc.hpp"
#include "CubeRover/WatchDogInterface/WatchDogInterface.hpp" // need the `reset_values_possible` enum definitions
#include "CubeRover/Wf121/Wf121.hpp"
#include "CubeRover/Wf121/Wf121DirectMessage.hpp"
#include "CubeRover/Wf121/SimpleAsyncFifoBuffer.hpp"

namespace CubeRover
{

    // Target (minimum) time between NetworkManager general telemetry updates,
    // in milliseconds:
    // (note, telemetry triggered by things like a state change may occur more frequently)
    static const uint32_t NETWORK_MANAGER_GENERAL_TELEM_UPDATE_INTERVAL_MS = 5000; // 5s

    // How long (in ms) we'll wait for a Radio heartbeat before assuming it needs
    // a reset (NOTE: Standard heartbeat interval is once every 10s):
    static const uint32_t RADIO_HEARTBEAT_TIMEOUT_MS = 360000; // Several Minutes (6mins) - it might not be HBing b/c it's trying to connect(?) - it should be several X roundtrip comms to Earth (to decrease odds of Earth sending a Radio-Gnd cmd and the Radio being off when it gets there).

    // How many times we'll request the Radio to be reset before we realize the
    // problem might actually be us (Hercules) and ask WD to reset us:
    // NOTE: This is number of consecutive resets without the radio going back
    // into good state (connected).
    static const uint32_t RADIO_RESET_CONSECUTIVE_MAX_COUNT__RESET_HERCULES_THRESH = 5;

    // Define empty Friend class (which autogenerated and uneditable C++ `WatchDogInterfaceComponentBase` Class already friends)
    // so NetworkManager can access protected & private enums from FPrime XML by inheriting `WatchDogInterfaceComponentBaseFriend`:
    class WatchDogInterfaceComponentBaseFriend
    {
    public:
        // Grab any members we want from `WatchDogInterfaceComponentBase`:
        // ID to pass to WatchDog ResetSpecific to reset the radio:
        static const uint8_t WD_RADIO_RESET_ID = WatchDogInterfaceComponentBase::reset_values_possible::Reset_Radio; // 0x04;
        // ID to pass to WatchDog ResetSpecific to reset the Hercules:
        static const uint8_t WD_HERCULES_RESET_ID = WatchDogInterfaceComponentBase::reset_values_possible::Reset_Hercules; // 0x01;
        // (^should be able to pull these straight from `reset_values_possible` enum)
    };

    /*
     * Simple struct containing data about what happened to a command.
     * (See `Send_BgApi_Command_cmdHandler` for more details)
     */
    struct BgApiCommandPassthroughRecord
    {
        uint32_t packetId = 0;
        NetworkManagerComponentImpl::nm_radio_rec0_bgapi_command_ack_status resultingStatus = NetworkManagerComponentImpl::nm_radio_rec0_bgapi_command_ack_status::nm_bgapi_rec0_EMPTY_RECORD;
    };
    // Number of entries to be stored in the
    // `m_bgApiCommandPassthroughRecordBook` queue (ultimately used for
    // `log_WARNING_LO_RadioBgApiCommandRecords`, so this number must be at
    // least as large as the number of records that event expects):
    static const uint8_t NUM_BGAPI_COMMAND_PASSTHROUGH_RECORDS = 3;

    class NetworkManagerComponentImpl : public NetworkManagerComponentBase, public WatchDogInterfaceComponentBaseFriend
    {

    public:
        // ----------------------------------------------------------------------
        // Construction, initialization, and destruction
        // ----------------------------------------------------------------------

        //! Construct object NetworkManager
        //!
        NetworkManagerComponentImpl(
#if FW_OBJECT_NAMES == 1
            const char *const compName /*!< The component name*/
#else
            void
#endif
        );

        //! Initialize object NetworkManager
        //!
        void init(
            const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
        );

        //! Destroy object NetworkManager
        //!
        ~NetworkManagerComponentImpl(void);

        static nm_radio_communications_mode nmCurrentCommunicationMode;

        PRIVATE :

            // ----------------------------------------------------------------------
            // Handler implementations for user-defined typed input ports
            // ----------------------------------------------------------------------

            //! Handler implementation for fileDownlinkBufferSendIn
            //!
            void
            downlink_handler(
                const NATIVE_INT_TYPE portNum, /*!< The port number*/
                Fw::Buffer &fwBuffer);

        //! Handler implementation for schedIn
        //!
        void schedIn_handler(
            const NATIVE_INT_TYPE portNum, /*!< The port number*/
            NATIVE_UINT_TYPE context       /*!< The call order*/
        );

        //! Handler for command Set_Radio_Communications_Mode
        /* Sets the Radio communications mode. For Radio debugging and UART/DFU programming. */
        void Set_Radio_Communications_Mode_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            nm_radio_communications_mode mode);

        //! Handler for command Set_Radio_Uart_Baud
        /* Sets the (peristent) baud rate for UART communication with the
                WF121 Radio. Note: if Radio-Hercules comms appear not to work
                properly after issuing this command, you may have to reboot.

                A `RadioUartBaudRateChange` event is emitted when
                called (if no change was made, `changeMade=FALSE`). */
        void Set_Radio_Uart_Baud_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            U32 newBaud);

        //! Handler for command Set_Radio_BgApi_Passthrough
        /* If TRUE:
                    Turns off the state machine in Hercules managing UART BGAPI
                    communications with the WF121 Radio and begins forwarding all
                    BGAPI responses and events to Ground via the
                    `WatchDog DebugPrintf` pipeline.
                    During this time, the `Send_BgApi_Command` CAN be used to send
                    BGAPI commands to the Radio.

                    If FALSE:
                    Turns back on and resets the state machine in Hercules managing
                    UART BGAPI communications with the WF121 Radio and stops
                    forwarding all BGAPI responses and events to Ground via the
                    `WatchDog DebugPrintf` pipeline.
                    During this time, the `Send_BgApi_Command` CANNOT be used to
                    send BGAPI commands to the Radio.

                    Changes are only made if the passthrough mode is being changed.

                    A `RadioBgApiPassthroughChange` event is emitted when
                    called (if no change was made, `changeMade=FALSE`). */
        void Set_Radio_BgApi_Passthrough_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            bool passthrough);

        //! Handler for command Send_BgApi_Command
        /* Forwards the given BGAPI command (packed as binary) to the
                    Radio.
                    A `RadioSendBgApiCommandAck` event is emitted when this command is received */
        void Send_BgApi_Command_cmdHandler(
            FwOpcodeType opCode,                        /*!< The opcode*/
            U32 cmdSeq,                                 /*!< The command sequence number*/
            U32 crc32,                                  /*!<
                                                             CRC32 of the packed BGAPI packet, as a uint32.
                                                         */
            U32 packetId,                               /*!<
                                                          ID of the packet, assigned by ground. This is just
                                                          included in the response event so ground can know what
                                                          packet to resend if it needs to resend a packet.
                                                      */
            const Fw::IrisCmdByteStringArg &bgapiPacket /*!<
                            The data as a 'string', with a MAX length of 134B
                            (4B of BGAPI header + 1B 'BGAPI uint8array' length byte
                            + 128B of data + 1B null termination). To increase this
                            limit, you'll likely need to bump up
                            'FW_COM_BUFFER_MAX_SIZE' and 'FW_CMD_STRING_MAX_SIZE'
                            in 'fprime/Fw/Cfg/Config.hpp' (read the notes there for
                            more information about required padding). Note that
                            increasing the max string size has a pretty big effect
                            on the total program size in memory.


                            Here string here just means a null terminated char
                            array. To be specific, the NULL termination is EXCLUDED
                            from the length and does NOT need to be (and should not
                            be) included in the data sent. That is, if length is 3,
                            the data sent would be [0x00, 0x03, byte0, byte1, byte2]
                            and the memory in the `CmdStringArg->m_buf` inside
                            Hercules would look like: [byte0, byte1, byte2, NULL].
                        */
        );

        // User defined methods, members, and structs

        // Pointer to the RadioDriver being used (we add this level of indirection
        // and don't just use `CORE_RADIO_DRIVER` in case we want to change what's
        // used in the future):
        Wf121::RadioDriver *m_pRadioDriver;
        // Single common working buffer used for handling RX'd or TX'ing UDP Payloads:
        // (NOT for long term storage of data / passing data out of NetworkManager):
        Wf121::UdpPayload m_udpPayloadWorkingBuffer;

        // Latest WiFi state downlinked by NetworkManager:
        WIFIState m_lastDownlinkedWifiState;

        // Time of last general telemetry downlink, in ms since Hercules boot:
        uint32_t m_lastTelemDownlinkTimeMs;

        // How many times we've asked for the Radio to be reset:
        uint8_t m_radioConsecutiveResetRequestCounter;
        // State of packet counters the last time we ordered a reset
        // (used to determine if Radio is back in good state and we can reset
        // `m_radioConsecutiveResetRequestCounter`):
        uint32_t m_rxPacketCountOnLastReset;
        uint32_t m_txPacketCountOnLastReset;

        // Buffer for data uplinked through the Radio via UDP:
        Wf121::UdpRxPayload m_uplinkBuffer;

        // Records of what happened to the last three uplinked BGAPI commands
        // processed as FIFO queue:
        // (See `Send_BgApi_Command_cmdHandler` for more details)
        Wf121::SimpleAsyncFifoBuffer<BgApiCommandPassthroughRecord, NUM_BGAPI_COMMAND_PASSTHROUGH_RECORDS> m_bgApiCommandPassthroughRecordBook;

        void update(); // Behavior of periodic status update
        void getUplinkDatagram();

        // Helper function to convert RadioSwState (used inside RadioDriver) to
        // WIFIState (used by FPrime telem).
        NetworkManagerComponentBase::WIFIState convertRadioState2WifiState(Wf121::DirectMessage::RadioSwState state);

        // What to do when RadioDriver is says its in a bad state and needs help
        // (i.e. needs NetworkManager to reset something):
        void handleRadioFault();
    };

} // end namespace CubeRover

#endif
