#include <HAL/include/FreeRTOS.h>
#include <HAL/include/os_task.h>
#include <HAL/include/os_portmacro.h>
#include <HAL/include/sys_dma.h>

#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/EightyCharString.hpp>

#include <App/DMA.h>

#include <CubeRover/Wf121/Wf121.hpp>
#include <CubeRover/Wf121/Wf121SerialInterface.hpp>
#include <CubeRover/Wf121/NetworkInterface.hpp>
#include <CubeRover/Wf121/Timestamp.hpp>
#include <CubeRover/Wf121/Wf121BgApiPassthroughTxTask.hpp>

// Handle to active task (this):
static TaskHandle_t xActiveTask = nullptr; // not init'd yet

namespace Wf121
{
    // Statically-allocated queue of BGAPI messages to send to the Radio.
    // NOTE: This is *NOT* a circular buffer. Data is just dropped if it
    // fills (to ensure command order - i.e. so we don't drop Command A if
    // it was supposed to be received before Command B).
    // This is filled here, by `NetworkInterface`, and is designed to be
    // drained by the `NetworkManager` FPrime component.
    // NOTE: Static allocation here only works if there's only one instance of
    // NetworkInterface (which should be the case) - it belongs to RadioDriver,
    // which belongs to NetworkManager, which there should only be one of.
    // Size of Messages in `m_xBgApiTxMessageQueue`:
    static const size_t BGAPI_TX_MESSAGE_QUEUE_ITEM_SIZE = sizeof(Wf121BgApiPassthroughTxTask::BgApiPassthroughMessage);
    // Number of Messages to keep in the `m_xBgApiTxMessageQueue` (note: each payload is 6+WF121_BGAPI_PASSTHROUGH_MAX_MESSAGE_SIZE bytes long).
    static const uint8_t BGAPI_TX_MESSAGE_QUEUE_DEPTH = 3;
    // Static queue storage area:
    static uint8_t ucBgApiTxMessageQueueStorageArea[BGAPI_TX_MESSAGE_QUEUE_DEPTH * BGAPI_TX_MESSAGE_QUEUE_ITEM_SIZE];
    /* The variable used to hold the queue's data structure. */
    static StaticQueue_t xBgApiTxMessageStaticQueue;

    // Statically-allocated storage for BGAPI message send response statuses:
    // (the `m_xBgApiTxStatusQueue`):
    static const size_t BGAPI_TX_STATUS_QUEUE_ITEM_SIZE = sizeof(Wf121BgApiPassthroughTxTask::BgApiCommandSendStatusMessage);
    // Number of Messages to keep in the `m_xBgApiTxMessageQueue` (note: each payload is 8 bytes long).
    static const uint8_t BGAPI_TX_STATUS_QUEUE_DEPTH = BGAPI_TX_MESSAGE_QUEUE_DEPTH + 1;
    // Static queue storage area:
    static uint8_t ucBgApiTxStatusQueueStorageArea[BGAPI_TX_STATUS_QUEUE_DEPTH * BGAPI_TX_STATUS_QUEUE_ITEM_SIZE];
    /* The variable used to hold the queue's data structure. */
    static StaticQueue_t xBgApiTxStatusStaticQueue;

    Wf121BgApiPassthroughTxTask::Wf121BgApiPassthroughTxTask(BgApi::BgApiDriver *bgapid)
        : m_pBgApiDriver(bgapid),
        : m_keepRunning(true),
          m_isRunning(false)
    {
        // Set up write buffer:
        m_xBgApiTxStatusQueue = NULL;  // for now (NULL until initialized)
        m_xBgApiTxMessageQueue = NULL; // for now (NULL until initialized)
    }

    // This probably will never be called, but it should up to properly work anyway
    Wf121BgApiPassthroughTxTask::~Wf121BgApiPassthroughTxTask()
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

