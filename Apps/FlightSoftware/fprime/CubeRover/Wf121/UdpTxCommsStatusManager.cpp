#include <CubeRover/Wf121/UdpTxCommsStatusManager.hpp>

namespace Wf121
{
    // ErrorCode Queue to push the Radio's response to `SetTransmitSize`:
    // (size of one makes this like a cross between a semaphore and a mailbox)
    // Allows the UdpTxTask to know not just that we got a response but
    // also what kind of response it was (success/fail):
    static StaticQueue_t xStaticQueue_SetTransmitSize_Response;
    static uint8_t ucQueueStorageArea_SetTransmitSize_Response[1 * sizeof(BgApi::ErrorCode)];
    // ErrorCode Queue to push the Radio's response to `SendEndpointUdp`:
    // (size of one makes this like a cross between a semaphore and a mailbox)
    // Allows the UdpTxTask to know not just that we got a response but
    // also what kind of response it was (success/fail):
    static StaticQueue_t xStaticQueue_SendEndpointUdp_Response;
    static uint8_t ucQueueStorageArea_SendEndpointUdp_Response[1 * sizeof(BgApi::ErrorCode)];

    UdpTxCommsStatusManager::UdpTxCommsStatusManager() : xQueue_SetTransmitSize_Response(NULL),                                   // null until init.
                                                         xQueue_SendEndpointUdp_Response(NULL),                                   // null until init.
                                                         currentlyAwaitedCommand(UdpTxCommsStatusManager::AwaitableCommand::NONE) // waiting on nothing by default
    {
    }

    void UdpTxCommsStatusManager::init()
    {
        // Create the mailbox queues:
        this->xQueue_SetTransmitSize_Response = xQueueCreateStatic(1,
                                                                   sizeof(BgApi::ErrorCode),
                                                                   ucQueueStorageArea_SetTransmitSize_Response,
                                                                   &xStaticQueue_SetTransmitSize_Response);
        configASSERT(xQueue_SetTransmitSize_Response);
        this->xQueue_SendEndpointUdp_Response = xQueueCreateStatic(1,
                                                                   sizeof(BgApi::ErrorCode),
                                                                   ucQueueStorageArea_SendEndpointUdp_Response,
                                                                   &xStaticQueue_SendEndpointUdp_Response);
        configASSERT(xQueue_SendEndpointUdp_Response);
    }

    // Reset all mailbox queues to their empty state (we're starting a new send operation):
    void UdpTxCommsStatusManager::reset()
    {
        if (this->xQueue_SetTransmitSize_Response != NULL)
        {
            xQueueReset(this->xQueue_SetTransmitSize_Response);
        }
        if (this->xQueue_SendEndpointUdp_Response != NULL)
        {
            xQueueReset(this->xQueue_SendEndpointUdp_Response);
        }

        // Signal that we're no longer waiting for a response for any command.
        setCurrentlyAwaitedCommand(AwaitableCommand::NONE);
    }

    // Get what command we're currently awaiting a response for (in a mutex-safe way):
    UdpTxCommsStatusManager::AwaitableCommand UdpTxCommsStatusManager::getCurrentlyAwaitedCommand()
    {
        AwaitableCommand cmd;
        this->mutex.lock();
        cmd = this->currentlyAwaitedCommand;
        this->mutex.unLock();
        return cmd;
    }
    // Set what command we're currently awaiting a response for (in a mutex-safe way):
    UdpTxCommsStatusManager::AwaitableCommand UdpTxCommsStatusManager::AwaitableCommand(UdpTxCommsStatusManager::AwaitableCommand cmd)
    {
        this->mutex.lock();
        this->currentlyAwaitedCommand = cmd;
        this->mutex.unLock();
    }

    // Block (yield) the calling task until we get a `SetTransmitSize` response.
    // Return that response or TIMEOUT if we waited too long or
    // INTERNAL__TRY_AGAIN if the messaging system wasn't set up yet or
    // INTERNAL__BAD_SYNTAX if a BadSyntax event was emitted while we were awaiting a response.
    BgApi::ErrorCode UdpTxCommsStatusManager::awaitResponse_setTransmitSize()
    {
        return UdpTxCommsStatusManager::awaitResponse(AwaitableCommand::SET_TRANSMIT_SIZE,
                                                      &this->xQueue_SetTransmitSize_Response);
    }
    // Block (yield) the calling task until we get a `SendEndpointUdp` response (for the downlink endpoint).
    // Return that response or TIMEOUT if we waited too long or
    // INTERNAL__TRY_AGAIN if the messaging system wasn't set up yet or
    // INTERNAL__BAD_SYNTAX if a BadSyntax event was emitted while we were awaiting a response.
    BgApi::ErrorCode UdpTxCommsStatusManager::awaitResponse_sendEndpointUdp()
    {
        return UdpTxCommsStatusManager::awaitResponse(AwaitableCommand::SEND_ENDPOINT_UDP,
                                                      &this->xQueue_SendEndpointUdp_Response);
    }

