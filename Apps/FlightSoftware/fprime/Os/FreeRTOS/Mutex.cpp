#include <Os/Mutex.hpp>
#include <Fw/Types/Assert.hpp>
#include <HAL/include/FreeRTOS.h>
#include <HAL/include/os_semphr.h>

namespace Os
{
    /**
     * @brief      Constructs the object.
     */
    Mutex::Mutex(void){
        SemaphoreHandle_t xSemaphore;
        xSemaphore = xSemaphoreCreateMutex();
        if (xSemaphore == NULL){
            // puts("Failed to create mutex.");
            // FW_ASSERT(xSemaphore == 0, (U32) xSemaphore);
        }
        m_handle = (POINTER_CAST) xSemaphore;
    }

    /**
     * @brief      Destroys the object.
     */
    Mutex::~Mutex(void){
        if (this->m_handle != NULL){
          vSemaphoreDelete((SemaphoreHandle_t) m_handle);
        }
        else{
          // puts("MutexHandle hasn't been created.");
        }
    }

    /**
     * @brief      Lock the mutex
     */
    void Mutex::lock(void){
        SemaphoreHandle_t xSemaphore = (SemaphoreHandle_t) m_handle;
        if(xSemaphore){
            if(xSemaphoreTake(xSemaphore, 1000) != pdPASS) {
                // puts("Failed to get access within 1s.");
            }
            else {
                // puts("Lock successfully.");
            }
        }
        else {
            // puts("MutexHandle hasn't been created.");
        }
    }

    /**
     * @brief      Unlock the  mutex
     */
    void Mutex::unLock(void){
        SemaphoreHandle_t xSemaphore = (SemaphoreHandle_t) m_handle;
        if(xSemaphore){
            if (xSemaphoreGive(xSemaphore) != pdPASS) {
                // puts("Failed because the calling task is not the mutex holder.");
            }
            else {
                // puts("Unlock successfully.");
            }
        }
        else {
            // puts("MutexHandle hasn't been created.");
        }
    }

}

