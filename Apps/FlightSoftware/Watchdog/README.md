# Watchdog

This branch contains code to run on the MSP430FR5994 acting as a Watchdog in the CubeRover.

Documents that may be useful when reading/developing this code:

* [MSP430FR5994 user guide](https://www.ti.com/lit/ug/slau367o/slau367o.pdf)
* [MSP430FR5994 datasheet](https://www.ti.com/lit/ds/symlink/msp430fr5994.pdf)
* SBC schematics
* Various behaviour specification documents (located in the GDrive)

## Responsibilities

The Watchdog's primary responsibilities are:
- Basic keepalive functionalities, ensuring proper operation of all other processors and hardware
- Control of all power systems (switches)
- Low-power low-speed communications routing when in wired (RS422) mode during transit and checkout
- Thermal control of heaters

### Architectures
The following architectures detail how to Watchdog is connected to the other hardware onboard the SBC. Detailed versions can be found in `Apps/FlightSoftware/resources/`.

#### **Embedded Systems (Overview)**
![Iris Embedded Systems Architecture, Simplified](../resources/Embedded%20Architecture%205.0,%20Simplified.png?raw=true "Iris Embedded Systems Architecture, Simplified")

#### **Power Architecture**
![Iris Power Architecture](../resources/Power%20Architecture%205.0.png?raw=true "Iris Power Architecture")
