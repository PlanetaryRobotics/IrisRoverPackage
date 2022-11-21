# Runs a python script using pyenv as if it was executed from the `GroundSoftware` directory.
SCRIPT_PATH=$1
shift # eat the arg
PYTHONPATH='.':$PYTHONPATH pyenv exec python3 $SCRIPT_PATH $@