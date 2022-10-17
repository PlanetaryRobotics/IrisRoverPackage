#!/bin/bash
# Rebuilds all FPrime static libraries `Libs/`. This is generally needed if a change to one of the 
# static library changes, especially if switching between branches that has a change. Remember to
# delete `Libs/` since install_libs.sh only copies over libraries it finds.
# 
# To build from pristine state:
# Ensure that any unsaved changes are stashed or backedup otherwise
# $ git clean -xdf
# $ cd CubeRover      # or any of the Libs directories
# Make sure your build environment has the paths correctly setup, for example:
#     - mk/configs/compiler/defines_ccs.mk: Make sure the path to the compiler
#     - mk/configs/compiler/defines_common.mk: Make sure the path to the project is correct
# $ make gen_make
# $ cd ..
# $ ./build_libs.sh
# $ ./install_libs.sh

umask 022   # Tries to fix the make: execvp <build product>: Permission denied...

pushd Os
make clean
make TIR4
popd

pushd Fw
./clean.sh
make TIR4
popd

pushd Utils
./clean.sh
make TIR4
popd

pushd Svc
./clean.sh

# Health depends on WatchDog port, for some reason Fprime builds this out of order
pushd WatchDog
make TIR4
popd

make TIR4
popd

# Has compoennts that depend on ports in Svc
pushd Drv
./clean.sh
make TIR4
popd
