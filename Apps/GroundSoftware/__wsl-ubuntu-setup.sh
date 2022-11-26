# Setup and install all dependencies for the project:

# Update package managers:
sudo apt update
sudo apt-get update
# Upgrade system:
sudo apt-get upgrade -y

# Install Ubuntu prerequisites:
sudo apt-get install build-essential libffi-dev libsqlite3-dev zlib1g-dev xz-utils sqlite3 llvm libreadline-dev libgdbm-dev openssl libxml2-dev libbz2-dev curl libxmlsec1-dev wget tk-dev libssl-dev libncursesw5-dev libgdbm-compat-dev liblzma-dev uuid-dev bzip2
sudo apt-get install pass gnupg

# Add python PPA and install python 3.10:
sudo apt-get install -y software-properties-common
sudo add-apt-repository ppa:deadsnakes/ppa
sudo apt-get update
sudo apt-get -y install python3.11

# Install Git Credential Manager:
cd ~ # go to home (so GCM isn't installed in the repo)
curl -LO https://raw.githubusercontent.com/GitCredentialManager/git-credential-manager/main/src/linux/Packaging.Linux/install-from-source.sh && sh ./install-from-source.sh && git-credential-manager-core configure
cd - # return here for subsequent ops
# Set GCM_CREDENTIAL_STORE to gpg if one is not set:
if [ -z "${GCM_CREDENTIAL_STORE+x}" ]; then # if $GCM_CREDENTIAL_STORE isn't defined
    echo '' >> ~/.bashrc;
    echo 'export GCM_CREDENTIAL_STORE="gpg"' >> ~/.bashrc;
fi

## PYENV
# Configure shell for pyenv (before installing):
# the sed invocation inserts the lines at the start of the file
# after any initial comment lines
sed -Ei -e '/^([^#]|$)/ {a \
export PYENV_ROOT="$HOME/.pyenv"
a \
export PATH="$PYENV_ROOT/bin:$PATH"
a \
' -e ':a' -e '$!{n;ba};}' ~/.profile
echo 'eval "$(pyenv init --path)"' >>~/.profile

echo 'eval "$(pyenv init -)"' >> ~/.bashrc

# Add pyenv to bashrc (before installing):
echo '' >> ~/.bashrc
echo '# Add pyenv to local path automatically:' >> ~/.bashrc
echo 'eval "$(pyenv virtualenv-init -)"' >> ~/.bashrc

# Install pyenv
set -e
curl -s -S -L https://raw.githubusercontent.com/pyenv/pyenv-installer/master/bin/pyenv-installer | bash
