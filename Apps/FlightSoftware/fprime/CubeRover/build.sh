# This was my build step (mostly because my arm compiler won't link *.a without executable enabled??
git checkout ../Autocoders/*
make clean
make TIR4
cp --verbose ./Top/CubeRoverTopologyAppAc.hpp.bak ./Top/CubeRoverTopologyAppAc.hpp
find .. | grep --color=never -e ".a$" | xargs --verbose chmod +x
make TIR4
./clean.sh

# Now go to CCS, delete the TI build output folders (which SHOULD be removed by clean.sh
# Build
