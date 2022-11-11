/***
 * This Task handles writing raw (ideally BGAPI) data to the WF121 Radio.
 * This is a STRICT alternative to Wf121UdpTxTask for writing to the Radio UART
 * interface (should only run while that task isn't).
 *
 * NOTE: If you just want to send raw data at the radio, you can use
 * Wf121Serial::dmaSend BUT SHOULD BE REALLY CAREFUL BECAUSE IT'S BAD PRACTICE
 * TO HAVE MORE THAN ONE TASK ALLOWED TO INTERACT WITH AN INTERFACE AT A TIME.
 * Instead, consider switching in/out a comms task, like how `Wf121UdpTxTask`
 * shares the interface with `Wf121BgApiPassthroughTxTask`.
 */

#ifndef CUBEROVER_WF121_WF121_BGAPI_PASSTHROUGH_TX_TASK_HPP_
#define CUBEROVER_WF121_WF121_BGAPI_PASSTHROUGH_TX_TASK_HPP_

#include <Include/FswPacket.hpp>
#include <CubeRover/Wf121/Wf121SerialInterface.hpp>
#include <CubeRover/Wf121/UdpPayload.hpp>
#include <CubeRover/Wf121/Wf121BgApi.hpp>

#include <Os/Task.hpp>
#include <Os/Mutex.hpp>

#include <cassert>
#include <cstdio>

#include "sci.h"
#include "FreeRTOS.h"
#include "os_queue.h"

// When BGAPI Passthrough is off, how frequently should the Task should
// check back to see if BGAPI Passthrough has been turned on (so the task can
// proceed):
static const TickType_t WF121_BGAPI_TX_TASK_PASSTHROUGH_PAUSE_CHECK_MS = 100 / portTICK_PERIOD_MS; // every 100ms (100 ticks)

// - How long this task should suspend before checking back in to see if we
//   meet all criteria for sending data to the Radio.
// - Since this is a high priority task, it's not a good idea for this to be 0
//   (though it *can* be zero) in order to prevent Task starvation.
// - In fact, since it's only likely to happen either A.) At boot when the
//   radio is still connecting or B.) anytime the Radio resets due to an
//   internal fault (or requested reset), it's okay for this to be quite long
//   to give other Tasks room to work.
// NOTE: This only suspends this task, so it doesn't block the main Task.
// NOTE: FreeRTOS scheduler ticks are every 1ms.
static const TickType_t WF121_BGAPI_READY_TO_SEND_POLLING_CHECK_INTERVAL = 200 / portTICK_PERIOD_MS; // every 200ms (200 ticks)

// Max number of bytes that can be sent in one BGAPI passthrough message (based
// on the fact that we have to uplink that data as a string).
// NOTE: Needs to be a define (not a const int) because it's used to size arrays.
#define WF121_BGAPI_PASSTHROUGH_MAX_MESSAGE_SIZE FW_CMD_ARG_BUFFER_MAX_SIZE

// Max number of FreeRTOS Scheduler ticks to allow the
// `Wf121BgApiPassthroughTxTask` to wait for the Status Queue to become free
// while attempting to put data into it.
// If the status Queue doesn't have space available by that time, the
// enqueuing status will be dropped - this would only really happen when there
// are other major issues halting the main FPrime queue, preventing the queue
// from being drained. In which case, it's unlikely we'll be trying to send
// multiple BGAPI messages in a row without hearing a response (it's unlikely
// we'd even be able to in this case.
static const TickType_t WF121_BGAPI_PASSTHROUGH_STATUS_ENQUEUE_WAIT_TICKS = 10;

