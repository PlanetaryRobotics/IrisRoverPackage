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
    class UdpTxCommsStatusManager
    {
    public:
        // How long to block (yield) the UDP TX Task while awaiting a BGAPI response packet before giving up and timing out:
        // NOTE: this can be long as all we do is suspend the TX task until we get a response:
        static const TickType_t UDP_TX_RESPONSE_TIMEOUT_TICKS = (30 * 1000) / portTICK_PERIOD_MS; // 30 sec

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
            SET_TRANSMIT_SIZE = 0x10,
            SEND_ENDPOINT_UDP = 0x20
        };

        // Sets the response for the given command being awaited:
        void setCommandResponse(AwaitableCommand cmd, BgApi::ErrorCode response);

        // Sets the command response for the currently awaited command (used
        // for things like SyntaxError that could crop up any time and we need
        // to direct to the appropriate Mailbox Queue):
        void setResponseForCurrentlyAwaitedCommand(BgApi::ErrorCode response);

        // Block (yield) the calling task until we get a `SetTransmitSize` response.
        // Return that response or TIMEOUT if we waited too long or
        // INTERNAL__TRY_AGAIN if the messaging system wasn't set up yet or
        // INTERNAL__BAD_SYNTAX if a BadSyntax event was emitted while we were awaiting a response.
        BgApi::ErrorCode awaitResponse_setTransmitSize();
        // Block (yield) the calling task until we get a `SendEndpointUdp` response (for the downlink endpoint).
        // Return that response or TIMEOUT if we waited too long or
        // INTERNAL__TRY_AGAIN if the messaging system wasn't set up yet or
        // INTERNAL__BAD_SYNTAX if a BadSyntax event was emitted while we were awaiting a response.
        BgApi::ErrorCode awaitResponse_sendEndpointUdp();

        // Tell the UdpTxTask that we got the given response code for the `SetTransmitSize` command:
        void setTransmitSize_Response(BgApi::ErrorCode response);

        // Tell the UdpTxTask that we got the given response code for the `SendEndpointUdp` command:
        void sendEndpointUdp_Response(BgApi::ErrorCode response);

    private:
        // Mutex to protect internal data (NOTE: Queues do this themselves)
        ::Os::Mutex mutex;

        QueueHandle_t xQueue_SetTransmitSize_Response;
        QueueHandle_t xQueue_SendEndpointUdp_Response;

        // Internal ID for command we're currently awaiting a response for:
        AwaitableCommand currentlyAwaitedCommand;

        // Get what command we're currently awaiting a response for (in a mutex-safe way):
        AwaitableCommand getCurrentlyAwaitedCommand();
        // Set what command we're currently awaiting a response for (in a mutex-safe way):
        void setCurrentlyAwaitedCommand(AwaitableCommand cmd);

        // Helper function to block (yield) the calling task until we get a
        // response for the given queue correspond to the given `AwaitableCommand`.
        // Return that response or TIMEOUT if we waited too long or
        // INTERNAL__TRY_AGAIN if the messaging system wasn't set up yet or
        // INTERNAL__BAD_SYNTAX if a BadSyntax event was emitted while we were awaiting a response.
        BgApi::ErrorCode awaitResponse(AwaitableCommand cmd, QueueHandle_t *blockingQueue);
    };
}

#endif /* CUBEROVER_WF121_WF121_UDP_TX_COMMS_STATUS_MANAGER_HPP_ */
