# Replaces all _sb2 imports with python3-style relative imports in the given file.
#
# Need to perform a regex capture and replace on a file but sed is being
# finnicky about character sets so using this instead.
# ^-turns out sed would be perfectly okay here... the issue was actually that
# an old __pycache__ was being included in the search directory (which contains
# non-utf8 bytes, which broke sed). This has been fixed using `maxdepth 1`, so
# we could go back to sed but this works so no use in changing it.
import sys
import re
import os.path
file = sys.argv[1]
parent_dir = os.path.dirname(os.path.abspath(file))
bad_import_pattern = r'(import\s*\w*_pb2)'
print(f"Fixing {file} imports...")
# Check the file:
with open(file, 'r') as f:
    lines = f.readlines()
    for i, line in enumerate(lines):
        # Check if line contains a pb2 import.
        matches = re.findall(bad_import_pattern, line)
        if len(matches) > 0:
            # Get the name of the file being imported:
            imported_file = matches[0].split(r' ')[-1] + ".py"
            # Check if the file being imported exists in `parent_dir`
            if os.path.exists(os.path.join(parent_dir, imported_file)):
                # IFF the imported file is in the directory as the file being
                # examined, make the import relative:
                lines[i] = re.sub(bad_import_pattern, r'from . \1', line)
# Rewrite the file (inefficient but that's fine for this case):
with open(file, 'w') as f:
    for line in lines:
        f.write(line)
