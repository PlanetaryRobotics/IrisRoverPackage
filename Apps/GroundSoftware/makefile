#!/bin/bash

PYTHON ?= python
PYTHONVERSION ?= 3.10.8
VENV_NAME ?= py_irisbackendv3

.PHONY = help init update clean run run-infra run-tests proxies standards build-docs run-docs build-cte data data-image console xconsole console-debug xconsole-debug xterm-normal xterm-wide port-investigator port-culler cloc xcvr-slip xcvr-wifi xcvr app-commander app-telemetry-display app-message-printer app-packet-bytes-printer app-gui

.DEFAULT_GOAL = help
all: help

help:
	@echo "---- Iris Backend v3 Makefile ----"
	@echo "	make help: Information"
	@echo " "
	@echo "	make init: Clean and resetup the project on a *new* system"
	@echo " "
	@echo "	make update: Install any new dependencies and update existing dependencies"
	@echo " "
	@echo "	make clean: Teardown and Cleanup the Development Environment"
	@echo " "
	@echo "	make run: Rebuild DataStandards and bring up all core IPC Apps"
	@echo " "
	@echo "	make run-infra: Rebuild DataStandards and bring up just core infrastructure."
	@echo " "
	@echo "	make run-tests: Run all test suites and produce coverage report"
	@echo " "
	@echo "	make proxies: Bring up all topic proxies for all IPC apps."
	@echo " "
	@echo "	make standards: Rebuild DataStandards and cache them"
	@echo " "
	@echo "	make build-docs: Build code documentation (WIP)"
	@echo " "
	@echo "	make run-docs: Spins up an interactive documentation server (WIP)"
	@echo " "
	@echo "	make build-cte: Build the Commands, Telemetry, & Events (CT&E) Table. Run as: 'make build-cte FILE=your_file_name'."
	@echo " "
	@echo "	make data: Play an infinitely looping recording of rover test data from a PCAP into IPC."
	@echo " "
	@echo "	make data-image: Play a fixed non-looping recording recording of rover test data from a PCAP into IPC, that starts with downlink of image(s)."
	@echo " "
	@echo "	make console: Run the Iris Console CLI application"
	@echo " "
	@echo "	make xconsole: Run the Iris Console CLI application in XTerm (for WSL users)"
	@echo " "
	@echo "	make console-debug: Run the Iris Console CLI application for the debug harness (J36_debugger)."
	@echo " "
	@echo "	make xconsole-debug: Run the Iris Console CLI application for the debug harness (J36_debugger) in XTerm (for WSL users)"
	@echo " "
	@echo "	make xterm-normal: Launches the program in the PROG variable inside a new XTerm window with normal settings and the geometry given in GEOM. Run as: 'make xterm-normal GEOM=\"300x100+0+0\" PROG=\"./run-script.sh whatever.py\"'."
	@echo " "
	@echo "	make xterm-wide: Launches the program in the PROG variable inside a new XTerm window with wide settings and the geometry given in GEOM. Run as: 'make xterm-wide GEOM=\"300x100+0+0\" PROG=\"./run-script.sh whatever.py\"'."
	@echo " "
	@echo "	make port-investigator: Finds out who's using all GDS IPC ports."
	@echo " "
	@echo "	make port-culler: Kills every process using a GDS IPC port (kills any zombie processes)."
	@echo " "
	@echo "	make cloc: Count Lines Of Code (and comments) by language, excluding builds and libraries"
	@echo " "
	@echo "	make xcvr-slip: Bring up a UDP/SLIP transceiver (for wired RS422 communication with Rover)."
	@echo " "
	@echo "	make xcvr-wifi: Bring up a WIFI transceiver (for wireless communication with Rover)."
	@echo " "
	@echo "	make xcvr: Bring up all transceivers in parallel. Any transceivers that can connect to the rover will forward data to it in both directions."
	@echo " "
	@echo "	make app-commander: Run the Commander IPC App."
	@echo " "
	@echo "	make app-telemetry-display: Run the TelemetryDisplay IPC App."
	@echo " "
	@echo "	make app-message-printer: Run the MessagePrinter IPC App."
	@echo " "
	@echo "	make app-packet-bytes-printer: Run the PacketBytesPrinter IPC App."
	@echo " "
	@echo "	make app-gui: Bring up the server for the GDS GUI (viewed in a browser)."
	@echo "----------------------------------"


