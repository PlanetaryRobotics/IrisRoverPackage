# Setup and install all dependencies for the project for a Linux target:
set -x

# Install linux dependencies:
chmod +x ./setup/parts/linux_deps.sh
./setup/parts/linux_deps.sh

# Install pyenv:
chmod +x ./setup/parts/install_pyenv.sh
./setup/parts/install_pyenv.sh

# Install Redis Stack:
echo "!! If you want to use storage or the GDS-GUI, manually install Redis Stack >=7.0 from https://redis.io/download/."
# We need Redis Stack >=7.0, which is not available by default via package
# managers yet, so this will need to be manually installed:
# chmod +x ./setup/parts/install_redis_linux.sh
# ./setup/parts/install_redis_linux.sh

set +x