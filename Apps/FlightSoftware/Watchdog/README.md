# Iris - FSW - Watchdog
Flight Software for the Iris Lunar Rover Watchdog (MSP430FR5994).

This requires using TI Code Composer Studio in a linux environment, testing working using CCS v9 and Ubuntu 18.04.

The entire CCS build suite can be run in a docker container, allowing this to be built on any machine.

## 0. Prerequisites:
- `Docker Desktop` installed
- \>= 11GB of free space
- 
OR

An installation of Code Composer Studio v9.1.0 or v9.2.0 on a Linux machine. v10+ doesn't work. In this case, all the steps involving Docker can be skipped. Just use the archived workspace `_ccs_workspace.docker.tar`, open the `Watchdog` project, and proceed.

## 1. Setup
### 1.1. Build Image:
After pulling the repo, run `docker compose build` to build a base Ubuntu image with CCS properly set up.

If something goes wrong during this step, you can get greater insight into what happened using `docker compose build --progress=plain`.

### 1.2. Setup CCS Project:
Run `docker compose up wd-setup-project` to make sure the CCS workspace is configured with this project correctly. It likely is but this will make sure. Only need to run this once.

### 1.3. Run initial build:
It's recommended that the first thing you do is run a clean build. This will ensure everything is ready to go and will save all common libraries so subsequent builds are faster:
`docker composer up wd-build-clean`

*NOTE: Unless you change the project substantially and require changes to the CCS configuration (i.e. you change `Dockerfile`, `.env`, or any of the scripts in `./docker`), you don't need to run any of this again.*

## 2. Build

## 2.1. Build Types:
To perform a normal incremental build, call:
`docker compose up wd-build`

To rebuild the whole project:
`docker compose up wd-full`

To clean up the build artifacts:
`docker compose up wd-clean`

## 2.2. Build Configurations:
Many CCS projects, including this one, have different build configurations that can be used for different deployments or development phases.

**The default configuration built** is: `WD_CCS_PROJ_CFG_DEFAULT`, defined in `.env`.

**To list all available configurations:**
`docker compose run --rm wd-term /list_ccs_configs.sh`

**To build a specific config**, append `WD_CONFIG=Config_Name ` before the any of the build commands. For example:

`WD_CONFIG=Debug_with_flag docker compose up wd-build-full` or

`WD_CONFIG=Debug_Radio_Flashing docker compose up wd-build-full` or

`WD_CONFIG=Debug_Radio_Herc_Flashing docker compose up wd-build-clean`

NOTE: **`Debug_with_flag`** was the flight version.

NOTE: The actual definitions for these configurations are in `.cproject`. This can be edited directly, though it's easier to do it via the Code Composer Studio GUI.

### 2.3 A note on build logs:
After each build is complete (successful or not), a `.build.log` file is made / rewritten in `./out/` under the name of the configuration built. This contains the direct output of running the CCS build operation.

NOTE: If you want to see the full docker output but your terminal cuts off and you want to see full build logs, check `Docker Desktop` or run `docker compose logs wd-build`. You can dump this into a file with `docker compose logs wd-build > "out/build.log" 2>&1`.

## 3. Flashing:
Binaries built go into a named directory and into the `./out/` directory. The `.out` files should be able to be flashed using TI's `MSPFlasher` on any operating system.

## 4. Debugging:
To interactively debug the MCU, you'll need to use a native CCS install on a linux machine. You can use the `Dockerfile` as guidance.

In theory, this isn't strictly necessary. It should be possible to just use a local CCS install (of the same version) on any machine and connect to the Docker container via a proxy, as detailed in  

## 5. Debug Docker:
If you need to debug the build environment, you can a container with an interactive shell by calling:

`docker compose run -it --rm wd-term` (to enter as the default `iris` user) or

`docker compose run --rm -it --user root wd-term` (to enter as `root`)

To exit the interactive session when done (and close and remove the container), just run `exit`. This returns you to your host shell.

## 6. Notes:
### 6.1. Running on Apple Silicon (M1, etc.)
Running this on ARM-based silicon like an Apple M1 chip has been tested but is technically experimental since it has to run in AMD64 emulation mode. No practical issues have been noted at the time of writing, however.