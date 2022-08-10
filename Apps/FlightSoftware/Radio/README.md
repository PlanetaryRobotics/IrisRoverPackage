# Iris WF121 WiFi Radio Firmware

Firmware for the PIC MCU on the WF121 radio aboard the Iris Lunar Rover

This firmware runs as a compiled BASIC `BGScript` inside a VM that runs on top of the core WF121 firmware (see `docs/UG207`).

The firmware auto connects and establishes a UDP client and server with the network specified in `src/network_settings.bgs`. The device can also be commanded using `BGAPI` (see `docs/API_Guide`) over its UART1 interface (connected to Iris' Hercules MCU) and will emit `BGAPI` events over that socket. WiFi Data can be sent/received over UART by issuing the `endpoint_send` `BGAPI` command and monitoring `tcpip_udp_data` `BGAPI` event. This firmware is designed to be fully self-suffient in setting up and maintaining WiFi connection with the lander, allowing Hercules to treat it as just a fancy serial port which uses `BGAPI` commands and events as its communication protocol.

## Setup
- On a Windows machine (Windows 10 verified):
    - Run `resources/bluegiga-wi-fi-1.4.1-91.exe` to install the appropriate Bluegiga tools (GUI and compiler). 
        - Make sure to install it to `C\Bluegiga\wf121-1.4.1-91\` (i.e. install directory should be `C:\`). If you have to change this, update `compile.bat`.
    - Download `MPLAB IPE` (v6) if you want to program using ICSP (the PICKit and not using the UART-DFU bootloader).

## Building:
- From `Powershell`, run `.\compile.bat`.

## Flashing:
### **Over serial with DFU over UART:**

- Use the BlueGiga WF121 GUI software (installed above).
- See further instructions in `docs/UG221` (at the end).
- **It's imperative that you turn OFF the `Send packets without waiting response` checkbox.**
    - If this is left on, it will likely fail to flash and can corrupt the bootload. The fix is uploading over ICSP (see below). There are no issues doing this but, in some situations, that connector may not be available.
### **Over ICSP (for the first load or to avoid using UART):**
- Run `compile.bat` first.
    - This is important b/c programming this way will erase the MAC address and `compile.bat` includes a call to update the `hex` file's MAC address.
- Use PICKit v3 programming hardware
- Use the MPLAB IPE v6
    - Set device to PIC32MX695F512H
    - Plug into J6 on the SBC (Rev I).
        - Leave the programmer unpowered (disconnected from PC)
    - Power On Radio.
    - Connect programmer to PC.
    - Click `Connect`.
    - Load the `hex` file.
    - Program.

    
## Critical Docs
Critical reference documents can be found in `docs`. Notably, these include examples and more detailed (visual) instructions on things like building and flashing.
