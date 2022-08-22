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

        UdpTxCommsStatus() : xQueue_SetTransmitSize_Response(NULL), // null until init.
                             xQueue_SendEndpointUdp_Response(NULL)  // null until init.
        {
        }

        void init()
        {
            // Create the mailbox queues:
            this->xQueue_SetTransmitSize_Response = xQueueCreateStatic(1,
                                                                       sizeof(ErrorCode),
                                                                       ucQueueStorageArea_SetTransmitSize_Response,
                                                                       &xStaticQueue_SetTransmitSize_Response);
            configASSERT(xQueue_SetTransmitSize_Response);
            this->xQueue_SendEndpointUdp_Response = xQueueCreateStatic(1,
                                                                       sizeof(ErrorCode),
                                                                       ucQueueStorageArea_SendEndpointUdp_Response,
                                                                       &xStaticQueue_SendEndpointUdp_Response);
            configASSERT(xQueue_SendEndpointUdp_Response);
        }

        // Reset all mailbox queues to their empty state (we're starting a new send operation):
        void reset()
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
        BgApi::ErrorCode awaitResponse_setTransmitSize()
        {
            return UdpTxCommsStatusManager::awaitResponse(this->xQueue_SetTransmitSize_Response);
        }
        // Block (yield) the calling task until we get a `SendEndpointUdp` response (for the downlink endpoint).
        // Return that response or TIMEOUT if we waited too long or
        // TRY_AGAIN if the messaging system wasn't set up yet.
        BgApi::ErrorCode awaitResponse_sendEndpointUdp()
        {
            return UdpTxCommsStatusManager::awaitResponse(this->xQueue_SendEndpointUdp_Response);
        }

        // Tell the UdpTxTask that we got the given response code for the `SetTransmitSize` command:
        void setTransmitSize_Response(BgApi::ErrorCode response)
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
                           (TickType_t)0);
            }
        }

        // Tell the UdpTxTask that we got the given response code for the `SendEndpointUdp` command:
        void sendEndpointUdp_Response(BgApi::ErrorCode response)
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
                           (TickType_t)0);
            }
        }

    private:
        // ErrorCode Queue to push the Radio's response to `SetTransmitSize`:
        // (size of one makes this like a cross between a semaphore and a mailbox)
        // Allows the UdpTxTask to know not just that we got a response but
        // also what kind of response it was (success/fail):
        static StaticQueue_t xStaticQueue_SetTransmitSize_Response;
        static uint8_t ucQueueStorageArea_SetTransmitSize_Response[1 * sizeof(BgApi::ErrorCode)];
        QueueHandle_t xQueue_SetTransmitSize_Response;
        // ErrorCode Queue to push the Radio's response to `SendEndpointUdp`:
        // (size of one makes this like a cross between a semaphore and a mailbox)
        // Allows the UdpTxTask to know not just that we got a response but
        // also what kind of response it was (success/fail):
        static StaticQueue_t xStaticQueue_SendEndpointUdp_Response;
        static uint8_t ucQueueStorageArea_SendEndpointUdp_Response[1 * sizeof(BgApi::ErrorCode)];
        QueueHandle_t xQueue_SendEndpointUdp_Response;

        // Helper function to block (yield) the calling task until we get a
        // response for the given queue.
        // Return that response or TIMEOUT if we waited too long or
        // TRY_AGAIN if the messaging system wasn't set up yet.
        static BgApi::ErrorCode awaitResponse(QueueHandle_t *blockingQueue)
        {
            BgApi::ErrorCode errorCode;

            if (*blockingQueue != NULL)
            {
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
    };
}

#endif /* CUBEROVER_WF121_WF121_UDP_TX_COMMS_STATUS_MANAGER_HPP_ */