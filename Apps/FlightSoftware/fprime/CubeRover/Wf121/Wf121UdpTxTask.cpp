#include <HAL/include/FreeRTOS.h>
#include <HAL/include/os_task.h>
#include <HAL/include/os_portmacro.h>
#include <HAL/include/sys_dma.h>

#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/EightyCharString.hpp>

#include <App/DMA.h>

#include <CubeRover/Wf121/Wf121.hpp>
#include <CubeRover/Wf121/Wf121UdpTxTask.hpp>

// Handle to active task (this):
static TaskHandle_t xActiveTask = nullptr; // not init'd yet

namespace Wf121
{
    Wf121UdpTxTask::Wf121UdpTxTask(Wf121TxTaskManager *pttm)
        : m_pTxTaskManager(pttm),
          m_keepRunning(true),
          m_isRunning(false)
    {
        /* nothing special to do here */
    }

    // This probably will never be called, but it should up to properly work anyway
    Wf121UdpTxTask::~Wf121UdpTxTask()
    {
        // Stop interrupt notifications
        xActiveTask = nullptr;

        // Stop looping
        m_keepRunning = false;

        //        // Make sure we aren't blocked
        //        if (xActiveTask != nullptr)
        //        {
        //            xTaskNotifyGive(xActiveTask);
        //        }

        // Join the thread
        void *value;
        this->join(&value);
    }

    ::Os::Task::TaskStatus Wf121UdpTxTask::startTask(NATIVE_INT_TYPE priority,
                                                     NATIVE_INT_TYPE stackSize,
                                                     NATIVE_INT_TYPE cpuAffinity)
    {
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

        configASSERT(stat == pdPASS); // Bad news if the task wasn't created
        xActiveTask = tid;

        configASSERT(xActiveTask != 0);

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

        BgApi::BgApiCommBuffer *dataToSend;
        while (task->m_keepRunning)
        {
            // Dispatch to the appropriate handler and let it tell us what to
            // send and when (i.e. it doesn't return until it needs us to send
            // data):
            dataToSend = task->m_pTxTaskManager->udpTxUpdateHandler(task);

            // Halt the task while Passthrough is enabled, checking back every
            // once in a while to see if we've been freed (we don't need to
            // respond right away (in tight timing) to being freed):
            if (Wf121::persistentBgApiPassthroughEnabled())
            {
                // If pass through is enabled, don't send this data...
                while (Wf121::persistentBgApiPassthroughEnabled())
                {
                    // Instead, check back every once in a while to see if
                    // we've been freed.
                    vTaskDelay(WF121_UDP_TX_TASK_PASSTHROUGH_PAUSE_CHECK_MS);
                }
                // Once freed, restart the loop (toss whatever data it was we were going to send):
                continue;
            }

            // Only attempt to send if we need to send non-zero number of bytes:
            if (dataToSend->dataLen != 0)
            {
                // block task until we can send those bytes:
                bool sendSuccess = false;
                uint32_t tries = 0;
                while (!sendSuccess && tries < Wf121UdpTxTask::MAX_DMA_SEND_TRIES)
                {
                    sendSuccess = Wf121Serial::dmaSend((void *)(dataToSend->rawData), dataToSend->dataLen, true);
                    tries++;
                }
                if (tries == Wf121UdpTxTask::MAX_DMA_SEND_TRIES)
                {
                    // We failed to push data into the DMA send buffer too many
                    // times (a lot of times). What do we do?
                    // Likely something's wrong with it.
                    // Do an assert to nuke Hercules and let the WD get us out
                    // of here:
                    configASSERT(pdFALSE);
                }
            }
            // ... next loop we do it all over again.
        }
    }

} // end namespace
