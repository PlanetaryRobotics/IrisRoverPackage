# Background
If you're new to JPL's FPrime or this environment, it's recommended that you read `FPRIME_NOTES/FPRIME_README.md` and the `docs` directory (instructions by JPL for FPrime) and `Primer.md` (by Iris for our use of FPrime).

# Source Code Locations:
- FPrime source code is in `FlightSoftware/fprime/`.
- Iris Flight MCU source code is in `FlightSoftware/PrimaryFlightController/FlightMCU/`.
- The code for Iris' FPrime Apps is in `FlightSoftware/fprime/CubeRover/`
  - CCS symlinks to this directory from `FlightMCU` in `<linkedResources>` (see `.project`).

**All development for the Primary Flight MCU (Hercules) should be from this directory (`FlightSoftware/PrimaryFlightController/FlightMCU/`).**

# **Setup**
NOTE: All makefiles are designed to be run in an environment that supports bash.

## **Setup Your Development Environment**

### **Install Build Dependencies**

- Install TI Code Composer Studio (CCS) 9.1.0
- Install `make`
- Install `python3` (at least 3.8.5)

### **Update `local.env` Settings**
If you intend to use the `make` build process, update `local.env` with the correct paths and settings for your build environment. This file is local to your machine and won't be committed.

### **Advised Way to Author Code on Ubuntu**
If you intend to author commits to this repo:
- Run: `git config user.name <your-name>` (this is just the name used on commits, not a log-in)
- Run: `git config user.email <your-email>` (this is just the email used on commits, not a log-in)
- If you're on linux and don't have a password manager or log-in system set up for `git` yet:
    - `GCM` has been installed to manage `git` credentials
    - `pass` has been installed as a password manager
    - `gpg` has been installed and set as `GCM_CREDENTIAL_STORE` if one wasn't set yet.
    - If you haven't initialized `pass` yet, run `pass init <pick-a-gpg-key-for-your-password-manager-and-save-it>`.

# **Initialize Development Environment**
- Run: `make init`
    - This will perform `make clean` first then install all dependencies. If you want to only perform specific parts of the setup, look in the `init` routine of `makefile`.

# **Develop**

Development makes use of the `make` tool. Run `make help` to see all available operations.

## Known good development and build environment:

- **OS:** Ubuntu 18.04
- **IDE:** TI Code Composer Studio 9.1.0
- **Machine:** µHermes laptop (ThinkPad)

## Developing in Code Composer Studio:
You don't have to use Code Composer Studio as your IDE if you don't want.
It will still be used by the build process no matter what. If you want to use it:

- Open Code Composer Studio.
- Create a new workspace or use an existing one.
    - Don't worry, all important build/flash settings are held in this project. Any workspace settings are developer specific. The build pipeline uses its own disposable workspace for each build process anyway.
- Go to `Import Project`, and choose the `FlightMCU` directory (this one).

## **Build Fprime**
`make build-fprime`

This rebuilds the FPrime structure (from XML). This should be done at least once before calling `make build` and after any change to the XML.

## **Build Source (CCS Project)**
`make build-ccs`

This rebuilds the Iris source code but **does not** rebuild the FPrime structure (from XML).

## **Build FPrime then Iris Source**
`make build-all` or `make ba`.

This builds everything possible (calls `make build-fprime` then `make build-ccs`).

## Cleanup development environment (before reinitializing):
Useful if you want a clean build.
`make clean`

# **Program**

## **Flash Binary**
To flash a pre-built binary onto the Hercules, run:

`make flash`

## **Build then Flash**
If you want to re-build the CCS project and then flash (this just calls `make build-ccs` then `make flash`):

`make build-flash` or `make b2f`

## **Build FPrime and Source then Flash**
If you want to rebuild the FPrime structure (from XML) then rebuild the CCS project and then flash (this just calls `make build-all` then `make flash`):

`make build-all-flash` or `make ba2f`

It's recommended that you do this before committing code.