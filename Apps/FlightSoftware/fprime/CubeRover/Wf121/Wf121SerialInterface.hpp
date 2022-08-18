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
    class DmaWriteStatus {
         // How long, in ms, the DMA can block before the timeout expires:
        static const uint32_t DMA_BLOCKING_TIMEOUT_MS = 500;
    public:
        DmaWriteStatus() : writeBusy(false), blockingStartTimeMs(0){
            /* nothing else to do */
        }

        // Returns whether the DMA Write is busy (in a mutex-safe way):
        bool isBusy(){
            bool busy;
            this->mutex.lock();
            busy = writeBusy;
            this->mutex.unLock();
            return busy;
        }

        // Sets the busy status to the given value (in a mutex-safe way):
        void setBusy(bool x){
            this->mutex.lock();
            busy = x;
            this->mutex.unLock();
        }

        // Obtains mutex lock, sets `blockingStartTimeMs` to the current time
        // in milliseconds since Hercules boot, releases lock. 
        void startBlockingTimer(){
            // Do all the computation to get the time first...
            uint32_t now = static_cast<uint32_t>(getTime().get_time_ms());
            // ... and only lock the mutex when absolutely needed:
            this->mutex.lock();
            this->blockingStartTimeMs = now;
            this->mutex.unLock();
        }

        // Return whether the blocking timer has expired (obtains current time,
        // grabs the mutex, grabs the start time, releases the mutex, returns
        // if timed out).
        bool blockingTimedOut(){
            // Do all the computation to get the time first...
            uint32_t now = static_cast<uint32_t>(getTime().get_time_ms());
            uint32_t startTime;
            // ... and only lock the mutex when absolutely needed:
            this->mutex.lock();
            startTime = this->blockingStartTimeMs;
            this->mutex.unLock();
            return (now - startTime) > DmaWriteStatus::DMA_BLOCKING_TIMEOUT_MS;
        }

    private:
        // Mutex that should be locked anytime the status is read or modified:
        ::Os::Mutex mutex;
        // Whether DMA write is currently busy:
        bool writeBusy;
        // Time used for blocking timeout, in ms since Hercules boot:
        uint32_t blockingStartTimeMs;
    };

    // Signal that we're ready to receive another byte through the SCI RX ISR.
    void signalReadyForInterrupt(void);

    // Whether the DMA send is ready:
    inline bool dmaSendReady(){
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
    bool dmaSend(void *buffer, int size, bool blocking=true);
}

#endif /* CUBEROVER_WF121_WF121_SERIAL_HPP_ */