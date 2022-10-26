#ifndef CUBEROVER_WF121_WF121_SERIAL_HPP_
#define CUBEROVER_WF121_WF121_SERIAL_HPP_
/**
 * Code governing the serial interface with the Bluegiga WF121 module over SCI
 * (UART) serial.
 */
#include "sci.h"
#include "sys_common.h"
#include "system.h"
#include "sys_dma.h"
#include <App/DMA.h>
#include "gio.h"

#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_semphr.h"
#include "os_task.h"
#include <Os/Mutex.hpp>
#include <CubeRover/Wf121/Timestamp.hpp>

#define WF121_SCI_REG sciREG          // if you change this, make sure to change which ISR is being used too
#define WF121_SCI_BAUD_DEFAULT 115200 // default value for the WF121 SCI BAUD RATE (when code is loaded. actual value used is the persistent `Wf121::Wf121Serial::persistent_wf121_sci_baud`)
// Whether or not to use control flow:
#define WF121_USE_CTS_RTS 1
// DMA TX Interfaces (uses SCI):
#define WF121_TX_DMA_CH SCI_TX_DMA_CH
#define WF121_TX_DMA_ISR SCI_TX_DMA_ISR

// How much the processor should wait before checking back in on dmaSend completion while polling for it.
// NOTE: Polling only happens when DMA or the writeDone Semaphore aren't set up yet (which they always should be) - this is just a precaution to handle an edge case.
// Since this is a high priority task, it's not a good idea for this to be 0 (though it *can* be zero) in order to prevent Task starvation.
// NOTE: FreeRTOS scheduler ticks are every 1ms.
static const TickType_t WF121_DMA_SEND_POLLING_CHECK_INTERVAL = 10 / portTICK_PERIOD_MS; // every 10ms (10 ticks)

// How long the dmaSend task should wait for the Semaphore to be available before timing out
// (since the task won't be doing anything during this time, it's okay for it to be a long time).
// NOTE: This is a multiple of the expected send time for the given data being sent (using the `smartTimeout` based on the baud rate):
static const TickType_t WF121_DMA_SEND_SEMAPHORE_WAIT_MULTIPLE = 3; // Longest allowable wait time is 3x the expected send time.
// Minimum number of FreeRTOS scheduler ticks to wait (minimum for the max. amount of time we'll wait for the Semaphore to tell us the write is done)
static const TickType_t WF121_DMA_SEND_SEMAPHORE_WAIT_MIN_TICKS = 50 / portTICK_PERIOD_MS; // wait no less than 50ms.

namespace Wf121
{
    namespace Wf121Serial // Wf121::Wf121Serial
    {
        // Use an enum of allowed values - not just an int - so memory corruption /
        // fading (if in SRAM) can be detected and corrected by resetting to a default:
        enum Wf121AllowedBaudRate
        {
            WF121_BAUD_10_000_000 = 10000000,
            WF121_BAUD_5_000_000 = 5000000,
            WF121_BAUD_2_500_000 = 2500000,
            WF121_BAUD_2_000_000 = 2000000,
            WF121_BAUD_1_000_000 = 1000000,
            WF121_BAUD_115_200 = 115200,
            WF121_BAUD_57_600 = 57600,
            WF121_BAUD_38_400 = 38400,
            WF121_BAUD_19_200 = 19200,
            WF121_BAUD_14_400 = 14400,
            WF121_BAUD_9_600 = 9600,
            WF121_BAUD_4_800 = 4800
        };
        // Checks if the given baud rate is in the list of supported baud rates:
        bool checkBaudRate(uint32_t baud_int)
        {
            Wf121AllowedBaudRate baud = static_cast<Wf121AllowedBaudRate>(baud_int);
            switch (baud)
            {
            case WF121_BAUD_10_000_000:
            case WF121_BAUD_5_000_000:
            case WF121_BAUD_2_500_000:
            case WF121_BAUD_2_000_000:
            case WF121_BAUD_1_000_000:
            case WF121_BAUD_115_200:
            case WF121_BAUD_57_600:
            case WF121_BAUD_38_400:
            case WF121_BAUD_19_200:
            case WF121_BAUD_14_400:
            case WF121_BAUD_9_600:
            case WF121_BAUD_4_800:
                // All fine.
                return true;
                break;
            default:
                // Value not recognized.
                return false;
            }
        }
        extern Wf121AllowedBaudRate persistent_wf121_sci_baud;
        Os::Mutex persistent_wf121_sci_baud_mutex;

