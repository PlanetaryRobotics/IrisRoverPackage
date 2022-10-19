#include "Wf121SerialInterface.hpp"

namespace Wf121
{
    namespace Wf121Serial // Wf121::Wf121Serial
    {
        // Whether all serial SCI, DMA, etc. has been initialzed and can be used
        // (specifically to determine if we can use DMA send or not):
        bool wf121FinishedInitializingSerial = false;

        // Whether we're not allowed to write to the serial interface. This is
        // only true if the interface has been deinitialized for external
        // debugging / programming.
        uint32_t wf121WritesAllowed = 1;
        enum
        {
            // Since disabling writes causes packets to be silently dropped, we
            // want to be sure this mode can't just be entered by a cosmic-ray
            // bitflip. So, we'll only enter this mode if `wf121WritesAllowed`
            // has 1 specific 32bit value. This mode is only for use on Earth,
            // so it's not really a problem if we can easily exit it.
            WF121_NO_WRITES_ALLOWED = 0xFEEDF00D // I'm hungry
        };

        // Mutex-protected information about DMA TX:
        static DmaWriteStatus dmaWriteStatus(true); // use smart timeouts

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
            // (semaphores are only init'd if this is the first call to avoid a
            // memory leak)
            dmaWriteStatus.init();

            wf121FinishedInitializingSerial = true;
            wf121WritesAllowed = 1;
        }

        // De-initialize comms (so an external device can take over):
        void deinit(void)
        {
            // Flag that we no longer want to write:
            wf121FinishedInitializingSerial = false;
            wf121WritesAllowed = WF121_NO_WRITES_ALLOWED;

            // Tear down the SCI:
            // Enter Communications Reset (so it can be configured - no comms
            // will be allowed):
            sciEnterResetState(WF121_SCI_REG);

            // Manipulate registers (based on sciInit in sci.c):

            // Disable Serial transmit and receive:
            // See https://www.ti.com/lit/ug/spnu514c/spnu514c.pdf?ts=1666185943372 (Table 28-11).
            WF121_SCI_REG->GCR1 &= (uint32)((uint32)0U << 25U)    /* transmit */
                                   & (uint32)((uint32)0U << 24U); /* receive */

            // See https://www.ti.com/lit/ug/spnu514c/spnu514c.pdf?ts=1666185943372 (Table 28-29,30, when FUNC=0, DIR now matters.).
            /** - set SCI pins output direction to input */
            WF121_SCI_REG->PIO1 = (uint32)((uint32)0U << 2U)    /* tx pin */
                                  | (uint32)((uint32)0U << 1U); /* rx pin */

            // See https://www.ti.com/lit/ug/spnu514c/spnu514c.pdf?ts=1666185943372 (Table 28-29).
            // Set SCI to GIO (disable functional mode):
            sciSetFunctional(WF121_SCI_REG,                   // PIO0
                             (uint32)((uint32)0U << 2U)       /* tx pin */
                                 & (uint32)((uint32)0U << 1U) /* rx pin */
            );

            /** - set SCI pins pullup/pulldown enable (disable pull control)*/
            // See https://www.ti.com/lit/ug/spnu514c/spnu514c.pdf?ts=1666185943372 (Table 28-37).
            WF121_SCI_REG->PIO7 = (uint32)((uint32)1U << 2U)    /* tx pin */
                                  | (uint32)((uint32)1U << 1U); /* rx pin */

            // Put GCR0 (SCI hardware) into reset:
            // See https://www.ti.com/lit/ug/spnu514c/spnu514c.pdf?ts=1666185943372 (Table 28-10).
            sciREG->GCR0 = 0U;

            // Tell the Radio that we're ready for it to send data (so it has
            // no problem forwarding data to the external device)
            ReadyForData();
        }

