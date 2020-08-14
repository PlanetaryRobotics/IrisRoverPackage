#!/usr/bin/env python




##################################################
## DEPENDENCIES
import sys
import os
import os.path
try:
    import builtins as builtin
except ImportError:
    import __builtin__ as builtin
from os.path import getmtime, exists
import time
import types
from Cheetah.Version import MinCompatibleVersion as RequiredCheetahVersion
from Cheetah.Version import MinCompatibleVersionTuple as RequiredCheetahVersionTuple
from Cheetah.Template import Template
from Cheetah.DummyTransaction import *
from Cheetah.NameMapper import NotFound, valueForName, valueFromSearchList, valueFromFrameOrSearchList
from Cheetah.CacheRegion import CacheRegion
import Cheetah.Filters as Filters
import Cheetah.ErrorCatchers as ErrorCatchers

##################################################
## MODULE CONSTANTS
VFFSL=valueFromFrameOrSearchList
VFSL=valueFromSearchList
VFN=valueForName
currentTime=time.time
__CHEETAH_version__ = '2.4.4'
__CHEETAH_versionTuple__ = (2, 4, 4, 'development', 0)
__CHEETAH_genTime__ = 1469638650.528505
__CHEETAH_genTimestamp__ = 'Wed Jul 27 09:57:30 2016'
__CHEETAH_src__ = 'Autocoders/utils/NoseTests/Template/test_template.tmpl'
__CHEETAH_srcLastModified__ = 'Thu Jul  7 11:03:19 2016'
__CHEETAH_docstring__ = 'Autogenerated by Cheetah: The Python-Powered Template Engine'

if __CHEETAH_versionTuple__ < RequiredCheetahVersionTuple:
    raise AssertionError(
      'This template was compiled with Cheetah version'
      ' %s. Templates compiled before version %s must be recompiled.'%(
         __CHEETAH_version__, RequiredCheetahVersion))

##################################################
## CLASSES

