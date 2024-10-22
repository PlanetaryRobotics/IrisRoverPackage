#!/usr/bin/env bash

# Alias for pyenv for use in a docker image so all `pyenv exec python3` commands
# (e.g. in makefile) execute as `python3`.
# If this is placed in `PATH` as `pyenv` and made executable, calls to
# `pyenv exec python3` will act just like `python3`.

if [[ "$1" == "exec" && ( "$2" == "python3" || "$2" == "python" ) ]]; then
    exec python3 "${@:3}"
else
    # Don't override the command, just see if it'll evaluate:
    exec pyenv "$@"
fi
