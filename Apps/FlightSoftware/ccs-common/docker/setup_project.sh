#!/bin/bash
# Sets up the CCS project. Only needs to be run once.
# Takes 2 arguments workspace_directory and project directory.
# NOTE: Rebuild the docker image (`docker compose build`) if you change this.

cd ${2}

echo "Attempting to add project to workspace. NOTE: This will fail if it's already there. That's okay."

# Import Project (if not already there):
/home/iris/ti/ccs910/ccs/eclipse/eclipse -noSplash -data ${1} -application com.ti.ccstudio.apps.projectImport \
    -ccs.location ${2} \
    -ccs.autoImportReferencedProjects

cd -

echo "NOTE: If this failed b/c the project was already there, that's okay."
# (this will return an error if the project is already there, so we `exit 0` no matter what)
exit 0 
