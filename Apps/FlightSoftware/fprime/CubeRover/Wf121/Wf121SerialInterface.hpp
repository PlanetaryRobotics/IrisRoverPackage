#ifndef CUBEROVER_WF121_WF121_SERIAL_HPP_
#define CUBEROVER_WF121_WF121_SERIAL_HPP_
/**
 * Code governing the serial interface with the Bluegiga WF121 module over SCI
 * (UART) serial.
 */
#include "sci.h"
#include "gio.h"

#define WF121_SCI_REG sciREG // if you change this, make sure to change which ISR is being used too
#define WF121_SCI_BAUD 115200
// Whether or not to use control flow:
#define WF121_USE_CTS_RTS 1

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

    // Signal that we're ready to receive another byte through the SCI RX ISR.
    void signalReadyForInterrupt(void);
}

#endif /* CUBEROVER_WF121_WF121_SERIAL_HPP_ */