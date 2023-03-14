#include <HAL/include/FreeRTOS.h>
#include <HAL/include/os_task.h>
#include <HAL/include/os_portmacro.h>
#include <HAL/include/sys_dma.h>

#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/EightyCharString.hpp>

#include <App/DMA.h>

#include <CubeRover/Camera/CameraTask.hpp>

#include "S25fl512l.hpp"
#include "gio.h"
#include "spi.h"
#include "lin.h"

// Handle to active task (this):
static TaskHandle_t xActiveTask = nullptr; // not init'd yet

namespace Camera
{
    // Simple container for a pointer to a buffer and the mutex that protects it.

    ProtectedDownlinkRequest::ProtectedDownlinkRequest() : downlinkRequest(DownlinkRequest())
    {
        // Nothing else to do.
    }
    // Mutex protected getters and setters:
    void ProtectedDownlinkRequest::setData(DownlinkRequest newDownlinkRequest)
    {
        this->mutex.lock();
        this->downlinkRequest = newDownlinkRequest;
        this->mutex.unLock();
    }
    DownlinkRequest ProtectedDownlinkRequest::getData()
    {
        DownlinkRequest fetchedValue;
        this->mutex.lock();
        fetchedValue = this->downlinkRequest;
        this->mutex.unLock();
        return fetchedValue;
    }
    // Suspends this Task until the downlink is flagged as complete:
    void ProtectedDownlinkRequest::awaitCompletion()
    {
        while (!(this->getData().done))
        {
            // Bytes still need to be downlinked
            // (i.e. they haven't been downlinked yet)
            // Check back on this task in a bit:
            vTaskDelay(CAMERA_TASK_DOWNLINK_POLLING_TIME_MS / portTICK_PERIOD_MS);
        }
    }

    ProtectedCameraState::ProtectedCameraState() : state(CameraState::IDLE)
    {
        // nothing else to do
    }
    // Mutex protected getters and setters:
    void ProtectedCameraState::setState(CameraState targetState)
    {
        this->mutex.lock();
        this->state = targetState;
        this->mutex.unLock();
    }
    CameraState ProtectedCameraState::getState()
    {
        CameraState fetchedState;
        this->mutex.lock();
        fetchedState = this->state;
        this->mutex.unLock();
        return fetchedState;
    }

    ProtectedImageRequest::ProtectedImageRequest() : imgReq(ImageRequest())
    {
        // nothing else to do
    }
    // Mutex protected getters and setters:
    void ProtectedImageRequest::setData(ImageRequest newImgReq)
    {
        this->mutex.lock();
        this->imgReq = newImgReq;
        this->mutex.unLock();
    }
    ImageRequest ProtectedImageRequest::getData()
    {
        ImageRequest fetchedImgReq;
        this->mutex.lock();
        fetchedImgReq = this->imgReq;
        this->mutex.unLock();
        return fetchedImgReq;
    }

    CameraTask::CameraTask(CameraDownlinkManager *pDownlinkManager)
        : m_pDownlinkManager(pDownlinkManager),
          m_imageRequest(),
          m_cameraState(),
          m_keepRunning(true),
          m_isRunning(false)
    {
        /* nothing special to do here */
    }

    // This probably will never be called, but it should up to properly work anyway
    CameraTask::~CameraTask()
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

    ::Os::Task::TaskStatus CameraTask::startTask(NATIVE_INT_TYPE priority,
                                                 NATIVE_INT_TYPE stackSize,
                                                 NATIVE_INT_TYPE cpuAffinity)
    {
        if (m_isRunning)
        {
            return Os::Task::TASK_UNKNOWN_ERROR;
        }

        m_keepRunning = true;
        Fw::EightyCharString task_name("CameraCoreTask"); // max len is 16

        TaskHandle_t tid;
        BaseType_t stat = xTaskCreate(CameraTask::cameraTaskExecutor,
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

    void CameraTask::cameraTaskExecutor(void *arg)
    {
        CameraTask *task = static_cast<CameraTask *>(arg);

        // Block task for 50ms to wait for serial to come up and be ready
        // (NOTE: this is already handled elsewhere and this task isn't started
        // until I/O is ready **but** this gives us some safety to wait for
        // hardware to get ready):
        vTaskDelay(50 / portTICK_PERIOD_MS);

        while (!task->m_keepRunning)
        {
            // Wait until keepRunning has been set true
        };

        // Accessors for convenience:
        CameraDownlinkManager *pDownlinkManager = task->m_pDownlinkManager;
        ProtectedBufferPointer *pDownlinkBuffer = &(pDownlinkManager->protectedDownlinkBuffer);
        ProtectedDownlinkRequest *pDownlinkRequest = &(pDownlinkManager->protectedDownlinkRequest);
        ProtectedCameraState *pCameraState = &(task->m_cameraState);
        ProtectedImageRequest *pImageRequest = &(task->m_imageRequest);

        // !!! NOTES:
        // Set: `pDownlinkBuffer->pData = [YOUR STATIC BUFFER HEAD]` here once.
        // Use `pDownlinkBuffer->mutex.lock()` and `pDownlinkBuffer->mutex.unLock()` before editing the buffer's contents
        //
        // Use `pDownlinkLength->setValue(numBytesToDownlink)` to indicate how many bytes you want to downlink.
        // NOTE: ^ THIS ALSO IS THE FLAG INDICATING THAT DATA NEEDS TO BE DOWNLINKED:

        //    m_fpgaFlash.setupDevice();  // need this? (pulled it out of Camera.hpp when converting it to task manager)

        while (task->m_keepRunning)
        {
            // !!! STATE MACHINE RUNNER IN HERE !!!

            // This while loop runner is just an example. The key thing here is
            // the way it's getting and setting state.
            // EXTERNAL TASK will
            switch (pCameraState->getState())
            {
            case CameraState::IDLE:
                // Do nothing.
                vTaskDelay(1);
                break;

            case CameraState::SETUP:
                // Do some setup.
                // Data specifying the image request can be fetched like this:
                ImageRequest imgReq = pImageRequest->getData();

                // Move on to the `DOWNLINK_LINE` state:
                pCameraState->setState(CameraState::DOWNLINK_LINE);
                break;

            case CameraState::DOWNLINK_LINE:
                // Read the data for the line into `pDownlinkBuffer->pData`
                // Do any downsampling, etc.
                //
                // Trigger a new downlink of a line by setting the value of
                // `pDownlinkRequest` to something with a non-zero `done`
                // field:
                pDownlinkRequest->setData({
                    .done = false,
                    .callbackId = 0,             // fill with correct value
                    .captureTimeMs = 0x1234,     // fill with correct value
                    .downlinkLineNumber = 0,     // fill with correct value
                    .totalDownlinkLineCount = 1, // fill with correct value
                    .numBytesToDownlink = 1,     // fill with correct value
                });

                // Then wait like this to find out when the downlinking is done:
                // (suspends the task until the value is set to zero):
                pDownlinkRequest->awaitCompletion();

                // Do whatever. Stay in this state, go to idle if done, etc.
                break;

            case CameraState::ERROR:
                // do nothing
                break;

            default:
                // do nothing
            }
        }
    }

} // end namespace
