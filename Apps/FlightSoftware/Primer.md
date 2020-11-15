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
