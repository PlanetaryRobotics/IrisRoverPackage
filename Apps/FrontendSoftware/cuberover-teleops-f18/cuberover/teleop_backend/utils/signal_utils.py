import functools
import multiprocessing
import signal


class SignalObject:
    MAX_TERMINATE_CALLED = 3

    def __init__(self, shutdown_event):
        self.terminate_called = 0
        self.shutdown_event = shutdown_event

    def interrupted(self):
        self.terminate_called += 1
        self.shutdown_event.set()
        if self.terminate_called >= self.MAX_TERMINATE_CALLED:
            raise KeyboardInterrupt()


def default_signal_handler(signal_object: SignalObject, signal, frame):
    signal_object.interrupted()


def __setup_signal_handler(signal_num, signal_object, handler_func):
    handler = functools.partial(handler_func, signal_object)
    signal.signal(signal_num, handler)
    siginterrupt = getattr(signal, "siginterrupt", None)
    if siginterrupt:
        siginterrupt(signal_num, False)


def setup_signal_handler_for_process(shutdown_event: multiprocessing.Event,
                                     int_handler=default_signal_handler,
                                     term_handler=default_signal_handler):
    signal_object = SignalObject(shutdown_event=shutdown_event)
    __setup_signal_handler(signal.SIGINT, signal_object, int_handler)
    __setup_signal_handler(signal.SIGTERM, signal_object, term_handler)
