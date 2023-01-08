#!/bin/bash

PYTHON ?= python3
PYTHONVERSION ?= 3.10.8
VENV_NAME ?= py_irisfrontendbil

.PHONY = help init update clean build-bil cloc

.DEFAULT_GOAL = help
all: help

help:
	@echo "---- Iris Frontend BIL Makefile ----"
	@echo "	make help: Information"
	@echo " "
	@echo "	make init: Clean and resetup the project on a *new* system"
	@echo "	make update: Install any new dependencies and update existing dependencies"
	@echo " "
	@echo "	make clean: Teardown and Cleanup the Development Environment"
	@echo " "
	@echo "	make build-bil: Build the BIL into a server executable."
	@echo " "
	@echo "	make cloc: Count Lines Of Code (and comments) by language, excluding builds and libraries."
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
	env PYTHON_CONFIGURE_OPTS="--enable-shared"; \
	if [ $$pyenv_version != "${PYTHONVERSION}" ]; then pyenv install ${PYTHONVERSION}; fi; \
	pyenv virtualenv ${PYTHONVERSION} ${VENV_NAME}; \
	pyenv local ${VENV_NAME}; \
	echo "$$(pyenv versions)"; \
	pyenv activate ${VENV_NAME};

	# Make all scripts executable:
	chmod +x ./setup.sh
	chmod +x ./dev-setup.sh
	chmod +x ./build_py_protos.sh
	chmod +x ./build_js_protos.sh

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
# Builds the BIL:
####
build-bil:
	set -x
	make update
	./dev-setup.sh
	./build_py_protos.sh
	./build_js_protos.sh
	./build_cs_protos.sh
	set +x

####
# Count Lines of Code (for fun):
####
cloc:
	cloc --exclude-dir=__pycache__,node_modules,.idea,.mypy_cache,.pytest_cache .