namespace Wf121
{
    /**
     * The task responsible for sending packets of data to the WF121 Radio's
     * UDP endpoint. Specifically, this Task is responsible for owning the
     * dmaSend calls, while the actual logic for what to send and when is
     * delegated to `Wf121TxTaskManager`.
     */
    class Wf121BgApiPassthroughTxTask : public ::Os::Task
    {
    public:
        // Result of attempting to send a BgApi Message.
        // This is a *strict subset* of the values found
        // `nm_radio_send_bgapi_command_ack_status` but the int values need to
        // exactly match those used for the corresponding members in
        // `nm_radio_send_bgapi_command_ack_status`.
        // This is a subset and not identical because a number of the failure
        // cases in `nm_radio_send_bgapi_command_ack_status` will happen before
        // a message even gets posted to this Task's input queue.
        enum class BgApiCommandSendStatus : U32
        {
            // Bad length received (decoded). Either 0 or bigger than max
            // possible size: WF121_BGAPI_PASSTHROUGH_MAX_MESSAGE_SIZE. This
            // should also be checked outside of this task but we'll check here
            // too.
            BAD_LEN = 0x0BADDA77,
            // All given data was valid but failed to send the packet to the
            // Radio over UART. Try again?:
            UART_SEND_FAILED = 0xDA7AFA11,
            // Hercules is in the wrong state to do this (not in passthrough
            // mode - need to send
            // `Set_Radio_BgApi_Passthrough[passthrough=TRUE]` first):
            BAD_STATE = 0xBAD57A7E,
            // Data passed all validation and was sent to the Radio
            // successfully over UART but no response was received from the
            // radio, or we didn't look for a response if the command was
            // flagged as `DONT_EXPECT_RESPONSE` (this can be expected for some
            // BGAPI DFU flashing commands):
            SUCCESS_NO_RESP = 0x0005097A,
            // Data passed all validation and was sent to the Radio
            // successfully over UART and a response was received from the
            // radio (this doesn't necessarily happen for some BGAPI DFU
            // flashing commands):
            SUCCESS = 0x600DDA7A
        };

        // OUTPUT MESSAGE
        // Simple struct for containing the result of (trying to) sending
        // the `BgApiPassthroughMessage` with the given `packetId`.
        // For use in the `m_xBgApiTxStatusQueue` IPC Queue for sending data
        // from this task back o the main FPrime task.
        struct BgApiCommandSendStatusMessage
        {
            uint32_t packetId;
            BgApiCommandSendStatus resultingStatus;
        };

        // INPUT MESSAGE
        // Simple struct for containing raw BgApi passthrough data (NOTE:
        // length is the total length of all bytes to be sent).
        // For use in the `m_xBgApiTxMessageQueue` IPC Queue for sending data
        // from the main FPrime task to this task.
        struct BgApiPassthroughMessage
        {
            BgApiPassthroughMessage() : packetId(0), dataLen(0), expectResponse(true) {}
            // GSW-Assigned ID of the packet:
            uint32_t packetId;
            // Number of bytes in the comm buffer (i.e. number of bytes read / to be
            // sent):
            uint16_t dataLen;
            // Whether we should expect (and wait for) the Radio to respond to this command:
            bool expectResponse;
            // Actual raw data:
            uint8_t rawData[WF121_BGAPI_PASSTHROUGH_MAX_MESSAGE_SIZE] __attribute__((aligned(8))); // align to 8B for more efficient DMA send

            // Clear all data (overwrite with fixed sentinel byte):
            // (NOTE: don't want to just blindly do this on every ctor call
            // since, for a buffer this size, it's slightly expensive)
            void clear()
            {
                dataLen = 0;
                // Write 0xFA to all "empty" bytes in this array, this just makes debugging easier
                memset(this->rawData, 0xAB, WF121_BGAPI_PASSTHROUGH_MAX_MESSAGE_SIZE);
            }
        };

        /**
         * @brief Constructor. Does not start the task.
         */
        explicit Wf121BgApiPassthroughTxTask(BgApi::BgApiDriver *bgapid);

        /**
         * @brief Destructor. Stops the task if it is currently running.
         */
        ~Wf121BgApiPassthroughTxTask();

