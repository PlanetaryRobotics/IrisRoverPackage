## Setup
NOTE: All makefiles are designed to be run in an environment that supports bash.

# Install Dependencies
This package requires `make`, `python` (>=3.8), and (`pyenv` and `pyenv-virtualenv` via `pyenv-installer`) to be installed, and make sure to add `pyenv` to the local path (follow the instructions in the terminal after `pyenv-installer` completes).

For ubuntu install prerequisites: `sudo apt-get install build-essential libffi-dev libsqlite3-dev zlib1g-dev xz-utils sqlite3 llvm libreadline-dev libgdbm-dev openssl libxml2-dev libbz2-dev curl libxmlsec1-dev wget tk-dev libssl-dev libncursesw5-dev libgdbm-compat-dev liblzma-dev uuid-dev bzip2

**Note:** you should be able to run `pyenv-installer` by running
`curl https://pyenv.run | bash`
but manually inspect this url's contents before running to ensure it hasn't changed ownership or something and been replaced with a malicious target.

**Note:** If `pyenv` has a build failure (when running `make init` or `pyenv install X.X.X`), make sure all `pyenv` prerequisites are present on your machine (see [pyenv - Common Build Problems](https://github.com/pyenv/pyenv/wiki/common-build-problems) for help.)

# Setup Development Environment:
Run:
`make init`
Will perform `make clean` first. If you want to only perform specific parts of the setup, look in the `init` routine of `makefile`.

## Develop

*Note*: As a dev, you'll likely want to `pip install pyls-mypy` so that your IDE 
can lint mypy (type annotations) via the Python Language Server.

### Known good development and build environment:

**OS:** MacOS

**Editor:** VSCode

**Linting:** `mypy` on VSCode using the [mypy](https://marketplace.visualstudio.com/items?itemName=matangover.mypy) extension installed. Be careful with the way the python language server is set up. Some linter setups won't find and lint imported python packages for typing (Spyder <=4.2.0 and vscode with pylance both had this problem). This setup does.

# Update Dependencies:
`make update`

# Run application:
`make run`

# Teardown and cleanup development environment (before reinitializing):
`make clean`

## Build
*TODO*
