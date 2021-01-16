# F-Prime Primer

Notes about F-Prime by Justin N. **Read the F-Prime user guide and source for more information!**

## Differentiating Telemetry and Logs 

> 6.7.3 Telemetry A telemetry channel is intended to be used for periodically measure[d] data.
> `tlmWrite_<channel name>`

> 6.7.4 Events Events are intermittent and are all recorded to reconstruct a series of actions or events after the fact.
> `log_<severity>_<event name>`

At JPL logs are also commonly referred to as **EVR**s or Event Reports.

For these timestamped values it is important to note two commonly used additional acronyms in the industry:

- **ERT**: Earth Receive Time (Wall clock time when a packet from the rover is received)
- **SCET**: Spacecraft Event Time (Rover time when an event is generated)

## Component and Input Port Types

```
        | Owns thread  | Message |                    |
Type    | of execution | Queue   | Input Port Types   |
========|==============|=========|====================|
Passive |     No       |  No     | Sync,Guarded       |
Queued  |     No       |  Yes    | Async,Sync,Guarded |
Active  |     Yes      |  Yes    | Async,Sync,Guarded |
```

Passive components define the base level of functionality for all components. Queues extend
the passive component class by including a Queue for each async input. Active components 
extend queued components by including a `start(..)` method which starts a task for that
component. In FreeRTOS, this method only creates the thread, but does not start the
scheduler. These component base classes are defined in `Fw/Comp`.

### Guarded and Synchronous Inputs 

The autocoder generates the following call chain for synchronous and guarded inputs:

- 0:   Caller: `retType <output port name>_out(args)`                      (_ComponentAc.*_)
- 0.5: Callee: `retType m_p_<port name>_in(args)`                          (_ComponentAc.*_)
- 1:   Callee: `retType <port name>_handlerBase(args)`                     (_ComponentAc.*_)
- 1.5: Callee:    lock mutex if input port is guarded                      (_ComponentAc.cpp_)
- 2:   Callee: `retType <port name>_handler(args)`                         (_Impl.*_)
- 3:   Callee: User implemented behavior and returns retType               (_Impl.*_)
- 3.5: Callee:    unlock mutex if guarded input                            (_ComponentAc.cpp_)
- ... return up the call stack back to the caller

Note that we were on the caller's thread of execution the entire procedure.

### Async Inputs

For asynchronous inputs the autocoder generates methods which are called when initially
starting F-prime. In `constructApp()`, the `start(..)` method is called which creates the
task for that component. The task function pointer is an internal method to 
`ActiveComponentBase` called `s_baseTask`. This task has the loop function for the task
and defines the message for breaking out of the loop and destroying the task.

In this "event loop", the `doDispatch()` virtual method is called which is generated
by the autocoder in ComponentAc.* . The call chain for async inputs is as follows:

#### From the Caller 

- 0:  Caller: `void <output port name>_out(args)`                     (_ComponentAc.*_)
- 0.5:Callee: `void m_p_<port name>_in(args)`                         (_ComponentAc.*_)
- 1:  Callee: `void <port name>_handlerBase(args)`                    (_ComponentAc.*_)
- 2:  Callee:    `<port name>_preMsgHook(..)`                         (_ComponentAc.*_)
- 3:  Callee:    Serialize args and add to queue                      (_ComponentAc.cpp_)
- ... return up the call stack back to the caller

#### The Callee Event Loop

- Event Loop
    - 1. `doDispatch()`
    - 2. BLOCKING dequeue and deserialize args
    - 3. void `<port name>_handler(args)`
    -    User implemented behavior
    - loop

The blocking nature of the message queue ensures that if there is no messages to
parse, another task can pre-empt this task until there is something to do.

Queued tasks which have no thread of execution must call doDispatch() in its implmentation
to handle asynchronous messages.

### Examples

- `Svc/Health`: Queued component with async/sync inputs
- `Svc/ActiveRateGroup`: Active component with async/sync inputs
- `Svc/TlmChan`: Active component with guarded/async inputs

## Telemetry

For components that declare telemetry items, the respective ports to interface are
automatically generated. These ports are:

- `port="Tlm" type="Tlm"`
- `port="time" type="Time"`

and they shall be attached to a TlmChan component for proper storage and routing. The
time connection is optional, but neccessary for timestamps,

The telemetry item definitions take an id, name, and data type. The autocoded source will
implement function handles for writing these telemetry items automatically. 

`tlmWrite_<name>(<type>);`

