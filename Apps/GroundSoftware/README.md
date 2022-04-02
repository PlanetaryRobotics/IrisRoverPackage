## Setup
NOTE: All makefiles are designed to be run in an environment that supports bash.

# Install Dependencies

## Automated Dependencies Setup on Ubuntu (in WSL)
**Note:** The following automated setup was testing on a `Ubuntu 18.04 WSL2` install but will likely also work on a stand-alone install of `Ubuntu`.

To install and setup the dependencies for this project on `Ubuntu`:
- Run: `chmod +x ./__wsl-ubuntu-setup.sh`
- Run: `sudo ./__wsl-ubuntu-setup.sh`
- Close your terminal.
- Restart Ubuntu
    - If using WSL, you can do this by opening Powershell and running `wsl.exe --shutdown` and then reopening the `Ubuntu` terminal.
- Make sure `pyenv` installed correctly by running: `which pyenv`:
    - This should echo a directory to the console.
    - If not, or if the terminal started up with new errors at the top, or if any errors were reported during the install process:
        - Open `./__wsl-ubuntu-setup.sh`
        - Run each command in the script, one line at a time.
        - Resolve any errors as they come up and report them to the `CubeRoverPackage/Apps/GroundSofware` devs.

## Authoring Code on Ubuntu (in WSL)
If you intend to author commits to this repo:
- Run: `git config user.name <your-name>` (this is just the name used on commits, not a log-in)
- Run: `git config user.email <your-email>` (this is just the name used on commits, not a log-in)
- If you're on linux and don't have a password manager or log-in system set up for `git` yet:
    - `GCM` has been installed to manage `git` credentials
    - `pass` has been installed as a password manager
    - `gpg` has been installed and set as `GCM_CREDENTIAL_STORE` if one wasn't set yet.
    - If you haven't initialized `pass` yet, run `pass init <pick-a-gpg-key-for-your-password-manager-and-save-it>`.

## Manual Setup
It is advised you set up your environment using the automated scripts above; however, if you prefer or need not to run these scripts, the key setup steps are listed below. In addition, it is recommended that you look through each step in the automated scripts used above to make sure you're not missing anything critical.

This package requires `make`, `python` (>=3.8), and (`pyenv` and `pyenv-virtualenv` via `pyenv-installer`) to be installed, and make sure to add `pyenv` to the local path (follow the instructions in the terminal after `pyenv-installer` completes).

For ubuntu, install the following prerequisites: `sudo apt-get install build-essential libffi-dev libsqlite3-dev zlib1g-dev xz-utils sqlite3 llvm libreadline-dev libgdbm-dev openssl libxml2-dev libbz2-dev curl libxmlsec1-dev wget tk-dev libssl-dev libncursesw5-dev libgdbm-compat-dev liblzma-dev uuid-dev bzip2 sudo apt-get install pass gnupg`

**Note:** you should be able to run `pyenv-installer` by running
`curl https://pyenv.run | bash`
but manually inspect this url's contents before running to ensure it hasn't changed ownership or something and been replaced with a malicious target.

**Note:** If `pyenv` has a build failure (when running `make init` or `pyenv install X.X.X`), make sure all `pyenv` prerequisites are present on your machine (see [pyenv - Common Build Problems](https://github.com/pyenv/pyenv/wiki/common-build-problems) for help.)

# Setup Development Environment
Run:
`make init`
Will perform `make clean` first. If you want to only perform specific parts of the setup, look in the `init` routine of `makefile`.

## Develop

*Note*: As a dev, you'll likely want to `pip install pyls-mypy` so that your IDE 
can lint mypy (type annotations) via the Python Language Server.

### Known good development and build environment:

**OS:** MacOS, Ubuntu 18.04, Ubuntu 18.04 in WSL2 inside Windows 11

**Editor:** VSCode (with `Remote - WSL` extension if using WSL)

**Linting:** `mypy` on VSCode using the [mypy](https://marketplace.visualstudio.com/items?itemName=matangover.mypy) extension installed. Be careful with the way the python language server is set up. Some linter setups won't find and lint imported python packages for typing (Spyder <=4.2.0 and vscode with pylance both had this problem). This setup does.

# Update Dependencies:
`make update`

# Run application:
`make run`

# Teardown and cleanup development environment (before reinitializing):
`make clean`

## Build
*TODO*
