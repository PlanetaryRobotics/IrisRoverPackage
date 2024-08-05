#!/bin/bash
## Builds the WD flight software.
## Usage: build.sh [TYPE] [CONFIG]
##
## Arguments:
##   TYPE       [Optional] Build type: incremental, full, clean
##   CONFIG     [Optional] Configuration: which project configuration to build (e.g. Debug)
##

# NOTE: Rebuild the docker image (`docker compose build`) if you change this.

cd ${WD_PROJ_DIR}

# Build:
/home/iris/ti/ccs910/ccs/eclipse/eclipse -noSplash -data ${WD_CCS_WORKSPACE_DIR} \
    -application com.ti.ccstudio.apps.projectBuild \
    -ccs.projects ${WD_CCS_PROJ_NAME} \
    -ccs.configuration ${2:-${WD_CCS_PROJ_CFG_DEFAULT}} \
    -ccs.buildType ${1:-incremental} \
    -ccs.listErrors \
    -ccs.listProblems \
2>&1 | tee ./out/${2:-${WD_CCS_PROJ_CFG_DEFAULT}}.build.log

# Copy key products to ./out/
[ -f "./${2:-${WD_CCS_PROJ_CFG_DEFAULT}}/Watchdog.out" ] && cp ./${2:-${WD_CCS_PROJ_CFG_DEFAULT}}/Watchdog.out ./out/${2:-${WD_CCS_PROJ_CFG_DEFAULT}}.out
[ -f "./${2:-${WD_CCS_PROJ_CFG_DEFAULT}}/Watchdog.map" ] && cp ./${2:-${WD_CCS_PROJ_CFG_DEFAULT}}/Watchdog.map ./out/${2:-${WD_CCS_PROJ_CFG_DEFAULT}}.map
[ -f "./${2:-${WD_CCS_PROJ_CFG_DEFAULT}}/Watchdog_linkInfo.xml" ] && cp ./${2:-${WD_CCS_PROJ_CFG_DEFAULT}}/Watchdog_linkInfo.xml ./out/${2:-${WD_CCS_PROJ_CFG_DEFAULT}}_linkInfo.xml

# Finally, print memory summary:
[ -f "./out/${2:-${WD_CCS_PROJ_CFG_DEFAULT}}_linkInfo.xml" ] && python3 /memory_allocation.py ./out/${2:-${WD_CCS_PROJ_CFG_DEFAULT}}_linkInfo.xml

# Then re-summarize the build results at the bottom (so it's immediately visible):
# Print project warnings:
echo "--------------------------------------"
cat ./out/${2:-${WD_CCS_PROJ_CFG_DEFAULT}}.build.log | grep -Pzo "(?s)Warnings for project.*?======================================"
echo ""
# Print project errors:
echo "--------------------------------------"
cat ./out/${2:-${WD_CCS_PROJ_CFG_DEFAULT}}.build.log | grep -Pzo "(?s)Errors for project.*?======================================"
echo ""
# Print project summary:
cat ./out/${2:-${WD_CCS_PROJ_CFG_DEFAULT}}.build.log | grep --color=always -E "CCS headless build complete!"
