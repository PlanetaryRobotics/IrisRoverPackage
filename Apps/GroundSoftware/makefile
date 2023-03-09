#!/bin/bash

PYTHON ?= python
PYTHONVERSION ?= 3.10.8
VENV_NAME ?= py_irisbackendv3

.PHONY = help init update clean run run-tests standards build-docs run-docs build-cte console xconsole cloc

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
	@echo "	make build-cte: Build the Commands, Telemetry, & Events (CT&E) Table. Run as: 'make build-cte FILE=your_file_name'"
	@echo " "
	@echo "	make console: Run the Iris Console CLI application"
	@echo " "
	@echo "	make xconsole: Run the Iris Console CLI application in XTerm (for WSL users)"
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
# Run the Iris Console CLI application:
####
console:
	./run-script.sh __run_console.py

####
# Run the Iris Console CLI application in XTerm (for WSL users):
####
xconsole:
	xterm -hold -e make console &

####
# Count Lines of Code (for fun):
####
cloc:
	cloc --exclude-dir=__pycache__,node_modules,.idea,.mypy_cache,.pytest_cache .