####
# Setup the Development Environment:
####
init:
	# Cleanup first:
	make clean
	
	# Update `pyenv`:
	pyenv install ${PYTHONVERSION};

	# Install the appropriate python version:
	eval "$$(pyenv init -)";

	# Load the correct version of python and setup the virtual environment:
	eval "$$(pyenv init -)"; \
	eval "$$(pyenv virtualenv-init -)"; \
	pyenv local ${PYTHONVERSION}; \
	pyenv_version="$$(pyenv version-name)"; \
	if [ $$pyenv_version != "${PYTHONVERSION}" ]; then pyenv install ${PYTHONVERSION}; fi; \
	pyenv virtualenv ${PYTHONVERSION} ${VENV_NAME}; \
	pyenv local ${VENV_NAME}; \
	echo "$$(pyenv versions)"; \
	pyenv activate ${VENV_NAME};

	# Make all scripts executable:
	chmod +x ./__dev-setup.sh
	chmod +x ./run-script.sh
	
	# Install pip dependencies:
	make update
	
####
# Install any new dependencies and update existing dependencies:
####
update:
	pyenv exec ${PYTHON} -m pip install -r requirements.txt;

####
# Teardown and Cleanup the Development Environment:
####
clean:
	# Remove virtual environment:
	eval "$$(pyenv init -)"; \
	eval "$$(pyenv virtualenv-init -)"; \
	pyenv deactivate; \
	pyenv local system; \
	pyenv local ${VENV_NAME}; \
	pyenv_version="$$(pyenv version-name)"; \
	if [ $$pyenv_version == "${VENV_NAME}" ]; then yes | pyenv virtualenv-delete ${VENV_NAME}; fi
	pyenv local system;

####
# Rebuild DataStandards and bring up all core IPC Apps:
####
run:
# Rebuild latest datastandards and bring up infrastructure:
	make run-infra
# Bring up visualizers:
	make xterm-normal \
		GEOM="185x63+0+0" \
		PROG="make app-telemetry-display"
	make xterm-normal \
		GEOM="185x10+0+800" \
		PROG="make app-message-printer"
	make xterm-normal \
		GEOM="73x73+1000+0" \
		PROG="make app-packet-bytes-printer"
# Bring up commander:
	make xterm-normal \
		GEOM="185x63+0+0" \
		PROG="make app-commander"

####
# Rebuild DataStandards and bring up just core infrastructure:
####
run-infra:
# Rebuild latest datastandards first so every app has access via cache:
	make standards
# Bring up proxies (hide this window at the back of the stack):
	make xterm-normal \
		GEOM="150x10+0+0" \
		PROG="make proxies"
# Start the processors:
	make xterm-normal \
		GEOM="150x10+0+205" \
		PROG="./run-script.sh ipc_apps/dl_processor.py"
# Bring up a Redis Stack Server
# (only needed for a subset of apps like GDS-GUI, if you're not using any
# redis-dependent apps, you won't need redis installed, in which case this will
# fail which is fine):
	make xterm-normal \
		GEOM="150x10+0+410" \
		PROG="redis-stack-server"

####
# Run all test suites:
####
run-tests:
# Run all Pytest Tests, Docstrings, and check for coverage in IrisBackendv3:
	./run-script.sh -m pytest --cov=IrisBackendv3

####
# Bring up all topic proxies for all IPC apps.
####
proxies:
# Run all Pytest Tests, Docstrings, and check for coverage in IrisBackendv3:
	./run-script.sh ./ipc_apps/proxies.py

####
# Rebuild DataStandards and cache them:
####
standards:
	./run-script.sh ./scripts/tiny_apps/datastandards_build_cache.py -l NOTICE

####
# Build code documentation:
####
build-docs:
	./run-script.sh -m pdoc -o ./out/docs/ IrisBackendv3 --force

####
# Spins up an interactive documentation server (WIP):
####
run-docs:
	./run-script.sh -m pdoc --http localhost:9191 IrisBackendv3

####
# Build the Commands, Telemetry, & Events (CT&E) Table:
# Run as: `make build-cte FILE=your_file_name`
####
build-cte:
	./run-script.sh ./scripts/tiny_apps/datastandards_lookup.py -f $(FILE)

####
# Play an infinitely looping recording of rover test data from a PCAP into IPC.
####
data:
	./run-script.sh ./scripts/testing/ipc_demo/pcap_xcvr.py --loop

####
# Play a fixed non-looping recording of rover test data from a PCAP into IPC,
# that starts with downlink of image(s).
#
# NOTE: `-t 80` (milliseconds) is approximately accurate to real life image
# 		downlink (~17700 packets in ~24minutes).
####
data-image:
	./run-script.sh scripts/testing/ipc_demo/pcap_xcvr.py \
		-t 15 -g 0 --no-loop \
		-f test-data/img_rc9.5.3_em4_flight_lens_pocari_4__v4_cam_A_short_cable__sunlamp_pointed_up.pcap
