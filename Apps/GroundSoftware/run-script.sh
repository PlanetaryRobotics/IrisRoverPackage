# Runs a python script using pyenv as if it was executed from the `GroundSoftware` directory.
# Passes through all args.
# Example usage: ./run-script.sh ./scripts/tiny-apps/datastandards_lookup.py -l
SCRIPT_PATH=$1
shift # eat the arg
PYTHONPATH='.':$PYTHONPATH pyenv exec python3 $SCRIPT_PATH $@