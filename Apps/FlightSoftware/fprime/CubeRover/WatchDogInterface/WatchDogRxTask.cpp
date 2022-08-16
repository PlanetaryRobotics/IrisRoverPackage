#include <HAL/include/FreeRTOS.h>
#include <HAL/include/os_task.h>
#include <HAL/include/os_portmacro.h>
#include <HAL/include/sys_dma.h>

#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/EightyCharString.hpp>

#include <CubeRover/WatchDogInterface/WatchDogRxTask.hpp>

#include <App/DMA.h>

#include <cassert>
#include <cstdio>

#include "sci.h"
#include "FreeRTOS.h"
#include "os_queue.h"

#define USE_DMA_OR_POLL false

static TaskHandle_t xTaskToNotify = nullptr;
static volatile bool dmaReadBusy = false;
static QueueHandle_t rxByteQueue;

extern "C" void SCILIN_RX_DMA_ISR(dmaInterrupt_t inttype)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    dmaReadBusy = false;

    if (xTaskToNotify == nullptr)
    {
        return;
    }

    // Notify the task that
    vTaskNotifyGiveFromISR(xTaskToNotify, &xHigherPriorityTaskWoken);

    /* If xHigherPriorityTaskWoken is now set to pdTRUE then a
    context switch should be performed to ensure the interrupt
    returns directly to the highest priority task. */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

extern "C" void scilin_ISR(uint32 flags)
{
    if (SCI_RX_INT == flags && rxByteQueue != 0)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        uint8_t rxByte = (uint8_t)(scilinREG->RD & 0x000000FFU);

        xQueueSendFromISR(rxByteQueue, &rxByte, &xHigherPriorityTaskWoken);

        /* If xHigherPriorityTaskWoken is now set to pdTRUE then a
        context switch should be performed to ensure the interrupt
        returns directly to the highest priority task. */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    scilinREG->SETINT = (uint32)SCI_RX_INT;
}

namespace CubeRover
{
    WatchDogRxTask::WatchDogRxTask()
        : m_numCallbacksRegistered(0),
          m_mpsm(),
          m_keepRunning(true),
          m_isRunning(false)
    {
        rxByteQueue = xQueueCreate(256, sizeof(uint8_t));
        assert(rxByteQueue != 0);
    }

    // This probably will never be called, but I set it up to properly work anyway
    WatchDogRxTask::~WatchDogRxTask()
    {
        // Stop interrupt notifications
        xTaskToNotify = nullptr;

        // Stop looping
        m_keepRunning = false;

        // Make sure we aren't blocked
        if (xTaskToNotify != nullptr)
        {
            xTaskNotifyGive(xTaskToNotify);
        }

        // Join the thread
        void *value;
        this->join(&value);
    }

    ::Os::Task::TaskStatus WatchDogRxTask::startTask(NATIVE_INT_TYPE priority,
                                                     NATIVE_INT_TYPE stackSize,
                                                     NATIVE_INT_TYPE cpuAffinity)
    {
        if (m_isRunning)
        {
            return Os::Task::TASK_UNKNOWN_ERROR;
        }

        scilinREG->SETINT = (uint32)SCI_RX_INT;

        m_keepRunning = true;
        Fw::EightyCharString task("WatchDogRxTask");

        TaskHandle_t tid;
        BaseType_t stat = xTaskCreate(WatchDogRxTask::rxHandlerTaskFunction,
                                      task.toChar(),
                                      stackSize,
                                      this,
                                      priority,
                                      &tid);

        assert(stat == pdPASS);
        xTaskToNotify = tid;

        assert(xTaskToNotify != 0);

        m_isRunning = true;
        return Os::Task::TASK_OK;
    }

    bool WatchDogRxTask::registerCallback(WatchDogRxCallbackProcessor *callback)
    {
        if (m_numCallbacksRegistered >= WATCHDOG_RX_TASK__MAX_NUM_CALLBACKS)
        {
            return false;
        }

        m_callbacks[m_numCallbacksRegistered] = callback;
        m_numCallbacksRegistered++;

        return true;
    }

#define NUM_BYTE_UPDATES 96

#define RX_BYTE_RESULT__PROGRESS 1
#define RX_BYTE_RESULT__BAD 2
#define RX_BYTE_RESULT__DONE 3

