# Dockerfile that sets up a TI CCS v9.1.0 instance that can be used for building the Watchdog FW.
# Note: This setup was done in such a way that it should be fairly simple to also get it up and running for the Hercules Primary Flight MCU and Motor Control MCUs.
# Reference Dockerfile: https://github.com/ahills60/ti-ccs-docker/blob/master/Dockerfile
# Reference Dockerfile: https://github.com/nmenon/ccs-msp430-ci/blob/master/Dockerfile
# Reference Dockerfile: https://hub.docker.com/r/roomz/ccs-ci/dockerfile
# Also incorporates the CCS instructions from `f-prime installation v2`: https://docs.google.com/document/d/1pXg0kUi5iw1Kkr1MWUebdEOPWL3u7ArK/edit

FROM iris_ccs_common:latest

# Ensure we're root for system configs:
USER root

# Pull in args:
ARG WD_PROJ_DIR
ARG WD_CCS_WORKSPACE_DIR
# Make ARGs available as ENV variables:
ENV WD_PROJ_DIR  ${WD_PROJ_DIR?/home/iris/IRP/Apps/FlightSoftware/Watchdog}
ENV WD_CCS_WORKSPACE_DIR  ${WD_CCS_WORKSPACE_DIR?/home/iris/workspace}

# Make sure all required project directories exist:
RUN mkdir -p \
    ${WD_CCS_WORKSPACE_DIR} \
    ${WD_PROJ_DIR}

# Make sure `iris` user still has access to everythig it needs:
RUN chown -R iris:iris ${WD_CCS_WORKSPACE_DIR} ${WD_PROJ_DIR}

# Switch back to `iris` for running CCS:
USER iris

# Add archived CCS Workspace:
COPY ./_ccs_workspace.docker.tar ${WD_CCS_WORKSPACE_DIR}
RUN <<EOF
cd ${WD_CCS_WORKSPACE_DIR}
tar -xf _ccs_workspace.docker.tar --strip-components=1
rm -f _ccs_workspace.docker.tar
EOF

# Import CCS Project parameters:
ARG WD_CCS_PROJ_NAME
ENV WD_CCS_PROJ_NAME ${WD_CCS_PROJ_NAME}
ARG WD_CCS_PROJ_CFG_DEFAULT
ENV WD_CCS_PROJ_CFG_DEFAULT ${WD_CCS_PROJ_CFG_DEFAULT}

# Import Project (if not already there):
# (this will return an error if the project is already there, so we `exit 0` no matter what)
VOLUME ${WD_PROJ_DIR}
WORKDIR ${WD_PROJ_DIR}

# Make project-specific scripts executable by all users:
# (NOTE: they have to be copied somewhere outside of the project volume since changes there will get discarded after this exits since nothing is mounted there rn)
USER root
COPY ./docker/build.sh /build.sh
RUN chmod ugo+x /build.sh
USER iris

# ENTRYPOINT []
