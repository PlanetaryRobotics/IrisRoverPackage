/** @file Task.cpp
 *  @brief Task implementation in FreeRTOS.
 *
 *  This contains the Task structure implementation in FreeRTOS.
 *
 *  @author Lydia Xing (lydiaxing)
 *  @author Jae Choi (Absolute0K)
 *  @bug No known bugs.
 */

#include <Os/Task.hpp>
#include <Fw/Types/Assert.hpp>

#include <string.h>
#include <time.h>
#include <stdio.h>
#include <HAL/include/FreeRTOS.h>
#include <HAL/include/os_task.h>

namespace Os {

    /**
     * @brief      Constructs the object.
     */
    Task::Task() : m_handle(0), m_identifier(0), m_affinity(-1), m_started(false), m_suspendedOnPurpose(false)
    {
    }


    /**
     * @brief      Start a task
     *
     * @param[in]  name         The name
     * @param[in]  identifier   The identifier
     * @param[in]  priority     The priority
     * @param[in]  stackSize    The stack size
     * @param[in]  routine      The routine
     * @param      arg          The argument
     * @param[in]  cpuAffinity  The cpu affinity
     *
     * @return     { description_of_the_return_value }
     */
    Task::TaskStatus Task::start(const Fw::StringBase &name, NATIVE_INT_TYPE identifier, NATIVE_INT_TYPE priority, NATIVE_INT_TYPE stackSize, taskRoutine routine, void* arg, NATIVE_INT_TYPE cpuAffinity)
    {
        this->m_name = "TP_";
        this->m_name += name;
        this->m_identifier = identifier;

        Task::TaskStatus tStat = TASK_OK;
      	TaskHandle_t tid = (TaskHandle_t) this->m_handle;

      	BaseType_t stat = xTaskCreate(routine, this->m_name.toChar(), stackSize, arg, priority, &tid);

        switch (stat) {
            case pdPASS:             
                // If a registry has been registered, register task
                if (Task::s_taskRegistry) {
                    Task::s_taskRegistry->addTask(this);
                    Task::s_numTasks++;
                }
                break;
            case errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY:
                 tStat = TASK_INVALID_PARAMS;
                break;
            default:
                tStat = TASK_UNKNOWN_ERROR;
                break;
        }

        return tStat;
    }


    /**
     * @brief      Create a delay
     *
     * @param[in]  milliseconds  The milliseconds
     *
     * @return     Task status
     */
    Task::TaskStatus Task::delay(NATIVE_UINT_TYPE milliseconds)
    {
        TickType_t xLastWakeTime = xTaskGetTickCount();

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(milliseconds));

        //TODO: add return TASK_DELAY_ERROR
        return TASK_OK; // for coverage analysis
    }


    /**
     * @brief      Destroys the object.
     */
    Task::~Task()
    {
        // If a registry has been registered, remove task
        if (Task::s_taskRegistry) {
            Task::s_taskRegistry->removeTask(this);
            vTaskDelete((TaskHandle_t) this->m_handle);
        }
    }

    /**
     * @brief      Suspend a task
     *
     * @param[in]  onPurpose  On purpose
     */
    void Task::suspend(bool onPurpose)
    {
        // FW_ASSERT(0);
        vTaskSuspend((TaskHandle_t) this->m_handle);
    }


    /**
     * @brief      Resume task
     */
    void Task::resume(void)
    {
        // FW_ASSERT(0);
        vTaskResume((TaskHandle_t) this->m_handle);
    }


    /**
     * @brief      Determines if suspended.
     *
     * @return     True if suspended, False otherwise.
     */
    bool Task::isSuspended(void)
    {
        // FW_ASSERT(0);
        return (eTaskGetState((TaskHandle_t) this->m_handle) == eSuspended);
    }


    /**
     * @brief      Join task thread
     *
     * @param      value_ptr  The value pointer
     *
     * @return     Task status
     */
    Task::TaskStatus Task::join(void **value_ptr) {

        if (!(this->m_handle)) {
            return TASK_JOIN_ERROR;
        }

        //block until the task becomes deleted or invalid
        while(eTaskGetState((TaskHandle_t) this->m_handle) != eDeleted && 
              eTaskGetState((TaskHandle_t) this->m_handle) != eInvalid);

        if(eTaskGetState((TaskHandle_t) this->m_handle) == eDeleted )
            return TASK_OK;

        if(eTaskGetState((TaskHandle_t) this->m_handle) == eInvalid )
            return TASK_JOIN_ERROR;
    
        return TASK_UNKNOWN_ERROR;
    }

}
