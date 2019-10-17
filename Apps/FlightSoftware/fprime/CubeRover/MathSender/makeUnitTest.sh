#!/bin/bash

make testcomp

mkdir -p test/ut

mv TesterBase.* test/ut/
mv GTestBase.* test/ut/
mv Tester.* test/ut/

PROJECT_NAME=$(basename $(dirname $PWD))
COMPONENT_NAME=$(basename $PWD)

cd test/ut/
touch main.cpp
touch mod.mk

echo "TEST_SRC = main.cpp \ " > mod.mk
echo "Tester.cpp \ " >> mod.mk
echo "GTestBase.cpp \ " >> mod.mk
echo "TesterBase.cpp" >> mod.mk

echo "TEST_MODS = ${PROJECT_NAME}/${COMPONENT_NAME}" >> mod.mk