        // Getter that checks if the value is valid and corrects if not:
        // (accounts for possible memory fading if stored in SRAM and a POR occurred)
        uint32_t getWf121SciBaud()
        {
            // Make sure the value is valid. If not, reinit to default baud rate:
            uint32_t retVal;
            persistent_wf121_sci_baud_mutex.lock();
            if (!checkBaudRate(persistent_wf121_sci_baud))
            {
                persistent_wf121_sci_baud = static_cast<Wf121AllowedBaudRate>(WF121_SCI_BAUD_DEFAULT);
            }
            retVal = persistent_wf121_sci_baud;
            persistent_wf121_sci_baud_mutex.unLock();

            return retVal;
        }

        // Initialize comms:
        void init(void);

        // De-initialize comms (so an external device can take over):
        void deinit(void);

        // Re-initialize comms after they've been deinit'd during program
        // execution:
        void reinit(void);

        // Changes the persistent_wf121_sci_baud to the given uint32 and resets
        // the UART so that new baud applies. If there are issues after calling
        // this, reset Hercules and the new rate should be applied.
        void changeUartBaud(uint32_t newBaud);

        // Set the RTS GPIO pin to the given state:
        inline void setRTS(bool state)
        {
#if WF121_USE_CTS_RTS
            // NOTE: our RTS pin is connected to the WF121's CTS pin, which is on PB3.
            gioSetBit(gioPORTB, 3, state);
#else
            return;       // if no control flow, then we just throw this out
#endif //#if WF121_USE_CTS_RTS
        }

        // Get the CTS GPIO pin state:
        inline bool getCTS(void)
        {
#if WF121_USE_CTS_RTS
            // NOTE: our CTS pin is connected to the WF121's RTS pin, which is on PB2.
            return gioGetBit(gioPORTB, 2);
#else
            return false; // if no control flow, then we just assume we're always good to send data (active low)
#endif //#if WF121_USE_CTS_RTS
        }

        // Set control flow to indicate that we're ready to receive data:
        inline void ReadyForData(void) { setRTS(false); } // active low
        // Set control flow to indicate that we're not ready to receive data:
        inline void NotReadyForData(void) { setRTS(true); } // active low
        // Check WF121's control flow status to see if we're allowed to send data:
        inline bool CanSendData(void) { return getCTS() == false; } // active low

        // Simple container struct with mutex containing info about DMA write status:
        // Also includes a smart timeout calculator.
        static StaticSemaphore_t xSemaphoreBuffer_dmaWriteStatus_writeDone;
        struct DmaWriteStatus
        {
        public:
            // Mutex that should be locked anytime the status is read or modified:
            ::Os::Mutex mutex;
            // Whether DMA write is currently busy:
            volatile bool writeBusy; // written to directly in an ISR
            // Time used for blocking timeout, in ms since Hercules boot:
            uint32_t blockingStartTimeMs;
            // Semaphore to give whenever writing operations complete (note: this
            // only works once the DMA ISR is active):
            SemaphoreHandle_t xSemaphore_writeDone;

            // Extra layer of safety to make sure only one process can use the
            // core SCI resource (`sciDMASend` for this interface) at a time.
            // Given that `Wf121UdpTxTask` should own `dmaSend` responsibility in
            // the first place, this *shouldn't* be an issue but extra protection
            // doesn't hurt and this makes it a guarantee.
            // Additional benefit is it can prevent anyone from using `sciSend` if
            // someone's actively using `sciDMASend` (which shouldn't happen anyway
            // - but this makes sure it doesn't)
            ::Os::Mutex sciResourceProtectionMutex;