    ::Os::Task::TaskStatus Wf121BgApiPassthroughTxTask::startTask(NATIVE_INT_TYPE priority,
                                                                  NATIVE_INT_TYPE stackSize,
                                                                  NATIVE_INT_TYPE cpuAffinity)
    {
        if (m_isRunning)
        {
            return Os::Task::TASK_UNKNOWN_ERROR;
        }

        // Create and initialize TX Comms output queue (do this before the
        // actual task starts).
        m_xBgApiTxStatusQueue = xQueueCreateStatic(
            /* The number of items the queue can hold. */
            BGAPI_TX_STATUS_QUEUE_DEPTH,
            BGAPI_TX_STATUS_QUEUE_ITEM_SIZE,
            ucBgApiTxStatusQueueStorageArea,
            &xBgApiTxStatusStaticQueue);
        /* Queue buffer was not NULL so xQueue should not be NULL. */
        configASSERT(m_xBgApiTxStatusQueue);

        // Create and initialize TX Comms input queue (do this before the
        // actual task starts).
        m_xBgApiTxMessageQueue = xQueueCreateStatic(
            /* The number of items the queue can hold. */
            BGAPI_TX_MESSAGE_QUEUE_DEPTH,
            BGAPI_TX_MESSAGE_QUEUE_ITEM_SIZE,
            ucBgApiTxMessageQueueStorageArea,
            &xBgApiTxMessageStaticQueue);
        /* Queue buffer was not NULL so xQueue should not be NULL. */
        configASSERT(m_xBgApiTxMessageQueue);

        m_keepRunning = true;
        Fw::EightyCharString task_name("Wf121BgApiTxTask"); // max len is 16

        TaskHandle_t tid;
        BaseType_t stat = xTaskCreate(Wf121BgApiPassthroughTxTask::bgapiTxHandlerTaskFunction,
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

    /**
     * @brief Pushes a status into the BGAPI Passthrough Message Queue
     * (`m_xBgApiTxMessageQueue`). This is for use by other tasks to tell
     * this task what to send.
     *
     * NOTE: BGAPI message sending happens asynchronously and a resulting
     * status (tagged with packetId) will be placed in the BGAPI Send Status
     * queue (`m_xBgApiTxStatusQueue`) once sending is completed
     * successfully or times out. This status can be retrieved using
     * `getMessageResponse`.
     *
     * @param pMsg Pointer to the BGAPI message to enqueue.
     * @param blockingTicks How many FreeRTOS scheduler ticks to block the
     *      active task for while waiting for the queue to have room to fit
     *      this data. Default is 3 as a safety but 0 is perfectly
     *      acceptable, in which case it will returns immediately, whether
     *      or not room was available. Room should always be available unless
     *      this task is halted for some reason (or you're sending messages
     *      to a dead Radio at > BGAPI_TX_MESSAGE_QUEUE_DEPTH messages per
     *      BGAPI_CMD_PROCESSING_TIMEOUT_MS without checking for a response
     *      - currently this comes out to >1message/1.2sec), in which case
     *      there are bigger problems.
     *
     * @return Whether the new message was enqueued successfully.
     */
    bool Wf121BgApiPassthroughTxTask::enqueueMessage(BgApiPassthroughMessage *pMsg, TickType_t blockingTicks)
    {
        // Push data into inter-process status Queue:
        // NOTE: This is *NOT* a circular buffer. Data is just dropped if it
        // fills (to ensure command order - i.e. so we don't drop Command A
        // if it was supposed to be received before Command B).
        // Earth will get command ACKs and we'll use these to determine if
        // a command wasn't received and we need to resend.
        if (m_xBgApiTxMessageQueue != NULL)
        {
            // Push into queue. Drop if queue isn't available in `blockingTicks`.
            // (Queue being available means there's space for another item in it).
            // **DON'T** increase this tick count to some large value,
            // it will halt everything. It should be safe for it to be 0
            // even, we're making it *slightly* non-zero here only as a
            // precaution.
            // NOTE: This send procedure is a **COPY** (so we don't care
            // about `pStatus` after this).
            if (xQueueSend(m_xBgApiTxMessageQueue, (void *)pMsg, blockingTicks) == pdPASS)
            {
                // Enqueued successfully.
                return true;
            }
            else
            {
                /* Queue was full and wouldn't accept new data.
                   As noted above, this is not great but it's fine, we just
                   drop the packet. Likely the system health is very bad
                   right now, so we need to just let it slide.
                */
                return false;
            } // xQueueSend: m_xBgApiTxMessageQueue
        }     // m_xBgApiTxMessageQueue exists?
        return false;
    }

    /**
     * @brief Pushes a status into the BGAPI Send Status queue
     * (`m_xBgApiTxStatusQueue`). Only for use inside this task (hence why
     * it's private).
     *
     * @param pStatus Pointer to status message to enqueue.
     *
     * @return Whether the new status was enqueued successfully.
     */
    bool Wf121BgApiPassthroughTxTask::enqueueMessageResponse(BgApiCommandSendStatusMessage *pStatus)
    {
        // Push data into inter-process status Queue:
        // NOTE: This is *NOT* a circular buffer. Data is just dropped if it
        // fills (to ensure command order - i.e. so we don't drop Command A
        // if it was supposed to be received before Command B).
        // Earth will get command ACKs and we'll use these to determine if
        // a command wasn't received and we need to resend.
        if (m_xBgApiTxStatusQueue != NULL)
        {
            // Push into queue. Drop if queue isn't available in
            // `WF121_BGAPI_PASSTHROUGH_STATUS_ENQUEUE_WAIT_TICKS` ticks.
            // (Queue being available means there's space for another item in it).
            // **DON'T** increase this tick count to some large value,
            // it will halt everything. It should be safe for it to be 0
            // even, we're making it *slightly* non-zero here only as a
            // precaution.
            // NOTE: This send procedure is a **COPY** (so we don't care
            // about `pStatus` after this).
            if (xQueueSend(m_xBgApiTxStatusQueue, (void *)pStatus, WF121_BGAPI_PASSTHROUGH_STATUS_ENQUEUE_WAIT_TICKS) == pdPASS)
            {
                // Enqueued successfully.
                return true;
            }
            else
            {
                /* Queue was full and wouldn't accept new data.
                   As noted above, this is not great but it's fine, we just
                   drop the packet. Likely the system health is very bad
                   right now, so we need to just let it slide.
                */
                return false;
            } // xQueueSend: m_xBgApiTxStatusQueue
        }     // m_xBgApiTxStatusQueue exists?
        return false;
    }

    /**
     * @brief Checks the BGAPI Send Status queue (`m_xBgApiTxStatusQueue`)
     * to see if there are any new response statuses. If so, it copies that
     * data into `pStatus`.
     *
     * NOTE: BGAPI message sending happens asynchronously and a status is
     * only placed in the output queue only when sending is completed
     * successfully or times out. All this does is see if there are any
     * unread statuses.
     *
     * NOTE: This queue only has a depth of `BGAPI_TX_STATUS_QUEUE_DEPTH`
     * payloads. By design, to preserve order, `Wf121BgApiPassthroughTxTask`
     * will drop new response statuses if the Queue is full, so make sure
     * this function is being called enough to keep the Queue below
     * `BGAPI_TX_STATUS_QUEUE_DEPTH` (that is, you should check this
     * frequently and definitely at an equal or higher frequency than you
     * send messages into the `m_xBgApiTxMessageQueue`).
     *
     * @param pStatus Pointer to status message to load data into.
     * @param blockingTicks How many FreeRTOS scheduler ticks to block the
     *      active task for while waiting for data to be available.
     *      Default is 0, which means it returns immediately, whether or not
     *      data was available. Since statuses are Queued, this behavior is
     *      desirable b/c it lets us reap all the benefits of using a Queue
     *      to not have to wait for things in the first place.
     *
     * @return Whether a new status was found.
     */
    bool Wf121BgApiPassthroughTxTask::getMessageResponse(BgApiCommandSendStatusMessage *pStatus, TickType_t blockingTicks)
    {
        if (m_xBgApiTxStatusQueue != NULL)
        {
            // Grab a value from the Queue. Give up (assume nothing is
            // currently available) if nothing is available within N ticks.
            // **DON'T** increase this tick count to some large value,
            // it will halt everything. It should be safe for it to even be 0,
            // you could make it *slightly* non-zero here only as a precaution.
            // NOTE: This send procedure is a **COPY**.
            // NOTE: At FreeRTOS 1000Hz tick rate, each tick is 1ms.
            if (xQueueReceive(m_xBgApiTxStatusQueue, (void *)pStatus, blockingTicks) == pdPASS)
            {
                // Got a status from the queue!
                // Nothing special to do here, but this is where you'd do it.
                return true;
            }
            else
            {
                // Nothing was available (which is fine), just no new data yet.
                return false;
            }
        }
        return false;
    }

    /**
     * @brief Waits until the Radio's BGS is done processing any outstanding
     * commands (either b/c we received a response or we infer it's done b/c we
     * haven't received a response for >> the Radio's max processing time
     * (timeout is BGAPI_CMD_PROCESSING_TIMEOUT_MS).
     *
     * @return The time [ms] it took for us to get a response or time out (at
     * this level, we can't directly know which it is, however, if you call this
     * right after flagging that processing has started and the time is
     * anywhere near `BGAPI_CMD_PROCESSING_TIMEOUT_MS`, it was likely a
     * timeout - NOTE: it could time out still return
     * < BGAPI_CMD_PROCESSING_TIMEOUT_MS because the timeout timer starts when
     * command processing is started not when we start this wait function.
     * Also NOTE that BGAPI_CMD_PROCESSING_TIMEOUT_MS is >> than the Radio's
     * actual processing time so if this returns even
     * >0.5*BGAPI_CMD_PROCESSING_TIMEOUT_MS, the Radio likely didn't send a
     * response to what we sent (or possibly we missed it due to data
     * corruption).
     * NOTE: If the BGAPI processor thinks the radio is busy when you call this,
     * the task will suspend for
     * `WF121_BGAPI_READY_TO_SEND_POLLING_CHECK_INTERVAL`, meaning that the
     * result will always be approximately a multiple of
     * `WF121_BGAPI_READY_TO_SEND_POLLING_CHECK_INTERVAL`.
     */
    uint32_t Wf121BgApiPassthroughTxTask::pollUntilBgApiReady()
    {
        // Poll (suspending this task) until BGAPI is done processing
        // the last command sent.
        uint32_t start = Timestamp::getTimeMs();
        while (m_pBgApiDriver->CommandIsProcessing())
        {
            // NOTE: `CommandIsProcessing` includes a timeout check
            // of `BGAPI_CMD_PROCESSING_TIMEOUT_MS` from the time processingCmd
            // processing was last set to true (so this loop isn't infinite).
            vTaskDelay(WF121_BGAPI_READY_TO_SEND_POLLING_CHECK_INTERVAL);
        }
        return Timestamp::getTimeMs() - start;
    }

    /**
     * @brief Core data-sending (and response checking) operation
     * (dispatched to by `bgapiTxHandlerTaskFunction` once all queue guards
     * etc. have been taken care of).
     *
     * @param pMsg Pointer to the `BgApiPassthroughMessage` to send.
     *
     * @return The resulting status of the send operation.
     */
    BgApiCommandSendStatus Wf121BgApiPassthroughTxTask::attemptSend(BgApiPassthroughMessage *pMsg)
    {
        // Only attempt to send if we need to send non-zero number of bytes:
        if (pMsg->dataLen == 0 || pMsg->dataLen > WF121_BGAPI_PASSTHROUGH_MAX_MESSAGE_SIZE)
        {
            return BgApiCommandSendStatus::BAD_LEN;
        }

        // Make sure Hercules' BGAPI processor isn't currently awaiting a
        // command response from the Radio for something else:
        pollUntilBgApiReady();

        // Do one final passthrough check right before sending (be VERY sure
        // we're not garbling UdpTx's data):
        // If we're not currently in passthrough, that means it changed while
        // we were waiting for BgApi to be ready.
        // We should just throw out this packet (not send it), and let Ground
        // know what happened. Ground will handle it from there if it thinks a
        // resend is necessary.
        if (!Wf121::persistentBgApiPassthroughEnabled())
        {
            // If pass through isn't enabled, don't send this data...
            // and let the outside world know:
            return BgApiCommandSendStatus::BAD_STATE;
        }

        // Flag that we're about to send something (and should expect a
        // response):
        // READ THE COMMENT IN THE FUNCTION FOR SOME PRECAUTIONS IF YOU'RE
        // THINKING ABOUT USING IT ELSEWHERE.
        m_pRadioDriver->m_networkInterface.AwaitCommandResponse();

        // block task until we can send those bytes:
        bool sendSuccess = false;
        uint32_t tries = 0;
        while (!sendSuccess && tries < Wf121BgApiPassthroughTxTask::MAX_DMA_SEND_TRIES)
        {
            sendSuccess = Wf121Serial::dmaSend((void *)(pMsg->rawData), pMsg->dataLen, true);
            tries++;
        }
        if (!sendSuccess && tries == Wf121BgApiPassthroughTxTask::MAX_DMA_SEND_TRIES)
        {
            // Report failed UART send after hitting max tries:
            return BgApiCommandSendStatus::UART_SEND_FAILED;
        }

        // Send worked. Wait for response from Radio, if we are expecting a
        // response:
        if (!pMsg->expectResponse)
        {
            // We were successful but still let Ground know the truth that we
            // didn't receive a response (in this case b/c we didn't look for
            // one):
            return BgApiCommandSendStatus::SUCCESS_NO_RESP;
        }
        else
        {
            uint32_t responseTime = pollUntilBgApiReady();

            if (responseTime > 0.5 * BGAPI_CMD_PROCESSING_TIMEOUT_MS)
            {
                // Took way too long to get a response for this to be normal.
                // Likely we didn't actually get a response and just timed out.
                // (See comment in `pollUntilBgApiReady` for more details about why
                // we're checking for this in this way.)
                // Let Ground know that the final result was that we sent
                // successfully but didn't get a response from the Radio (or we
                // missed it due to data corruption or something).
                return BgApiCommandSendStatus::SUCCESS_NO_RESP;
            }
            else
            {
                // We got a response in a reasonable amount of time. Everything
                // worked. Let Ground know.
                return BgApiCommandSendStatus::SUCCESS;
            }
        }
    }

    void Wf121BgApiPassthroughTxTask::bgapiTxHandlerTaskFunction(void *arg)
    {
        // Note: This is a FreeRTOS Task, so "blocking forever" just means
        // the **Task** doesn't do anything else when FreeRTOS switches to it;
        // system itself won't halt.
        // More on FreeRTOS Tasks: https://www.freertos.org/taskandcr.html
        Wf121BgApiPassthroughTxTask *task = static_cast<Wf121BgApiPassthroughTxTask *>(arg);

        // Block task for 50ms to wait for serial to come up and be ready
        // (NOTE: this is already handled elsewhere and this task isn't started
        // until serial is ready **but** this gives us some safety to wait for
        // hardware to get ready):
        vTaskDelay(50 / portTICK_PERIOD_MS);

        while (!task->m_keepRunning)
        {
            // Wait until keepRunning has been set true
        };

        // Data struct for working with TXing BGAPI data internally (receiving data from the Queue):
        BgApiPassthroughMessage xBgApiTxWorkingData;
        // Pre-fill working buffer with sentinel byte:
        xBgApiTxWorkingData.clear();

        while (task->m_keepRunning)
        {
            while (task->m_xBgApiTxMessageQueue == NULL)
            {
                // Make sure Queue is initialized before trying to receive on it.
                // Should be by this point but, if we're here, clearly something fucked up.
                // We want this to be a tight loop that halts everything so that,
                // if this isn't resolved quickly (i.e. if it wasn't a temporary blip),
                // WD resets us.
            }

            // Block Task until new data shows up in the BGAPI TX Queue to send.
            // This  can be forever since there's nothing else for this task to
            // do until there's available data:
            // NOTE: This receive procedure is a **COPY**.
            while (xQueueReceive(task->m_xBgApiTxMessageQueue, (void *)(&xBgApiTxWorkingData), portMAX_DELAY) != pdPASS)
            {
                // No data was received but awaiting data timed out (after a **really** long time)
                // This shouldn't ever happen unless someone set `INCLUDE_vTaskSuspend` to `0`.
                // If that is the case, just go back to listening.
            }

            // Suspend the task if Passthrough isn't enabled,
            // checking back every once in a while to see if it's been enabled
            // (we don't need to respond right away (in tight timing) to being
            // freed):
            if (!Wf121::persistentBgApiPassthroughEnabled())
            {
                // If pass through isn't enabled, don't send this data...
                // and let the outside world know:
                task->enqueueMessageResponse({xBgApiTxWorkingData.packetId,
                                              BgApiCommandSendStatus::BAD_STATE});
                // Then just wait for it to come back up...
                while (!Wf121::persistentBgApiPassthroughEnabled())
                {
                    // Instead, check back every once in a while to see if
                    // we've been freed.
                    vTaskDelay(WF121_BGAPI_TX_TASK_PASSTHROUGH_PAUSE_CHECK_MS);
                }
                // Once freed, restart the loop (toss whatever data it was we were going to send):
                continue;
            }

            // Attempt to send data (and check for ACK response from Radio):
            BgApiCommandSendStatus status = task->attemptSend(&xBgApiTxWorkingData);

            // Enqueue whatever the status was:
            task->enqueueMessageResponse({xBgApiTxWorkingData.packetId,
                                          status});

            // ... next loop we do it all over again.
        }
    }

} // end namespace
