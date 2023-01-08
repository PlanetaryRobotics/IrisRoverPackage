# Iris Terminal Backend Interface Layer (BIL)
The Frontend BIL is a gRPC server that allows the Frontend application to talk to the Ground Software Backend using simple RPC server queries. This is a separate, independently packaged python application that runs locally on each target terminal machine.

## Setup
Eventually (once GSW IPC is complete), this will just be a standalone GSW IPC App, living inside the `Apps/GroundSoftware` directory and using all of the same setup. Until then, the `BIL` setup will be a subset of the setup used for `IrisBackendv3`.

 - If you don't have the dev dependencies like `pyenv` installed yet, run:
   - `chmod +x setup.sh`
   - `./setup.sh`
 - Run `make init`
 - If you plan on developing the BIL, run `./dev-setup.sh` to install dev-only dependencies.

## Development


## Deployment