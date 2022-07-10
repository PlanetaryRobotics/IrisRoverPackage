#!/bin/bash

SHELL = /bin/bash  # specify to not use zsh

include local.env # load env file
export # make everything found in local.env accessible to all `make` operations

.PHONY = help cloc init clean build-fprime build-ccs build-all ba build-flash b2f build-all-flash ba2f

.DEFAULT_GOAL = help
all: help

help:
	@echo " "
	@echo "---- Iris Hercules Makefile ----"
	@echo "(Primary Flight Microcontroller)"
	@echo " "
	@echo "-- Primary Operations --"
	@echo " "
	@echo "	make help: Information"
	@echo " "
	@echo "	make cloc: Count Lines Of Code (and comments) by language, excluding builds and libraries."
	@echo " "
	@echo "	make init: Clean and resetup the project on a *new* system."
	@echo " "
	@echo "	make clean: Teardown and Cleanup the Development Environment."
	@echo " "
	@echo "	make build-fprime: Builds the FPrime structure (from XML)."
	@echo " "
	@echo "	make build-ccs: Builds the Iris source code in the TI CCS project (incl. Iris/CubeRover FPrime Apps but does not rebuild the FPrime structure from XML)."
	@echo " "
	@echo "	make flash: Flashes a pre-built binary onto the Flight MCU."
	@echo " "
	@echo " "
	@echo "-- Combined Operations --"
	@echo " "
	@echo "	make build-all: Builds everything. Calls build-fprime then build-ccs."
	@echo " "
	@echo "	make build-flash: Calls build-ccs then flash (for when only CCS code and not FPrime XML has changed)."
	@echo " "
	@echo "	make build-all-flash: Calls build-all then flash (for when any of the code could have changed, incl. FPrime XML)."
	@echo " "
	@echo " "
	@echo "-- Shorthand Commands --"
	@echo " "
	@echo "	make ba: Alias for make build-all."
	@echo " "
	@echo "	make b2f: Build to flash. Alias for make build-flash. Not bf so it's clear that this isn't an alias for build-fprime."
	@echo " "
	@echo "	make ba2f: Build all to flash. Alias for make build-all-flash."
	@echo "----------------------------------"

####
# Count Lines of Code (for fun):
####
cloc:
	cloc --exclude-dir=.jxbrowser-data,HAL,Debug,.launches,.metadata --exclude-ext=html,htm . ../../fprime/CubeRover

####
# Setup the Development Environment:
####
init:
	# Make sure nothing is left lingering around from a previous instance:
	make clean

	# Make all scripts executable:
	chmod -R +x scripts

	# Install python requirements:
	python3 -m pip install -r flash_tool_requirements.txt

####
# Teardown and Cleanup the Development Environment:
####
clean:
	@echo "> TODO."
	# Fprime cleaning stuff

	# Destroy any left over CCS workspace:
	./scripts/ccs_temp_workspace__destroy.sh

	# Clean up all build files:
	./scripts/ccs_project__clean.sh

####
# (Re)Builds the FPrime Structure (from XML):
####
build-fprime:
	@echo "> TODO."

####
# (Re)Builds the Iris source code in the CCS project (incl. Iris/CubeRover
# FPrime Apps but does not rebuild the FPrime structure from XML).
####
build-ccs:
	./scripts/ccs_project__build.sh

####
# Flashes a pre-built binary onto the Flight MCU.
####
flash:
	python3 ./scripts/ccs_project__flash.py

####
# Combined Operations and Shorthand Aliases:
####
build-all:
	make build-fprime
	make build-ccs
	# TODO: redirect fprime and CCS build logs to their own files.

ba:
	make build-all

build-flash:
	make build-ccs
	make flash

b2f:
	make build-flash

build-all-flash:
	make build-all
	make flash

ba2f:
	make build-all-flash

# TODO: Add pre-commit and post-pull hooks