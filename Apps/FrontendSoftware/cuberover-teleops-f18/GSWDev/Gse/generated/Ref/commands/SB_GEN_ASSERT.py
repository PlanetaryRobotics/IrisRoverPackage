'''
Created on Saturday, 23 March 2019
@author: raewynd

THIS FILE IS AUTOMATICALLY GENERATED - DO NOT EDIT!!!

XML Source: /home/raewynd/cmu_lunar_robotics/cuberover-teleops-f18/GSWDev/Ref/SendBuffApp/SendBuffComponentAi.xml
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

from models.common import command
# Each file represents the information for a single command
# These module variables are used to instance the command object within the Gse


COMPONENT = "Ref::SendBuff"

MNEMONIC = "SB_GEN_ASSERT"

OP_CODE  = 0x54

CMD_DESCRIPTION = "Generate an ASSERT"

# Set arguments list with default values here.
ARGUMENTS = [
    ("arg1","First ASSERT Argument",U32Type()), 
    ("arg2","Second ASSERT Argument",U32Type()), 
    ("arg3","Third ASSERT Argument",U32Type()), 
    ("arg4","Fourth ASSERT Argument",U32Type()), 
    ("arg5","Fifth ASSERT Argument",U32Type()), 
    ("arg6","Sixth ASSERT Argument",U32Type()), 
    ]

if __name__ == '__main__':
    testcmd = command.Command(COMPONENT, MNEMONIC, OP_CODE, CMD_DESCRIPTION, ARGUMENTS)
    data = testcmd.serialize()
    type_base.showBytes(data)
