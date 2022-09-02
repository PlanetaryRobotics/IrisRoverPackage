#include <CubeRover/Wf121/UdpTxCommsStatusManager.hpp>

#include <CubeRover/Wf121/Timestamp.hpp>

namespace Wf121
{
    // ErrorCode Queue to push the Radio's response to `GetUdpInterlock`:
    // (size of one makes this like a cross between a semaphore and a mailbox)
    // Allows the UdpTxTask to know not just that we got a response but
    // also what kind of response it was (success/fail):
    static StaticQueue_t xStaticQueue_GetUdpInterlock_Response;
    static uint8_t ucQueueStorageArea_GetUdpInterlock_Response[1 * sizeof(BgApi::ErrorCode)];
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

    UdpTxCommsStatusManager::UdpTxCommsStatusManager() : xQueue_SetTransmitSize_Response(NULL),                                                     // null until init.
                                                         xQueue_SendEndpointUdp_Response(NULL),                                                     // null until init.
                                                         currentlyAwaitedCommand(UdpTxCommsStatusManager::AwaitableCommand::NONE),                  // waiting on nothing by default
                                                         latestUpdatedInterlockStatus(DirectMessage::RadioUdpInterlockStatus::RADIO_HAS_INTERLOCK), // by default, we assume we don't have it (because we don't)
                                                         lastInterlockAcquisitionTimeMs(0)
    {
    }

