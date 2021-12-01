#!/bin/sh
mkdir -p ./Libs
find . -path ./Libs -prune -o -print | grep -E "tiR4-ti-cgt-arm-bin/(.+)\.a$" | xargs cp -vft ./Libs/
