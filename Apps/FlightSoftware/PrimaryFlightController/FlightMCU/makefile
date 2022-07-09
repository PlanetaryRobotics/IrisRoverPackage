#!/bin/bash

SHELL = /bin/bash  # specify to not use zsh

include local.env # load env file
export # make everything found in local.env accessible to all `make` operations

.PHONY = help cloc init clean fprime build max-build mb build-flash bf max-build-flash mbf

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
	@echo "	make fprime: Builds the FPrime structure (from XML)."
	@echo " "
	@echo "	make build: Builds the Iris source code (incl. Iris/CubeRover FPrime Apps but does not rebuild the FPrime structure from XML)."
	@echo " "
	@echo "	make flash: Flashes a pre-built binary onto the Flight MCU."
	@echo " "
	@echo " "
	@echo "-- Combined Operations --"
	@echo " "
	@echo "	make max-build: Builds everything. Calls make fprime then make build."
	@echo " "
	@echo "	make build-flash: Calls make build then make flash."
	@echo " "
	@echo "	make max-build-flash: Calls make max-build then make flash."
	@echo " "
	@echo " "
	@echo "-- Shorthand Commands --"
	@echo " "
	@echo "	make mb: Alias for make max-build."
	@echo " "
	@echo "	make bf: Alias for make build-flash."
	@echo " "
	@echo "	make mbf: Alias for make max-build-flash."
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
	# Make all scripts executable:
	chmod -R +x scripts

	# Cleanup first:
	make clean

####
# Teardown and Cleanup the Development Environment:
####
clean:
	@echo "> TODO."
	# Fprime cleaning stuff

####
# (Re)Builds the FPrime Structure (from XML):
####
fprime:
	@echo "> TODO."

####
# (Re)Builds the Iris source code (incl. Iris/CubeRover FPrime Apps but does
# not rebuild the FPrime structure from XML).
####
build:
	./scripts/build.sh

####
# Flashes a pre-built binary onto the Flight MCU.
####
flash:
	@echo "> TODO."

####
# Combined Operations and Shorthand Aliases:
####
max-build:
	make fprime
	make build

mb:
	make max-build

build-flash:
	make build
	make flash

bf:
	make build-flash

max-build-flash:
	make max-build
	make flash

mbf:
	make max-build-flash