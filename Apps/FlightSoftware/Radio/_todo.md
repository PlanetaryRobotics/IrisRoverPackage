- Add catch for wifi disconnect events (trial this)
- Determine if we should restrict channels (probably only useful for earth)
- Add fixed-length state strings (and uart them) - make optional

- Cross reference with all hooks in examples
- Review adding any extra commands used in Herc code but not present here (diff it)
- Document state machine.

- Make anomaly_handling.bgs comprehensive
    - Test anomaly handling.

- Add Watchdog communication via `RADIO_KICK` and `WD_INT` lines.

- Radio heartbeat to Earth (just a couple bytes every couple minutes)?
    - Could be useful to let us know its still alive since there **could** be a situation in which we can command Hercules but not get telemetry from it if it gets overwhelmed and misses the announcement of the UDP `endpoint`.

- Do a quick sanity check and make sure pulsing `MCLR` doesn't clear the `BGScript` from memory, when it's been flashed using either the PICKit ICSP or the UART-DFU.
    - Nowhere does it say this happens, but `MCLR` looks suspiciously like it stands for `Memory Clear` and this would be a dumb way to die on the Moon.


- Finish `state_driver.puml` (documenting `state_driver.bgs`).

- Add the ability to pass in new WiFi credentials from Earth (for volatile use).

- Emit the message that says what endpoint the UDP client is on on a timer (so there's no change Hercules misses it).
    - Maybe also do this with `signal_quality` so Hercules doesn't have to poll for it.
        - Doesn't have to be frequent, maybe once every couple seconds.
        - Include anything Hercules would have to regularly poll for automatically.
            - (could this risk slowly filling up and eventually overwhelming the UART buffer? check how big it is and measure the clear rate).
        - Docs (`UG209`) say you can't have more than one **recurring** timer but can you have a recurring timer and the single-shot timers already in use? Test this.