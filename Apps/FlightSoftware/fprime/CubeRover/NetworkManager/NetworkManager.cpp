// ======================================================================
// \title  NetworkManagerComponentImpl.hpp
// \author Connor Colombo (colombo@cmu.edu)
// \brief  cpp file for NetworkManager component implementation class.
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

#include <CubeRover/NetworkManager/NetworkManager.hpp>
#include <CubeRover/GroundInterface/GroundInterface.hpp>
#include <CubeRover/WatchDogInterface/WatchDogInterface.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <cstring>

#include <CubeRover/Wf121/Timestamp.hpp>
#include <CubeRover/Wf121/Wf121DirectMessage.hpp>
#include <CubeRover/Wf121/Wf121SerialInterface.hpp>
#include <CubeRover/Wf121/NetworkInterface.hpp>
#include <CubeRover/Wf121/Timestamp.hpp>

extern CubeRover::WatchDogInterfaceComponentImpl watchDogInterface;

namespace CubeRover
{
    // Local helper function that computes the CRC32 of the given buffer:
    static uint32_t computeCrc32(const char *bufferData, const NATIVE_INT_TYPE bufferLen)
    {
        // Compute CRC32 of data:
        unsigned long computedCrc32 = 0xffffffffL;
        FW_ASSERT(bufferData);
        char c;
        for (int index = 0; index < bufferLen; index++)
        {
            c = ((char *)bufferData)[index];
            computedCrc32 = update_crc_32(computedCrc32, c);
        }
        // For CRC32 we need to return the one's compliment of the result:
        computedCrc32 = ~(computedCrc32);
        return computedCrc32;
    }

    // Set nmCurrentCommunicationMode to the default:
    NetworkManagerComponentImpl::nm_radio_communications_mode NetworkManagerComponentImpl::nmCurrentCommunicationMode = NetworkManagerComponentImpl::nm_radio_communications_mode::HERCULES;

    // STATICALLY allocate a SINGLE copy of the RadioDriver (both of these are
    // very important for proper set up of the internal Tasks):
    static Wf121::RadioDriver CORE_RADIO_DRIVER;

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    NetworkManagerComponentImpl ::
#if FW_OBJECT_NAMES == 1
        NetworkManagerComponentImpl(
            const char *const compName) : NetworkManagerComponentBase(compName),
                                          m_pRadioDriver(&CORE_RADIO_DRIVER),
                                          m_bgApiMsgOutWorkingBuffer(),
                                          m_bgApiStatusInWorkingBuffer(),
                                          m_bgApiCommandPassthroughRecordBook()
#else
        NetworkManagerComponentImpl(void) : m_pRadioDriver(&CORE_RADIO_DRIVER),
                                            m_bgApiMsgOutWorkingBuffer(),
                                            m_bgApiStatusInWorkingBuffer(),
                                            m_bgApiCommandPassthroughRecordBook()
#endif
    {
        // Initialize process variables:
        m_lastTelemDownlinkTimeMs = 0;
        m_radioConsecutiveResetRequestCounter = 0;
        m_rxPacketCountOnLastReset = 0;
        m_txPacketCountOnLastReset = 0;
        m_lastDownlinkedWifiState = convertRadioState2WifiState(Wf121::DirectMessage::RadioSwState::NONE);

        // Initialize working buffers with known sentinel bytes:
        m_bgApiMsgOutWorkingBuffer.clear();
    }

    void NetworkManagerComponentImpl ::
        init(const NATIVE_INT_TYPE instance)
    {
        // Init the base FPrime component:
        NetworkManagerComponentBase::init(instance);

        // Initialize the working buffer with known sentinel bytes:
        m_udpPayloadWorkingBuffer.clear();
        // Initialize the uplink buffer with known sentinel bytes:
        m_uplinkBuffer.clear();

        // Init the RadioDriver (and all its sub-tasks):
        m_pRadioDriver->init();

        // Log the initial Radio UART baud rate (can be changed and is persistent):
        log_ACTIVITY_HI_RadioUartBaudRateChange(
            false, // no change is being made here
            // from and to are the same because no change is being made here:
            Wf121::Wf121Serial::getWf121SciBaud(),
            Wf121::Wf121Serial::getWf121SciBaud());

        // Log the initial BGAPI Passthrough state (can be changed and is persistent):
        log_ACTIVITY_HI_RadioBgApiPassthroughChange(
            false, // no change is being made here
            // from and to are the same because no change is being made here:
            Wf121::persistentBgApiPassthroughEnabled(),
            Wf121::persistentBgApiPassthroughEnabled());
    }

