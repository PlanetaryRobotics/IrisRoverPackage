# Run the Legacy IrisConsole application.
# (some legacy external scripts expect `__run_console.py` to be here so we just
# expose the script here).
from scripts.tiny_apps.run_console import main

if __name__ == "__main__":
    main()