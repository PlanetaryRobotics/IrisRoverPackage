# Iris Lunar Rover Flight Software (c.2023)

## Watchdog (MSP430): `./Watchdog`
- Uses TI CSS v9 on Ubuntu

## Primary Flight Controller (Hercules): `./PrimaryFlightController` and `./fprime`:
- Uses TI CSS v9 on Ubuntu
- TI project root directory is `./PrimaryFlightController/FlightMCU`
    - This contains core application architecure, HAL, and FreeRTOS infrastructure
    - Project should be built and run from here.

- FPrime components and source directory is `./fprime`
    - This contains all the applications and drivers that run on the FlightMCU.
    - Iris (nee `CubeRover`) FPrime components in `./fprime/CubeRover`
    - FPrime "`Ac`" components aren't built in the repo and exist only as FPrime XML.
        - Whenever loading this repo on a new machine (or making changes to FPrime XML), run:

```bash
    cd ./PrimaryFlightController/FlightMCU
    ./fprime_auto_rebuild.sh
```

- Detailed terminal outputs from `fprime_auto_rebuild.sh`, including details on any errors, can be found in `./PrimaryFlightController/FlightMCU/fprime_build_full_outputs`
    - NOTE: errors don't matter when building `fprime/CubeRover` unless they occur under the second (final) `TIR4` build for `CubeRover`.

## Motor Controllers (MSP430): `./MotorControl`
- Uses TI CSS v9 on Ubuntu (watchdog workspace)

## Radio (WF121): `./Radio`
- Needs to be built on Windows. See `Radio/README` for more details.
