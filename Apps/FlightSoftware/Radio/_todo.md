
- Add Watchdog communication via `RADIO_KICK` and `WD_INT` lines.

- Do a quick sanity check and make sure pulsing `MCLR` doesn't clear the `BGScript` from memory, when it's been flashed using either the PICKit ICSP or the UART-DFU.
    - Nowhere does it say this happens, but `MCLR` looks suspiciously like it stands for `Memory Clear` and this would be a dumb way to die on the Moon.

- Finish `state_driver.puml` (documenting `state_driver.bgs`).

- Add the ability to pass in new WiFi credentials from Earth (for volatile use).


# Critical Todos:
+ Restrict to Ch.11 only
+ Fix set_transmit_size
+ Interlock with Hercules Comms
+ Properly configure I/O
+ Basic GPIO to WD on Connection status
- (and only transmit GND HBs if not in statis)
- Anomaly check for events like software_exception (so we can have a critical reset)
- Add sentinel byte check (every HB) using a couple sentinel bytes at beginning
  of dim memory space (every bgs dim just in case?) to make sure the stack isn't
  overwriting into dim space (per manual). -> fatal:so (stack overflow)
  ^ could just check it with any timer at the bottom of main? (incl. stack
  overflow checker at bottom of main)
    - Also, as a precaution, check all *really* important operational values to
    make sure they have a known possible value (e.g. check state_4B_abbr)
    - Also can serve as a basic cosmic radiation safety
- Test behavior on disconnect while Hercules is actively TXing UDP (and also when it's not)
- Run a long (>72hrs) continuous test with Hercules and check for any fatal or
  fatal:crit/swe/so messages.