Module IrisBackendv3.codec.bgapi.custom_listener
================================================

Classes
-------

`BGApiListenerConnHandler(connection, event_handler, listening_to_host, *apis)`
:   This class handles threading when reading and writing to a connection,
    with reading performed as a bidirectional listener. All data received goes
    into the event queue (it will have non-homogeneous type).
    Any responses will go to the response queue, and any events will get
    dispatched to the event handler.
    
    This constructor should always be called with keyword arguments. Arguments are:
    
    *group* should be None; reserved for future extension when a ThreadGroup
    class is implemented.
    
    *target* is the callable object to be invoked by the run()
    method. Defaults to None, meaning nothing is called.
    
    *name* is the thread name. By default, a unique name is constructed of
    the form "Thread-N" where N is a small decimal number.
    
    *args* is the argument tuple for the target invocation. Defaults to ().
    
    *kwargs* is a dictionary of keyword arguments for the target
    invocation. Defaults to {}.
    
    If a subclass overrides the constructor, it must make sure to invoke
    the base class constructor (Thread.__init__()) before doing anything
    else to the thread.

    ### Ancestors (in MRO)

    * threading.Thread

    ### Class variables

    `READ_TIMEOUT`
    :

    `WRITE_TIMEOUT`
    :

    ### Methods

    `read(self, size=1)`
    :

    `run(self)`
    :   Method representing the thread's activity.
        
        You may override this method in a subclass. The standard run() method
        invokes the callable object passed to the object's constructor as the
        target argument, if any, with sequential and keyword arguments taken
        from the args and kwargs arguments, respectively.

    `send_command(self, bgcmd, response_timeout)`
    :

    `stop(self)`
    :

`BGLibListener(connection, apis, event_handler=None, response_timeout=1, log_id=None, listening_to_host: bool = False)`
:   Provides an interface to the Bluegiga binary APIs as a bidirectional
    listener.
    
    Populates the BGLib with the provided APIs
    
    The commands are grouped by the API name, defined in the API file. For
    example, if the API is the Silicon Labs Bluetooth API, it will appear
    as an attribute `bt` in the BGLib object:
    `>>> bglib.bt.system.hello()`
    
    help() is available for further details, e.g., help(bglib.bt.system)
    
    :param connection: Instance of a Connector subclass, e.g., bgapi.SerialConnector
    :param apis: A list of API file names or ParsedApi objects
    :param event_handler: A function which takes BGEvent objects, or None in
        which case events are placed in the event queue
    :param listening_to_host: True if reading from Host, False is reading
        from device (e.g. BGAPI). Default: `False`.

    ### Methods

    `close(self)`
    :

    `gen_events(self, timeout=0.0, max_events=None, max_time=None)`
    :   Generator retrieving events from the device.
        
        :param timeout: Timeout in seconds to wait for single event, or None for infinite
        :param max_events: Maximum number of events to receive, or None for no limit
        :param max_time: Maximum time receiving events, or None for no limit
        :return: Generator which generates BGEvent objects

    `get_event(self, timeout=0.0)`
    :   Get single event from the device.
        
        :param timeout: Timeout in seconds to wait for the event, or None for infinite
        :return: BGEvent object, or None if no event received

    `get_events(self, timeout=0.0, max_events=None, max_time=None)`
    :   Get events from the device.
        
        :param timeout: Timeout in seconds to wait for single event, or None for infinite
        :param max_events: Maximum number of events to receive, or None for no limit
        :param max_time: Maximum time receiving events, or None for no limit
        :return: List of BGEvent objects

    `is_open(self)`
    :

    `open(self, daemon: Optional[bool] = None)`
    :

    `set_keep_device_awake_function(self, function)`
    :   Set a function for keeping device awake
        
        The function should take one argument, which denotes if the device
        should be kept awake or not. If the value of the argument is zero,
        then the device doesn't need to be kept awake. Any other value means
        that the device should be kept awake.