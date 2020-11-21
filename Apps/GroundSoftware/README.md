## Setup
NOTE: All makefiles are designed to be run in an environment that supports bash.

# Install Dependencies
This package requires `make`, `python`, and (`pyenv` and `pyenv-virtualenv` via `pyenv-installer`) to be installed.

# Setup Development Environment:
Run:
`make init`
Will perform `make clean` first. If you want to only perform specific parts of the setup, look in the `init` routine of `makefile`.

## Develop

# Update Dependencies:
`make update`

# Run application:
`make run`

# Teardown and cleanup development environment (before reinitializing):
`make clean`

## Build
*TODO*