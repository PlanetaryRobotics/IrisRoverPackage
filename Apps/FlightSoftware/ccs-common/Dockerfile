# iris-ccs-common
# Dockerfile that sets up a TI CCS v9.1.0 instance that can be used for building the Watchdog FW.
# Note: This setup was done in such a way that it should be fairly simple to also get it up and running for the Hercules Primary Flight MCU and Motor Control MCUs.
# Reference Dockerfile: https://github.com/ahills60/ti-ccs-docker/blob/master/Dockerfile
# Reference Dockerfile: https://github.com/nmenon/ccs-msp430-ci/blob/master/Dockerfile
# Reference Dockerfile: https://hub.docker.com/r/roomz/ccs-ci/dockerfile
# Also incorporates the CCS instructions from `f-prime installation v2`: https://docs.google.com/document/d/1pXg0kUi5iw1Kkr1MWUebdEOPWL3u7ArK/edit

FROM ubuntu:18.04

RUN ln -sf /usr/share/zoneinfo/EST /etc/localtime

# Make sure all required project directories exist:
RUN mkdir -p \
    /home/iris

# Make `iris` user (so CCS doesn't run as root, needed):
RUN <<EOF
groupadd -r iris -g 1000
useradd -u 1000 -r -g iris -d /home/iris -s /bin/bash -c "Docker user for CCS" iris
chown -R iris:iris /home/iris
EOF

# Do most of this as root still:
USER root

# Uncomment multiverse & security apt sources:
RUN sed -i 's/# \(.*multiverse$\)/\1/g' /etc/apt/sources.list
RUN sed -i 's/# \(deb-src.*\-security.*$\)/\1/g' /etc/apt/sources.list

# Install packages and clean up after, in one layer to reduce image size:
RUN <<EOF
export DEBIAN_FRONTEND=noninteractive; dpkg --add-architecture i386 \
    && apt-get update \
    && apt-get -y upgrade \
    && apt-get install -y --no-install-recommends \
    apt-utils \
    libc6:i386 \
    autoconf libtool build-essential libc6-i386 libusb-0.1-4 libgconf-2-4 \
    software-properties-common python2.7 libpython2.7  \
    byobu curl git git-lfs htop man unzip vim wget \
    libasound2                    \
    libstdc++6			\
    libxt6			\
    libcanberra-gtk-module        \
    unzip         		\
    wget                          \
    build-essential               \
    ca-certificates               \
    curl                          \
    libgconf-2-4                  \
    libdbus-glib-1-2              \
    libxtst6                      \
    at-spi2-core                  \
    binutils                      \
    python3-pip			\
    libgconf-2-4			\
    libncurses5			\
    libpython2.7			\
    build-essential		\
    libgtk-3-dev			\
    libnss3			\
    libxss1			\
    python-lxml \
    python-cheetah \
    libnspr4 \
    udev
