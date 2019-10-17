#!/bin/bash

#call makefile and select implement option
make impl

CURRENT_DIR=$(basename "$PWD")

rm -f *Impl.cpp *Impl.hpp
mv *Impl.cpp-template "${CURRENT_DIR}ComponentImpl.cpp"
mv *Impl.hpp-template ${CURRENT_DIR}ComponentImpl.hpp