    // Tell the UdpTxTask that we got the given response code for the `SetTransmitSize` command:
    void UdpTxCommsStatusManager::setTransmitSize_Response(BgApi::ErrorCode response)
    {
        // Push response into the queue, blocking for 0 ticks
        // (if there's already something in there, just let that be
        // consumed instead - apparently we got a double response):
        // - we're only trying to augment semaphore behavior by also
        // passing a response ErrorCode and this is how Semaphores do it:
        if (this->xQueue_SetTransmitSize_Response != NULL)
        {
            xQueueSend(this->xQueue_SetTransmitSize_Response,
                       (void *)&response,
                       (TickType_t)1); // allow waiting 1 tick in case its about to be reset (we'll want to fill it again)
        }
    }

    // Tell the UdpTxTask that we got the given response code for the `SendEndpointUdp` command:
    void UdpTxCommsStatusManager::sendEndpointUdp_Response(BgApi::ErrorCode response)
    {
        // Push response into the queue, blocking for 0 ticks
        // (if there's already something in there, just let that be
        // consumed instead - apparently we got a double response):
        // - we're only trying to augment semaphore behavior by also
        // passing a response ErrorCode and this is how Semaphores do it:
        if (this->xQueue_SendEndpointUdp_Response != NULL)
        {
            xQueueSend(this->xQueue_SendEndpointUdp_Response,
                       (void *)&response,
                       (TickType_t)1); // allow waiting 1 tick in case its about to be reset (we'll want to fill it again)
        }
    }

    // Sets the response for the given command being awaited:
    void UdpTxCommsStatusManager::setCommandResponse(UdpTxCommsStatusManager::AwaitableCommand cmd, BgApi::ErrorCode response)
    {
        switch (cmd)
        {
        case AwaitableCommand::SET_TRANSMIT_SIZE:
            setTransmitSize_Response(response);
            break;
        case AwaitableCommand::SEND_ENDPOINT_UDP:
            sendEndpointUdp_Response(response);
            break;
        case AwaitableCommand::NONE:
            // Not waiting on anything, so nothing to do with this information.
        default:
            // Shouldn't be here. No way for us to know what was meant by this. Do nothing.
        }
    }

    // Sets the command response for the currently awaited command (used
    // for things like SyntaxError that could crop up any time and we need
    // to direct to the appropriate Mailbox Queue):
    void UdpTxCommsStatusManager::setResponseForCurrentlyAwaitedCommand(BgApi::ErrorCode response)
    {
        setCommandResponse(getCurrentlyAwaitedCommand(), response);
    }

    // Helper function to block (yield) the calling task until we get a
    // response for the given queue correspond to the given `AwaitableCommand`.
    // Return that response or TIMEOUT if we waited too long or
    // INTERNAL__TRY_AGAIN if the messaging system wasn't set up yet or
    // INTERNAL__BAD_SYNTAX if a BadSyntax event was emitted while we were awaiting a response.
    BgApi::ErrorCode UdpTxCommsStatusManager::awaitResponse(UdpTxCommsStatusManager::AwaitableCommand cmd, QueueHandle_t *blockingQueue)
    {
        BgApi::ErrorCode errorCode;

        if (*blockingQueue != NULL)
        {
            // Clear the queue in question first because the callbacks that fill
            // the mailbox queues are triggered when ANYBODY (us or Radio's
            // internal BGScript) sends the command in question so it could be
            // already full because someone else (the Radio's internal BGScript)
            // sent and got a response to this command:
            xQueueReset(*blockingQueue);

            // Signal that we're now waiting for a response for the given command:
            setCurrentlyAwaitedCommand(cmd);

            // Block (yield) Task until timeout or data received:
            if (xQueueReceive(*blockingQueue,
                              &(errorCode),
                              UdpTxCommsStatusManager::UDP_TX_RESPONSE_TIMEOUT_TICKS) == pdPASS)
            {
                /* errorCode now contains a copy of the response code. */
            }
            else
            {
                errorCode = BgApi::ErrorCode::TIMEOUT;
            }
        }
        else
        {
            // Tell the caller that the program wasn't ready yet:
            errorCode = BgApi::ErrorCode::INTERNAL__TRY_AGAIN;
        }

        // Signal that we're no longer waiting for a response for any command.
        setCurrentlyAwaitedCommand(AwaitableCommand::NONE);

        return errorCode;
    }
}
