/**
 * @file UdpTxCommsStatusManager.hpp
 * @author Connor Colombo (colombo@cmu.edu)
 * @brief Class for managing inter-process communication about the UDP TX comms
 * status.
 *
 * @date 2022-08-21
 */

#ifndef CUBEROVER_WF121_WF121_UDP_TX_COMMS_STATUS_MANAGER_HPP_
#define CUBEROVER_WF121_WF121_UDP_TX_COMMS_STATUS_MANAGER_HPP_

#include <CubeRover/Wf121/Wf121BgApi.hpp>
#include <CubeRover/Wf121/Wf121DirectMessage.hpp>

#include <HAL/include/FreeRTOS.h>
#include <HAL/include/os_task.h>
#include <Fw/Time/Time.hpp>

#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>

#include <cassert>
#include <cstring>
#include <cstdio>

#include "FreeRTOS.h"
#include "os_queue.h"

namespace Wf121
{
    // How long it takes for the UDP interlock to expire automatically.
    // In milliseconds since we were last given the interlock.
    // For more details on the interlock see `ground_reports.bgs` in the Radio
    // firmware.
    // ! Check `HERCULES_INTERLOCK_PERIOD_MS` in ground_reports.bgs` in the Radio
    // firmware. These two numbers should match.
    static uint32_t UDP_INTERLOCK_EXPIRATION_TIME_MS = 1350;

    class UdpTxCommsStatusManager
    {
    public:
        // How long to block (yield) the UDP TX Task while awaiting a BGAPI response packet before giving up and timing out:
        // NOTE: this can be long as all we do is suspend the TX task until we get a response:
        // To prevent losing interlock while waiting, let's make this less than the interlock period (which is set as 1350ms in the now immutable Radio FW)
        static const TickType_t UDP_TX_RESPONSE_TIMEOUT_TICKS = (1100) / portTICK_PERIOD_MS;

        UdpTxCommsStatusManager();

        // Set up FreeRTOS Mailbox Queues, etc:
        void init();

        // Reset all mailbox queues to their empty state (we're starting a new send operation):
        void reset();

        // All the different commands we could be awaiting a response for:
        // (NOTE: this is internal so values are arbitrary and don't correspond to anything in BGAPI).
        enum class AwaitableCommand
        {
            NONE = 0x00, // Not currently awaiting anything:
            GET_UDP_INTERLOCK = 0x0A,
            SET_TRANSMIT_SIZE = 0x10,
            SEND_ENDPOINT_UDP = 0x20
        };

        // Sets the response for the given command being awaited:
        void setCommandResponse(AwaitableCommand cmd, BgApi::ErrorCode response);

        // Sets the command response for the currently awaited command (used
        // for things like SyntaxError that could crop up any time and we need
        // to direct to the appropriate Mailbox Queue):
        void setResponseForCurrentlyAwaitedCommand(BgApi::ErrorCode response);

        // Sets up everything so any new `getUdpInterlock` command responses will be
        // captured by the appropriate Mailbox Queue.
        // This allows us to collect data (e.g. really fast commands responses)
        // after we send the command but before we start the blocking await.
        //
        // Returns `true` if we were able to successfully set everything up and
        // start listening (`false` if, for ex, the queue was init'd yet).
        bool startListeningFor_getUdpInterlock_Response();
        // Sets up everything so any new `setTransmitSize` command responses will be
        // captured by the appropriate Mailbox Queue.
        // This allows us to collect data (e.g. really fast commands responses)
        // after we send the command but before we start the blocking await.
        //
        // Returns `true` if we were able to successfully set everything up and
        // start listening (`false` if, for ex, the queue was init'd yet).
        bool startListeningFor_setTransmitSize_Response();
        // Sets up everything so any new `sendEndpointUdp` command responses will be
        // captured by the appropriate Mailbox Queue.
        // This allows us to collect data (e.g. really fast commands responses)
        // after we send the command but before we start the blocking await.
        //
        // Returns `true` if we were able to successfully set everything up and
        // start listening (`false` if, for ex, the queue was init'd yet).
        bool startListeningFor_sendEndpointUdp_Response();

