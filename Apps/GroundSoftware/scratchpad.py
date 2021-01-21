from lxml import etree
import os.path
import pickle
import jsonpickle
from typing import List, Dict

from IrisBackendv3.utils import NameIdDict
from IrisBackendv3.data_standards import FswDataType
from IrisBackendv3.data_standards.module import Argument, Command

arg1 = Argument('amount', FswDataType.U8)
arg2 = Argument('speed', FswDataType.U8)
arg3 = Argument('callback', FswDataType.U16)

ea = jsonpickle.encode(arg1, keys=True)
da = jsonpickle.decode(ea)

print(ea)
print(arg1 == da)

ids: List[int] = [0x03, 0x01, 0x02]
names: List[str] = ['reset', 'mover', 'image']
vals: List[Argument] = [arg3, arg1, arg2]

commands: NameIdDict[Argument] = NameIdDict(zip(zip(ids, names), vals))
commands[0x01, 'move'] = commands[0x01]

print(commands)

json = jsonpickle.encode(commands, keys=True, indent=2)
# print(json)
with open('scratchpad.json', 'w', encoding='utf-8') as file:
    print(json, file=file)

cmds2 = jsonpickle.decode(json)
print(cmds2)

print(cmds2 == commands)


# Test CDATA Extraction:
test_file = './test-data/SampleFPrimeXml.xml'
tree_topology = etree.parse(test_file)

command_tree = tree_topology.xpath("/component/commands/command")
command_src = command_tree[1]
arg_tree = command_src.xpath("args/arg")
arg_src = arg_tree[0]
print(arg_src)


#
#
#
# Inputs:
search_dir: str = '../FlightSoftware/fprime'
cache_dir: str = './DataStandardsCache'

# Settings:
# Relative Locations of Import Definition Files and Directories w.r.t. `search_dir`:
uri_topology = './CubeRover/Top/CubeRoverTopologyAppAi.xml'

###
# Important Tree XPath Selectors:
###
# Select all file imports:
xpath_imports = "/assembly/import_component_type/text()"

# Grab a component's name from file:
xpath_component_name = "/component/@name"
# Grab component's corresponding ID from topology:
xpath_component_id = "/assembly/instance[@type= '{name}']/@base_id"

# Grab component commands:
xpath_commands = "/component/commands/command"

# Grab component telemetry channels:
xpath_telemetry = "/component/telemetry/channel"

# Grab component events:
xpath_events = "/component/events/event"

tree_topology = etree.parse(os.path.join(search_dir, uri_topology))

# print(etree.tostring(tree_topology))

xpath_imports = "/assembly/import_component_type/text()"

topology_imports = tree_topology.xpath(xpath_imports)

print(topology_imports)

uri = topology_imports[-1]
tree = etree.parse(os.path.join(search_dir, uri))

name = tree.xpath(xpath_component_name)[0]
print(name)

ID_selector = xpath_component_id.format(name=name)
ID = tree_topology.xpath(ID_selector)[0]
print(ID)

commands = tree.xpath(xpath_commands)
telemetry = tree.xpath(xpath_telemetry)
events = tree.xpath(xpath_events)

# print([etree.tostring(c) for c in commands])
# print([etree.tostring(c) for c in telemetry])
# print([etree.tostring(c) for c in events])
#
##
#
