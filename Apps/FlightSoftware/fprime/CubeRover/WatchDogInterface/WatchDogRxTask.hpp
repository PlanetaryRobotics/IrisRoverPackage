#ifndef WatchDogInterface_WatchDogRxTask_HPP
#define WatchDogInterface_WatchDogRxTask_HPP

#include <Os/Task.hpp>

#include <Include/FswPacket.hpp>
#include <CubeRover/WatchDogInterface/WatchDogMpsm.hpp>

#define WATCHDOG_RX_TASK__MAX_NUM_CALLBACKS 1

namespace CubeRover
{   
    // Defines the contract that classes that must be implemented (subclassed) by classes that can 
    // register for callbacks from the WatchDogRxTask.This callback will be called with each message
    // that is received. by the WatchDogRxTask to which the object implementing this function is
    // registered.
    class WatchDogRxCallbackProcessor
    {
        public:
            virtual void rxCallback(WatchDogMpsm::Message& msg, bool goodParity) = 0;
    };

    class WatchDogRxTask : public ::Os::Task
    {
        public:
            explicit WatchDogRxTask();

            ~WatchDogRxTask();
        
            ::Os::Task::TaskStatus start(NATIVE_INT_TYPE priority,
                                         NATIVE_INT_TYPE stackSize,
                                         NATIVE_INT_TYPE cpuAffinity = -1); //!< start the task

            bool registerCallback(WatchDogRxCallbackProcessor* callback);

        private:
            WatchDogRxCallbackProcessor* m_callbacks[WATCHDOG_RX_TASK__MAX_NUM_CALLBACKS];
            size_t m_numCallbacksRegistered;

            WatchDogMpsm m_mpsm;
            bool m_keepRunning;
            bool m_isRunning;

            uint8_t m_dataBuffer[WATCHDOG_MAX_PAYLOAD] __attribute__((aligned(8)));

            static void rxHandlerTaskFunction(void* arg);

            void callAllCallbacks(WatchDogMpsm::Message& msg, bool goodParity);
    };


} // end namespace CubeRover

#endif // #ifndef WatchDogInterface_WatchDogRxTask_HPP