    struct RxByteLog
    {
        uint8_t data;
        uint8_t result;
    } __attribute__((packed));

    static RxByteLog rxByteUpdates[NUM_BYTE_UPDATES] = {};
    static size_t updatesUsed = 0;
    static size_t updatesHead = 0;

    void writeUpdate(uint8_t data, uint8_t result)
    {
        bool overwrite = (updatesUsed == NUM_BYTE_UPDATES);
        size_t writeIndex = overwrite ? updatesHead : updatesUsed;

        rxByteUpdates[writeIndex].data = data;
        rxByteUpdates[writeIndex].result = result;

        if (overwrite)
        {
            updatesHead++;

            while (updatesHead >= NUM_BYTE_UPDATES)
            {
                updatesHead -= NUM_BYTE_UPDATES;
            }
        }
        else
        {
            updatesUsed++;
        }
    }

    void WatchDogRxTask::printRxUpdates()
    {
        for (size_t i = 0; i < updatesUsed; ++i)
        {
            size_t wrapped = updatesHead + i;
            while (wrapped >= NUM_BYTE_UPDATES)
            {
                wrapped -= NUM_BYTE_UPDATES;
            }

            char result;
            switch (rxByteUpdates[wrapped].result)
            {
            case RX_BYTE_RESULT__PROGRESS:
                result = 'P';
                break;

            case RX_BYTE_RESULT__BAD:
                result = 'B';
                break;

            case RX_BYTE_RESULT__DONE:
                result = 'D';
                break;

            default:
                result = 'X';
                break;
            }

            fprintf(stderr, "%02d: %02X %c\n", i, rxByteUpdates[wrapped].data, result);
        }
        fprintf(stderr, "\n\n");

        updatesHead = 0;
        updatesUsed = 0;
    }

    void WatchDogRxTask::rxHandlerTaskFunction(void *arg)
    {
        WatchDogRxTask *task = static_cast<WatchDogRxTask *>(arg);

        // First, construct the Message we'll use throughout
        WatchDogMpsm::Message msg(task->m_dataBuffer, sizeof(task->m_dataBuffer));

        while (!task->m_keepRunning)
            ; // Wait until keepRunning has been set true

        while (task->m_keepRunning)
        {
            uint8_t newData = 0;
            // Effectively blocks forever until something is put into the queue
            if (xQueueReceive(rxByteQueue, &newData, portMAX_DELAY))
            {
                bool resetMpsmMsg = false;
                WatchDogMpsm::ProcessStatus pStatus = task->m_mpsm.process(msg, newData);

                if (WatchDogMpsm::PS_DONE_VALID == pStatus)
                {
                    // We've gotten a full message, so call our callbacks then reset
                    task->callAllCallbacks(msg, true);
                    writeUpdate(newData, RX_BYTE_RESULT__DONE);
                    resetMpsmMsg = true;
                }
                else if (WatchDogMpsm::PS_DONE_BAD_PARITY_HEADER == pStatus)
                {
                    // Got a full header that was valid other than parity, so callback with this info then reset
                    task->callAllCallbacks(msg, false);
                    writeUpdate(newData, RX_BYTE_RESULT__BAD);
                    resetMpsmMsg = true;
                }
                else
                {
                    writeUpdate(newData, RX_BYTE_RESULT__PROGRESS);
                }

                if (resetMpsmMsg)
                {
                    msg.reset();
                }
            }
        }
    }

    void WatchDogRxTask::callAllCallbacks(WatchDogMpsm::Message &msg, bool goodParity)
    {
        for (size_t i = 0; i < m_numCallbacksRegistered; ++i)
        {
            m_callbacks[i]->rxCallback(msg, goodParity);
        }
    }

} // end namespace CubeRover
