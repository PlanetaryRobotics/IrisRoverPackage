'''
Created on Saturday, 23 March 2019
@author: raewynd

THIS FILE IS AUTOMATICALLY GENERATED - DO NOT EDIT!!!

XML Source: /home/raewynd/cmu_lunar_robotics/cuberover-teleops-f18/GSWDev/Svc/ActiveLogger/ActiveLoggerComponentAi.xml
'''

# Import the types this way so they do not need prefixing for execution.
from models.serialize.type_exceptions import *
from models.serialize.type_base import *

from models.serialize.bool_type import *
from models.serialize.enum_type import *
from models.serialize.f32_type import *
from models.serialize.f64_type import *

from models.serialize.u8_type import *
from models.serialize.u16_type import *
from models.serialize.u32_type import *
from models.serialize.u64_type import *

from models.serialize.i8_type import *
from models.serialize.i16_type import *
from models.serialize.i32_type import *
from models.serialize.i64_type import *

from models.serialize.string_type import *
from models.serialize.serializable_type import *

from models.common import event
# Each file represents the information for a single event
# These module variables are used to instance the event object within the Gse


COMPONENT = "Svc::ActiveLogger"

NAME = "ALOG_FILE_WRITE_ERR"
ID  = 0x1a5
SEVERITY = "WARNING_HI"
FORMAT_STRING = "Event buffer write failed in stage %s with error %d"
EVENT_DESCRIPTION = "Failed to write circular buffer"

# Set arguments list with default values here.
ARGUMENTS = [
    ("stage","The write stage",EnumType("LogWriteError",{"LOG_WRITE_OPEN":0,"LOG_WRITE_FATAL_DELIMETER":1,"LOG_WRITE_FATAL_RECORD":2,"LOG_WRITE_WARNING_HI_DELIMETER":3,"LOG_WRITE_WARNING_HI_RECORD":4,"LOG_WRITE_WARNING_LO_DELIMETER":5,"LOG_WRITE_WARNING_LO_RECORD":6,"LOG_WRITE_COMMAND_DELIMETER":7,"LOG_WRITE_COMMAND_RECORD":8,"LOG_WRITE_ACTIVITY_HI_DELIMETER":9,"LOG_WRITE_ACTIVITY_HI_RECORD":10,"LOG_WRITE_ACTIVITY_LO_DELIMETER":11,"LOG_WRITE_ACTIVITY_LO_RECORD":12,"LOG_WRITE_DIAGNOSTIC_DELIMETER":13,"LOG_WRITE_DIAGNOSTIC_RECORD":14,})), 
    ("error","The error code",I32Type()), 
    ]
