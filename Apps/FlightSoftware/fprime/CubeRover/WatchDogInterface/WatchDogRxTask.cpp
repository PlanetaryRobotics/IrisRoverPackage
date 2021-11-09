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

static TaskHandle_t xTaskToNotify = nullptr;
static volatile bool dmaReadBusy = false;

extern "C" void dmaCh0_ISR(dmaInterrupt_t inttype) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    dmaReadBusy = false;

    if (xTaskToNotify == nullptr) {
        return;
    }

    // Notify the task that 
    vTaskNotifyGiveFromISR(xTaskToNotify, &xHigherPriorityTaskWoken);

    /* If xHigherPriorityTaskWoken is now set to pdTRUE then a
    context switch should be performed to ensure the interrupt
    returns directly to the highest priority task. */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

namespace CubeRover
{
    WatchDogRxTask::WatchDogRxTask()
        : m_numCallbacksRegistered(0),
          m_mpsm(),
          m_keepRunning(true),
          m_isRunning(false)
    {
    }

    // This probably will never be called, but I set it up to properly work anyway
    WatchDogRxTask::~WatchDogRxTask()
    {
        // Stop interrupt notifications
        xTaskToNotify = nullptr;

        // Stop looping
        m_keepRunning = false;

        // Make sure we aren't blocked
        if (xTaskToNotify != nullptr) {
            xTaskNotifyGive(xTaskToNotify);
        }
        
        // Join the thread
        void* value;
        this->join(&value);
    }

