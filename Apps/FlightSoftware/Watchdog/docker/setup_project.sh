#!/bin/bash
# Sets up the CCS project. Only needs to be run once.
# NOTE: Rebuild the docker image (`docker compose build`) if you change this.

cd ${WD_PROJ_DIR}

echo "Attempting to add project to workspace. NOTE: This will fail if it's already there. That's okay."

# Import Project (if not already there):
/home/iris/ti/ccs910/ccs/eclipse/eclipse -noSplash -data ${WD_CCS_WORKSPACE_DIR} -application com.ti.ccstudio.apps.projectImport \
    -ccs.location ${WD_PROJ_DIR} \
    -ccs.autoImportReferencedProjects

echo "NOTE: If this failed b/c the project was already there, that's okay."
# (this will return an error if the project is already there, so we `exit 0` no matter what)
exit 0 
