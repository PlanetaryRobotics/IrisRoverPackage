All testing & util scripts (not part of the core application) have been moved here.

These used to be stored in the `GroundSoftware` root directory, leading to a lot of clutter. This behavior is now deprecated.

To run any of the `*.py` files, call `./run-script.sh ./scripts/your-script.py` from `GroundSoftware`. Example:
```
./run-script.sh ./scripts/datastandards_lookup.py -l
```

To run Jupyter notebooks, follow the instructions in: `scripts/notebooks`.