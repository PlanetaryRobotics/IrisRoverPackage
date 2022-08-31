
- Add Watchdog communication via `RADIO_KICK` and `WD_INT` lines.

- Do a quick sanity check and make sure pulsing `MCLR` doesn't clear the `BGScript` from memory, when it's been flashed using either the PICKit ICSP or the UART-DFU.
    - Nowhere does it say this happens, but `MCLR` looks suspiciously like it stands for `Memory Clear` and this would be a dumb way to die on the Moon.

- Finish `state_driver.puml` (documenting `state_driver.bgs`).

- Add the ability to pass in new WiFi credentials from Earth (for volatile use).


# Critical Todos:
+ Restrict to Ch.11 only
- Fix set_transmit_size
- Interlock with Hercules Comms
- Basic GPIO to WD on Connection status (and only transmit GND HBs if not in statis)
- Anomaly check for events like software_exception (so we can have a critical reset)