When this method is called, a timestamp is taken and the telemetry item is serialized
into a `Fw::TlmBuffer` object. The if of the serialized telemetry item is the id of 
the telemetry channel plus the base_id of the component (the base id is defined in the
topology xml (generated into the `TopologyAppAc.cpp` file).

The TlmChan component has a "scheduled" run handler which will forward collected telemetry
out the PktSend port, one item per call, as a member allocated `Fw::ComBuffer`. The contents
is a serialized `Fw::TlmPacket` object containing the id, timestamp and value. Note that the
periodic task will only send fresh telemetry items as it iterates over all telemetry items.

`Fw::TlmPacket` serialization is as follows: 

1. `FW_PACKET_TELEM` aka packet type (`enum ComPacket::ComPacketType` aka int)
2. Id                                (`FwChanIdType` aka `U32`, default #defined in `Fw/Cfg/Config.hpp`)
3. Time                              (`Fw::Time` see below)
4. Buffer                            (Variable based on telemetry value type)

Packet type, and time are only serialized if `FW_AMPCS_COMPATIBLE` is **false**..

### Fw::Time serialization:

1. Time base    (`enum TimeBase (aka int) in `Fw/Cfg/Config.hpp`)
2. Time context (`FwTimeContextStoreType` aka U8, default #defined in `Fw/Cfg/Config.hpp`)
3. Seoncds      (`U32`)
4. uSeconds     (`U32`)

Note that time base and time context are only serialized if `FW_USE_TIME_BASE` or
`FW_USE_TIME_CONTEXT` are #defined respectively. By default they are enabled in
`Fw/Cfg/Config.hpp`.

## Topology Checklist

- Object is declared with unique base ID corresponding to the C&TL 
- If rate driven component, that component is connected to its respective rate group's `RateGroupMemberOut` with type `Svc::Sched`
- The component is connected to the CubeRoverTime `timeGetPort` of type `Time`
- The component is connected to TlmChan `TlmRecv` port of type `Tlm`
- The component is connected to CmdDispatcher `compCmdReg` port of type `CmdReg`
- The component is connected to CmdDispatcher `compCmdSend` port of type `Cmd`
- The component is connected to CmdDispatcher `compCmdStat` port of type `CmdResponse`
- TODO: Log connections


## Bitfield Standards
An aside on how Ground Software expects the standards for bitfields to be defined. Reach out to Connor Colombo (`colombo@cmu.edu`, `@connor_colombo`) with any questions.

### Specifics
Each **component** which expects a bitfield as a command argument (eg. `Configure_Camera0` command) or telemetry which returns a bitfield must include the structs which define how those bitfields are structured in a file in the component's directory (eg. in `fprime/CubeRover/Camera` for the Camera component). If not such file is present, all it will be assumed no bitfields are required and all command arguments will be populated using their raw types (eg. `U64`).

### Requirements and Naming Conventions:
- All bitfields for a component must be contained in a single file called `{ComponentName}_Bitfields.hpp` which contains only structs in the component's directory. Eg: `fprime/CubeRover/Camera/Camera_Bitfields.hpp` for the Camera component.
- All structs should be named `Command_{mnemonic}_Arg_{name}` where `mnemonic` is the command's mnemonic from the defining XML file and `name` is the argument's name from the XML file.
- All structs should be packed.

**Note:** The expected fields for all bitfield structs are listed in C&TL but the official source of truth shared by both FSW and GSW will be these bitfield definition files; so, name your struct fields reasonably since these will be the names that show up on the frontend (though reformatted to be human readable).

### Important Notes on Bitfield Packing:

#### Requirements:
Below are the base requirements when writing a bitfield struct to ensure consistency between GSW and FSW. If you're looking for the rationale, see the next section for all platform-specific details of the handling of packed structs containing bitfields.
- Declare the signing of each struct field (especially if signed). Don't just use **int**.
- **No floats** or doubles inside bitfield structs.
- **No 64b types** inside bitfield structs (no `long long` or `double`).
- Ensure the total size of the struct is an **integer multiple of the size of a storage-unit** (4-bytes), eg. 32b, 64b, 96b, etc.
- **Use padding bits at the end** of the struct to fill out the struct so that it contains the same number of bits as an integer multiple of the storage-unit (see above).
- 

The specifics for the layout of bitfield structures in memory is highly platform and compiler specific. As such, GSW will be sure to pack data in accordance with the FSW compiler. For Hercules, TI CCS uses the `TI ARM Optimizing C/C++ Compiler` and has the `--unaligned_access` flag on and supports the `__packed__` attribute. The implications of this are that for a packed structure:
From **ARM Optimizing C/C++ Compiler
v18.1.0.LTS User Guide (TI SPNU151R)**:
- Additional bytes of padding usually added to preserve word-alignment are omitted (one byte sequence begins right after the previous). That is members are byte-aligned, not word-aligned.
- Bitfields inside a packed structure are **bitaligned** and **there are no bits of padding between adjacent bitfields**. However, non-bitfield members will still start on the next byte.
- Packed structures in an array are packed together without and trailing padding between them.

- Bitfields can range from 1 to 32 bits.
- Multiple bitfields can be stored within the same byte.
- **BUT** Bitfields can **never span across a 4-byte storage-unit boundary**.
- Storage unit counting starts at the beginning of a struct *unless* the struct is __packed__ and stored in an array of other __packed__ structs, in which case it's possible for a struct to not start at the beginning of a storage-unit if the previous struct in the array didn't end at the end of a storage unit boundary.

- In little-endian mode (Hercules), bitfields are packed into registers from **LSBit to MSBit in the order they're defined**.
- In little-endian mode (Hercules), bitfields are packed into memory from LSbyte to MSbyte.
- **Always indicate signing**. In a bitfield just using `int` will be interpreted as `unsigned int` unless you explictly state `signed int`


From **Arm Compiler armcc User Guide**:
- Bits fill so that the first field occupies the lowest-addressed bits. Remember: Hercules is little-endian (lowest-addressed = least-significant).

- Bitfields fill up word-sized containers and only fill inside the same container if there's enough space.
- Bitfields will not span from one partially-filled word-sized container to another.
- ... but bitfields may span from one partially-filled byte to another.

- Bitfields are stored inside 'containers'.
- A bitfield must be wholly stored inside its container and not span between containers.
- A bitfield that does not fit in a container is placed in the next container **of the same type**.

This information comes from the [ARM Optimizing C/C++ Compiler
v18.1.0.LTS User Guide (TI SPNU151R)](https://www.ti.com/lit/ug/spnu151r/spnu151r.pdf) (Section **5.16.4 Type Attributes** and Section **6.2.2 Bit Fields**) and the [Arm Compiler armcc User Guide section on Structures, unions, enumerations, and bitfields in ARM C and C++](https://developer.arm.com/documentation/dui0472/m/c-and-c---implementation-details/structures--unions--enumerations--and-bitfields-in-arm-c-and-c--) to clear up any ambiguities (though, there's not a 100% guarantee that TI didn't change any of this in their implementation; so, check any of these assumptions).

### Example
This example should clarify all bitfield requirements

The modified and abbreviated `Camera` component shown below has two commands `Configure_Camera0` and `Camera0_Crop` which require bitfield arguments. Each of these commands has one bitfield argument called `config`, though `Configure_Camera0` has two arguments in total. This is all defined in the XML file `fprime/CubeRover/Camera/CameraComponentAi.xml` as follows.
```xml
<component name="Camera" kind="passive" namespace="CubeRover">
    ...
    <commands>
        ...
        <command kind="sync" opcode="0x3" mnemonic="Configure_Camera0">
            <comment>
                Configure camera 0
            </comment>
            <args>
                <arg name="somethingElse" type="U8">
                    <comment>
                        Some other normal non-bitfield U8 argument. This is here to illustrate why the arg must
                    </comment>
                </arg>
                <arg name="config" type="U64">
                    <comment>
                        Packed bitfield containing camera configurations. Defined in Camera bitfield definition file.
                    </comment>
                </arg>
            </args>
        </command>
        <command kind="sync" opcode="0x4" mnemonic="Camera0_Crop">
            <comment>
                Defines the crop for camera 0
            </comment>
            <args>
                <arg name="config" type="U64">
                    <comment>
                        Packed bitfield containing crop locations. Defined in Camera bitfield definition file.
                    </comment>
                </arg>
            </args>
        </command>
        ...
    </commands>
    ...
</component>

```

The two separate bitfield structs required for these two commands should then be defined in `fprime/CubeRover/Camera/Camera_Bitfields.hpp` and formatted as shown below.
```cpp
...
// include guards, includes, etc.
// but no non-struct Cpp code in this file
...
struct Command_Configure_Camera0_Arg_config {
    uint8_t compression : 2;
    uint8_t :6; // pad out the byte
    uint32_t shutter_width : 20;

    uint16_t shutter_delay : 13;
    uint8_t row_bin : 2;
    uint8_t col_bin : 2;
    uint16_t horiz_blanking : 12;
    uint16_t vert_blanking : 11;
    uint8_t reserved : 2; // pad up to U64 (arg type)
} __attribute__((__packed__));

struct Command_Camera0_Crop_Arg_config {
    uint32_t upperLeftX : 12;
    uint32_t upperLeftY : 11;
    uint32_t height : 12;
    uint32_t width : 11;
    uint32_t reserved : 18; // pad up to U64 (arg type)
} __attribute__((__packed__));
...
```
