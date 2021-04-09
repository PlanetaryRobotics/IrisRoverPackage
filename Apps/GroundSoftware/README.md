## Setup
NOTE: All makefiles are designed to be run in an environment that supports bash.

# Install Dependencies
This package requires `make`, `python` (>=3.8), and (`pyenv` and `pyenv-virtualenv` via `pyenv-installer`) to be installed.

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