    void UdpTxCommsStatusManager::init()
    {
        // Create the mailbox queues:
        this->xQueue_GetUdpInterlock_Response = xQueueCreateStatic(1,
                                                                   sizeof(BgApi::ErrorCode),
                                                                   ucQueueStorageArea_GetUdpInterlock_Response,
                                                                   &xStaticQueue_GetUdpInterlock_Response);
        configASSERT(xQueue_GetUdpInterlock_Response);

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
        if (this->xQueue_GetUdpInterlock_Response != NULL)
        {
            xQueueReset(this->xQueue_GetUdpInterlock_Response);
        }
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
    void UdpTxCommsStatusManager::setCurrentlyAwaitedCommand(UdpTxCommsStatusManager::AwaitableCommand cmd)
    {
        this->mutex.lock();
        this->currentlyAwaitedCommand = cmd;
        this->mutex.unLock();
    }

    // Block (yield) the calling task until we get a `GetUdpInterlock` response.
    // Return that response or TIMEOUT if we waited too long or
    // INTERNAL__TRY_AGAIN if the messaging system wasn't set up yet or
    // INTERNAL__BAD_SYNTAX if a BadSyntax event was emitted while we were awaiting a response.
    // INTERNAL__LOST_INTERLOCK if we lose the interlock while awaiting this response.
    BgApi::ErrorCode UdpTxCommsStatusManager::awaitResponse_getUdpInterlock()
    {
        return UdpTxCommsStatusManager::awaitResponse(AwaitableCommand::GET_UDP_INTERLOCK,
                                                      &this->xQueue_GetUdpInterlock_Response);
    }
    // Block (yield) the calling task until we get a `SetTransmitSize` response.
    // Return that response or TIMEOUT if we waited too long or
    // INTERNAL__TRY_AGAIN if the messaging system wasn't set up yet or
    // INTERNAL__BAD_SYNTAX if a BadSyntax event was emitted while we were awaiting a response.
    // INTERNAL__LOST_INTERLOCK if we lose the interlock while awaiting this response.
    BgApi::ErrorCode UdpTxCommsStatusManager::awaitResponse_setTransmitSize()
    {
        return UdpTxCommsStatusManager::awaitResponse(AwaitableCommand::SET_TRANSMIT_SIZE,
                                                      &this->xQueue_SetTransmitSize_Response);
    }
    // Block (yield) the calling task until we get a `SendEndpointUdp` response (for the downlink endpoint).
    // Return that response or TIMEOUT if we waited too long or
    // INTERNAL__TRY_AGAIN if the messaging system wasn't set up yet or
    // INTERNAL__BAD_SYNTAX if a BadSyntax event was emitted while we were awaiting a response.
    // INTERNAL__LOST_INTERLOCK if we lose the interlock while awaiting this response.
    BgApi::ErrorCode UdpTxCommsStatusManager::awaitResponse_sendEndpointUdp()
    {
        return UdpTxCommsStatusManager::awaitResponse(AwaitableCommand::SEND_ENDPOINT_UDP,
                                                      &this->xQueue_SendEndpointUdp_Response);
    }

    // Helper function to process the given response for the given Mailbox Queue:
    void UdpTxCommsStatusManager::processMailboxResponse(BgApi::ErrorCode response, QueueHandle_t *queue)
    {
        // Push response into the queue, blocking for 0 ticks
        // (if there's already something in there, just let that be
        // consumed instead - apparently we got a double response):
        // - we're only trying to augment semaphore behavior by also
        // passing a response ErrorCode and this is how Semaphores do it:
        if (*queue != NULL)
        {
            xQueueSend(*queue,
                       (void *)&response,
                       (TickType_t)1); // allow waiting 1 tick in case it's about to be reset (we'll want to fill it again)
        }
    }

    // Tell the UdpTxTask that we got the given response code for the `GetUdpInterlock` command:
    void UdpTxCommsStatusManager::getUdpInterlock_Response(BgApi::ErrorCode response)
    {
        processMailboxResponse(response, &(this->xQueue_GetUdpInterlock_Response));
    }

    // Tell the UdpTxTask that we got the given response code for the `SetTransmitSize` command:
    void UdpTxCommsStatusManager::setTransmitSize_Response(BgApi::ErrorCode response)
    {
        processMailboxResponse(response, &(this->xQueue_SetTransmitSize_Response));
    }

    // Tell the UdpTxTask that we got the given response code for the `SendEndpointUdp` command:
    void UdpTxCommsStatusManager::sendEndpointUdp_Response(BgApi::ErrorCode response)
    {
        processMailboxResponse(response, &(this->xQueue_SendEndpointUdp_Response));
    }

    // Sets the response for the given command being awaited:
    void UdpTxCommsStatusManager::setCommandResponse(UdpTxCommsStatusManager::AwaitableCommand cmd, BgApi::ErrorCode response)
    {
        switch (cmd)
        {
        case AwaitableCommand::GET_UDP_INTERLOCK:
            getUdpInterlock_Response(response);
            break;
        case AwaitableCommand::SET_TRANSMIT_SIZE:
            setTransmitSize_Response(response);
            break;
        case AwaitableCommand::SEND_ENDPOINT_UDP:
            sendEndpointUdp_Response(response);
            break;
        case AwaitableCommand::NONE:
            // Not waiting on anything, so nothing to do with this information.
            // Possible this was an interlock release when we're not awaiting
            // anything (not sending data) which is fine and expected.
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

    // Set the latest interlock status and update the acquisition time if
    // we got an update.
    void setLatestUdpInterlockStatus(DirectMessage::RadioUdpInterlockStatus status)
    {
        // Do timestamp computation outside the mutex to minimize mutex lock time:
        uint32_t now;
        bool acquired = status == DirectMessage::RadioUdpInterlockStatus::HERC_HAS_INTERLOCK;
        if (acquired)
        {
            now = Timestamp::getTimeMs();
        }

        this->mutex.lock();
        {
            latestUpdatedInterlockStatus = status;
            if (acquired)
            {
                lastInterlockAcquisitionTimeMs = now;
            }
        }
        this->mutex.unlock();
    }

    // Get the current interlock status, accounting for the expiration time.
    DirectMessage::RadioUdpInterlockStatus getLatestUdpInterlockStatus()
    {
        DirectMessage::RadioUdpInterlockStatus status;
        uint32_t lastUpdateMs;

        // Grab data quickly then parse it outside the mutex:
        this->mutex.lock();
        {
            status = latestUpdatedInterlockStatus;
            lastUpdateMs = lastInterlockAcquisitionTimeMs;
        }
        this->mutex.unlock();

        if (status == DirectMessage::RadioUdpInterlockStatus::HERC_HAS_INTERLOCK)
        {
            // if status says we have the interlock, check the time to make sure
            // it hasn't expired:
            uint32_t now = Timestamp::getTimeMs();
            if ((now - lastUpdateMs) > UDP_INTERLOCK_EXPIRATION_TIME_MS)
            {
                // Interlock should have expired. Reflect that:
                status = DirectMessage::RadioUdpInterlockStatus::RADIO_HAS_INTERLOCK;
            }
        }
        else if (status == DirectMessage::RadioUdpInterlockStatus::BAD_MESSAGE)
        {
            // If status says our latest update was corrupted, assume that means
            // we don't have it, just to be safe and we need to re-request it:
            status = DirectMessage::RadioUdpInterlockStatus::RADIO_HAS_INTERLOCK;
        }

        return status;
    }
}