add-apt-repository universe
apt-get install -y python3.10 python3.10-dev
apt-get clean
rm -rf /var/lib/apt/lists/*
EOF

# Set up for CCS install:
RUN export JAVA_TOOL_OPTIONS=-Xss1280k
RUN mkdir -p /root/Downloads
RUN mkdir -p /home/iris/

# NOTE: Even though Iris FM1 was built using CCS9.1.0, we have to use CCS 9.2.0
# to get a working headless installer. The differences between these two
# versions and their associated compilers appears to be sufficiently minimal;
# however, they may not produce byte-equivalent binaries.
# 
# NOTE: Prior to CCS 9.2.0, a recorded "response file" was required to configure
# the installation. If you need to go back to CCS 9.1.0, use this response file
# (couldn't get it the install to fully work headlessly but this part is
# correct (it's a capture)):
# RUN touch /root/ccs_response_file.txt
# RUN cat <<EOF >> /root/ccs_response_file.txt
# CreateDesktopShortcut: No
# CreateQuickLaunchShortcut: No
# InstallDir: /home/iris/ti/ccs910/
# InstallMode: Standard
# LaunchApplication: No
# edition_selection: Custom
# feature_selections: EMU_TI EMU_SD IDE_MSP430 MSP430_BUILD_DEBUG MSP430_GCC_SUPPORT EMU_MSP430 TIVA_STELLARIS_DRIVERS SEGGER_J_LINK SEGGER_J_LINK_DRIVERS JRE JRE32 P2APP ECLIPSE ECLIPSECORE P2TOOLS GMF IDE_MAIN IDE_WORKFLOW DVT_COMMON DVT_UIA DVT_VIA_CCS TIREX XULRUNNER INSTALLER IDE_CHROME IDE_TOOLS_COMMON IDE_GC TARGET_CONTENT_CORE TARGET_CONTENT_CORE_LEGACY NODEJS USERGUIDE DS DS_FLASH CLOUDAGENT SYSCONFIG MSVC80 MSVC2012 MSVC2015 MSP430_CGT_HELP CGT_DEFAULTLIBS_MSP430 DMED GCC_MSP430_COMPILER DVT_ENERGYTRACE MSP432_BUILD_DEBUG EMU_MSP432 EMU_MSP432_FLASH ARM_CGT_HELP CGT_DEFAULTLIBS_TMS470 IDE_ARM GCC_ARM_COMPILER DVT_TRACE DEV_SUPPORT_CC26XX EMU_STELLARIS DEV_SUPPORT_CC32XX DEV_SUPPORT_CC3220 DEV_SUPPORT_CC32XX_COMMON DEV_SUPPORT_CC3230 IDE_C2000 EMU_C2000_FLASH EMU_C2000 GREE_C2000_SUPPORT C2000_CGT_HELP CGT_DEFAULTLIBS_C2800 EMU_HERCULES
# product_selections: PF_MSP430 PF_MSP432 PF_CC2X PF_CC3X PF_C28 PF_HERCULES
# EOF
# RUN <<EOF
# curl -L http://software-dl.ti.com/ccs/esd/CCSv9/CCS_9_1_0/exports/CCS9.1.0.00010_linux-x64.tar.gz \
# --output /root/Downloads/CCS9.1.0.00010_linux-x64.tar.gz --silent
# tar xf /root/Downloads/CCS9.1.0.00010_linux-x64.tar.gz --directory /root/Downloads/
# rm -rf /root/Downloads/CCS9.1.0.00010_linux-x64.tar.gz
# chmod 777 /root/Downloads/CCS9.1.0.00010_linux-x64/ccs_setup_linux64_9.1.0.00010.bin
# /root/Downloads/CCS9.1.0.00010_linux-x64/ccs_setup_linux64_9.1.0.00010.bin \
#     --mode unattended \
#     --prefix /home/iris/ti/ccs910 \
#     --response-file /root/ccs_response_file.txt
# rm -rf /root/Downloads/CCS9.1.0.00010_linux-x64/
# EOF

# Download, install, & clean up CCS in one step:
# NOTE: Even though we have to use CCS 9.2.0 to get the new headless installer,
# we're using a directory called `ccs910` b/c that's what the archived
# workspaces build on CCS 9.1.0 expect. Directory name is irrelevant.
USER iris
RUN <<EOF
echo "Code Composer Studio v9.2.0..."
mkdir -p ~/ccs_install/
curl -L http://software-dl.ti.com/ccs/esd/CCSv9/CCS_9_2_0/exports/CCS9.2.0.00013_linux-x64.tar.gz | \
    tar xvz --strip-components=1 -C ~/ccs_install/
echo "CCS Download & Extraction Complete. Installing . . ."
chmod 777 ~/ccs_install/ccs_setup_9.2.0.00013.bin
~/ccs_install/ccs_setup_9.2.0.00013.bin \
    --mode unattended \
    --unattendedmodeui minimal \
    --prefix /home/iris/ti/ccs910 \
    --enable-components PF_MSP430,PF_MSP432,PF_CC2X,PF_CC3X,PF_C28,PF_HERCULES \
    --install-Segger true
rm -rf ~/ccs_install/
EOF
USER root
# NOTE: these are the args to install emulator tools. Segger seems to work but
# it fails with Blackhawk.
# --install-BlackHawk true \
# --install-Segger true

# Install SDKs:
# MSP430 SDK: Export protected. Requires TI log in. Skipping...
# MSPFlasher (part of MSP430 SDK but can be downloaded separately w/out login):
USER iris
RUN <<EOF
echo "MSPFlasher..."
mkdir -p ~/ccs_install/
wget -q -O ~/ccs_install/msp_flasher.zip https://dr-download.ti.com/software-development/software-programming-tool/MD-szn5bCveqt/1.03.20.00/MSPFlasher-1_03_20_00-linux-x64-installer.zip
unzip -d ~/ccs_install/ ~/ccs_install/msp_flasher.zip
echo "MSPFlasher Download & Extraction Complete. Installing . . ."
chmod 777 ~/ccs_install/MSPFlasher-1.3.20-linux-x64-installer.run
~/ccs_install/MSPFlasher-1.3.20-linux-x64-installer.run --mode unattended
rm -rf ~/ccs_install/
EOF
USER root
# TI Hercules SDK: None.

# Don't need any MSP432 Simplelink tools. If you do need MSP432 simplelink,
# there are a number of extra drivers to install.
# See Dockerfile references at the top for more details.

# Add Uniflash Tool (in case we want to automate flashing via Docker, e.g. for HITL):
# NOTE: This adds ~1.7GB to the Docker image size.
# NOTE: This is optional, so do it as late as possible (before switching to USER)
# so it doesn't invalidate subsequent layers in the image if turned off.
ARG BUILD_FLASHING_TOOLS
USER iris
RUN <<EOF
if [ "$BUILD_FLASHING_TOOLS" = "include-flash-tools" ]
then
    dpkg -r --force-depends libusb-1.0-0
    apt-get clean -yq && \
    apt-get update -yq && \
    DEBIAN_FRONTEND=noninteractive \
    apt-get install -y --no-install-recommends libusb-1.0-0 && \
    apt-get clean && rm -rf /var/lib/apt/lists/*
    wget -q -O /root/Downloads/uniflash.run https://dr-download.ti.com/software-development/software-programming-tool/MD-QeJBJLj8gq/8.7.0/uniflash_sl.8.7.0.4818.run
    echo "Uniflash Download & Extraction Complete. Installing . . ."
    chmod 777 /root/Downloads/uniflash.run
    /root/Downloads/uniflash.run --mode unattended
    rm -rf /root/Downloads/uniflash.run
fi
EOF
USER root

# Perform configurations in root space:
USER root

# Run Supplemental CCS Install Scripts:
RUN /home/iris/ti/ccs910/ccs/install_scripts/install_drivers.sh

# Install all versions of compilers used across the projects (OPT_CODEGEN_VERSION in .cproject):

# Watchdog/.cproject uses MSP430 CGT v18.12.2.LTS, 20.2.4.LTS:
RUN <<EOF
echo "MSP430 CGT 18.12.2..."
mkdir -p /ccs_install/
curl -L https://software-dl.ti.com/codegen/esd/cgt_public_sw/MSP430/18.12.2.LTS/ti_cgt_msp430_18.12.2.LTS_linux_installer_x86.bin > /ccs_install/cgt_msp.bin
echo "MSP430 CGT 18.12.2 Download Complete. Installing . . ."
chmod 777 /ccs_install/cgt_msp.bin
/ccs_install/cgt_msp.bin --prefix /home/iris/ti/ccs910/ccs/tools/compiler/ --mode unattended --unattendedmodeui minimal
rm -rf /ccs_install/
EOF
RUN <<EOF
echo "MSP430 CGT 20.2.4..."
mkdir -p /ccs_install/
curl -L https://software-dl.ti.com/codegen/esd/cgt_public_sw/MSP430/20.2.4.LTS/ti_cgt_msp430_20.2.4.LTS_linux-x64_installer.bin > /ccs_install/cgt_msp.bin
echo "MSP430 CGT 20.2.4 Download Complete. Installing . . ."
chmod 777 /ccs_install/cgt_msp.bin
/ccs_install/cgt_msp.bin --prefix /home/iris/ti/ccs910/ccs/tools/compiler/ --mode unattended --unattendedmodeui minimal
rm -rf /ccs_install/
EOF

# MotorControl/.cproject uses MSP430 CGT v18.12.2.LTS AND 18.12.4.LTS:
RUN <<EOF
echo "MSP430 CGT 18.12.4..."
mkdir -p /ccs_install/
curl -L https://software-dl.ti.com/codegen/esd/cgt_public_sw/MSP430/18.12.4.LTS/ti_cgt_msp430_18.12.4.LTS_linux_installer_x86.bin > /ccs_install/cgt_msp.bin
echo "MSP430 CGT 18.12.4 Download Complete. Installing . . ."
chmod 777 /ccs_install/cgt_msp.bin
/ccs_install/cgt_msp.bin --prefix /home/iris/ti/ccs910/ccs/tools/compiler/ --mode unattended --unattendedmodeui minimal
rm -rf /ccs_install/
EOF

# PrimaryFlightController/FlightMCU/.cproject uses TMS470 CGT v18.12.2.LTS:
RUN <<EOF
echo "TMS470 (ARM) CGT 18.12.2..."
mkdir -p /ccs_install/
curl -L https://software-dl.ti.com/codegen/esd/cgt_public_sw/TMS470/18.12.2.LTS/ti_cgt_tms470_18.12.2.LTS_linux_installer_x86.bin > /ccs_install/cgt_tms.bin
echo "TMS470 (ARM) CGT 18.12.2 Download Complete. Installing . . ."
chmod 777 /ccs_install/cgt_tms.bin
/ccs_install/cgt_tms.bin --prefix /home/iris/ti/ccs910/ccs/tools/compiler/ --mode unattended --unattendedmodeui minimal
rm -rf /ccs_install/
EOF

# Install TI's CG_XML tools for build analysis:
RUN <<EOF
echo "TI CG_XML..."
mkdir -p /ccs_install/ /home/iris/ti/cg_xml/
curl -L https://software-dl.ti.com/ccs/non-esd/releases/other/applications_packages/cg_xml/cg_xml_v2_61_00/cgxml-2.61.00-linux-installer.run > /ccs_install/cg_xml.run
echo "TI CG_XML Download Complete. Installing . . ."
chmod 777 /ccs_install/cg_xml.run
/ccs_install/cg_xml.run --prefix /home/iris/ti/cg_xml/ --mode unattended --unattendedmodeui minimal
rm -rf /ccs_install/
EOF

# Make sure `iris` user still has access to everythig it needs:
USER root
RUN chown -R iris:iris /home/iris

# Create a symlink for CCS dir and exec, from both user & root homes:
RUN ln -s /home/iris/ti/ccs910/ccs /home/iris/ccs_dir
RUN ln -s /home/iris/ti/ccs910/ccs /root/ccs_dir
RUN ln -s /home/iris/ti/ccs910/ccs/eclipse/eclipse /home/iris/ccs_cli
RUN ln -s /home/iris/ti/ccs910/ccs/eclipse/eclipse /root/ccs_cli

# Add CCS to path:
ENV PATH="/home/iris/ti/ccs910/ccs/eclipse:${PATH}"
# Add compilers (code generation tools) to path:
ENV PATH="${PATH}:/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin"
ENV PATH="${PATH}:/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.3.LTS/bin"

ENV PATH="${PATH}:/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/bin"
# NOTE: MSP430 CGT v18.12.3 is what ships with CCS v9.2.0:
ENV PATH="${PATH}:/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin"
ENV PATH="${PATH}:/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.4.LTS/bin"
ENV PATH="${PATH}:/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_20.2.4.LTS/bin"

ENV PATH="${PATH}:/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.3.LTS/bin"

# Add TI's CG_XML to PATH:
ENV PATH="${PATH}:/home/iris/cg_xml/bin:/home/iris/cg_xml/utils"

# Pre-compile libraries needed for MSP and Hercules to avoid compile during each build:
# RUN ~/ccs_dir/tools/compiler/ti-cgt-arm_18.12.3.LTS/lib/mklib # <- don't know which pattern filter to use here. This will just get caught by the initial build step later...
USER iris
RUN ~/ccs_dir/tools/compiler/ti-cgt-msp430_18.12.2.LTS/lib/mklib --pattern=rts430x_sc_sd_eabi.lib
RUN ~/ccs_dir/tools/compiler/ti-cgt-msp430_18.12.4.LTS/lib/mklib --pattern=rts430x_sc_sd_eabi.lib
RUN ~/ccs_dir/tools/compiler/ti-cgt-msp430_20.2.4.LTS/lib/mklib --pattern=rts430x_sc_sd_eabi.lib
USER root

# Ensure requisite python libraries are present in default python3 install:
RUN pip3 install lxml rich
ENV PYTHONIOENCODING=utf-8
# Allow `docker` output columns to be wider (need at least 150 for memory allocation view. Going above 180 makes Docker Desktop uncomfortable.)
ENV COLUMNS=185

# Make common scripts executable by all users:
# (NOTE: they have to be copied somewhere outside of the project volume since changes there will get discarded after this exits since nothing is mounted there rn)
USER root
COPY ./docker/setup_project.sh /setup_project.sh
COPY ./docker/list_ccs_configs.sh /list_ccs_configs.sh
COPY ./docker/memory_allocation.py /memory_allocation.py
RUN chmod ugo+x /setup_project.sh /list_ccs_configs.sh

# Default to entry as `iris` user:
USER iris

# ENTRYPOINT []
