#!/bin/sh
# **********************************************************************

sudo updatedb
locate $PWD/*tiR4-ti-cgt-arm-bin/*.a > tmp.txt
xargs -a tmp.txt cp -t $ASTRO_ROOT/Libs
rm -f tmp.txt