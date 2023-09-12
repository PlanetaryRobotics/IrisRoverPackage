# Setup and install all dependencies for the project for WSL.
# This script only exists for legacy reasons. All setup installers are located
# in `GroundSoftware/setup now`.
set -x
chmod +x ./setup/wsl_setup.sh
./setup/wsl_setup.sh
set +x