#		-f test-data/img_rc9.5.3_em4_flight_lens_pocari_2__v4_cam_A_short_cable.pcap
#		-f test-data/img_rc9.5.3_v3_test_lens_flight_lens_pocari_2.pcap
#		-f test-data/img_rc9.5.3_flight_lens_full_good_psu_try2__after_seamless_w_old_lens__no_harness_demate_just_lens_swap.pcap
#		-f test-data/img_rc9.5.3_flight_lens_mount_but_old_lens__4000_middle_packets.pcap
# 		-f test-data/img_rc9.5.3_flight_lens_full_good_psu.pcap
#		-f test-data/img_rc9.5.3_flight_lens_reboot_at_14000_packets.pcap
# 		-f test-data/img_rc9.5.3_flight_lens_full_good_psu.pcap
#		-f test-data/img_rc9.5.3_sbc4_cam1_field_of_irises_6.7_only_laptop_lighting_dim__12mm_lens_sep__shorted_ferrite.pcap

####
# Run the Iris Console CLI application:
####
console:
	./run-script.sh __run_console.py

####
# Run the Iris Console CLI application in XTerm (for WSL users):
####
xconsole:
	make xterm-wide GEOM="300x100+0+0" PROG="make console"

####
# Run the Iris Console CLI application for the debug harness (J36_debugger):
####
console-debug:
	./run-script.sh __run_console.py -s J36_debugger

####
# Run the Iris Console CLI application for the debug harness (J36_debugger) in XTerm (for WSL users):
####
xconsole-debug:
	make xterm-wide GEOM="300x100+0+0" PROG="make console-debug"

####
# Launches the program in the PROG variable inside a new XTerm window with 
# normal settings and the geometry given in GEOM.
# Run as: 'make xterm-normal GEOM="300x100+0+0" PROG="./run-script.sh whatever.py"'
####
xterm-normal:
	xterm \
	-xrm "XTerm*Background: Grey3" \
	-xrm "XTerm*Foreground: white" \
	-xrm "XTerm*faceName: DejaVu Sans Mono:size=11:antialias=true:hinting=true" \
	-xrm "XTerm*font: 6x13bold" \
	-xrm "XTerm*saveLines: 5000" \
	-xrm "XTerm*HiForeColor: white" \
	-xrm "XTerm*HiBackColor: Grey15" \
	-xrm "XTerm*scrollTtyOutput: false" \
	-xrm "XTerm*geometry: $(GEOM)" \
	-hold -e $(PROG) &

####
# Launches the program in the PROG variable inside a new XTerm window with wide
# settings and the geometry given in GEOM.
# Run as: 'make xterm-wide GEOM="300x100+0+0" PROG="./run-script.sh whatever.py"'
####
xterm-wide:
	xterm \
	-xrm "XTerm*Background: Grey3" \
	-xrm "XTerm*Foreground: white" \
	-xrm "XTerm*faceName: DejaVu Sans Mono:size=8:antialias=true:hinting=true" \
	-xrm "XTerm*font: 6x10" \
	-xrm "XTerm*saveLines: 500" \
	-xrm "XTerm*HiForeColor: white" \
	-xrm "XTerm*HiBackColor: Grey15" \
	-xrm "XTerm*geometry: $(GEOM)" \
	-hold -e $(PROG) &

####
# Finds out who's using all GDS IPC ports:
####
port-investigator:
	./run-script.sh ./scripts/find_port_user.py

####
# Kills every process using a GDS IPC port (kills any zombie processes):
####
port-culler:
	./run-script.sh ./scripts/find_port_user.py --kill

####
# Count Lines of Code (for fun):
####
cloc:
	cloc --exclude-dir=__pycache__,node_modules,.idea,.mypy_cache,.pytest_cache .



	@echo "	make xcvr-slip: "
	@echo " "
	@echo "	make xcvr-wifi: "
	@echo " "
	@echo "	make xcvr: "
	@echo " "

####
# Bring up a UDP/SLIP transceiver (for wired RS422 communication with Rover):
####
xcvr-slip:
	echo "Not implemented yet."

####
# Bring up a WIFI transceiver (for wireless communication with Rover):
####
xcvr-wifi:
	./run-script.sh IrisBackendv3/transceiver/ipc_process.py -x WIFI

####
# Bring up all transceivers in parallel. Any transceivers that can connect to the rover will forward data to it in both directions:
####
xcvr:
	./run-script.sh IrisBackendv3/transceiver/ipc_process.py -x WIFI

####
# Run the Commander IPC App:
####
app-commander:
	./run-script.sh ipc_apps/commander.py

####
# Run the TelemetryDisplay IPC App:
####
app-telemetry-display:
	./run-script.sh ipc_apps/telemetry_display.py

####
# Run the MessagePrinter IPC App:
####
app-message-printer:
	./run-script.sh ipc_apps/packet_printer.py -n MessagePrinter --no-full-packets --message-packets --events --no-packet-bytes

####
# Run the PacketBytesPrinter IPC App:
####
app-packet-bytes-printer:
	./run-script.sh ipc_apps/packet_printer.py -n PacketBytesPrinter --no-full-packets --packet-bytes --no-message-packets --no-events

####
# Bring up the server for the GDS GUI (viewed in a browser):
####
app-gui:
	./run-script.sh ipc_apps/gui.py
