import functools
import multiprocessing
import signal
import types
import typing



class SignalObject:
    """An object used to support custom signal handling behavior.

    When this object is interrupted (which occurs when a SIGINT or SIGTERM signal is received), it increments the
    internal counter of this object. When interrupt is called even once, the shutdown event is immediately signalled,
    but no exception will be immediately thrown. However, when the internal counter becomes greater than or equal to
    MAX_TERMINATE_CALLED, then a KeyboardInterrupt is raised.

    The above behavior means that we can signal a gracious shutdown when interrupted once, but if the user Ctrl-C's
    multiple times then we'll except out by raising the KeyboardInterrupt.

    Attributes:
        MAX_TERMINATE_CALLED: A class variable specifying how many times interrupted() can be called before a
                              KeyboardInterrupt is raised.
        terminate_called: The count of the number of times interrupted() has been called.
        shutdown_event: The multiprocessing.Event that will be set if interrupted() is called even once.
    """

    MAX_TERMINATE_CALLED = 3

    def __init__(self, shutdown_event: multiprocessing.Event):
        """Constructor.

        Args:
            shutdown_event: The multiprocessing.Event that will be set if interrupted() is called even once.
        """
        self.terminate_called = 0
        self.shutdown_event = shutdown_event

    def interrupted(self):
        """Indicates that this object has been signaled.

        Returns:
            None

        Raises:
            KeyboardInterrupt: If this function has been called MAX_TERMINATE_CALLED or more times.
        """
        self.terminate_called += 1
        self.shutdown_event.set()
        if self.terminate_called >= self.MAX_TERMINATE_CALLED:
            raise KeyboardInterrupt()




def default_signal_handler(signal_object: SignalObject, signal: int, frame: typing.Optional[types.FrameType]) -> None:
    """The default function to call when a signal occurs, which calls interrupted() on the given SignalObject.

    Args:
        signal_object: The SignalObject to be triggered.
        signal: The number of the received signal.
        frame: The interrupted stack frame.
    """
    signal_object.interrupted()


SignalObjectSignalHandlerCallableType = typing.Callable[[SignalObject, int, typing.Optional[types.FrameType]], None]


def __setup_signal_handler(signal_num: int,
                           signal_object: SignalObject,
                           handler_func: SignalObjectSignalHandlerCallableType) -> None:
    """Installs a signal handler function that uses a SignalObject for the given signal number.

    Args:
        signal_num: The number of the signal for which the handler will be installed.
        signal_object: The SignalObject to be triggered when a signal is received.
        handler_func: The callable function to be called when a signal occurs. Specifically, the given signal_object is
                      bound to an instance of this callable, then the result of that bind is installed as the signal
                      handler function.

    Returns:
        None
    """
    handler = functools.partial(handler_func, signal_object)
    signal.signal(signal_num, handler)
    siginterrupt = getattr(signal, "siginterrupt", None)
    if siginterrupt:
        siginterrupt(signal_num, False)


def setup_signal_handler_for_process(shutdown_event: multiprocessing.Event,
                                     int_handler: SignalObjectSignalHandlerCallableType = default_signal_handler,
                                     term_handler: SignalObjectSignalHandlerCallableType = default_signal_handler):
    """

    Args:
        shutdown_event: The event to be set when either a SIGINT or SIGTERM signal is received.
        int_handler: The SignalObject-based signal handling callable to be invoked when a SIGINT is received.
        term_handler: The SignalObject-based signal handling callable to be invoked when a SIGTERM is received.
    """
    signal_object = SignalObject(shutdown_event=shutdown_event)
    __setup_signal_handler(signal.SIGINT, signal_object, int_handler)
    __setup_signal_handler(signal.SIGTERM, signal_object, term_handler)