        // Re-initialize comms after they've been deinit'd during program
        // execution:
        void reinit(void)
        {
            // Bring SCI back up:

            // Manipulate registers (based on sciInit in sci.c):

            /** - bring SCI hardware out of reset */
            // See https://www.ti.com/lit/ug/spnu514c/spnu514c.pdf?ts=1666185943372 (Table 28-10).
            sciREG->GCR0 = 0U;
            sciREG->GCR0 = 1U;

            // Set SCI to functional (SCI, not GIO) mode:
            sciSetFunctional(WF121_SCI_REG,
                             (uint32)((uint32)1U << 2U)        /* tx pin */
                                 | (uint32)((uint32)1U << 1U) /* rx pin */
            );
            // See https://www.ti.com/lit/ug/spnu514c/spnu514c.pdf?ts=1666185943372 (Table 28-30, when FUNC=1, DIR doesn't matter.).

            /** - set SCI pins pullup/pulldown enable (enable pull control)*/
            // See https://www.ti.com/lit/ug/spnu514c/spnu514c.pdf?ts=1666185943372 (Table 28-37).
            WF121_SCI_REG->PIO7 = (uint32)((uint32)0U << 2U)    /* tx pin */
                                  | (uint32)((uint32)0U << 1U); /* rx pin */

            // Manipulate registers (based on sciInit in sci.c):
            // Enable Serial transmit and receive:
            // See https://www.ti.com/lit/ug/spnu514c/spnu514c.pdf?ts=1666185943372 (Table 28-11).
            WF121_SCI_REG->GCR1 |= (uint32)((uint32)1U << 25U)    /* enable transmit */
                                   | (uint32)((uint32)1U << 24U); /* enable receive */

            // Exit Communications Reset (configuration over):
            sciExitResetState(WF121_SCI_REG);

            // Perform normal init:
            init();

            // Signal that we're ready for data (this isn't normally done in
            // init because, in Wf121::RadioDriver::init we init serial, start
            // serial Tasks, then signal we're ready. However, this is
            // happening during program execution, once those tasks are already
            // set up, so we can just flag immediately.)
            ReadyForData();
        }

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
                        vTaskDelay(dmaWriteStatus.blockingTimeRemaining() / 2 / portTICK_PERIOD_MS + 1);
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
            if (wf121WritesAllowed == WF121_NO_WRITES_ALLOWED)
            {
                // We're in a special mode and aren't currently allowed to send
                // data.
                // We don't want the system to freak out here, so we'll just
                // silently drop the packets by returning true.
                return true;
            }

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
            if (wf121WritesAllowed == WF121_NO_WRITES_ALLOWED)
            {
                // We're in a special mode and aren't currently allowed to send
                // data.
                // We don't want the system to freak out here, so we'll just
                // silently drop the packets by returning true.
                return true;
            }

            // old non-DMA code (for posterity - and in case of need for rapid
            // change-over):
            dmaWriteStatus.sciResourceProtectionMutex.lock();
            sciSend(WF121_SCI_REG, size, static_cast<uint8_t *>(buffer));
            dmaWriteStatus.sciResourceProtectionMutex.unLock();
            return true;
        }
    }
} // Wf121::Wf121Serial

extern "C" void WF121_TX_DMA_ISR(dmaInterrupt_t inttype)
{
    // Don't use normal mutex lock/unlock here b/c we're in an ISR, which
    // doesn't obey scheduler ticks (so we need to use special ISR functions)...

    // Just write the data (it's atomic):
    Wf121::Wf121Serial::dmaWriteStatus.writeBusy = false;
    // Since we didn't need to use the mutex, we don't need to give/unlock it
    // or perform deferred interrupt yielding (new value will just be grabbed
    // in the next call to `dmaWriteStatus`.)

    // Let the blocking dmaSend task know it's allowed to move forward:
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    /* Unblock the task by releasing the semaphore. */
    if (Wf121::Wf121Serial::dmaWriteStatus.xSemaphore_writeDone != NULL)
    {
        xSemaphoreGiveFromISR(Wf121::Wf121Serial::dmaWriteStatus.xSemaphore_writeDone, &xHigherPriorityTaskWoken);

        /* If xHigherPriorityTaskWoken was set to true you
        we should yield.  The actual macro used here is
        port specific. */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
