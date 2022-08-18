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
    static volatile DmaWriteStatus dmaWriteStatus;

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

    bool pollDMASendFinished()
    {
        bool timedout = false;
        // Make sure we're set up:
        if (!wf121FinishedInitializingSerial)
        {
            // If not set up yet:
            // Wait until ready (or timeout):
            while (!dmaSendReady() && !timedout){
                timedout = dmaWriteStatus.blockingTimedOut();
            };
            // If we finished b/c we're done (not timeout), flag that we're no
            // longer busy:
            if(!timedout){
                dmaWriteStatus.setBusy(false);
                sciDMASendCleanup(WF121_TX_DMA_CH);
            }
        }

        // Now wait for done or timeout:
        timedout = false;
        while(dmaWriteStatus.isBusy() && !timedout){
            timedout = dmaWriteStatus.blockingTimedOut();
        };

        return !timedout;
    }

    // Returns negative on error:
    bool dmaSend(void *buffer, int size, bool blocking)
    {
        // old non-DMA code (for posterity - and in case of need for rapid
        // change-over):
        // sciSend(m_sci, size, static_cast<uint8_t *>(buffer));
        // return true;

        bool timedout = false;
        
        // Make sure device is not busy first:
        if (blocking){
            while (dmaWriteStatus.isBusy() || !dmaSendDone()){
                // loop until not busy
                
            timedout = dmaWriteStatus.blockingTimedOut();
            // ! TODO: (WORKING-HERE) [CWC]: Make this like poll and re-check the wait logic.
                if(dmaWriteStatus.blockingTimedOut()){
                    // If we time out while looping, just eject and say we gave up:
                    return false;
                }
            };
        } else if (dmaWriteStatus.isBusy()){
            return false;
        }

        // Actually send the buffer:
        sciDMASend(WF121_TX_DMA_CH, static_cast<char *>(buffer), size, ACCESS_8_BIT, &wf121DmaWriteBusy);

        // Restart the timer:
        dmaWriteStatus.time

        if (blocking)
            pollDMASendFinished();
        return true;
    }
}
