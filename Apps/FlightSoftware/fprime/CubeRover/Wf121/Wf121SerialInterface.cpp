#include "Wf121SerialInterface.hpp"

#include "sci.h"
#include "sys_common.h"
#include "system.h"

namespace Wf121::Wf121Serial
{
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
}
