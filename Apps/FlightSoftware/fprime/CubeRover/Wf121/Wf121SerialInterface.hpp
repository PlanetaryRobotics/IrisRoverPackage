#ifndef CUBEROVER_WF121_WF121_SERIAL_HPP_
#define CUBEROVER_WF121_WF121_SERIAL_HPP_
/**
 * Code governing the serial interface with the Bluegiga WF121 module over SCI
 * (UART) serial.
 */
#include "sci.h"
#include "gio.h"
#include <Os/Mutex.hpp>
#include <Fw/Time/Time.hpp>

#define WF121_SCI_REG sciREG // if you change this, make sure to change which ISR is being used too
#define WF121_SCI_BAUD 115200
// Whether or not to use control flow:
#define WF121_USE_CTS_RTS 1
// DMA TX Interfaces (uses SCI):
#define WF121_TX_DMA_CH SCI_TX_DMA_CH
#define WF121_TX_DMA_ISR SCI_TX_DMA_ISR

namespace Wf121::Wf121Serial
{
    // Initialize comms:
    void init(void);

    // Set the RTS GPIO pin to the given state:
    void setRTS(bool state);
    // Get the CTS GPIO pin state:
    bool getCTS(void);
    // Set control flow to indicate that we're ready to receive data:
    void ReadyForData(void);
    // Set control flow to indicate that we're not ready to receive data:
    void NotReadyForData(void);
    // Check WF121's control flow status to see if we're allowed to send data:
    bool CanSendData(void);

    // Simple container struct with mutex containing info about DMA write status:
    // Also includes a smart timeout calculator.
    struct DmaWriteStatus
    {
    public:
        // Mutex that should be locked anytime the status is read or modified:
        ::Os::Mutex mutex;
        // Whether DMA write is currently busy:
        bool writeBusy;
        // Time used for blocking timeout, in ms since Hercules boot:
        uint32_t blockingStartTimeMs;

        DmaWriteStatus(bool smartTimeout) : writeBusy(false),
                                            blockingStartTimeMs(0),
                                            useSmartTimeouts(smartTimeouts),
                                            smartTimeoutMs(DmaWriteStatus::DMA_BLOCKING_TIMEOUT_MS)
        {
            /* nothing else to do */
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
            uint32_t now = static_cast<uint32_t>(getTime().get_time_ms());
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
            uint32_t smartTimeoutMs = 15000UL \         // coefficient (see above)
                                      * dataSize \      // bytes
                                      / WF121_SCI_BAUD; // baud/sec
            this->mutex.lock();
            this->smartTimeoutMs = smartTimeoutMs;
            this->mutex.unLock();
        }

        // Return how much time should be remaining in the block for the
        // current write (if blocking).
        uint32_t blockingTimeRemaining()
        {
            // Do all the computation to get the time first...
            uint32_t now = static_cast<uint32_t>(getTime().get_time_ms());
            uint32_t startTime;
            uint32_t timeoutMs;
            uint32_t timeRemaining;
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

            timeRemaining = (startTime + timeoutMs) - now;
            return timeRemaining > 0 ? timeRemaining : 0;
        }

        // Return whether the allowable time to block has expired (obtains
        // current time, grabs the mutex, grabs the time info, releases the
        // mutex, returns if timed out).
        bool blockingTimedOut()
        {
            // Do all the computation to get the time first...
            uint32_t now = static_cast<uint32_t>(getTime().get_time_ms());
            uint32_t startTime;
            // ... and only lock the mutex when absolutely needed:
            if (useSmartTimeouts)
            {
                uint32_t smartTimeoutMs;
                this->mutex.lock();
                startTime = this->blockingStartTimeMs;
                smartTimeoutMs = this->smartTimeoutMs;
                this->mutex.unLock();
                return (now - startTime) > smartTimeoutMs;
            }
            else
            {
                this->mutex.lock();
                startTime = this->blockingStartTimeMs;
                this->mutex.unLock();
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

    // Polls DMA send finish to see if we've finished our DMA sending.
    // Returns whether the finish was caused by completion (true) or a timeout
    // (false).
    bool pollDMASendFinished();

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
    bool dmaSend(void *buffer, int size, bool blocking = true);

    // Perform a normal SCI send. Note: this is a non-DMA blocking send
    // operation.
    // Don't use this unless DMA has magically broken and it's urgent.
    // Returns: true (always), to make it drop-in compatible with `dmaSend`.
    bool nonDmaSend(void *buffer, int size);
}

#endif /* CUBEROVER_WF121_WF121_SERIAL_HPP_ */