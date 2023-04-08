#!/bin/bash

PYTHON ?= python
PYTHONVERSION ?= 3.10.8
VENV_NAME ?= py_irisbackendv3

.PHONY = help init update clean run run-tests standards build-docs run-docs build-cte commander console xconsole xterm-normal xterm-wide cloc

.DEFAULT_GOAL = help
all: help

help:
	@echo "---- Iris Backend v3 Makefile ----"
	@echo "	make help: Information"
	@echo " "
	@echo "	make init: Clean and resetup the project on a *new* system"
	@echo "	make update: Install any new dependencies and update existing dependencies"
	@echo " "
	@echo "	make clean: Teardown and Cleanup the Development Environment"
	@echo " "
	@echo "	make run: Rebuild DataStandards and bring up all core IPC Apps"
	@echo " "
	@echo "	make run-tests: Run all test suites and produce coverage report"
	@echo " "
	@echo "	make standards: Rebuild DataStandards and cache them"
	@echo " "
	@echo "	make build-docs: Build code documentation (WIP)"
	@echo " "
	@echo "	make run-docs: Spins up an interactive documentation server (WIP)"
	@echo " "
	@echo "	make build-cte: Build the Commands, Telemetry, & Events (CT&E) Table. Run as: 'make build-cte FILE=your_file_name'."
	@echo " "
	@echo "	make commander: Run the Commander IPC App."
	@echo " "
	@echo "	make console: Run the Iris Console CLI application"
	@echo " "
	@echo "	make xconsole: Run the Iris Console CLI application in XTerm (for WSL users)"
	@echo " "
	@echo "	make xterm-normal: Launches the program in the PROG variable inside a new XTerm window with normal settings and the geometry given in GEOM. Run as: 'make xterm-normal GEOM=\"300x100+0+0\" PROG=\"./run-script.sh whatever.py\"'."
	@echo " "
	@echo "	make xterm-wide: Launches the program in the PROG variable inside a new XTerm window with wide settings and the geometry given in GEOM. Run as: 'make xterm-wide GEOM=\"300x100+0+0\" PROG=\"./run-script.sh whatever.py\"'."
	@echo " "
	@echo "	make cloc: Count Lines Of Code (and comments) by language, excluding builds and libraries"
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
# Rebuild latest datastandards first so every app has access via cache:
	make standards

	make xterm-normal \
		GEOM="185x63+0+0" \
		PROG="./run-script.sh ipc_apps/telemetry_display.py"
	make xterm-normal \
		GEOM="185x10+0+800" \
		PROG="./run-script.sh ipc_apps/packet_printer.py -n MessagePrinter --no-full-packets --message-packets --events --no-packet-bytes"
	make xterm-normal \
		GEOM="73x73+1000+0" \
		PROG="./run-script.sh ipc_apps/packet_printer.py -n PacketBytesPrinter --no-full-packets --packet-bytes --no-message-packets --no-events"
#	./run-script.sh ./ipc-apps/packet-printer.py

####
# Run all test suites:
####
run-tests:
# Run all Pytest Tests, Docstrings, and check for coverage in IrisBackendv3:
	./run-script.sh -m pytest --cov=IrisBackendv3

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
# Run the Commander IPC App:
####
commander:
	./run-script.sh ipc_apps/commander.py

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
# Count Lines of Code (for fun):
####
cloc:
	cloc --exclude-dir=__pycache__,node_modules,.idea,.mypy_cache,.pytest_cache .