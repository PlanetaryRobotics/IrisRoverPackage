# Setup and install all dependencies for the project for a MacOS target with
# Homebrew (brew) installed:
set -x

# Install pyenv:
chmod +x ./setup/parts/install_pyenv.sh
./setup/parts/install_pyenv.sh

# Install Redis Stack:
chmod +x ./setup/parts/install_redis_mac.sh
./setup/parts/install_redis_mac.sh

set +x