        /**
         * @brief Starts the FreeRTOS task that underlies this object.
         *
         * @param priority The priority to use for the task being started.
         * @param stackSize The stack size to use for the task being started.
         * @param cpuAffinity The CPU affinity to use for the task being
         *  started, or -1 to have no affinity.
         *
         * @return The status of starting the task.
         */
        ::Os::Task::TaskStatus startTask(NATIVE_INT_TYPE priority,
                                         NATIVE_INT_TYPE stackSize,
                                         NATIVE_INT_TYPE cpuAffinity = -1); //!< start the task

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
         *     **DON'T** increase this to some large value as it will halt
         *      everything in the RTOS Task you're calling this from.
         *
         * @return Whether the new message was enqueued successfully.
         */
        bool enqueueMessage(BgApiPassthroughMessage *pMsg, TickType_t blockingTicks = 3);

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
         *     **DON'T** increase this to some large value as it will halt
         *      everything in the RTOS Task you're calling this from.
         *
         * @return Whether a new status was found.
         */
        bool getMessageResponse(BgApiCommandSendStatusMessage *pStatus, TickType_t blockingTicks = 0);

    private:
        // Max number of times for the Task to attempt to push data to the DMA
        // TX buffer (note this can be **very** large without much penalty
        // since the task just blocks (yields) itself while waiting letting
        // other things try):
        static const uint32_t MAX_DMA_SEND_TRIES = 25;

        // Whether or not the task should keep running. The main loop in the
        // task thread is controlled by this.
        bool m_keepRunning;

        // Whether or not the task has been started. Only used to prevent
        // calling start(...) after it has already been called before.
        bool m_isRunning;

        // Internal BGAPI Processor (we need access to it to determine if we've
        // received a response from the radio):
        BgApi::BgApiDriver *m_pBgApiDriver;

        // OUTPUT QUEUE
        // Handle to statically-allocated queue of the resulting status from
        // trying to send BGAPI Passthrough messages.
        // NOTE: This is *NOT* a circular buffer. Data is just dropped if it
        // fills (to ensure command order - i.e. so we don't drop Command A if
        // it was supposed to be received before Command B).
        // This designed to be drained by an external task (specifically the
        // `NetworkManager`  FPrime component) and filled here by the
        // `Wf121BgApiPassthroughTxTask`.
        QueueHandle_t m_xBgApiTxStatusQueue;

        // INPUT QUEUE
        // Handle to statically-allocated queue of all BGAPI Passthrough
        // messages to be sent.
        // NOTE: This is *NOT* a circular buffer. Data is just dropped if it
        // fills (to ensure command order - i.e. so we don't drop Command A if
        // it was supposed to be received before Command B).
        // This is filled by an external task (specifically the `NetworkManager`
        // FPrime component) and is designed to be
        // drained here by the `Wf121BgApiPassthroughTxTask`.
        QueueHandle_t m_xBgApiTxMessageQueue;

        /**
         * @brief Pushes a status into the BGAPI Send Status queue
         * (`m_xBgApiTxStatusQueue`). Only for use inside this task (hence why
         * it's private).
         *
         * @param pStatus Pointer to status message to enqueue.
         *
         * @return Whether the new status was enqueued successfully.
         */
        bool enqueueMessageResponse(BgApiCommandSendStatusMessage status);

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
        uint32_t pollUntilBgApiReady();

        /**
         * @brief Core data-sending (and response checking) operation
         * (dispatched to by `bgapiTxHandlerTaskFunction` once all queue guards
         * etc. have been taken care of).
         *
         * @param pMsg Pointer to the `BgApiPassthroughMessage` to send.
         *
         * @return The resulting status of the send operation.
         */
        BgApiCommandSendStatus attemptSend(BgApiPassthroughMessage *pMsg);

        /**
         * The function that implements the task thread.
         *
         * @param arg The argument to the thread, which in this case will be
         *  the `this` pointer for this object.
         */
        static void bgapiTxHandlerTaskFunction(void *arg);
    };

} // end namespace Wf121

#endif // #ifndef CUBEROVER_WF121_WF121_BGAPI_PASSTHROUGH_TX_TASK_HPP_
