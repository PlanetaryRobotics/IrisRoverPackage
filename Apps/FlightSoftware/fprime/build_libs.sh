# To build from pristine state:
# fprime$ git clean -xdf
# fprime$ cd CubeRover      # or any of the Libs directories
# fprime$ make gen_make
# fprime$ cd ..
# fprime$ ./install_libs.sh

pushd Os
make TIR4
popd

pushd Fw
make TIR4
popd

pushd Drv
make TIR4
popd

pushd Svc
make TIR4
popd

pushd Utils
make TIR4
popd
