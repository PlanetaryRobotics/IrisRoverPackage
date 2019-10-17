# The MODULE_DIR path should always be the current directory of the Makefile.
MODULE_DIR = CubeRover/MathReceiver
MODULE = $(subst /,,$(MODULE_DIR))

BUILD_ROOT ?= $(subst /$(MODULE_DIR),,$(CURDIR))
export BUILD_ROOT

include $(BUILD_ROOT)/mk/makefiles/module_targets.mk