    NetworkManagerComponentImpl ::
        ~NetworkManagerComponentImpl(void)
    {
        // This should NEVER get called
    }

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    void NetworkManagerComponentImpl ::
        downlink_handler(
            const NATIVE_INT_TYPE portNum,
            Fw::Buffer &fwBuffer)
    {
        // Grab the data from the FW Buffer:
        uint8_t *buffer = reinterpret_cast<uint8_t *>(fwBuffer.getdata());

        // Copy it into the outbound payload:
        m_udpPayloadWorkingBuffer.copyIn(fwBuffer.getsize(), buffer);

        // Queue up the Payload for downlink:
        m_pRadioDriver->m_networkInterface.sendUdpPayload(&m_udpPayloadWorkingBuffer);
    }

    void NetworkManagerComponentImpl ::
        schedIn_handler(
            const NATIVE_INT_TYPE portNum,
            NATIVE_UINT_TYPE context)
    {
        // Perform standard updates:
        update();
        // Check for any new asynchronous responses to BGAPI Passthrough
        // messages (and handle them):
        checkForBgApiPassthroughResponse();
        // See if there's any available uplinked data:
        getUplinkDatagram();
    }

    //! Handler for command Set_Radio_Communications_Mode
    /* Sets the Radio communications mode. For Radio debugging and UART/DFU programming. */
    void NetworkManagerComponentImpl::Set_Radio_Communications_Mode_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        nm_radio_communications_mode mode)
    {
        if (nmCurrentCommunicationMode == nm_radio_communications_mode::HERCULES)
        {
            // If currently in HERCULES (default) mode, deinit to switch to EXTERNAL mode:
            Wf121::Wf121Serial::deinit();
            log_ACTIVITY_HI_RadioCommunicationsModeChange(static_cast<nm_radio_communications_mode_from>(nmCurrentCommunicationMode), nm_radio_communications_mode_to::to_EXTERNAL);
            nmCurrentCommunicationMode = nm_radio_communications_mode::EXTERNAL;
        }
        else
        {
            // If currently in EXTERNAL mode (or, just, not the default due to a bitflip),
            // deinit to switch to HERCULES (default) mode:
            Wf121::Wf121Serial::reinit();
            log_ACTIVITY_HI_RadioCommunicationsModeChange(static_cast<nm_radio_communications_mode_from>(nmCurrentCommunicationMode), nm_radio_communications_mode_to::to_HERCULES);
            nmCurrentCommunicationMode = nm_radio_communications_mode::HERCULES;
        }
        // Flag the end state (even if a change wasn't made):
        log_ACTIVITY_LO_RadioCommunicationsModeState(static_cast<nm_radio_communications_mode_now>(nmCurrentCommunicationMode));

        // Signal that we're done:
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

    //! Handler for command Set_Radio_Uart_Baud
    /* Sets the (persistent) baud rate for UART communication with the
            WF121 Radio. Note: if Radio-Hercules comms appear not to work
            properly after issuing this command, you may have to reboot.

            A `RadioUartBaudRateChange` event is emitted when
            called (if no change was made, `changeMade=FALSE`). */
    void NetworkManagerComponentImpl::Set_Radio_Uart_Baud_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        U32 newBaud)
    {
        uint32_t initialBaud = Wf121::Wf121Serial::getWf121SciBaud();
        Wf121::Wf121Serial::changeUartBaud(newBaud);
        // Check that the value actually changed (was valid):
        uint32_t actualNewBaud = Wf121::Wf121Serial::getWf121SciBaud();
        log_ACTIVITY_HI_RadioUartBaudRateChange(
            (initialBaud != actualNewBaud),
            initialBaud,
            actualNewBaud);

        // Signal that we're done:
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

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
    void NetworkManagerComponentImpl::Set_Radio_BgApi_Passthrough_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq,          /*!< The command sequence number*/
        bool passthrough)
    {
        bool initialState = Wf121::persistentBgApiPassthroughEnabled();
        bool changeMade = Wf121::changeBgApiPassthroughState(passthrough);
        log_ACTIVITY_HI_RadioBgApiPassthroughChange(
            changeMade,
            initialState,
            Wf121::persistentBgApiPassthroughEnabled());

        // Signal that we're done:
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

    //! Handler for command Send_BgApi_Command
    /* Forwards the given BGAPI command (packed as binary) to the
                Radio.
                A `RadioSendBgApiCommandAck` event is emitted when this command is received */
    void NetworkManagerComponentImpl::Send_BgApi_Command_cmdHandler(
        FwOpcodeType opCode,                                                                     /*!< The opcode*/
        U32 cmdSeq,                                                                              /*!< The command sequence number*/
        U32 crc32,                                                                               /*!<
                                                                                                      CRC32 of the packed BGAPI packet, as a uint32.
                                                                                                  */
        U32 packetId,                                                                            /*!<
                                                                                                   ID of the packet, assigned by ground. This is just
                                                                                                   included in the response event so ground can know what
                                                                                                   packet to resend if it needs to resend a packet.
                                                                                               */
        NetworkManagerComponentImpl::nm_radio_send_bgapi_command_expect_response expectResponse, /*!<
                                                                                                    Whether or not we should expect (and wait for) a BGAPI
                                                                                                    response from the Radio after sending this command
                                                                                                    (certain BGAPI DFU flashing operations don't return a
                                                                                                    response). Setting this correctly will ensure fast and
                                                                                                    reliable data transfers. Using an enum here instead of
                                                                                                    a bool because it's easier to detect corruption with
                                                                                                */
        const Fw::IrisCmdByteStringArg &bgapiPacket                                              /*!<
                                                                                                  The data as a 'byte string', with a MAX length of 134B
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
    )
    {
        bool good_so_far = true;
        nm_radio_send_bgapi_command_ack_status status;

        // Grab data:
        const char *bgapiPacketData = bgapiPacket.toChar();
        NATIVE_INT_TYPE bgApiPacketLen = bgapiPacket.length();

        // Make sure len makes sense:
        if (bgApiPacketLen == 0 || bgApiPacketLen > WF121_BGAPI_PASSTHROUGH_MAX_MESSAGE_SIZE)
        {
            // Bad length received (decoded). Nothing was sent to the Radio.
            // Either 0 or bigger than max possible size: WF121_BGAPI_PASSTHROUGH_MAX_MESSAGE_SIZE:
            status = nm_radio_send_bgapi_command_ack_status::nm_bgapi_send_BAD_LEN;
            good_so_far = false;
        }

        // Validate expectResponse (really important this is correct):
        bool expectResponseBool;
        if (good_so_far)
        {
            if (
                expectResponse != nm_radio_send_bgapi_command_expect_response::nm_bgapi_cmd_EXPECT_RESPONSE &&
                expectResponse != nm_radio_send_bgapi_command_expect_response::nm_bgapi_cmd_DONT_EXPECT_RESPONSE)
            {
                // Expect response doesn't match any of the allowed values.
                // Data corruption likely. Don't continue and let ground know:
                status = nm_radio_send_bgapi_command_ack_status::nm_bgapi_send_BAD_EXPECT_RESPONSE_VAL;
                good_so_far = false;
            }
            else
            {
                expectResponseBool = expectResponse == nm_radio_send_bgapi_command_expect_response::nm_bgapi_cmd_EXPECT_RESPONSE;
            }
        }

        // Validate data CRC:
        U32 computedCrc32;
        if (good_so_far)
        {
            // Compute CRC32 of data:
            computedCrc32 = computeCrc32(bgapiPacketData, bgApiPacketLen);

            // Check CRC32
            if (crc32 != computedCrc32)
            {
                // Computed CRC of data received did not match target CRC received.
                status = nm_radio_send_bgapi_command_ack_status::nm_bgapi_send_CRC_FAIL;
                good_so_far = false;
            }
        }

        // Send out results and pass on to Task if necessary:
        if (good_so_far)
        {
            // Data passes validation. Send it off to
            // `Wf121BgApiPassthroughTxTask` to be sent to the Radio
            // (asynchronously so this Task doesn't have to be blocked).
            // NOTE: we only send one `log_WARNING_LO_RadioSendBgApiCommandAck`
            // per packet so that will be sent once we get a final status from
            // `Wf121BgApiPassthroughTxTask` via
            // `Wf121BgApiPassthroughTxTask::getMessageResponse`.
            // This is handled in `schedIn_handler`.
            m_bgApiMsgOutWorkingBuffer.packetId = packetId;
            m_bgApiMsgOutWorkingBuffer.dataLen = bgApiPacketLen;
            m_bgApiMsgOutWorkingBuffer.expectResponse = expectResponseBool;
            memcpy(m_bgApiMsgOutWorkingBuffer.rawData, bgapiPacketData, bgApiPacketLen);
            m_pRadioDriver->m_serialBgApiPassthroughTxTask.enqueueMessage(&m_bgApiMsgOutWorkingBuffer);

            // Signal that we're done (and the command worked okay, results are pending...)
            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
        }
        else
        {
            // Data failed validation somewhere, let Ground know that this
            // packet won't be downlinked and why:

            // Record the results in the record book (in case our ACK doesn't make
            // it to ground and it needs to request records of what happened):
            m_bgApiCommandPassthroughRecordBook.force_enqueue( //-
                {
                    .packetId = packetId,
                    .resultingStatus = static_cast<nm_radio_rec0_bgapi_command_ack_status>(status) //-
                }                                                                                  //-
            );

            // NOTE: Not actually a warning but sent using the `WARNING_LO` queue
            // because it has high importance, a (comparatively) large buffer,
            // and not many events use the `WARNING_LO` queue.
            log_WARNING_LO_RadioSendBgApiCommandAck(
                packetId,
                crc32,
                computedCrc32,
                status //-
            );

            // Signal that we're done:
            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_VALIDATION_ERROR);
        }
    }

    //! Handler for command Downlink_BgApi_Command_Records
    /* Triggers a `RadioBgApiCommandRecords` event to see what BgApi
                packets have been processed recently and what the outcomes were. */
    void NetworkManagerComponentImpl::Downlink_BgApi_Command_Records_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq           /*!< The command sequence number*/
    )
    {
        // Grab records:
        // (NOTE: FIFO queue reads them out oldest-first, but we'll
        // downlink the records with oldest-last).
        BgApiCommandPassthroughRecord records[NUM_BGAPI_COMMAND_PASSTHROUGH_RECORDS];
        m_bgApiCommandPassthroughRecordBook.straighten_into(records);

        // Send the log:
        log_WARNING_LO_RadioBgApiCommandRecords(
            records[2].packetId,
            static_cast<nm_radio_rec0_bgapi_command_ack_status>(records[2].resultingStatus),
            records[1].packetId,
            static_cast<nm_radio_rec1_bgapi_command_ack_status>(records[1].resultingStatus),
            records[0].packetId,
            static_cast<nm_radio_rec2_bgapi_command_ack_status>(records[0].resultingStatus));

        // Signal that we're done:
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

    // Helper function to convert RadioSwState (used inside RadioDriver) to
    // WIFIState (used by FPrime telem).
    // See `RadioSwState` in `Wf121/Wf121DirectMessage.hpp` for more details on
    // each state.
    //  TODO: [CWC] Consider also adding a RadioSwActivity telem item?
    NetworkManagerComponentBase::WIFIState NetworkManagerComponentImpl::convertRadioState2WifiState(Wf121::DirectMessage::RadioSwState state)
    {
        switch (state)
        {
        case Wf121::DirectMessage::RadioSwState::NONE:
            return WIFIState::nm_NONE;

        case Wf121::DirectMessage::RadioSwState::BAD_MESSAGE:
            return WIFIState::nm_BAD_MESSAGE;

        case Wf121::DirectMessage::RadioSwState::BOOT:
            return WIFIState::nm_BOOT;

        case Wf121::DirectMessage::RadioSwState::INIT:
            return WIFIState::nm_INIT;

        case Wf121::DirectMessage::RadioSwState::WIFI_ON:
            return WIFIState::nm_WIFI_ON;

        case Wf121::DirectMessage::RadioSwState::CONNECTED:
            return WIFIState::nm_CONNECTED;

        case Wf121::DirectMessage::RadioSwState::UDP_CONNECTED:
            return WIFIState::nm_UDP_CONNECTED;

        // Bad state (we don't know what's really going on inside the Radio rn):
        default:
            return WIFIState::nm_BAD_MESSAGE;
        }
    }

    // Check for any new asynchronous responses to BGAPI Passthrough
    // messages (and handle them):
    void NetworkManagerComponentImpl::checkForBgApiPassthroughResponse()
    {
        // Check for a response:
        // NOTE: The vast majority of the times this function is called,
        // nothing will be available so it's very important that blockingTicks
        // stays at 0 here since it's called frequently by the scheduler.
        bool gotResponse = m_pRadioDriver->m_serialBgApiPassthroughTxTask.getMessageResponse( //-
            &m_bgApiStatusInWorkingBuffer, 0                                                  //-
        );

        // Handle a response:
        if (gotResponse)
        {
            // Record the results in the record book (in case our ACK doesn't make
            // it to ground and it needs to request records of what happened):
            m_bgApiCommandPassthroughRecordBook.force_enqueue( //-
                {
                    .packetId = m_bgApiStatusInWorkingBuffer.packetId,
                    .resultingStatus = static_cast<nm_radio_rec0_bgapi_command_ack_status>(m_bgApiStatusInWorkingBuffer.resultingStatus) //-
                }                                                                                                                        //-
            );

            // NOTE: Not actually a warning but sent using the `WARNING_LO` queue
            // because it has high importance, a (comparatively) large buffer,
            // and not many events use the `WARNING_LO` queue.
            //
            // NOTE: Since, if we're here, the uplinked command necessarily
            // passed validation, we'll just downlink all 0xFF for the CRC bytes.
            // The alternatives would be:
            // 1.) Adding another log for this condition that doesn't have CRC
            // args (more program memory).
            // 2.) Passing the CRCs through every stage of the pipeline just so
            // they could get here (more RAM, which is especially precious).
            // Neither is really worth using since the alternative here is just
            // a tiny amount more comms data (which will only be used during
            // our 50kbps period).
            log_WARNING_LO_RadioSendBgApiCommandAck(
                m_bgApiStatusInWorkingBuffer.packetId,
                0xFF'FF'FF'FF,
                0xFF'FF'FF'FF,
                static_cast<nm_radio_send_bgapi_command_ack_status>(m_bgApiStatusInWorkingBuffer.resultingStatus) //-
            );
        }
    }

    void NetworkManagerComponentImpl::update()
    {
        // Flag to make sure all available telem is downlinked on the first call:
        static bool first_update_call = false;

        // Get the current time (right at the start of the update),
        // in ms since Hercules boot:
        uint32_t now = Wf121::Timestamp::getTimeMs();

        // See if we need to emit new telem because of a `RadioSwState` change:
        WIFIState currentWifiState = convertRadioState2WifiState(
            m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getRadioState());
        if (first_update_call || m_lastDownlinkedWifiState != currentWifiState)
        {
            log_ACTIVITY_HI_StateChange(static_cast<networkmanager_state_from>(m_lastDownlinkedWifiState), static_cast<networkmanager_state_to>(currentWifiState));
            tlmWrite_WIFIStateStatus(currentWifiState);
            m_lastDownlinkedWifiState = currentWifiState;
        }

        // See if it's time for us to update general WiFi telemetry:
        if ((now - m_lastTelemDownlinkTimeMs) >= NETWORK_MANAGER_GENERAL_TELEM_UPDATE_INTERVAL_MS)
        {
            m_lastTelemDownlinkTimeMs = now;
            tlmWrite_RSSI(m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getRssi());
            // Note: SNR is defunct (never really something we had live access to)
            // BUT we're going to trojan it for the time being in order to get numCompleteDirectMessages out so we can assess Radio-Herc Comms Health:
            tlmWrite_SNR(m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getNumCompleteDirectMessages() % 0xFF);
            tlmWrite_PktRecv(m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getUdpRxPacketCount());
            tlmWrite_PktSent(m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getUdpTxPacketCount());

            // Repeat the state now (in case it was lost before):
            tlmWrite_WIFIStateStatus(m_lastDownlinkedWifiState);

            // watchDogInterface.debugPrintfToWatchdog("RADIO: NM watching u%u d%u", m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getUplinkEndpoint(), m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getDownlinkEndpoint());
        }

        // Check if Radio has gotten back into a good state since the last reset
        // request, meaning we can reset the counter since any failure wouldn't
        // be consecutive with the last.
        // GOOD STATE is defined as:
        //      - UDP_CONNECTED
        //      - RX packet count has increased since reset (we can receive uplink data via Radio)
        //      - TX packet count has increased since reset (we can send downlink data through the Radio)
        if (
            m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getRadioState() == Wf121::DirectMessage::RadioSwState::UDP_CONNECTED &&
            m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getUdpRxPacketCount() > m_rxPacketCountOnLastReset &&
            m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getUdpTxPacketCount() > m_txPacketCountOnLastReset)
        {
            m_radioConsecutiveResetRequestCounter = 0;
        }

        uint32_t lastHeartbeatTimeMs = m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getLastHeartbeatTimeMs();
        // update `now` **AFTER** grabbing the heartbeat time so there's no chance the Heartbeat comes in between
        // getting `now` and checking the delta (i.e. so there's no chance heartbeatTime > now unless there's been an
        // overflow, which is okay):
        now = Wf121::Timestamp::getTimeMs();
        if (m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.needHelpCriticalCommsFailureOccurred() ||
            (now - lastHeartbeatTimeMs) > RADIO_HEARTBEAT_TIMEOUT_MS)
        {
            handleRadioFault();
            // Reset the critical flag (so we don't trigger this again on the next loop unless there's been another fault):
            m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.setCriticalCommsFailure_NeedHelp(false);
        }

        if (first_update_call)
        {
            first_update_call = false;
        }
    }

    // What to do when RadioDriver is says its in a bad state and needs help
    // (i.e. needs NetworkManager to reset something):
    // NOTE: This only really triggers if the Radio:
    //      A.) appears to be dead (isn't giving us heartbeats)
    //      B.) appears to be alive and connected (giving us heartbeats that
    //          say it's connected) BUT isn't responding to us sending it
    //          commands (i.e. Radio isn't receiving inbound UART comms).
    void NetworkManagerComponentImpl::handleRadioFault()
    {
        if (m_radioConsecutiveResetRequestCounter <= RADIO_RESET_CONSECUTIVE_MAX_COUNT__RESET_HERCULES_THRESH)
        {
            // Reset the Radio:
            watchDogInterface.Reset_Specific_Handler(WD_RADIO_RESET_ID);
            // TODO: [CWC] This (^) is how resets were done before but shouldn't we be using `WatchdogResetRequest` for this? Investigate.
            m_radioConsecutiveResetRequestCounter++;

            // Log the current packet counts (so we can see if the Radio gets back into a good state after the reset with bidirectional comms working):
            m_rxPacketCountOnLastReset = m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getUdpRxPacketCount();
            m_txPacketCountOnLastReset = m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.getUdpTxPacketCount();

            // Reset the Radio State data (since we expect the Radio is resetting):
            // Set the RadioSwState back to NONE (i.e. we don't know it anymore until the Radio wakes back up and gives us another Heartbeat):
            m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.setRadioState(Wf121::DirectMessage::RadioSwState::NONE);
            // Set last Heartbeat time to right now (so we don't automatically trigger another reset on the next `update` call):
            m_pRadioDriver->m_networkInterface.m_protectedRadioStatus.updateLastHeartbeatTime();
        }
        else
        {
            // We've asked for the Radio to be reset too many times without
            // good data from the Radio. Maybe we're the problem?
            watchDogInterface.Reset_Specific_Handler(WD_HERCULES_RESET_ID);
            // since we aren't asking for anything to happen to the radio, we
            // don't need to update any of the status info like we do above
            // when resetting the Radio.
        }
    }

    void NetworkManagerComponentImpl::getUplinkDatagram()
    {
        if (m_pRadioDriver->m_networkInterface.getAvailableUdpPayload(&m_uplinkBuffer))
        {
            // An RX payload was available and we got it:
            Fw::Buffer buffer(0, 0, reinterpret_cast<U64>(m_uplinkBuffer.data), m_uplinkBuffer.dataSize);
            uplink_out(0, buffer);
        }
    }

} // end namespace CubeRover