            DmaWriteStatus(bool smartTimeout) : writeBusy(false),
                                                blockingStartTimeMs(0),
                                                useSmartTimeouts(smartTimeout),
                                                smartTimeoutMs(DmaWriteStatus::DMA_BLOCKING_TIMEOUT_MS),
                                                xSemaphore_writeDone(NULL) // null until init'd
            {
                /* nothing else to do */
            }

            // Initialize semaphore(s):
            void init()
            {
/* Create a binary semaphore without using any dynamic memory
allocation.  The semaphore's data structures will be saved into
the xSemaphoreBuffer variable. */

// The following is just `xSemaphoreCreateBinaryStatic` but with
// the FreeRTOSmacro expanded so we can get the compiler to not
// whine about `NULL` (which FreeRTOS has there without the type
// cast):
#if (configSUPPORT_STATIC_ALLOCATION != 1)
#error configSUPPORT_STATIC_ALLOCATION=1 Required!
#endif
                if (this->xSemaphore_writeDone == NULL)
                {
                    // Only create the semaphore if it doesn't already exist
                    // (in case this is being called a second time from a reinit).
                    this->xSemaphore_writeDone = xQueueGenericCreateStatic((UBaseType_t)1,
                                                                           semSEMAPHORE_QUEUE_ITEM_LENGTH,
                                                                           (unsigned char *)NULL,
                                                                           &xSemaphoreBuffer_dmaWriteStatus_writeDone,
                                                                           queueQUEUE_TYPE_BINARY_SEMAPHORE);
                }
                // NOTE: Binary Semaphore initializes to 0 ("taken") so anything
                // that wants to "Take" it will have to wait for a "Give" first.

                /* The pxSemaphoreBuffer was not NULL, so it is expected that the
                handle will not be NULL. */
                configASSERT(this->xSemaphore_writeDone);
            }

            // Returns whether the DMA Write is busy (in a mutex-safe way):
            bool isBusy()
            {
                bool busy;
                this->mutex.lock();
                busy = writeBusy;
                this->mutex.unLock();
                return busy;
            }

            // Sets the busy status to the given value (in a mutex-safe way):
            void setBusy(bool x)
            {
                this->mutex.lock();
                writeBusy = x;
                this->mutex.unLock();
            }

            // Obtains mutex lock, sets `blockingStartTimeMs` to the current time
            // in milliseconds since Hercules boot, releases lock.
            void restartBlockingTimer()
            {
                // Do all the computation to get the time first...
                uint32_t now = Timestamp::getTimeMs();
                // ... and only lock the mutex when absolutely needed:
                this->mutex.lock();
                // Set the timer start time:
                this->blockingStartTimeMs = now;
                this->mutex.unLock();
            }

            // Sets an intelligent amount of time to wait before assuming the
            // transmission failed based on the number of bytes being transmitted
            // (a smart timeout). This is calculated as follows:
            //      timeout = 1.5 (safety factor) * 1000 (ms/sec) * dataSize (bytes) * (8+2)baud/byte / BAUD_RATE(baud/sec)
            // This gives a constant coefficient of 1.5 * 1000 * 10 =  15000
            // This setting is done in a mutex-safe manner.
            void setSmartTimeout(uint32_t dataSize)
            {
                // calculate before grabbing mutex (to hold mutex for as little
                // time as possible):
                uint32_t smartTimeoutMs = 15000UL * dataSize / persistent_wf121_sci_baud + 1; // coefficient * bytes / (baud/sec)
                this->mutex.lock();
                this->smartTimeoutMs = smartTimeoutMs;
                this->mutex.unLock();
            }

