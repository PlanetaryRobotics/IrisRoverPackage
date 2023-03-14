/***
 * This Task handles writing the actual process of capturing and downlinking
 * images.
 * This is a Task because each write requires talking with the FPGA and Flash,
 * and blocking until it gets a response.
 */

#ifndef CUBEROVER_CAMERA_MANAGER_TASK_HPP_
#define CUBEROVER_CAMERA_MANAGER_TASK_HPP_

#include <Os/Task.hpp>
#include <Os/Mutex.hpp>

#include <cassert>
#include <cstdio>

#include "FreeRTOS.h"
#include "os_queue.h"

#include "S25fl512l.hpp"

// --- SYSTEM IMAGE PARAMS ---
#define IMAGE_WIDTH        2592
#define IMAGE_HEIGHT       1944
//
//// RAD TODO - isn't downsampling a user-defined parameter?
//#define DOWNSAMPLING        1
//#define DOWNSAMPLED_IMG_WIDTH   (IMAGE_WIDTH / DOWNSAMPLING)
//#define DOWNSAMPLE_IMG_HEIGHT   (IMAGE_HEIGHT / DOWNSAMPLING)


namespace Camera
{

    // How many milliseconds to wait until checking again to see if a downlink
    // is complete:
    // (NOTE: line downlinks take a while so 50ms is a totally reasonable
    // number):
    static const uint8_t CAMERA_TASK_DOWNLINK_POLLING_TIME_MS = 50;

    enum class CameraSelection
    {
        // Feel free to add, remove, etc.
        CAM_0 = 0x00,
        CAM_1 = 0x01,
        BOTH = 0x02,
        // Don't take a new image, just downlink the last one:
        LAST_IMG = 0x03,
        // Downlink a dummy grid:
        DUMMY = 0xFF
    };

    struct ImageRequest
    {
        uint16_t callbackId;
        CameraSelection cam;
        uint16_t skipXPairs;  // number of PAIRS of columns to skip
        uint16_t skipYPairs;  // number of PAIRS of rows to skip
        uint16_t startXPairs; // number of PAIRS of columns to ignore before starting
        uint16_t startYPairs; // number of PAIRS of rows to ignore before starting
        uint16_t stopXPairs;  // number of PAIRS of columns to ignore at the end
        uint16_t stopYPairs;  // number of PAIRS of rows to ignore at the end
    };

    // The actual data that goes into a downlink request:
    // Keep this interface as is unless modification is necessary.
    struct DownlinkRequest
    {
        bool done; // This gets set to 1 when done:
        uint16_t callbackId;
        uint32_t captureTimeMs;          // time this image was captured
        uint16_t downlinkLineNumber;     // What line is being downlinked (out of the total number being downlinked)
        uint16_t totalDownlinkLineCount; // How many lines are BEING DOWNLINKED in this file
        uint16_t numBytesToDownlink;     // How many bytes should be downlinked in this line, starting from the ptr
    };

    // States used by the State Machine inside `cameraTaskExecutor`.
    enum class CameraState
    {
        // Just an example.
        // Add more/remove states as needed.
        // `SETUP` and `IDLE` are the only ones that need to stick around.

        // Where the state machine waits when it's not doing anything else:
        // (this is also where it starts)
        IDLE = 0x00,

        // Starts the process of downlinking an image:
        SETUP = 0x10,

        // Example. Reads a line THEN ASKS FOR IT TO BE DOWNLINKED
        // (we don't need the separate wait for downlink ready, flag, wait for
        // downlink complete. Instead just flag for downlink and the outside
        // will downlink once ready and flag that it's complete).
        DOWNLINK_LINE = 0x20,

        // Where the state machine goes when there's a problem:
        ERROR = 0xFF
    };

    struct ProtectedCameraState
    {
    public:
        ProtectedCameraState();

        // Mutex protected getters and setters:
        void setState(CameraState targetState);
        CameraState getState();

    private:
        CameraState state;
        ::Os::Mutex mutex;
    };

    struct ProtectedImageRequest
    {
    public:
        ProtectedImageRequest();

        // Mutex protected getters and setters:
        void setData(ImageRequest newImgReq);
        ImageRequest getData();

    private:
        ImageRequest imgReq;
        ::Os::Mutex mutex;
    };

    // Simple container for a pointer to a buffer and the mutex that protects it.
    struct ProtectedBufferPointer
    {
        // POINTER to the data we're protecting:
        uint8_t *pData;
        ::Os::Mutex mutex;
        ProtectedBuffer();
    };

    struct ProtectedDownlinkRequest
    {
    public:
        ProtectedDownlinkRequest();

        // Mutex protected getters and setters:
        void setData(DownlinkRequest newDownlinkRequest);
        DownlinkRequest getData();
        // Suspends this Task until the downlink is flagged as complete:
        void awaitCompletion();

    private:
        DownlinkRequest downlinkRequest;
        ::Os::Mutex mutex;
    };

    /**
     * @brief Defines the contract (i.e. interface) that must be implemented
     *      (via subclassing) by classes that want to handle downlink
     *      operations.
     * CameraComponent will subclass this.
     */
    class CameraDownlinkManager
    {
    public:
        // Data to be downlinked:
        ProtectedBufferPointer protectedDownlinkBuffer;
        // Data about the downlink being requested.
        // The `done` field is set to `0` when a new downlink is desired.
        // The `done` field is set to `1` when a downlink is complete.
        ProtectedDownlinkRequest protectedDownlinkRequest;
    };

    class CameraTask : public ::Os::Task
    {
    public:
        /**
         * @brief Constructor. Does not start the task.
         */
        explicit CameraTask(CameraDownlinkManager *pDownlinkManager);

        /**
         * @brief Destructor. Stops the task if it is currently running.
         */
        ~CameraTask();

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
        // Functor to handle the actual write operations in each loop of the Task:
        CameraDownlinkManager *m_pDownlinkManager;

        // Data about the image(s) requested:
        ProtectedImageRequest m_imageRequest;

        // State of the state machine.
        // EXTERNAL TASK WILL SET THIS TO `SETUP` after updating
        // `m_imageRequest` when it wants a new image.
        ProtectedCameraState m_cameraState;

        // Buffer for image data:
        uint8_t m_imageLineBuffer[IMAGE_WIDTH];
        // A second working buffer for image data:
        uint8_t m_imageLineDownsampleBuffer[IMAGE_WIDTH];

        // Whether or not the task should keep running. The main loop in the
        // task thread is controlled by this.
        bool m_keepRunning;

        // Whether or not the task has been started. Only used to prevent
        // calling start(...) after it has already been called before.
        bool m_isRunning;

        /**
         * The function that implements the task thread.
         *
         * @param arg The argument to the thread, which in this case will be
         *  the `this` pointer for this object.
         */
        static void cameraTaskExecutor(void *arg);
    };

} // end namespace CubeRover

#endif // #ifndef CUBEROVER_CAMERA_MANAGER_TASK_HPP_
