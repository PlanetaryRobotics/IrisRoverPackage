#!/bin/bash
# Lists all available CCS project configurations for the project in the current directory.
# (cd into the root directory of the project of interest then run this)
# NOTE: Rebuild the docker image (`docker compose build`) if you change this.

echo "All 'configuration' matches in '.cproject' :"
cat .cproject | awk '
{
    match($0, /(<configuration.*[Nn]ame=")([^"]+)"/, arr);
    if (arr[2] != "") {
        pre = arr[1];
        match_str = arr[2];
        post = substr($0, RSTART + RLENGTH - 1);
        printf "\n%s\033[32m%s\033[0m%s\n", pre, match_str, post;
    }
}'

printf "\nNames of 'out' artifact configurations in '.cproject':\n"
cat .cproject | awk '
{
    match($0, /(<configuration artifactExtension=\"out\".*[Nn]ame=")([^"]+)"/, arr);
    if (arr[2] != "") {
        pre = arr[1];
        match_str = arr[2];
        post = substr($0, RSTART + RLENGTH - 1);
        printf "\033[32m%s\033[0m\n", match_str;
    }
}'