        // Block (yield) the calling task until we get a `GetUdpInterlock` response.
        // Return that response or TIMEOUT if we waited too long or
        // INTERNAL__TRY_AGAIN if the messaging system wasn't set up yet or
        // INTERNAL__BAD_SYNTAX if a BadSyntax event was emitted while we were awaiting a response.
        // INTERNAL__LOST_INTERLOCK if we lose the interlock while awaiting this response.
        BgApi::ErrorCode awaitResponse_getUdpInterlock();
        // Block (yield) the calling task until we get a `SetTransmitSize` response.
        // Return that response or TIMEOUT if we waited too long or
        // INTERNAL__TRY_AGAIN if the messaging system wasn't set up yet or
        // INTERNAL__BAD_SYNTAX if a BadSyntax event was emitted while we were awaiting a response.
        // INTERNAL__LOST_INTERLOCK if we lose the interlock while awaiting this response.
        BgApi::ErrorCode awaitResponse_setTransmitSize();
        // Block (yield) the calling task until we get a `SendEndpointUdp` response (for the downlink endpoint).
        // Return that response or TIMEOUT if we waited too long or
        // INTERNAL__TRY_AGAIN if the messaging system wasn't set up yet or
        // INTERNAL__BAD_SYNTAX if a BadSyntax event was emitted while we were awaiting a response.
        // INTERNAL__LOST_INTERLOCK if we lose the interlock while awaiting this response.
        BgApi::ErrorCode awaitResponse_sendEndpointUdp();

        // Helper function to process the given response for the given Mailbox Queue:
        static void processMailboxResponse(BgApi::ErrorCode response, QueueHandle_t *queue);

        // Tell the UdpTxTask that we got the given response code for the `GetUdpInterlock` command:
        void getUdpInterlock_Response(BgApi::ErrorCode response);

        // Tell the UdpTxTask that we got the given response code for the `SetTransmitSize` command:
        void setTransmitSize_Response(BgApi::ErrorCode response);

        // Tell the UdpTxTask that we got the given response code for the `SendEndpointUdp` command:
        void sendEndpointUdp_Response(BgApi::ErrorCode response);

        // Set the latest interlock status and update the acquisition time if
        // we got an update.
        void setLatestUdpInterlockStatus(DirectMessage::RadioUdpInterlockStatus status);
        // Get the current interlock status, accounting for the expiration time
        // (i.e. if our latest update from the Radio said we had the lock but
        // it's been too long since we got that update, we'll infer it's
        // expired but we missed the message).
        DirectMessage::RadioUdpInterlockStatus getUdpInterlockStatus();

    private:
        // Mutex to protect internal data (NOTE: the Queues do this themselves)
        ::Os::Mutex mutex;

        QueueHandle_t xQueue_GetUdpInterlock_Response;
        QueueHandle_t xQueue_SetTransmitSize_Response;
        QueueHandle_t xQueue_SendEndpointUdp_Response;

        // Internal ID for command we're currently awaiting a response for:
        AwaitableCommand currentlyAwaitedCommand;

        // Get what command we're currently awaiting a response for (in a mutex-safe way):
        AwaitableCommand getCurrentlyAwaitedCommand();
        // Set what command we're currently awaiting a response for (in a mutex-safe way):
        void setCurrentlyAwaitedCommand(AwaitableCommand cmd);

        // Helper function that readies us to perform a blocking await for a command
        // response. This allows us to collect data (e.g. really fast commands
        // responses) after we send the command but before we start the blocking
        // await for it.
        // This was added out of necessity as, during testing, it was observed that
        // many command responses came in almost immediately, before the UDP TX
        // state machine was able to advance into an awaiting state.
        //
        // Returns `true` if we were able to successfully set everything up and
        // start listening (`false` if, for ex, the queue was init'd yet).
        bool prepareToAwaitResponse(AwaitableCommand cmd, QueueHandle_t *blockingQueue);

        // Helper function to block (yield) the calling task until we get a
        // response for the given queue correspond to the given `AwaitableCommand`.
        // Return that response or TIMEOUT if we waited too long or
        // INTERNAL__TRY_AGAIN if the messaging system wasn't set up yet or
        // INTERNAL__BAD_SYNTAX if a BadSyntax event was emitted while we were awaiting a response.
        BgApi::ErrorCode awaitResponse(AwaitableCommand cmd, QueueHandle_t *blockingQueue);

        // Status of the UDP interlock according to the latest update we've
        // received from the Radio (NOTE: as a matter of principle, this never
        // reflects our time-based expiration inferences. That's what
        // `getUdpInterlockStatus` is for.)
        //
        // We store this in addition to pushing the update to the awaiting
        // mailboxes because we need to obey the interlock even when we're not
        // awaiting a command response.
        DirectMessage::RadioUdpInterlockStatus latestUpdatedInterlockStatus;
        // Timestamp (in milliseconds since boot based on FreeRTOS scheduler
        // ticks) of the last time we **acquired** the UDP interlock. UDP
        // interlock expires after some time so we want to keep track of this
        // so we can infer that it's expired even if we haven't been told or
        // missed the message telling us so.
        uint32_t lastInterlockAcquisitionTimeMs;
    };
}

#endif /* CUBEROVER_WF121_WF121_UDP_TX_COMMS_STATUS_MANAGER_HPP_ */
