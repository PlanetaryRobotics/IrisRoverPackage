# F-Prime Primer

Notes about F-Prime (and some FreeRTOS, TI Hercules SoC, and ARM) by Justin N. with some additional notes by Connor C.

## Generally Helpful Resources

F-Prime user guide and source

Each section may also reference helpful documents and readings.

Also, all (or at least, most) FPrime-provided components contain a `./docs/sdd.md` file. In the bigger ones like `ActiveLogger` or `CommandDispatcher`, these include detailed information about the design, use, and configuration of these components, which is quite helpful, especially when modifying their `XXXCfg.hpp` files (where present).

Lastly, be aware that a lot of the functions of core FPrime are configured in `fprime/Fw/Cfg/`.

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

### `Fw::TlmPacket` serialization is as follows: 

1. `FW_PACKET_TELEM` aka packet type (`enum ComPacket::ComPacketType` aka int)
2. Id                                (`FwChanIdType` aka `U16`, default #defined in `Fw/Cfg/Config.hpp`, overridden in `CubeRoverFPrimeConfig.hpp`)
3. Time                              (`Fw::Time` see below)
4. Buffer                            (Variable based on telemetry value type)
Actual serialization process occurs progressively in `TlmPacket::serialize` in `Fw/Tlm/TlmPacket.cpp` (each `buffer.serialize` call in there adds data).


### `Fw::LogPacket` serialization is as follows: 
NOTE: Log serialization process very much parallels the telemetry process.

1. `FW_PACKET_LOG` aka packet type (`enum ComPacket::ComPacketType` aka int)
2. Id                                (`FwEventIdType` aka `U16`, default #defined in `Fw/Cfg/Config.hpp`, overridden in `CubeRoverFPrimeConfig.hpp`)
3. Time                              (`Fw::Time` see below)
4. Buffer                            (`Fw::LogBuffer` of serialized event arguments. `Fw::LogPacket` just takes a `U8*` to these and the actual serialization occurs in each autocoded `log_SEVERITY_LEVEL_eventName` handler in each `ComponentNameAc.cpp` file. Since we don't have `AMPCS` on, this is just all arguments, serialized in order, one after the other, packed together (like the arguments in a command).)

Actual serialization process occurs progressively in `LogPacket::serialize` in `Fw/Log/LogPacket.cpp` (each `buffer.serialize` call in there adds data). Note, we don't need to encode things like `severity` because that can be extracted from the message definitions using the `Id` as a lookup key.

Packet type, and time are only serialized if `FW_AMPCS_COMPATIBLE` is **false**.

### Fw::Time serialization:

1. Time base    (`enum TimeBase (aka int) in `Fw/Cfg/Config.hpp`)
2. Time context (`FwTimeContextStoreType` aka U8, default #defined in `Fw/Cfg/Config.hpp`)
3. Seconds      (`U32`)
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
- If the component has <events>, its `Log` port is connected to ActiveLogger `logRecv` port of type `Log`

## Command Uplink Format

Commands are treated as binary data (ComPacket) until they are received by the Command Dispatcher.
Command Packet deserializes the command from the ComPacket following this struct:

struct command {
    FwPacketDescriptorType magic;
    FwOpcodeType opcode;
    variable_length argument;
} __attribute__ ((packed));

## Bitfield Standards
An aside on how Ground Software expects the standards for bitfields to be defined. Reach out to Connor Colombo (`colombo@cmu.edu`, `@connor_colombo`) with any questions.

### Specifics
Each **component** which expects a bitfield as a command argument (eg. `Configure_Camera0` command) or telemetry which returns a bitfield must include the structs which define how those bitfields are structured in a file in the component's directory (eg. in `fprime/CubeRover/Camera` for the Camera component). If not such file is present, all it will be assumed no bitfields are required and all command arguments will be populated using their raw types (eg. `U64`).

### Requirements and Naming Conventions:
- All bitfields for a component must be contained in a single file called `{ComponentName}_Bitfields.hpp` which contains only structs in the component's directory. Eg: `fprime/CubeRover/Camera/Camera_Bitfields.hpp` for the Camera component.
- All structs should be packed.
- All structs should be named `Command_{mnemonic}_Arg_{name}` where `mnemonic` is the command's mnemonic from the defining XML file and `name` is the argument's name from the XML file.

**Note:** The expected fields for all bitfield structs are listed in C&TL but the official source of truth shared by both FSW and GSW will be these bitfield definition files; so, name your struct fields reasonably since these will be the names that show up on the frontend (though reformatted to be human readable).

### Example
This example should clarify all bitfield requirements

The modified and abbreviated `Camera` component shown below has two commands `Configure_Camera0` and `Camera0_Crop` which require bitfield arguments. Each of these commands has one bitfield argument called `config`, though `Configure_Camera0` has two arguments in total. This is all defined in the XML file `fprime/CubeRover/Camera/CameraComponentAi.xml` as follows.
```
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
```
...
# include guards, imports, etc.
# but no non-struct Cpp code in this file
...
struct Command_Configure_Camera0_Arg_config {
    uint8_t compression : 2;
    uint32_t shutter_width : 20;
    uint16_t shutter_delay : 13;
    uint8_t row_bin : 2;
    uint8_t col_bin : 2;
    uint16_t horiz_blanking : 12;
    uint16_t vert_blanking : 11;
    uint8_t reserved : 2; // pad up to U64 (arg type)
} __attribute__((packed));

struct Command_Camera0_Crop_Arg_config {
    uint32_t upperLeftX : 12;
    uint32_t upperLeftY : 11;
    uint32_t height : 12;
    uint32_t width : 11;
    uint32_t reserved : 18; // pad up to U64 (arg type)
} __attribute__((packed));
...
```

## Component Initializations and Interrupts on FreeRTOS

When writing constructors and initializers for FPrime components, these methods cannot rely on interrupts when
running in the context of prior to the scheduler running.
See [FreeRTOS FAQ (#4)](https://www.freertos.org/FAQHelp.html), "If a FreeRTOS API function is called before the scheduler has been started then interrupts will deliberately be left disabled, and not re-enable again until the first task starts to execute. This is done to protect the system from crashes caused by interrupts attempting to use FreeRTOS API functions during system initialisation, before the scheduler has been started, and while the scheduler may be in an inconsistent state."

## ARM MPU

TI Hercules RM46x TRM (SPNUC514C)
[TI Hercules Docs (has a really good FAQ)](https://software-dl.ti.com/hercules/hercules_docs/latest/hercules/index.html)
TI Hercules ARM MPU Subregion Usage Application Report (SPNA120)

ARM implements Memory Protection Unit (MPU) which works with the L1 memory controller to enforce access to and from L1 and external memory.
Access is granted based on current operating mode (CPSR reg: see Cortex R4 TRM Table 3-3)

There are 12 regions implemented on RM46 of increasing priority, where Region 1 is typically used for the Background region


Region 1: Background
0x00000000 - 0xffffffff (4GB)   Subregions 0 - 7 disabled
Normal Cacched Unshared
No Priviledge or user access no execution

Region 2: Flash
0x00000000 - 0x003fffff (4M) 
Normal cached and unshared
READ ONLY and excute

Region 3: RAM
0x08000000 - 0x0803ffff (256K)
normal chaced and unshared
read/write and execute

Region 4: ECC RAM
0x08000000 - 0x0803ffff (256K)
normal cached and unshared
read/write and execute

Region 5: CS2 
0x60000000 - 0x63ffffff (64MB)  Subregions 6 - 7 disabled
strongly ordered shareable
read/write execute

Region 6: SDRAM EMIF 64MB (CS0)
0x80000000 - 0x87ffffff (128MB)
strongly ordered shareable
read/write execute

Region 7:  Flash Wrapper Bus 2 (Flash ECC, OTP, and EEPROM)
0xf0000000 - 0xf07fffff (8MB)
Normal cached unshared
Priviledge rwad-write user read only no exec

Region 8: Peripherals frame - 2
0xfc000000 - 0xfcffffff (16MB)
Device unshared
Read/write no execute

Region 9: CRC (but only a small chunk??)
0xfe000000 - 0xfe0001ff (512B)
device unshared
read/write no exec

Region 10: Peripherals - Frame 1 & SYSTEM Modules
0xff000000 - 0xffffffff (16MB)
device unshared
read/write no execute