    ::Os::Task::TaskStatus WatchDogRxTask::startTask(NATIVE_INT_TYPE priority,
                                                     NATIVE_INT_TYPE stackSize,
                                                     NATIVE_INT_TYPE cpuAffinity)
    {
        if (m_isRunning) {
            return Os::Task::TASK_UNKNOWN_ERROR;
        }

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

    bool WatchDogRxTask::registerCallback(WatchDogRxCallbackProcessor* callback)
    {
        if (m_numCallbacksRegistered >= WATCHDOG_RX_TASK__MAX_NUM_CALLBACKS) {
            return false;
        }

        m_callbacks[m_numCallbacksRegistered] = callback;
        m_numCallbacksRegistered++; 

        return true;
    }

#define NUM_DMA_UPDATE_MSGS 6

    static char dmaUpdateMsgs[NUM_DMA_UPDATE_MSGS][72] = {};
    static size_t dmaUpdatesUsed = 0;
    static size_t dmaUpdatesHead = 0;

    void writeDmaUpdate(const char* dest, uint8_t* data, size_t dataSize) {
        bool overwrite = (dmaUpdatesUsed == NUM_DMA_UPDATE_MSGS);
        size_t writeIndex = overwrite ? dmaUpdatesHead : dmaUpdatesUsed;

        char* buf2 = dmaUpdateMsgs[writeIndex];
        char* endofbuf = buf2 + sizeof(dmaUpdateMsgs[writeIndex]);
        buf2 += sprintf(buf2, "%s: ", dest);

        for (size_t i = 0; i < dataSize; i++)
        {
            if (buf2 + 4 < endofbuf)
            {
                buf2 += sprintf(buf2, "%02X ", data[i]);
            }
        }
        *buf2 = '\0';

        if (overwrite) {
            dmaUpdatesHead++;

            while (dmaUpdatesHead >= NUM_DMA_UPDATE_MSGS) {
                dmaUpdatesHead -= NUM_DMA_UPDATE_MSGS;
            }
        } else {
            dmaUpdatesUsed++;
        }
    }

    void WatchDogRxTask::printDmaUpdates()
    {
        for (size_t i = 0; i < dmaUpdatesUsed; ++i) {
            size_t wrapped = dmaUpdatesHead + i;
            while (wrapped >= NUM_DMA_UPDATE_MSGS) {
                wrapped -= NUM_DMA_UPDATE_MSGS;
            }

            fprintf(stderr, "%d: %s\n", i, dmaUpdateMsgs[wrapped]);
        }
        fprintf(stderr, "\n\n");

        dmaUpdatesHead = 0;
        dmaUpdatesUsed = 0;
    }

    void WatchDogRxTask::rxHandlerTaskFunction(void* arg)
    {
        WatchDogRxTask* task = static_cast<WatchDogRxTask*>(arg);
        bool lookingForHeader = true;

        // First, construct the Message we'll use throughout
        WatchDogMpsm::Message msg(task->m_dataBuffer, sizeof(task->m_dataBuffer));

        uint8_t* lastTransferDestination = nullptr;
        unsigned lastTransferSize = 0;

        while (!task->m_keepRunning); // Wait until keepRunning has been set true

        while (task->m_keepRunning) {
            // First handle the last transfer (if this isn't the first loop, in which case this will be skipped)
            if (lastTransferDestination != nullptr && lastTransferSize != 0) {
                if (lookingForHeader) {
                    task->m_mpsm.notifyHeaderDmaComplete(msg, lastTransferDestination, lastTransferSize);
                    writeDmaUpdate("head", lastTransferDestination, lastTransferSize);
                } else {
                    task->m_mpsm.notifyDataDmaComplete(msg, lastTransferSize);
                    writeDmaUpdate("data", lastTransferDestination, lastTransferSize);
                }

                lastTransferDestination = nullptr;
                lastTransferSize = 0;
            }

            // Then handle the finished message (if there is one) and set up the next DMA transfer
            uint8_t* nextTransferDestination = nullptr;
            unsigned nextTransferSize = 0;

            if (lookingForHeader) {
                WatchDogMpsm::ParseHeaderStatus phStatus = task->m_mpsm.getHeaderDmaDetails(msg,
                                                                                            &nextTransferDestination,
                                                                                            nextTransferSize);

                bool doneBadParity = (WatchDogMpsm::ParseHeaderStatus::PHS_PARSED_HEADER_BAD_PARITY == phStatus);
                bool doneGoodParity = ((WatchDogMpsm::ParseHeaderStatus::PHS_PARSED_VALID_HEADER == phStatus)
                                       && msg.parsedHeader.payloadLength == 0);

                if (doneGoodParity || doneBadParity) {
                    task->callAllCallbacks(msg, doneGoodParity);
                    msg.reset();
                    lookingForHeader = true;

                    phStatus = task->m_mpsm.getHeaderDmaDetails(msg,
                                                                &nextTransferDestination,
                                                                nextTransferSize);

                    // We just reset the message, so we should always need more data here
                    assert(phStatus == WatchDogMpsm::ParseHeaderStatus::PHS_NEED_MORE_DATA);
                } else if (WatchDogMpsm::ParseHeaderStatus::PHS_PARSED_VALID_HEADER == phStatus) {
                    // We're done with the header but not with the message, because payload length is
                    // non-zero. Make the data dma request and change the "state" (represented by lookingForHeader)
                    WatchDogMpsm::ParseDataStatus pdStatus = task->m_mpsm.getDataDmaDetails(msg,
                                                                                            &nextTransferDestination,
                                                                                            nextTransferSize);

                    // It shouldn't be possible for us to not need data here
                    assert(pdStatus == WatchDogMpsm::ParseDataStatus::PDS_NEED_MORE_DATA);

                    lookingForHeader = false;
                } else if (WatchDogMpsm::ParseHeaderStatus::PHS_NEED_MORE_DATA != phStatus) {
                    // Shouldn't be possible to be in a status other than ParseDataStatus at this point
                    // If we were in VALID_HEADER with dataLen == 0 or BAD_PARITY, the first case would
                    // have been entered. If we were in VALID_HEADER wth dataLen > 0, the second case 
                    // would have been entered. The only other possibility should be NEED_MORE_DATA,
                    // so if that's NOT the case here then we want to assert.
                    assert(false);
                }
            } else { // Rather than header data, we should have received payload data in the last transfer
                WatchDogMpsm::ParseDataStatus pdStatus = task->m_mpsm.getDataDmaDetails(msg,
                                                                                        &nextTransferDestination,
                                                                                        nextTransferSize);


                // Since this is DMA and the receive buffer is large enough for the largest message size,
                // it should only ever take one transfer for data DMA to complete.
                assert(pdStatus == WatchDogMpsm::ParseDataStatus::PDS_PARSED_ALL_DATA);

                // Now handle the completed message
                task->callAllCallbacks(msg, true);
                msg.reset();
                lookingForHeader = true;

                WatchDogMpsm::ParseHeaderStatus phStatus = task->m_mpsm.getHeaderDmaDetails(msg,
                                                                                            &nextTransferDestination,
                                                                                            nextTransferSize);

                // We just reset the message, so we should always need more data here
                assert(phStatus == WatchDogMpsm::ParseHeaderStatus::PHS_NEED_MORE_DATA);
            }

            // Start the transfer (for either header or data)
            /*
            sciDMARecv(SCILIN_RX_DMA_CH, 
                       reinterpret_cast<char *>(nextTransferDestination),
                       nextTransferSize,
                       ACCESS_8_BIT,
                       &dmaReadBusy);
                       */
            int32_t payload_read = sciReceiveWithTimeout(scilinREG, nextTransferSize, reinterpret_cast<uint8_t *>(nextTransferDestination), 100000000);

            if ((size_t) payload_read != nextTransferSize) {
                fprintf(stderr, "pr: %d, nts: %d\n\n\n", payload_read, (int) nextTransferSize);
            }

            assert(nextTransferSize > 0);

            // Copy over the destination and size for the next iteration.
            lastTransferDestination = nextTransferDestination;
            lastTransferSize = nextTransferSize;

            // Block until there is more data to work with. The DMA completion interrupt will wake us up
            //ulTaskNotifyTake(pdTRUE, /* Clear the notification value before exiting. */
            //                 portMAX_DELAY); /* Block indefinitely. */
        }
    }

    void WatchDogRxTask::callAllCallbacks(WatchDogMpsm::Message& msg, bool goodParity)
    {
        for (size_t i = 0; i < m_numCallbacksRegistered; ++i) {
            m_callbacks[i]->rxCallback(msg, goodParity);
        }
    }


} // end namespace CubeRover
