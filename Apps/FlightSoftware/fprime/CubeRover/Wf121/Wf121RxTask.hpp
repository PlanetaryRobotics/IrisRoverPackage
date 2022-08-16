/***
 * The goal of using a Task to handle the SCI RX ISR is so that we can extract
 * bytes as soon as they're available from interface and then process them quickly
 *
 * So, the goal:
 * - Grab bytes the moment they're available.
 * - Process them using the Wf121Parser::Mpsm as often as the task is serviced.
 */

#ifndef CUBEROVER_WF121_WF121_RX_TASK_HPP_
#define CUBEROVER_WF121_WF121_RX_TASK_HPP_

#include <CubeRover/Wf121/Wf121Parser.hpp>
#include <Os/Task.hpp>

// Max. number of processors that can register to receive callbacks from the
// Wf121RxTask (should be only two: BGAPI processor & Direct Message Processor).
#define WF121_RX_TASK__MAX_NUM_CALLBACKS 5

namespace Wf121
{
    /**
     * @brief Defines the contract (i.e. interface) that must be implemented
     *      (via subclassing) by classes that want to receive callbacks from
     *      the `Wf121RxTask` with received messages.
     *
     * This callback will be called with each message that is received by the
     * `Wf121RxTask` instance to which the object implementing this function
     * is registered.
     */
    class Wf121RxCallbackProcessor
    {
    public:
        /**
         * @brief The callback invoked by the `Wf121RxTask` when it has
         *  received a message.
         *
         * @param msg Generic wrapper for message received from WF121.
         */
        virtual void rxCallback(Wf121Parser::GenericMessage &msg) = 0;

        virtual ~Wf121RxCallbackProcessor() = default;
    };

    /**
     * The task responsible for receiving and parsing messages from the WF121 Radio.
     *
     * NOTE from `WatchDogRxTask` which uses the same interface:
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
    class Wf121RxTask : public ::Os::Task
    {
    public:
        /**
         * @brief Constructor. Does not start the task.
         */
        explicit Wf121RxTask();

        /**
         * @brief Destructor. Stops the task if it is currently running.
         */
        ~Wf121RxTask();

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
         * @brief Registers the given object as a callback to be invoked once
         *  a message is recieved and parsed.
         *
         * @param callback The callback object to register.
         *
         * @return True if the registration succeeded, otherwise false.
         */
        bool registerCallback(Wf121RxCallbackProcessor *callback);

    private:
        // The array containing callbacks to be invoked upon receiving and
        // parsing a message.
        Wf121RxCallbackProcessor *m_callbacks[WF121_RX_TASK__MAX_NUM_CALLBACKS];

        // The number of indices in `m_callbacks` that contain callbacks.
        size_t m_numCallbacksRegistered;

        // The message parsing state machine that informs this task how to
        // behave in order to properly receive messages from the WF121.
        Wf121Parser::Mpsm m_mpsm;

        // Whether or not the task should keep running. The main loop in the
        // task thread is controlled by this.
        bool m_keepRunning;

        // Whether or not the task has been started. Only used to prevent
        // calling start(...) after it has already been called before.
        bool m_isRunning;

        // The buffer to used for holding the message payload received from
        // WF121 (bounded by the BGAPI packet size since the Radio-Herc
        // DIRECT_MESSAGES cap out at 255B, including header).
        uint8_t m_dataBuffer[WF121_BGAPI_MAX_PACKET_SIZE] __attribute__((aligned(8)));

        /**
         * The function that implements the task thread.
         *
         * @param arg The argument to the thread, which in this case will be
         *  the `this` pointer for this object.
         */
        static void rxHandlerTaskFunction(void *arg);

        /**
         * Simply iterates through `m_callbacks` and calls all callbacks with
         * the given parameters.
         *
         * @param msg The generic wrapper for the received message.
         */
        void callAllCallbacks(Wf121Parser::GenericMessage &msg);
    };

} // end namespace CubeRover

#endif // #ifndef CUBEROVER_WF121_WF121_RX_TASK_HPP_
