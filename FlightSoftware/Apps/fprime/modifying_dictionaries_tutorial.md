# Tutorial for adding new data to F Prime dictionaries

> :warning: **This is a walk-through of the process taken to add an attribute to a command argument. This is a non-trivial process, and adding data that takes a form other than a command argument attribute will have different details (though the general process will hopefully be the same).**

### Motivation
The goal of this process was to add a `unit` attribute to the definition of an F Prime command argument, and have this `unit` trickle through the autocoding process to end up in the Python and JSON dictionaries that can be generated from F Prime XML files.

### Contents
This tutorial will be set up as a walk-through of the changes I needed to make in order to accomplish the goal described above. I will also explain how I determined what changes to make, so that in the future others will be able to use this methodology to make other modifications.

### Process
My first step was honestly digging around in the `fprime/Autocoders` directory and looking for things related to the XML documents and the process of autocoding the dictionaries. The first thing I found was the XML schemas that define the legal structure of the XML files used by F Prime.

##### Schemas
I found the file `fprime/Autocoders/Python/schema/default/command_schema.rng`, which contained the following (some parts snipped for clarity:
```
<?xml version="1.0" encoding="UTF-8"?>
<grammar
    xmlns="http://relaxng.org/ns/structure/1.0"
    xmlns:a="http://relaxng.org/ns/compatibility/annotations/1.0"
    datatypeLibrary="http://www.w3.org/2001/XMLSchema-datatypes">

    <include href="common_elements.rng"/>
    <include href="common_types.rng"/>

    <start>
       <ref name = "command_root_define"></ref>
    </start>

    <define name = "command_root_define">
        <element name = "commands">
			[...]
            <oneOrMore>
                <element name = "command">
                    <!-- Required Attributes -->
					[...]
                    <!-- Optional Attributes -->
					[...]
                    <a:documentation>Defines optional comments and arguments within commands.</a:documentation>
                    <interleave>
                        [...]                       
                        <optional>
                            <ref name = "args_define"></ref>
                        </optional>
                    </interleave>
                </element>
            </oneOrMore>
        </element>
    </define>

    <define name = "args_define">
        <element name = "args">
            <a:documentation>Command arguments.</a:documentation>
            <zeroOrMore>
                <ref name = "external_arg_define"></ref>
            </zeroOrMore>
        </element>
    </define>
</grammar>
```
This specifies a `commands` element that must contain one or more `command` elements, and each `command` element has an optional element referenced as `args_define`. `args_define` is defined later down in the file as an `args` element that contains zero or more elements referenced as `external_arg_define`. `external_arg_define` is defined in the file `fprime/Autocoders/Python/schema/default/common_elements.rng`, which is included at the top of `command_schema.rng`. In `common_elements.rng` we find the following:
```
<define name = "external_arg_define">
        <a:documentation>Arguments without the pass by attributes. Used for events, commands, and telemetry.</a:documentation>
        <element name = "arg">

            <!-- Required Attributes -->

            <attribute name = "name">
                <a:documentation>Name of the argument.</a:documentation>
                <text/>
            </attribute>

            <!-- Optional Attributes -->

            <optional>
                <attribute name = "comment">
                    <a:documentation>Comments about the argument.</a:documentation>
                    <data type="string"></data>
                </attribute>
            </optional>

            <!-- The interleave is used to mix together Attribute typed paired with an enum element if type = ENUM and a comment block -->
            <interleave>
                <ref name = "type_size_choice_define"></ref>

                <optional>
                    <ref name = "comment_define"></ref>
                </optional>

            </interleave>
        </element>
    </define>
``` 
I modified this by adding an optional attribute named "unit" that accepts a string:
```
<optional>
    <attribute name = "unit">
        <a:documentation>The unit of the argument.</a:documentation>
        <data type="string"></data>
    </attribute>
</optional>
```
With this, I was done with the schemas. I next looked for where the schemas were being used, and found the  XML parsers that live in the `fprime/Autocoders/Python/src/fprime_ac/parsers` directory.

##### Parsers
Since commands are defined in components, I looked in `XmlComponentParser.py`, and I found that it indeed was responsible for parsing the command arguments. Specifically, I found that at around line 300 it parsed the command arguments into a `CommandArg` object:
```
for arg in command_tag:
	if arg.tag != 'arg':
	    PRINT.info("%s: Invalid tag %s in command args definition"%(xml_file,arg.tag))
	       sys.exit(-1)
	n = arg.attrib['name']
	t = arg.attrib['type']
	if t == 'string':
	    s = arg.attrib['size']
	else:
	    s = None
	command_arg_obj = CommandArg(n,t,s)
```
I also found that `CommandArg` was defined towards the bottom of the same file:
```
class CommandArg(object):
    """
    Data container for an command argument
    """
    def __init__(self,name,type,size=None,comment=None):
        self.__name = name
        self.__type = type
        self.__size = size
        self.__comment = comment

    def get_name(self):
        return self.__name
    def get_type(self):
        return self.__type
    def get_size(self):
        return self.__size
    def set_type(self, type):
        self.__type = type
    def get_comment(self):
        return self.__comment
    def set_comment(self, comment):
        self.__comment = comment
```
I first added a field for the unit string to the `CommandArg` data container class, as well as a constructor argument and a pair of setter/getter functions:
```
class CommandArg(object):
    """
    Data container for an command argument
    """
    def __init__(self,name,type,size=None,comment=None,unit=None):
        self.__name = name
        self.__type = type
        self.__size = size
        self.__comment = comment
        self.__unit = unit

    def get_name(self):
        return self.__name
    def get_type(self):
        return self.__type
    def get_size(self):
        return self.__size
    def set_type(self, type):
        self.__type = type
    def get_comment(self):
        return self.__comment
    def set_comment(self, comment):
        self.__comment = comment
    def get_unit(self):
        return self.__unit
    def set_unit(self, unit):
        self.__unit = unit
```
Then, I went back to the code that was constructing the `CommandArg` and modified it to read the `unit` attribute from the argument (if it was given), then pass that value (or `None` if it wasn't defined) to the constructor:
```
for arg in command_tag:
	if arg.tag != 'arg':
	    PRINT.info("%s: Invalid tag %s in command args definition"%(xml_file,arg.tag))
	       sys.exit(-1)
	n = arg.attrib['name']
	t = arg.attrib['type']
	if t == 'string':
	    s = arg.attrib['size']
	else:
	    s = None
	u = arg.attrib.get('unit', None)
	command_arg_obj = CommandArg(n,t,s,unit=u)
```
I didn't find any other command argument parsing in the other parser files, so I next looked for what actually called into the `XMLComponentParser`. I ended up finding that `fprime/Autocoders/Python/bin/codegen.py` was the entrypoint into all of this autocoding logic, as well as the code that called into the parsers.

##### codegen
I searched `codegen.py` for where it used the `CommandArg` objects, and found that this occurred starting at around line 386:
```
for arg in command.get_args():
	arg_elem = etree.Element("arg")
	arg_elem.attrib["name"] = arg.get_name()
	arg_type = arg.get_type()
	if type(arg_type) == type(tuple()):
	    enum_value = 0
	    type_name = "%s::%s::%s"%(comp_type,arg.get_name(),arg_type[0][1])
	    # Add enum entry
	    enum_elem = etree.Element("enum")
	    enum_elem.attrib["type"] = type_name
	    # Add enum members
	    for (membername,value,comment) in arg_type[1]:
	        enum_mem = etree.Element("item")
	        enum_mem.attrib["name"] = membername
	        # keep track of incrementing enum value
	        if value != None:
	            enum_value = int(value)

	        enum_mem.attrib["value"] = "%d"%enum_value
	        enum_value = enum_value + 1
	        if comment != None:
	            enum_mem.attrib["description"] = comment
	        enum_elem.append(enum_mem)
	    enum_list.append(enum_elem)
	else:
	    type_name = arg_type
	    if arg_type == "string":
	        arg_elem.attrib["len"] = arg.get_size()
	arg_elem.attrib["type"] = type_name
	arg_elem.attrib["unit"] = arg.get_unit()
	args_elem.append(arg_elem)
```
Reading into this code, it is constructing a dictionary from the different elements in `CommandArg`. Therefore, all I needed to do was make sure that the unit field was added to that dictionary, which I did just before the last line of the previous code block:
```
arg_elem.attrib["unit"] = arg.get_unit()
args_elem.append(arg_elem)
```
Next, I tried to figure out how this was used to actually generate Python dictionaries. However, I really struggled to find how this connected to the Python generation, so I stopped looking from the top down and instead starting working from the bottom up. While searching through the `Autocoders` directory earlier, I had found the directory `fprime/Autocoders/Python/src/fprime_ac/generators`, which contains Python template files, visitor classes, and classes to be visited. I started figuring things out from there.

##### Generators - Templates
First, I found `fprime/Autocoders/Python/src/fprime_ac/generators/templates/commands/CommandBody.tmpl`, which contains a template for Python code representing a command. Of note to me in the template was this section, which listed the arguments:
```
ARGUMENTS = [
#for $argname,$arg_description,$type in $arglist:
    ("${argname}","${arg_description}",${type}),
#end for
    ]
```
I didn't recognize the syntax, but clearly it is some for loop over values in a list of arguments, and then each iteration is adding a tuple to the list of arguments. I confirmed this by looking at the code generated without any modification, which for `CMD_TEST_CMD_1`,  defined in `fprime/Svc/CmdDispatcher/CommandDisatcherComponentAi.xml`, looked like this:
```
ARGUMENTS = [
    ("arg1","The I32 command argument",I32Type()),
    ("arg2","The F32 command argument",F32Type()"),
    ("arg3","The U8 command argument",U8Type()),
    ]
```
Mimicking the existing syntax, I added my unit attribute to the tuple in the template as follows: 
```
ARGUMENTS = [
#for $argname,$arg_description,$type,$unit in $arglist:
    ("${argname}","${arg_description}",${type},"${unit}"),
#end for
    ]
```
Then, I looked for the code that filled in this template. 
##### Generators - Visitors
I found `fprime/Autocoders/Python/src/fprime_ac/generators/visitors/CommandVisitor.py`, and I found that it was populating a `c.arglist` in the `DictBodyVisit` function, around like 233-238:
```
for arg_obj in obj.get_args():
	# convert XML types to Python classes                    
	(type_string,ser_import,dontcare,dontcare2) = DictTypeConverter.DictTypeConverter().convert(arg_obj.get_type(),arg_obj.get_size())
	if ser_import != None:
	    c.ser_import_list.append(ser_import)
	c.arglist.append((arg_obj.get_name(),arg_obj.get_comment(),type_string))
```
I added the unit to the tuple added to `c.arglist` on the last line, so it became:
```
c.arglist.append((arg_obj.get_name(),arg_obj.get_comment(),type_string,arg_obj.get_unit()))
```
At this point I thought I was done and tried to rebuild the dictionaries. So far I've only been able to rebuild them by doing a full clean then a full build (from `fprime/CubeRover`, run `make TIR4_clean && make TIR4`). However I found that I was getting an error that passed through `InstanceCommandVisitor.py` saying it needed more than 3 values to unpack. Looking into `InstanceCommandVisitor.py` I found that it also contained a section of code exactly like the one above in `CommandVisitor.py`, so I made the same modification to `InstanceCommandVisitor.py` that I did in `CommandVisitor.py`. I then tried re-running. I ended up getting a different error from `InstanceCommandVisitor.py`, complaining that the object `Arg` doesn't have a function `get_unit()`. I wanted to see exactly what class we were working with, so I added some [debugging code](https://stackoverflow.com/a/2020083) to `InstanceCommandVisitor.py` to determine the fully-qualified name of the `Arg` class that the error was complaining about. I found that the fully-qualified name was `fprime_ac.models.Arg.Arg`.

This pointed me to the `fprime/Autocoders/Python/src/fprime_ac/models/Arg.py` file, which contained the `Arg` class. I modified the `Arg` class just like I modified the `CommandArg` class in `XmlComponentParser.py` (minus adding the setter function), so I won't include that code here. However, I still wasn't sure what was constructing the `Arg` objects, so I was certain it wouldn't be passing the `unit` argument to the `Arg` constructor. To find what was constructing `Arg` I added some debugging code to the constructor of `Arg` that prints a stack trace when it is called:
```
import sys
import traceback
traceback.print_tb(file=sys.stdout)
```
This stack trace told me that the `Arg` class was being constructed by `fprime/Autocoders/Python/src/fprime_ac/models/CompFactory.py`, which was called from the `generate_component_instance_dictionary` function in `codegen.py` -- there's our missing link! I modified `CompFactory.py` from
```
for a in command_obj.get_args():
    name    = a.get_name()
    atype   = a.get_type()
    comment = a.get_comment()
    size    = a.get_size()
    arg_obj_list.append(Arg.Arg(name, atype, None, size, comment))
```
to 
```
for a in command_obj.get_args():
    name    = a.get_name()
    atype   = a.get_type()
    comment = a.get_comment()
    size    = a.get_size()
    unit    = a.get_unit()
    arg_obj_list.append(Arg.Arg(name, atype, None, size, comment, unit=unit))
```
Then I rebuilt once more, and I didn't get any more errors!

##### Python generation confirmation
I added some units to the `CMD_TEST_CMD_1` command arguments,  defined in `fprime/Svc/CmdDispatcher/CommandDisatcherComponentAi.xml`, so they looked like this:
```
<args>
	<arg name="arg1" type="I32" unit="degreesF">
        <comment>The I32 command argument</comment>
    </arg>
    <arg name="arg2" type="F32" unit="m/s">
        <comment>The F32 command argument</comment>
    </arg>
    <arg name="arg3" type="U8">
        <comment>The U8 command argument</comment>
    </arg>
</args>
```
Rebuilding once more, I finally see that the units are in the generated python command file (which was generated to `fprime/CubeRover/py_dict/commands/CMD_TEST_CMD_1.py`):
```
ARGUMENTS = [
    ("arg1","The I32 command argument",I32Type(),"degreesF"),
    ("arg2","The F32 command argument",F32Type(),"m/s"),
    ("arg3","The U8 command argument",U8Type(),""),
    ]
```

##### JSON cictionary generation
The JSON dictionary generation doesn't happen in `codegen.py`, but instead in `fprime/Autocoders/Python/bin/JSONDictionaryGen.py`. I found that this was creating a dictionary representing a command at around line 210:
```
arguments = []
for arg in command.get_args():
    typeItem = arg.get_type()
    typeObj = format_type_item(typeItem)
    arguments.append(typeObj)

metadata = {
    "id": opcode,
    "name": name,
    "instance": comp_name,
    "description": command.get_comment(),
    "component": component,
    "arguments" : arguments
}
```
This constructed a list of just the type of each argument. Rather then messing with the `arguments` key/value pair in the output dictionary, I decided to simply add a new key/value pair with the information I wanted. I added a key named `arguments_full_details` that has a list of tuples (one for each argument), where each tuple contains the argument name, type, and unit:
```
arguments = []
for arg in command.get_args():
    typeItem = arg.get_type()
    typeObj = format_type_item(typeItem)
    arguments.append(typeObj)

argument_full_details = []
for arg in command.get_args():
    arg_details = {}
    arg_details["name"] = arg.get_name()
    arg_details["unit"] = arg.get_unit()
    arg_details["type"] = format_type_item(arg.get_type())

metadata = {
    "id": opcode,
    "name": name,
    "instance": comp_name,
    "description": command.get_comment(),
    "component": component,
    "arguments" : arguments,
    "arguments_full_details": argument_full_details
}
```
We can then run the JSON dictionary generator from the `fprime` directory with the following command:
```
Autocoders/Python/bin/run_JSONDict.sh CubeRover/Top/CubeRoverTopologyAppAi.xml
```
This creates a file named `CubeRoverDictionary.json` in the `fprime` directory, and looking in this file we see that the new key/value pair are there as expected. In the the `CubeRover:commands` object we find the following:
```
"203": {
	"arguments_full_details": [
	    {
	        "type": "I32", 
	        "name": "arg1", 
	        "unit": "degreesF"
	    }, 
	    {
	        "type": "F32", 
	        "name": "arg2", 
	        "unit": "m/s"
	    }, 
	    {
	        "type": "U8", 
	        "name": "arg3", 
	        "unit": null
	    }
	], 
	"description": "No-op command", 
	"component": "Svc::CommandDispatcher", 
	"instance": "cmdDispatcher", 
	"arguments": [
	    "I32", 
	    "F32", 
	    "U8"
	], 
	"id": 203, 
	"name": "CMD_TEST_CMD_1"
	}
```
And finally, we've accomplished our goal.

### Summary
1. In `fprime/Autocoders/Python/schema/default/common_elements.rng`:
    - Modify `external_arg_define` (line 248) by adding a new optional attribute named `unit` with documentation describing what it is and with data type = "string"
2. In `fprime/Autocoders/Python/src/fprime_ac/parsers/XmlComponentParser.py`:
    - Modify `CommandArg` data container class (line 1122) to add `__unit` member and getter/setter for it
    - Modify command argument parsing (starting line 305) to parse an optional `unit` field from the XML etree, then pass it to the constructor of the `CommandArg` to set the unit (if there was one defined)
3. In `fprime/Autocoders/Python/bin/codegen.py`:
    - Modify XML dictionary command detail generation to add "unit" key/value to `arg_elem` dictionary (about line 414)
4. In `fprime/Autocoders/Python/src/fprime_ac/models/Arg.py`:
    - Add a `__unit` member, `unit` constructor param, and `get_unit` getter to the class `Arg`
5. In `fprime/Autocoders/Python/src/fprime_ac/models/CompFactory.py`
    - On line 155-156, get the unit from the `arg` object and include it in the `Arg.Arg` constructor call	  
6. In `fprime/Autocoders/Python/src/fprime_ac/generators/templates/commands/CommandBody.tmpl`:
    - Add location for `unit` field in python template in `ARGUMENTS` list of tuples, and add it to the list of items iterated over in for loop
7. 	In `fprime/Autocoders/Python/src/fprime_ac/generators/visitors/CommandVisitor.py`:
    - Line 238: Add the unit to the tuple appended to `c.arglist`
8. 	In `fprime/Autocoders/Python/src/fprime_ac/generators/visitors/InstanceCommandVisitor.py`:
    - Line 241: Add the unit to the tuple appended to `c.arglist`
9. 	In fprime/Autocoders/Python/bin/JSONDictionaryGen.py:
    - Line ~210: Add the argument details to the JSON dictionary. Rather than modifying the 
	  existing `arguments` list that was printed out (which only contained the data type of each arg),
	  I decided to add a new element to the output `metadata` for each command called  `arguments_full_details`, which all of the details of the argument that the frontend will want.