#!/bin/bash
# This was my build step (mostly because my arm compiler won't link *.a without executable enabled??
make clean
make TIR4

# Call component specific build scripts here
pushd Top
./build.sh
popd

pushd NetworkManager
./build.sh
popd

# find .. | grep --color=never -e ".a$" | xargs --verbose chmod +x
make TIR4
./clean.sh
# git checkout ../Autocoders/*      # If your git freaks out over file mode changing

# Now go to CCS, delete the TI build output folders (which SHOULD be removed by clean.sh
# Build