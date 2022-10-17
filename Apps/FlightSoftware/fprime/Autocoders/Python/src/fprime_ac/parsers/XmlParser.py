#!/bin/env python
#===============================================================================
# NAME: XmlParser.py
#
# DESCRIPTION:  This parses the XML component description files.
#
# USAGE:
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Jul. 16, 2013
#
# Copyright 2007, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules
#
import logging
import os
import sys
import time
from optparse import OptionParser
from lxml import etree

#
# Python extention modules and custom interfaces
#

#
# Universal globals used within module go here.
# (DO NOT USE MANY!)
#
# Global logger init. below.
PRINT = logging.getLogger('output')
DEBUG = logging.getLogger('debug')
#
class XmlParser(object):
    def __init__(self, xml_file=None):
        self.__root = None
        #
        if os.path.isfile(xml_file) == False:
            stri = "ERROR: Could not find specified XML file {}.".format(xml_file)
            PRINT.info(stri)
            raise IOError(stri)

        fd = open(xml_file,'r')

        element_tree = etree.parse(fd)
        self.__root = element_tree.getroot().tag

        fd.close()

    def __call__(self):
        """
        """
        return self.__root


if __name__ == '__main__':
    pass
