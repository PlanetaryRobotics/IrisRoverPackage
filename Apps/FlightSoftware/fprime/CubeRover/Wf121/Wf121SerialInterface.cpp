#include "Wf121SerialInterface.hpp"

#include "sci.h"
#include "sys_common.h"
#include "system.h"

namespace Wf121::Wf121Serial
{
    // Whether all serial SCI, DMA, etc. has been initialzed and can be used
    // (specifically to determine if we can use DMA send or not):
    bool wf121FinishedInitializingSerial = false;

    // Mutex-protected information about DMA TX:
    static volatile DmaWriteStatus dmaWriteStatus(true); // use smart timeouts

    // Initialize comms:
    void init(void)
    {
        // Before setting up SCI, make *sure* Radio knows not to be sending us
        // stuff right now:
        // (outside code should call `ReadyForData` once it has everything set
        // up to receive data):
        NotReadyForData();

        // Config the SCI:
        sciEnterResetState(WF121_SCI_REG);
        sciSetBaudrate(WF121_SCI_REG, WF121_SCI_BAUD);
        sciExitResetState(WF121_SCI_REG);

        // Set up any semaphores, etc. for the DMA Write Status:
        dmaWriteStatus.init();

        wf121FinishedInitializingSerial = true;
    }

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

    // Signal that we're ready to receive another byte through the SCI RX ISR.
    void signalReadyForInterrupt(void)
    {
        WF121_SCI_REG->SETINT = (uint32)SCI_RX_INT;
    }

    bool blockUntilDmaSendFinished()
    {
        bool timedout = false;
        if (dmaWriteStatus.isBusy())
        {
            // If DMA is not initialized yet or the Semaphore isn't initialized yet,
            // we can't rely on the DMA interrupt to fire and get us out, so we
            // have to explicitly poll instead:
            if (!wf121FinishedInitializingSerial || dmaWriteStatus.xSemaphore_writeDone == NULL)
            {
                if (!dmaSendReady())
                {
                    // If it's not done writing yet, block the task (allowing others to
                    // run) for the half the estimated time remaining in the write
                    // operation (it's unlikely we'll be done before then anyway):
                    vTaskDelay(dmaWriteStatus.blockingTimeRemaining() / 2 / portTICK_PERIOD_MS);
                    // Then wait until actually ready (or timeout):
                    while (!dmaSendReady() && !timedout)
                    {
                        // loop until ready
                        vTaskDelay(WF121_DMA_SEND_POLLING_CHECK_INTERVAL); // Check back in every 10ms (don't hog the processor)
                        timedout = dmaWriteStatus.blockingTimedOut();
                    };
                }
                // If it actually finished (didn't just time out), do what the
                // interrupt would have done (upon actually finishing):
                if (!timedout)
                {
                    sciDMASendCleanup(WF121_TX_DMA_CH); // clean up...
                    dmaWriteStatus.setBusy(false);      // ...and clear the flag
                }
            }
            else
            {
                // DMA is set up, so we can just count on the `WF121_TX_DMA_ISR` to
                // get us out of here:
                TickType_t maxTimeToWait = WF121_DMA_SEND_SEMAPHORE_WAIT_MULTIPLE * dmaWriteStatus.blockingTimeRemaining();
                maxTimeToWait = (maxTimeToWait > WF121_DMA_SEND_SEMAPHORE_WAIT_MIN_TICKS) ? maxTimeToWait : WF121_DMA_SEND_SEMAPHORE_WAIT_MIN_TICKS;
                if (xSemaphoreTake(dmaWriteStatus.xSemaphore_writeDone, maxTimeToWait) == pdTRUE)
                {
                    // Semaphore returned. Writing is done and we didn't time out:
                    timedout = false;
                }
                else
                {
                    // We timed out waiting for the flag to clear.
                    timedout = true;
                }
            }
        }

        return !timedout;
    }

    // Returns false on error:
    bool dmaSend(void *buffer, unsigned size, bool blocking)
    {
        if (blocking)
        {
            // Make sure device is not busy first (in case we didn't block on the
            // last write)
            if (!blockUntilDmaSendFinished())
            {
                // we timed out, return false:
                return false;
            }
        }
        else if (dmaWriteStatus.isBusy())
        {
            // we're busy right now and can't write
            return false;
        }

        // We're about to write, so restart the timer:
        dmaWriteStatus.setSmartTimeout(size);  // automatically determine how long this should take
        dmaWriteStatus.restartBlockingTimer(); // set start time to now

        // Actually send the buffer
        // (NOTE: technically, this will violate the `dmaWriteStatus` **but**
        // since it's only one bit and we're not modifying multibyte values
        // like the timer, that's fine. Other things that are dutifully obeying
        // the mutex won't be harmed and will continue to work as expected).
        // NOTE: This will block until `!dmaWriteStatus.writeBusy` but won't
        // clear it (that's done by the `WF121_TX_DMA_ISR`) so we should take
        // outside precautions (like we do above) to make sure
        // `!dmaWriteStatus.writeBusy` before calling.
        dmaWriteStatus.sciResourceProtectionMutex.lock();
        sciDMASend(WF121_TX_DMA_CH, static_cast<char *>(buffer), size, ACCESS_8_BIT, &dmaWriteStatus.writeBusy);
        dmaWriteStatus.sciResourceProtectionMutex.unLock();

        if (blocking)
        {
            return blockUntilDmaSendFinished(); // returns false if times out
        }
        else
        {
            return true;
        }
    }

    // Don't use this unless DMA has magically broken and it's urgent.
    bool nonDmaSend(void *buffer, int size)
    {
        // old non-DMA code (for posterity - and in case of need for rapid
        // change-over):
        dmaWriteStatus.sciResourceProtectionMutex.lock();
        sciSend(WF121_SCI_REG, size, static_cast<uint8_t *>(buffer));
        dmaWriteStatus.sciResourceProtectionMutex.unLock();
        return true;
    }
}

extern "C" void WF121_TX_DMA_ISR(dmaInterrupt_t inttype)
{
    static signed BaseType_t xHigherPriorityTaskWoken;
    // Don't use normal mutex lock/unlock here b/c we're in an ISR, which
    // doesn't obey scheduler ticks (so we need to use special ISR functions...

    // Just write the data (it's atomic):
    dmaWriteStatus.writeBusy = false;
    // Since we didn't need to use the mutex, we don't need to give/unlock it
    // or perform deferred interrupt yielding (new value will just be grabbed
    // in the next call to `dmaWriteStatus`.)

    // Let the blocking dmaSend task know it's allowed to move forward:
    xHigherPriorityTaskWoken = pdFALSE;
    /* Unblock the task by releasing the semaphore. */
    if (dmaWriteStatus.xSemaphore_writeDone != NULL)
    {
        xSemaphoreGiveFromISR(dmaWriteStatus.xSemaphore_writeDone, &xHigherPriorityTaskWoken);

        /* If xHigherPriorityTaskWoken was set to true you
        we should yield.  The actual macro used here is
        port specific. */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}