            // Return how much time should be remaining in the block for the
            // current write (if blocking).
            uint32_t blockingTimeRemaining()
            {
                uint32_t startTime;
                uint32_t timeoutMs;
                // ... and only lock the mutex when absolutely needed:
                if (useSmartTimeouts)
                {
                    this->mutex.lock();
                    startTime = this->blockingStartTimeMs;
                    timeoutMs = this->smartTimeoutMs;
                    this->mutex.unLock();
                }
                else
                {
                    this->mutex.lock();
                    startTime = this->blockingStartTimeMs;
                    this->mutex.unLock();
                    timeoutMs = DmaWriteStatus::DMA_BLOCKING_TIMEOUT_MS;
                }
                // Grab `now` **AFTER** the time so there's no chance startTime
                // > now (unless there's been an overflow, which is okay):
                uint32_t now = Timestamp::getTimeMs();
                uint32_t endTime = (startTime + timeoutMs);
                return (now < endTime) ? (endTime - now) : 0;
            }

            // Return whether the allowable time to block has expired (obtains
            // current time, grabs the mutex, grabs the time info, releases the
            // mutex, returns if timed out).
            bool blockingTimedOut()
            {
                uint32_t now;
                uint32_t startTime;
                // only lock the mutex when absolutely needed:
                if (useSmartTimeouts)
                {
                    uint32_t smartTimeoutMs;
                    this->mutex.lock();
                    startTime = this->blockingStartTimeMs;
                    smartTimeoutMs = this->smartTimeoutMs;
                    this->mutex.unLock();
                    // Grab `now` **AFTER** the time so there's no chance startTime
                    // > now (unless there's been an overflow, which is okay):
                    now = Timestamp::getTimeMs();
                    return (now - startTime) > smartTimeoutMs;
                }
                else
                {
                    this->mutex.lock();
                    startTime = this->blockingStartTimeMs;
                    this->mutex.unLock();
                    // Grab `now` **AFTER** the time so there's no chance startTime
                    // > now (unless there's been an overflow, which is okay):
                    now = Timestamp::getTimeMs();
                    return (now - startTime) > DmaWriteStatus::DMA_BLOCKING_TIMEOUT_MS;
                }
            }

        private:
            // Whether or not to use smart-timeouts. See `setSmartTimeout` for details.
            const bool useSmartTimeouts;
            // DMA blocking timeout value is milliseconds (if smart timeouts are used). See `setSmartTimeout` for details.
            uint32_t smartTimeoutMs;
            // Default blocking timeout if `!useSmartTimeouts`. How long, in ms,
            // the DMA can block before the timeout expires.
            static const uint32_t DMA_BLOCKING_TIMEOUT_MS = 500;
        };

        // Signal that we're ready to receive another byte through the SCI RX ISR.
        void signalReadyForInterrupt(void);

        // Whether the DMA is ready to accept a send:
        inline bool dmaSendReady()
        {
            return ((getDMAIntStatus(BTC) >> WF121_TX_DMA_CH) & 0x01U);
        }

        // Blocks (yields) the calling Task until the write operation is complete.
        // Returns whether the finish was caused by completion (true) or a timeout
        // (false).
        bool blockUntilDmaSendFinished();

        // Perform a DMA send function
        /**
         * @brief Send data to Radio using SCI DMA.
         *
         * - If `blocking`, the operation will wait until DMA becomes not busy
         *   (finishes previous write) or timeout.
         * - If `!blocking`, the operation will just quit and return `false` if DMA
         *   is busy.
         *
         * @param buffer The buffer of data to send to the Radio
         * @param size The size of the buffer to send to Radio.
         * @param blocking Whether we need to block other DMA requests.
         * @return Whether the operation performed successfully without error.
         */
        bool dmaSend(void *buffer, unsigned size, bool blocking = true);

        // Perform a normal SCI send. Note: this is a non-DMA blocking send
        // operation.
        // Don't use this unless DMA has magically broken and it's urgent.
        // Returns: true (always), to make it drop-in compatible with `dmaSend`.
        bool nonDmaSend(void *buffer, int size);
    }
} // Wf121::Wf121Serial

#endif /* CUBEROVER_WF121_WF121_SERIAL_HPP_ */
