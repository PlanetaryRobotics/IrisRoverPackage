#include <HAL/include/FreeRTOS.h>
#include <HAL/include/os_task.h>
#include <HAL/include/os_portmacro.h>
#include <HAL/include/sys_dma.h>

#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/EightyCharString.hpp>

#include <CubeRover/Wf121/Wf121RxTask.hpp>
#include <CubeRover/Wf121/Wf121SerialInterface.hpp>

#include <App/DMA.h>

#include <cassert>
#include <cstdio>

#include "sci.h"
#include "FreeRTOS.h"
#include "os_queue.h"

// RX Task to handle incoming data:
static TaskHandle_t xTaskToNotify = nullptr;
static volatile bool dmaReadBusy = false;
// Queue of received bytes (ISR pushes to it, RX Task drains from it - used for IPC between the ISR and the main program):
static QueueHandle_t rxByteQueue;

extern "C" void SCI_RX_DMA_ISR(dmaInterrupt_t inttype)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    dmaReadBusy = false;

    if (xTaskToNotify == nullptr)
    {
        return;
    }

    // Notify the RX task that data is available:
    vTaskNotifyGiveFromISR(xTaskToNotify, &xHigherPriorityTaskWoken);

    /* If xHigherPriorityTaskWoken is now set to pdTRUE then a
    context switch should be performed to ensure the interrupt
    returns directly to the highest priority task. */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// ISR to handle RX data from WF121 UART interface:
extern "C" void sci_ISR(uint32 flags)
{
    // Check if this interrupt is for a read event and we're ready to receive:
    if ((flags & (uint32)SCI_RX_INT) == (uint32)SCI_RX_INT && rxByteQueue != 0)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        // Read off 1 byte:
        uint8_t rxByte = (uint8_t)(sciREG->RD & 0x000000FFU);

        /* clear error flags */
        sciREG->FLR = ((uint32)SCI_FE_INT | (uint32)SCI_OE_INT | (uint32)SCI_PE_INT);

        // Push that byte into the queue so it's available to the outside task:
        xQueueSendFromISR(rxByteQueue, &rxByte, &xHigherPriorityTaskWoken);

        /* If xHigherPriorityTaskWoken is now set to pdTRUE then a
        context switch should be performed to ensure the interrupt
        returns directly to the highest priority task. */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    Wf121::Wf121Serial::signalReadyForInterrupt(); // Flag that we're ready for another SCI ISR
}

namespace Wf121
{
    Wf121RxTask::Wf121RxTask()
        : m_numCallbacksRegistered(0),
          m_mpsm(),
          m_keepRunning(true),
          m_isRunning(false)
    {
        rxByteQueue = xQueueCreate(2 * BgApi::MAX_PACKET_SIZE + 5, sizeof(uint8_t));
        configASSERT(rxByteQueue != 0);
    }

    // This probably will never be called, but it should up to properly work anyway
    Wf121RxTask::~Wf121RxTask()
    {
        // Stop interrupt notifications
        xTaskToNotify = nullptr;

        // Stop looping
        m_keepRunning = false;

//        // Make sure we aren't blocked
//        if (xTaskToNotify != nullptr)
//        {
//            xTaskNotifyGive(xTaskToNotify);
//        }

        // Join the thread
        void *value;
        this->join(&value);
    }

    ::Os::Task::TaskStatus Wf121RxTask::startTask(NATIVE_INT_TYPE priority,
                                                  NATIVE_INT_TYPE stackSize,
                                                  NATIVE_INT_TYPE cpuAffinity)
    {
        if (m_isRunning)
        {
            return Os::Task::TASK_UNKNOWN_ERROR;
        }

        Wf121Serial::signalReadyForInterrupt(); // flag that we're ready for SCI ISRs

        m_keepRunning = true;
        Fw::EightyCharString task_name("Wf121RxTask");

        TaskHandle_t tid;
        BaseType_t stat = xTaskCreate(Wf121RxTask::rxHandlerTaskFunction,
                                      task_name.toChar(),
                                      stackSize,
                                      this,
                                      priority,
                                      &tid);

        configASSERT(stat == pdPASS); // Bad news if the task wasn't created
        xTaskToNotify = tid;
        configASSERT(xTaskToNotify != 0);

        m_isRunning = true;
        return Os::Task::TASK_OK;
    }

    bool Wf121RxTask::registerCallback(Wf121RxCallbackProcessor *callback)
    {
        if (m_numCallbacksRegistered >= WF121_RX_TASK__MAX_NUM_CALLBACKS)
        {
            return false;
        }

        m_callbacks[m_numCallbacksRegistered] = callback;
        m_numCallbacksRegistered++;

        return true;
    }

    void Wf121RxTask::rxHandlerTaskFunction(void *arg)
    {
        // Note: This is a FreeRTOS Task, so "blocking forever" just means
        // the **Task** doesn't do anything else when FreeRTOS switches to it;
        // system itself won't halt.
        // More on FreeRTOS Tasks: https://www.freertos.org/taskandcr.html
        Wf121RxTask *task = static_cast<Wf121RxTask *>(arg);

        // First, construct the Message we'll use throughout
        Wf121Parser::GenericMessage msg(task->m_dataBuffer, sizeof(task->m_dataBuffer));

        while (!task->m_keepRunning)
        {
            // Wait until keepRunning has been set true
        };

        while (task->m_keepRunning)
        {
            uint8_t newData = 0;
            bool resetMpsmMsg = false;

            // Effectively "blocks forever" until something is put into the queue
            if (xQueueReceive(rxByteQueue, &newData, portMAX_DELAY) == pdPASS)
            {
//                printf("\n%02x\n", newData);
                Wf121Parser::Mpsm::ProcessStatus proc_stat = task->m_mpsm.process(msg, newData);

                switch (proc_stat)
                {
                // If a full message was parsed, handle all callbacks, then reset:
                case Wf121Parser::Mpsm::ProcessStatus::DM_PARSED:
                case Wf121Parser::Mpsm::ProcessStatus::BGAPI_PARSED:
                    // We got a full message, so call our callbacks then reset:
                    task->callAllCallbacks(msg);
                    resetMpsmMsg = true;
                    break;

                // Something bad happened, so we should toss anything we have:
                case Wf121Parser::Mpsm::ProcessStatus::POSSIBLE_CORRUPTION:
                    resetMpsmMsg = true;
                    break;


                case Wf121Parser::Mpsm::ProcessStatus::BAD_HEADER:
                case Wf121Parser::Mpsm::ProcessStatus::BAD_LENGTH:
                    // We got invalid / bad data. Probably just means
                    // we're out of sync, so keep scanning. Nothing to do
                    break;

                // If we don't need to do anything
                //(things are fine, just not done), do nothing:
                case Wf121Parser::Mpsm::ProcessStatus::BGAPI_HEADER_PARSED:
                case Wf121Parser::Mpsm::ProcessStatus::DM_HEADER_PARSED:
                case Wf121Parser::Mpsm::ProcessStatus::DM_LEN_PARSED:
                case Wf121Parser::Mpsm::ProcessStatus::WAITING_FOR_MORE_DATA:
                default:
                    // Do nothing.
                }
            }

            if (resetMpsmMsg)
            {
                msg.reset();
            }
        }
    }

    void Wf121RxTask::callAllCallbacks(Wf121Parser::GenericMessage &msg)
    {
        for (size_t i = 0; i < m_numCallbacksRegistered; ++i)
        {
            m_callbacks[i]->rxCallback(msg);
        }
    }

} // end namespace CubeRover
