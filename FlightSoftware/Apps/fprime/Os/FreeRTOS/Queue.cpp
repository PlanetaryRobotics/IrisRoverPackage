#include <Os/Queue.hpp>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <HAL/include/FreeRTOS.h>
#include <HAL/include/os_queue.h>

#include <iostream>
using namespace std;

#ifdef USE_TRACE_FACILITY
#include "trcKernelPort.h"
#endif

namespace Os {

    Queue::Queue() :
        m_handle(NULL) {
    }

    Queue::QueueStatus Queue::create(const Fw::StringBase &name, NATIVE_INT_TYPE depth, NATIVE_INT_TYPE msgSize){
        m_name = "/QP_";
        m_name += name;

        QueueHandle_t queueHandle;
        queueHandle = xQueueCreate(depth, msgSize + sizeof(msgSize));

        #ifdef USE_TRACE_FACILITY
            vTraceSetQueueName(queueHandle, m_name.toChar());
        #endif

        if(NULL == queueHandle) {
          return QUEUE_UNINITIALIZED;
        }

        m_handle = (POINTER_CAST) queueHandle;
        m_depth = depth;
        m_msgSize = msgSize;

        Queue::s_numQueues++;

        m_msgBuffer = (U8 *) pvPortMalloc(msgSize + sizeof(msgSize));

        return QUEUE_OK;
    }

    Queue::~Queue(){
        if(m_handle){
            vQueueDelete((QueueHandle_t) m_handle);
        }

        if(m_msgBuffer){
            vPortFree(m_msgBuffer);
        }
    }


    Queue::QueueStatus Queue::send(const U8* buffer, NATIVE_INT_TYPE size, NATIVE_INT_TYPE priority, QueueBlocking block){
        QueueHandle_t queueHandle = (QueueHandle_t) m_handle;

        if (NULL == queueHandle){
          return QUEUE_UNINITIALIZED;
        }
        if (NULL == buffer){
          return QUEUE_EMPTY_BUFFER;
        }

        m_msgBuffer[0] = size;
        memcpy(m_msgBuffer + sizeof(size), buffer, size);

        // if (size != getMsgSize()) return QUEUE_SIZE_MISMATCH
        if (block == QUEUE_NONBLOCKING){
            if (xQueueSendToBack(queueHandle, (void*) m_msgBuffer, (TickType_t)0) == errQUEUE_FULL){
                // printf("QUEUE IS FULL\n");
                return QUEUE_FULL;
            }
        }
        else{
            if (xQueueSendToBack(queueHandle, (void*) m_msgBuffer, (TickType_t) portMAX_DELAY ) != pdPASS){
                // printf("QUEUE IS ERROR\n");
                return QUEUE_UNKNOWN_ERROR;
            }
        }

        return QUEUE_OK;
    }


    Queue::QueueStatus Queue::receive(U8* buffer, NATIVE_INT_TYPE capacity, NATIVE_INT_TYPE &actualSize, NATIVE_INT_TYPE &priority, QueueBlocking block)
    {
        QueueHandle_t queueHandle = (QueueHandle_t) this->m_handle;

        if (NULL == queueHandle)
        {
            return QUEUE_UNINITIALIZED;
        }

        if (NULL == buffer)
        {
            return QUEUE_EMPTY_BUFFER;
        }

        // if (size != getMsgSize()) return QUEUE_SIZE_MISMATCH;

        if (block == QUEUE_NONBLOCKING)
        {
            if (xQueueReceive(queueHandle, (void*) m_msgBuffer, (TickType_t) 0) == errQUEUE_EMPTY)
            {
                return QUEUE_NO_MORE_MSGS;
            }
        }
        else
        {
            if (xQueueReceive(queueHandle, (void*) m_msgBuffer, (TickType_t) portMAX_DELAY) == errQUEUE_EMPTY)
            {
                return QUEUE_NO_MORE_MSGS;
            }
        }


        actualSize = m_msgBuffer[0];
        memcpy(buffer, m_msgBuffer + sizeof(actualSize), actualSize);


        return QUEUE_OK;
    }

    /**************************************************************
     *   WE NEED TO EDIT QUEUE.HPP FOR THE STUFF BELOW TO WORK    *
     **************************************************************/

    NATIVE_INT_TYPE Queue::getNumMsgs(void) const
    {
        return uxQueueMessagesWaiting((QueueHandle_t) this->m_handle);
    }

    NATIVE_INT_TYPE Queue::getMaxMsgs(void) const
    {
        // FW_ASSERT(0);
        return 0;
    }

    NATIVE_INT_TYPE Queue::getQueueSize(void) const
    {
        return this->m_depth;
    }

    NATIVE_INT_TYPE Queue::getMsgSize(void) const
    {
        return this->m_msgSize;
    }

}
