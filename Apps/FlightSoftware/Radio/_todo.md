
- Add Watchdog communication via `RADIO_KICK` and `WD_INT` lines.

- Do a quick sanity check and make sure pulsing `MCLR` doesn't clear the `BGScript` from memory, when it's been flashed using either the PICKit ICSP or the UART-DFU.
    - Nowhere does it say this happens, but `MCLR` looks suspiciously like it stands for `Memory Clear` and this would be a dumb way to die on the Moon.

- Finish `state_driver.puml` (documenting `state_driver.bgs`).

- Add the ability to pass in new WiFi credentials from Earth (for volatile use).

- Emit the message that says what endpoint the UDP client is on on a timer (so there's no change Hercules misses it).
        - Include anything Hercules would have to regularly poll for automatically.
            - (could this risk slowly filling up and eventually overwhelming the UART buffer? check how big it is and measure the clear rate).
        - Docs (`UG209`) say you can't have more than one **recurring** timer but can you have a recurring timer and the single-shot timers already in use? Test this.