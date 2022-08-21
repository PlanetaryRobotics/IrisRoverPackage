/***
 * This Task handles writing UDP payloads (not packets with a datagram) to the
 * WF121 UDP downlink endpoint.
 * This is a Task because each write requires telling the WF121 how many bytes
 * to expect (it can only take 255 at a time), blocking until it gets a
 * response, and then sending the actual data.
 *
 * NOTE: If you just want to send raw data at the radio, you can use
 * Wf121Serial::dmaSend.
 */

#ifndef CUBEROVER_WF121_WF121_UDP_TX_TASK_HPP_
#define CUBEROVER_WF121_WF121_UDP_TX_TASK_HPP_

#include <Include/FswPacket.hpp>
#include <CubeRover/Wf121/Wf121SerialInterface.hpp>
#include <CubeRover/Wf121/UdpPayload.hpp>

#include <Os/Task.hpp>
#include <Os/Mutex.hpp>

#include <cassert>
#include <cstdio>

#include "FreeRTOS.h"
#include "os_queue.h"

namespace Wf121
{
    /**
     * @brief Defines the contract (i.e. interface) that must be implemented
     *      (via subclassing) by classes that want to handle TX write operations
     *      (once the data is available).
     *
     * This callback will be called with each message that is received by the
     * `Wf121RxTask` instance to which the object implementing this function
     * is registered.
     */
    class Wf121TxTaskHandler
    {
    public:
        /**
         * @brief The callback invoked by the `Wf121RxTask` when it has
         *  received a message.
         *
         * @param msg Generic wrapper for message received from WF121.
         */
        virtual void txHandler(Wf121Parser::GenericMessage &msg) = 0;

        virtual ~Wf121RxCallbackProcessor() = default;
    };

    /**
     * The task responsible for sending packets of data to the WF121 Radio's
     * UDP endpoint.
     *
     * NOTE from `Wf121TxTask` which uses the same interface:
     * This is a subclass of Os::Task so that it can access the `m_handle` field of Os::Task. Os::Task was modified
     * in order to make this field protected. The `m_handle` field contains the native handle of the underlying
     * implementation of Os::Task. In our case, we expect this will always be run on the Hercules, and that the
     * underlying Os::Task implementation will be the one that is implemented using FreeRTOS. We want access to this
     * because we want to use FreeRTOS Task Notifications (https://www.freertos.org/RTOS-task-notifications.html)
     * rather than FreeRTOS Semaphores in order to implement the task blocking and then being woken up by an
     * external trigger. We want to use Task Notifications over Semaphores because "Unblocking an RTOS task with
     * a direct notification is 45% faster and uses less RAM than unblocking a task using an intermediary
     * object such as a binary semaphore." (Source: https://www.freertos.org/RTOS-task-notifications.html).
     * However, in order to use Task Notifications we need access to the underlying task handle, thus the
     * need for the modifications and usage described above.
     */
    class Wf121UdpTxTask : public ::Os::Task
    {
    public:
        /**
         * @brief Constructor. Does not start the task.
         */
        explicit Wf121UdpTxTask(QueueHandle_t *pTxPayloadQueue);

        /**
         * @brief Destructor. Stops the task if it is currently running.
         */
        ~Wf121UdpTxTask();

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

    private:
        // Pointer to FreeRTOS Queue of all UDP payloads to be sent
        // to the Radio (other Tasks push to it, this Task reads from it).
        QueueHandle_t *m_pTxPayloadQueue;

        // Function to be called when the task triggers:
        void (*foo)(void *);

        // Whether or not the task should keep running. The main loop in the
        // task thread is controlled by this.
        bool m_keepRunning;

        // Whether or not the task has been started. Only used to prevent
        // calling start(...) after it has already been called before.
        bool m_isRunning;

        // Data struct for working with TXing UDP data internally:
        UdpTxPayload m_xUdpTxWorkingData;

        /**
         * The function that implements the task thread.
         *
         * @param arg The argument to the thread, which in this case will be
         *  the `this` pointer for this object.
         */
        static void udpTxHandlerTaskFunction(void *arg);
    };

} // end namespace CubeRover

#endif // #ifndef CUBEROVER_WF121_WF121_UDP_TX_TASK_HPP_
