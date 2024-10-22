# Installs all linux-specific dependencies for the project:
set -x

# Install linux dependencies:
chmod +x ./linux_deps_os_core.sh
./linux_deps_os_core.sh

# Add python PPA and install python 3.10:
sudo apt-get install -y software-properties-common
sudo add-apt-repository ppa:deadsnakes/ppa
sudo apt-get update
sudo apt-get -y install python3.10

# Install Git Credential Manager:
sudo apt-get install pass gnupg
cd ~ # go to home (so GCM isn't installed in the repo)
curl -LO https://raw.githubusercontent.com/GitCredentialManager/git-credential-manager/main/src/linux/Packaging.Linux/install-from-source.sh && sh ./install-from-source.sh && git-credential-manager-core configure
cd - # return here for subsequent ops
# Set GCM_CREDENTIAL_STORE to gpg if one is not set:
if [ -z "${GCM_CREDENTIAL_STORE+x}" ]; then # if $GCM_CREDENTIAL_STORE isn't defined
    echo '' >> ~/.bashrc;
    echo 'export GCM_CREDENTIAL_STORE="gpg"' >> ~/.bashrc;
fi
set +x
