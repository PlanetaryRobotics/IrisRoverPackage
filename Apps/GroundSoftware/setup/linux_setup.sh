# Setup and install all dependencies for the project for a Linux target:
set -x

# Install linux dependencies:
chmod +x ./setup/parts/linux_deps.sh
./setup/parts/linux_deps.sh

# Install pyenv:
chmod +x ./setup/parts/install_pyenv.sh
./setup/parts/install_pyenv.sh

# Install Redis Stack:
chmod +x ./setup/parts/install_redis_linux.sh
./setup/parts/install_redis_linux.sh

set +x