# Iris WF121 WiFi Radio Firmware

Firmware for the PIC MCU on the WF121 radio aboard the Iris Lunar Rover.

This firmware runs as a compiled BASIC `BGScript` inside a VM that runs on top of the core WF121 firmware (see `docs/UG207`).

The firmware auto-connects and establishes a UDP client and server with the Lander as specified in `src/network_settings.bgs`. The device can also be commanded using `BGAPI` (see `docs/API_Guide`) over its UART1 interface (connected to Iris' Hercules MCU) and will emit `BGAPI` events over that socket. WiFi Data can be sent/received over UART by issuing the `endpoint_send` `BGAPI` command and monitoring `tcpip_udp_data` `BGAPI` event on the Hercules MCU. This firmware is designed to be fully self-sufficient in setting up and maintaining WiFi connection with the lander, allowing Hercules to treat it as just a fancy serial port which uses `BGAPI` commands and events as its communication protocol.

Additionally, this supports sending custom non-BGAPI status packets from the BGScript to the Hercules MCU over UART. See `src/hercules_messaging.bgs` for more details. A registry of common status / error messages can be found in `src/hercules_reports.bgs`.

## Setup
- On a Windows machine (Windows 10 confirmed to work):
    - Run `resources/bluegiga-wi-fi-1.4.1-91.exe` to install the appropriate Bluegiga tools (GUI and compiler). 
        - Make sure to install it to `C:\Bluegiga\wf121-1.4.1-91\` (i.e. install directory should be `C:\`). If you have to change this, update `compile.bat` accordingly.
    - Download `MPLAB IPE` (v6) if you want to program using ICSP (the PICKit and not using the UART-DFU bootloader).

## Configuration
- **Project configuration** can be found in `project.xml`.
    - This dictates what's built, how, and where it goes.
- **Hardware configuration** can be found in `config/hardware.xml`.
    - This dictates how the device's physical hardware is configured (IO ports, interfaces, etc.)

If you need to change the hardware setup, feel free to modify the respective `xml` files, just read the docs carefully first. See `docs/UG221` for details on the structure and contents of these files.

## Building
- From `Powershell`, run `.\compile.bat`.

## Flashing
### **Over serial with DFU over UART:**

- Use the BlueGiga WF121 GUI software (installed above).
- See further instructions in `docs/UG221` (at the end).
- **It's imperative that you turn OFF the `Send packets without waiting response` checkbox.**
    - If this is left on, it will likely fail to flash and can corrupt the bootloader. The fix is uploading over ICSP (see below). There are no issues doing this but, in some situations, that connector may not be available.
### **Over ICSP (for the first load or to avoid using UART):**
- Run `compile.bat` first.
    - This is important b/c programming this way will erase the MAC address and `compile.bat` includes a call to update the `hex` file's MAC address.
- Use PICKit v3 programming hardware
- Use the MPLAB IPE v6
    - Set device to PIC32MX695F512H
    - Plug into J6 on the SBC (Rev I).
        - Leave the programmer unpowered (disconnected from PC).
    - Make sure the Watchdog leaves the Radio RST pin floating (set as input on I/O expander).
    - Power On Radio.
    - Connect programmer to PC.
    - Click `Connect`.
    - Load the `hex` file from `build/iris_wifi_radio__auto_connect.hex`.
    - Click `Program`.
    - After successful programming, power everything off and then unplug the programmer.
    - If the programmer fails to communicate with the device, it's likely the RST pin (`MCLR`) is being held HIGH or LOW.

## Targets
This code is designed to work equally well when:
1. Uploaded directly to the WF121 on the `Iris Lunar Rover SBC` (Rev I) and communicating with the Hercules MCU.
    - For debugging, you may also add a serial dongle in parallel using the SBC's breakout connector but be careful to only do this with the Hercules unpowered.
2. Uploaded to a WF121 dev kit and communicating with the BlueGiga WiFi GUI via a 3.3V UART (TTL Serial) dongle.
    - For communication to work properly, you'll need to connect `RTS` and `CTS` on the UART dongle in addition to `TX` and `RX` since control-flow is required.
    - BlueGiga WiFi GUI can be installed using `resources/bluegiga-wi-fi-1.4.1-91.exe`.
    - Dev kit used for development was the `MIKROE WiFi 6 Click board`.
        - Note: "WiFi 6" here isn't referring to the WiFi 6 protocol, but rather where this board fits in their "WiFi Click" line-up (they offer up to "WiFi 11").
        - **NOTE:** This board ships blank, so the first time you flash it, you'll need to use the `ICSP` `PICKit 3` method detailed above in order to get the UART DFU bootloader on there.

## Critical Docs
Critical reference documents can be found in `docs`. Notably, these include examples and more detailed (visual) instructions on things like building and flashing.
