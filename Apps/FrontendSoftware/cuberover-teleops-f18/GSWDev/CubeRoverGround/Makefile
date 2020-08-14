# Makefile to run global make.
DEPLOYMENT := CubeRoverGround
BUILD_ROOT ?= $(subst /$(DEPLOYMENT),,$(CURDIR))

export BUILD_ROOT

default: LINUX dict_install

include $(BUILD_ROOT)/mk/makefiles/deployment_makefile.mk

