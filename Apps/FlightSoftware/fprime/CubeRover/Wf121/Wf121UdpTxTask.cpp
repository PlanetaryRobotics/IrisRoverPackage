#include <HAL/include/FreeRTOS.h>
#include <HAL/include/os_task.h>
#include <HAL/include/os_portmacro.h>
#include <HAL/include/sys_dma.h>

#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/EightyCharString.hpp>

#include <CubeRover/Wf121/Wf121RxTask.hpp>

#include <App/DMA.h>

#include <cassert>
#include <cstdio>

#include "sci.h"
#include "FreeRTOS.h"
#include "os_queue.h"

// Handle to active task (this):
static TaskHandle_t xActiveTask = nullptr; // not init'd yet

namespace Wf121
{
    Wf121UdpTxTask::Wf121UdpTxTask(QueueHandle_t *ptpq)
        : m_pTxPayloadQueue(ptpq),
          m_xUdpTxWorkingData(),
          m_keepRunning(true),
          m_isRunning(false)
    {
        m_xUdpTxWorkingData.clear(); // Pre-fill working buffer with sentinel byte
    }

    // This probably will never be called, but it should up to properly work anyway
    Wf121UdpTxTask::~Wf121UdpTxTask()
    {
        // Stop interrupt notifications
        xActiveTask = nullptr;

        // Stop looping
        m_keepRunning = false;

        // Make sure we aren't blocked
        if (xActiveTask != nullptr)
        {
            xTaskNotifyGive(xActiveTask);
        }

        // Join the thread
        void *value;
        this->join(&value);
    }

    ::Os::Task::TaskStatus Wf121UdpTxTask::startTask(NATIVE_INT_TYPE priority,
                                                     NATIVE_INT_TYPE stackSize,
                                                     NATIVE_INT_TYPE cpuAffinity)
    {
        // Make sure that by the time we're setting up the task, the queue has
        // been set up:
        assert(*m_pTxPayloadQueue != NULL);

        if (m_isRunning)
        {
            return Os::Task::TASK_UNKNOWN_ERROR;
        }

        m_keepRunning = true;
        Fw::EightyCharString task_name("Wf121UdpTxTask"); // max len is 16

        TaskHandle_t tid;
        BaseType_t stat = xTaskCreate(Wf121UdpTxTask::udpTxHandlerTaskFunction,
                                      task_name.toChar(),
                                      stackSize,
                                      this,
                                      priority,
                                      &tid);

        assert(stat == pdPASS); // Bad news if the task wasn't created
        xActiveTask = tid;

        assert(xActiveTask != 0);

        m_isRunning = true;
        return Os::Task::TASK_OK;
    }

    void Wf121UdpTxTask::udpTxHandlerTaskFunction(void *arg)
    {
        // Note: This is a FreeRTOS Task, so "blocking forever" just means
        // the **Task** doesn't do anything else when FreeRTOS switches to it;
        // system itself won't halt.
        // More on FreeRTOS Tasks: https://www.freertos.org/taskandcr.html
        Wf121UdpTxTask *task = static_cast<Wf121UdpTxTask *>(arg);

        // Block task for 50ms to wait for serial to come up and be ready
        // (NOTE: this is already handled elsewhere and this task isn't started
        // until serial is ready **but** this gives us some safety to wait for
        // hardware to get ready):
        vTaskDelay(50 / portTICK_PERIOD_MS);

        while (!task->m_keepRunning)
        {
            // Wait until keepRunning has been set true
        };

        while (task->m_keepRunning)
        {
            // Effectively "blocks forever" until something is put into the queue
            if (xQueueReceive(*m_pTxPayloadQueue, &m_xUdpTxWorkingData, portMAX_DELAY))
            {
                // Write

                // While !readyToDownlink
                // ! TODO: [CWC] readyToDownlink = state==connected && !BGAPI::awaitingCommandResponse && downlinkEndpoint != DROP_ENDPOINT

                // Reset comms status:
                m_udpTxCommsStatus.reset();

                // Send command to set transmit size:
                errorCode = SetTransmitSize(m_udpSendEndpoint,
                                            byteToSend); // !TODO this should return bytes now, not send (all the commands should do this)

                if (errorCode != NO_ERROR)
                    return errorCode;
                // dmaSend those bytes

                while (!m_udpTxCommsStatus.hasSetTxSizeBeenAcknowledged())
                {
                    // Block until response.
                    // ! TODO: Also add timeout that puts us back at the top of the sending process.
                    // (maybe just make all this a helper function in NI and if it returns false, we try again until MAX_TRIES).
                }

                // loop until all data is sent
                uint16_t target_num_send_endpoint_acknowledgements = 0;
                while (byteToSend > 0)
                {
                    // max number of bytes to send at one time is 255 by hardware restriction
                    uint8_t byteToSendThisLoop = (byteToSend > 0xff) ? 0xff : byteToSend;
                    m_commandSendEndpointSet = false;

                    errorCode = SendEndpoint(m_udpSendEndpoint,
                                             data,
                                             byteToSendThisLoop); // ! get bytes

                    // ! TODO: (WORKING-HERE) [CWC] Switch to binary semaphores to let the task know that the response was received (https://www.freertos.org/RTOS_Task_Notification_As_Binary_Semaphore.html)
                    // Block the task **not** indefinitely for the semaphore. If not received, reset, resend the command, and await the semaphore.

                    // dmaSend those bytes
                    target_num_send_endpoint_acknowledgements++;
                    while (m_udpTxCommsStatus.getNumSendEndpointAcknowledgements() < target_num_send_endpoint_acknowledgements)
                    {
                        // Block until response.
                        // ! TODO: Also add timeout that puts us back at the top of the sending process.
                    }

                    // wait until all data is sent
                    if (errorCode == NO_ERROR)
                    {
                        m_logNbOfBytesSent += byteToSendThisLoop;
                        data += byteToSendThisLoop;
                        byteToSend -= byteToSendThisLoop;
                    }
                    else
                    {
                        break; // exit loop on error
                    }
                }
            }
        }
    }
}

} // end namespace CubeRover
