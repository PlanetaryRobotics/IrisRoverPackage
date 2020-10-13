#!/bin/sh
mkdir -p ./Libs
find . -path ./Libs -prune -o -print | grep -E "tiR4-ti-cgt-arm-bin/(.+)\.a$" | tee tmp.txt | xargs cp -vft ./Libs/