class test_template(Template):

    ##################################################
    ## CHEETAH GENERATED METHODS


    def __init__(self, *args, **KWs):

        super(test_template, self).__init__(*args, **KWs)
        if not self._CHEETAH__instanceInitialized:
            cheetahKWArgs = {}
            allowedKWs = 'searchList namespaces filter filtersLib errorCatcher'.split()
            for k,v in KWs.items():
                if k in allowedKWs: cheetahKWArgs[k] = v
            self._initCheetahInstance(**cheetahKWArgs)
        

    def respond(self, trans=None):



        ## CHEETAH: main method generated for this template
        if (not trans and not self._CHEETAH__isBuffering and not callable(self.transaction)):
            trans = self.transaction # is None unless self.awake() was called
        if not trans:
            trans = DummyTransaction()
            _dummyTrans = True
        else: _dummyTrans = False
        write = trans.response().write
        SL = self._CHEETAH__searchList
        _filter = self._CHEETAH__currentFilter
        
        ########################################
        ## START - generated method body
        
        write(u"""import os

import subprocess
from subprocess import CalledProcessError
import pexpect
from pexpect import TIMEOUT, EOF 
import logging
    
def a_make_test():
    logging.debug('Checking make')

    if make():
        logging.debug('make: success')
        assert True
    else:
        logging.debug('make: failure')
        assert False

def b_make_ut_test():
    logging.debug('Checking make ut')

    if make_ut():
        logging.debug('make ut: success')
        assert True
    else:
        logging.debug('make ut: failure')
        assert False

## test methods are collected alphabetically. When adding new test methods
## define method names according to current order
## Ex:
## a_make_test() <--- Default Test
## b_make_ut()   <--- Default Test
## c_test()
## d_new_test()

""")
        ascii_val = 98
        for test_case in VFFSL(SL,"TestModuleValues",True)['test_cases']: # generated from line 38, col 1
            # Increment character
            ascii_val = VFFSL(SL,"ascii_val",True) + 1
            char = chr(VFFSL(SL,"ascii_val",True))
            write(u'''def ''')
            _v = VFFSL(SL,"char",True) # u'${char}' on line 42, col 5
            if _v is not None: write(_filter(_v, rawExpr=u'${char}')) # from line 42, col 5.
            write(u'''_''')
            _v = VFFSL(SL,"test_case.name",True) # u'${test_case.name}' on line 42, col 13
            if _v is not None: write(_filter(_v, rawExpr=u'${test_case.name}')) # from line 42, col 13.
            write(u'''_test():
    logging.debug("Entering ''')
            _v = VFFSL(SL,"char",True) # u'${char}' on line 43, col 29
            if _v is not None: write(_filter(_v, rawExpr=u'${char}')) # from line 43, col 29.
            write(u'''_''')
            _v = VFFSL(SL,"test_case.name",True) # u'${test_case.name}' on line 43, col 37
            if _v is not None: write(_filter(_v, rawExpr=u'${test_case.name}')) # from line 43, col 37.
            write(u'''_test()")
 
    try:
        ## Spawn executable
        p = pexpect.spawn("make run_ut")

        logging.debug(\'`make run_ut` executed.\')
        logging.debug(\'Executable now running.\')

''')
            for pout, psend in VFFSL(SL,"test_case.ptuples",True): # generated from line 52, col 9
                write(u"""        logging.debug('Expecting: """)
                _v = VFFSL(SL,"pout",True) # u'${pout}' on line 53, col 35
                if _v is not None: write(_filter(_v, rawExpr=u'${pout}')) # from line 53, col 35.
                write(u'''\')

        p.expect(".*''')
                _v = VFFSL(SL,"pout",True) # u'${pout}' on line 55, col 21
                if _v is not None: write(_filter(_v, rawExpr=u'${pout}')) # from line 55, col 21.
                write(u'''.*", timeout=3)
        p.sendline("''')
                _v = VFFSL(SL,"psend",True) # u'${psend}' on line 56, col 21
                if _v is not None: write(_filter(_v, rawExpr=u'${psend}')) # from line 56, col 21.
                write(u'''")

        logging.debug(\'Sent: ''')
                _v = VFFSL(SL,"psend",True) # u'${psend}' on line 58, col 30
                if _v is not None: write(_filter(_v, rawExpr=u'${psend}')) # from line 58, col 30.
                write(u"""')
""")
            write(u'''
        logging.debug(\'No timeout. Asserting test as true.\')
        ## If there was no timeout the pexpect test passed
        assert True

        ## A timeout occurs when pexpect cannot match the executable
        ## output with the designated expectation. In this case the
        ## key expectation is p.expect(expect_string, timeout=3)
        ## which tests what the method name describes
    except TIMEOUT as e:
        print "Timeout Error. Expected Value not returned."
        print "-------Program Output-------"
        print p.before
        print "-------Expected Output-------"
        print e.get_trace()
        assert False


''')
        write(u'''
def setup_module():
    appPath = "''')
        _v = VFFSL(SL,"TestModuleValues",True)['module_path'] # u"${TestModuleValues['module_path']}" on line 81, col 16
        if _v is not None: write(_filter(_v, rawExpr=u"${TestModuleValues['module_path']}")) # from line 81, col 16.
        write(u'''"

    logging.basicConfig(level=logging.''')
        _v = VFFSL(SL,"log_level",True) # u'${log_level}' on line 83, col 39
        if _v is not None: write(_filter(_v, rawExpr=u'${log_level}')) # from line 83, col 39.
        write(u''')

    logging.debug(\'Setting up test module.\')
    logging.debug(\'Application path: %s\' % appPath)

    os.chdir(appPath)

    logging.debug(\'Setup make\')
    make()
    logging.debug(\'Setup make ut\')
    make_ut()

def teardown_module():
    logging.debug(\'teardown_module\')

    cleanCmds = [\'make clean\', \'make ut_clean\']
    for cmd in cleanCmds:
        try:
            logging.debug(\'Calling: %s\' % cmd)
            subprocess.check_output(cmd, stderr = subprocess.STDOUT, shell=True)
        except subprocess.CalledProcessError as e:
            print "MAKE CLEAN ERROR"
            print "\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'"
            print e.output
    os.chdir("{BUILD_ROOT}/Autocoders/test".format(BUILD_ROOT=os.environ.get(\'BUILD_ROOT\')))

def make():
    try:
        subprocess.check_output(\'make\', stderr = subprocess.STDOUT, shell=True) 
        return True
    except CalledProcessError as e:
       print "MAKE ERROR"
       print "\'\'\'\'\'\'\'\'\'\'"
       print e.output
       return False

def make_ut():
    try:
        subprocess.check_output(\'make ut\', stderr = subprocess.STDOUT, shell=True)
        return True 
    except CalledProcessError as e:
        print "MAKE UT ERROR"
        print "\'\'\'\'\'\'\'\'\'\'\'\'\'"
        print e.output
        return False

''')
        
        ########################################
        ## END - generated method body
        
        return _dummyTrans and trans.response().getvalue() or ""
        
    ##################################################
    ## CHEETAH GENERATED ATTRIBUTES


    _CHEETAH__instanceInitialized = False

    _CHEETAH_version = __CHEETAH_version__

    _CHEETAH_versionTuple = __CHEETAH_versionTuple__

    _CHEETAH_genTime = __CHEETAH_genTime__

    _CHEETAH_genTimestamp = __CHEETAH_genTimestamp__

    _CHEETAH_src = __CHEETAH_src__

    _CHEETAH_srcLastModified = __CHEETAH_srcLastModified__

    _mainCheetahMethod_for_test_template= 'respond'

## END CLASS DEFINITION

if not hasattr(test_template, '_initCheetahAttributes'):
    templateAPIClass = getattr(test_template, '_CHEETAH_templateClass', Template)
    templateAPIClass._addCheetahPlumbingCodeToClass(test_template)


# CHEETAH was developed by Tavis Rudd and Mike Orr
# with code, advice and input from many other volunteers.
# For more information visit http://www.CheetahTemplate.org/

##################################################
## if run from command line:
if __name__ == '__main__':
    from Cheetah.TemplateCmdLineIface import CmdLineIface
    CmdLineIface(templateObj=test_template()).run()


