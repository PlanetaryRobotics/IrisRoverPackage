# Install core os-level linux dependencies - for docker too (no dev tools or python env):
set -x
# Update package managers:
sudo apt update
sudo apt-get update
# Upgrade system:
sudo apt-get upgrade -y

# Install Linux prerequisites:
sudo apt-get install build-essential libffi-dev libsqlite3-dev zlib1g-dev xz-utils sqlite3 llvm libreadline-dev libgdbm-dev openssl libxml2-dev libbz2-dev curl libxmlsec1-dev wget tk-dev libssl-dev libncursesw5-dev libgdbm-compat-dev liblzma-dev uuid-dev bzip2 aria2

set +x
