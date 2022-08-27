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

    UdpTxCommsStatusManager::UdpTxCommsStatusManager() : xQueue_SetTransmitSize_Response(NULL), // null until init.
                                                         xQueue_SendEndpointUdp_Response(NULL)  // null until init.
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
    }

    // Block (yield) the calling task until we get a `SetTransmitSize` response.
    // Return that response or TIMEOUT if we waited too long or
    // TRY_AGAIN if the messaging system wasn't set up yet.
    BgApi::ErrorCode UdpTxCommsStatusManager::awaitResponse_setTransmitSize()
    {
        return UdpTxCommsStatusManager::awaitResponse(&this->xQueue_SetTransmitSize_Response);
    }
    // Block (yield) the calling task until we get a `SendEndpointUdp` response (for the downlink endpoint).
    // Return that response or TIMEOUT if we waited too long or
    // TRY_AGAIN if the messaging system wasn't set up yet.
    BgApi::ErrorCode UdpTxCommsStatusManager::awaitResponse_sendEndpointUdp()
    {
        return UdpTxCommsStatusManager::awaitResponse(&this->xQueue_SendEndpointUdp_Response);
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

    // Helper function to block (yield) the calling task until we get a
    // response for the given queue.
    // Return that response or TIMEOUT if we waited too long or
    // TRY_AGAIN if the messaging system wasn't set up yet.
    BgApi::ErrorCode UdpTxCommsStatusManager::awaitResponse(QueueHandle_t *blockingQueue)
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
            errorCode = BgApi::ErrorCode::TRY_AGAIN;
        }

        return errorCode;
    }
}
