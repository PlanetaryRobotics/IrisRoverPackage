# Modification of `bglib.py` from `pybgapi` allowing bidirectional listening
# (listening for commands, command responses, and events).
# Copyright 2021 Silicon Laboratories Inc. www.silabs.com
#
# SPDX-License-Identifier: Zlib
#
# The licensor of this software is Silicon Laboratories Inc.
#
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.

import threading
try:
    import queue
except ImportError:
    import Queue as queue  # type: ignore
import time
import logging
from typing import Optional

from bgapi.apihelper import api_cmd_to_ascii, camelcase  # type: ignore
from bgapi.apiparser import ParsedApi  # type: ignore
from bgapi import serdeser  # type: ignore

from bgapi.connector import ConnectorTimeoutException  # type: ignore
from bgapi.bglib import *  # type: ignore


ERRORCODE_SUCCESS = 0

logger = logging.getLogger('bgapi')


class BGApiListenerConnHandler(threading.Thread):
    """This class handles threading when reading and writing to a connection,
    with reading performed as a bidirectional listener. All data received goes
    into the event queue (it will have non-homogeneous type).
    Any responses will go to the response queue, and any events will get
    dispatched to the event handler.
    """

    READ_TIMEOUT = 0.1
    WRITE_TIMEOUT = 1.0

    def __init__(self, connection, event_handler, listening_to_host, *apis):
        threading.Thread.__init__(self)
        self.daemon = True  # Daemon thread
        self.stop_flag = threading.Event()

        self.conn = connection
        self.event_handler = event_handler

        # True if reading from Host, False is reading from device (e.g. BGAPI)
        self.listening_to_host = listening_to_host

        self.ser = serdeser.Serializer(apis)
        self.deser = serdeser.Deserializer(apis)

        self.response_queue = queue.Queue(maxsize=1)
        self.waiting_response = threading.Event()

    def send_command(self, bgcmd, response_timeout):
        try:
            # Send command
            if not bgcmd.no_response:
                self.waiting_response.set()
            try:
                self.conn.write(self.ser.command(
                    bgcmd._device_name, bgcmd._class_name, bgcmd._msg_name, bgcmd))
            except ConnectorTimeoutException:
                raise CommandError("Send timeout")

            if not bgcmd.no_response:
                # Wait for response
                try:
                    api_node, headers, vals = self.response_queue.get(
                        timeout=response_timeout)
                    response = BGResponse(api_node, vals)

                    if response._errorcode != ERRORCODE_SUCCESS:
                        raise CommandFailedError(response=response)

                    return response
                except queue.Empty:
                    raise CommandError("No response")
        finally:
            self.waiting_response.clear()

    def read(self, size=1):
        data = bytearray()
        while len(data) < size:
            if self.stop_flag.is_set():
                return None
            data.extend(self.conn.read(size - len(data)))
        return bytes(data)

    def run(self):
        self.conn.set_read_timeout(self.READ_TIMEOUT)
        self.conn.set_write_timeout(self.WRITE_TIMEOUT)

        while not self.stop_flag.is_set():
            # Wait for the 1st byte of the header
            header_start = self.read(size=1)
            if header_start is None:
                # Stop flag set while reading header
                continue

            ####################################################################
            #                        WORKAROUND START
            ####################################################################
            # The transport layer currently doesn't guarantee an error-free
            # communication channel. The following code is an attempt to
            # eliminate junk data coming from the physical layer.
            # This code shall be removed once the transport layer is replaced
            # with a more robust one.
            ####################################################################
            # Check for valid header by checking the device type
            device_id = (header_start[0] & 0x78) >> 3
            if device_id not in self.ser.apis.keys():
                continue
            ####################################################################
            #                        WORKAROUND END
            ####################################################################

            # Wait for the rest of the header
            header_rest = self.read(size=serdeser.HEADER_LENGTH - 1)
            if header_rest is None:
                # Stop flag set while reading header
                continue

            header = header_start + header_rest
            cmdevt, device_id, payload_len, class_index, command_index = self.deser.parseHeader(
                header)

            # Wait for the payload
            payload = self.read(size=payload_len)
            if payload is None:
                # Stop flag set while reading payload
                continue

            (apicmdevt, headers, params) = self.deser.parse(
                header, payload, fromHost=self.listening_to_host)

            if cmdevt == serdeser.MSG_COMMAND:
                if self.listening_to_host:
                    # We're listening to the host, so this is a command sent by
                    # the host:
                    command = BGCommand(apicmdevt, params)
                    # So, just plop this in the event queue:
                    self.event_handler(command)
                else:
                    # We're acting as the host:
                    response = BGResponse(apicmdevt, params)
                    if self.waiting_response.is_set():
                        self.response_queue.put((apicmdevt, headers, params))
                    else:
                        # Received response, but not waiting for one. This is
                        # fine because we may not actually be the host.
                        logger.info(
                            "Received unexpected response '%s'", response)
                    # No matter what, plop this in the event queue:
                    self.event_handler(response)
            else:
                # Got event
                self.event_handler(BGEvent(apicmdevt, params))

    def stop(self):
        self.stop_flag.set()
        try:
            self.join()
        except RuntimeError:
            pass  # ignore error if trying to stop non-started thread


class BGLibListener(object):
    """Provides an interface to the Bluegiga binary APIs as a bidirectional
    listener."""

    def _handle_low_level_event(self, evt):
        logger.debug("{} < {}".format(self.log_id, evt))
        if self.event_handler:
            self.event_handler(evt)
        if not self.event_handler:
            self.event_queue.put(evt)

    def __init__(
        self,
        connection,
        apis,
        event_handler=None,
        response_timeout=1,
        log_id=None,
        listening_to_host: bool = False
    ) -> None:
        """Populates the BGLib with the provided APIs

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
        """
        if not isinstance(apis, (list, tuple)):
            apis = [apis]
        apis = [parse_api(api) for api in apis]  # type: ignore
        self.log_id = log_id or id(self)

        self.conn = connection
        self.event_handler = event_handler
        self.apis = apis
        self.response_timeout = response_timeout

        self.listening_to_host = listening_to_host

        self.conn.close()  # initially close the connection
        self.conn_handler: Optional[BGApiListenerConnHandler] = None
        self.event_queue: queue.Queue = queue.Queue()

        self.set_keep_device_awake_function(None)

        self.api_command_lock = threading.Lock()

        for api in apis:
            apidict = {}
            for class_name in api.names:
                classdict = {}
                # c = makeClass(class_name, api[class_name].description)
                for enum_group_name in api[class_name].enum_names:
                    for enum_name in api[class_name].enums[enum_group_name].names:
                        enum_val = api[class_name].enums[enum_group_name][enum_name]
                        classdict[enum_group_name.upper() + "_" +
                                  enum_val.name.upper()] = enum_val.value

                for def_group_name in api[class_name].define_names:
                    for def_name in api[class_name].defines[def_group_name].names:
                        def_val = api[class_name].defines[def_group_name][def_name]
                        classdict[def_group_name.upper() + "_" +
                                  def_val.name.upper()] = def_val.value

                for command_name in api[class_name].command_names:
                    cmd = self._createApiCommand(self, api.device_name, class_name,
                                                 command_name, api[class_name].commands[command_name])
                    classdict[command_name] = cmd
                classdict["__doc__"] = api[class_name].description
                api_class = type(class_name.capitalize(),
                                 (BGClass,), classdict)(class_name)  # type: ignore
                apidict[class_name] = api_class

            apidict["__doc__"] = api.description
            apidict["__version__"] = api.version
            setattr(self, api.name, type("%sApi" %
                    api.name.capitalize(), (object,), apidict)())

    def _createApiCommand(self, lib, device_name, class_name, command_name, apicmd):
        response_timeout = self.response_timeout

        def apiCommand(self, *args):
            if not lib.conn_handler:
                raise Exception("Connection is closed")

            lib.api_command_lock.acquire()
            lib._keep_device_awake(1)
            try:
                cmd = BGCommand(apicmd, args)
                logger.debug("{} > {}".format(lib.log_id, cmd))
                response = lib.conn_handler.send_command(
                    cmd, response_timeout=response_timeout)
                logger.debug("{} < {}".format(lib.log_id, response))
            except CommandError as e:
                logger.debug("{} ! {}".format(lib.log_id, e))
                raise
            finally:
                if not apicmd.no_return:
                    lib._keep_device_awake(0)
                lib.api_command_lock.release()

            if apicmd.no_return:
                return

            # Verify correct response
            if ((response._device_name, response._class_name, response._msg_name) !=
                    (cmd._device_name, cmd._class_name, cmd._msg_name)):
                raise CommandError("Wrong response")

            return response

        setattr(apiCommand, "__name__", camelcase(command_name))
        setattr(apiCommand, "_str", "{}_cmd_{}_{}".format(
            device_name, class_name, command_name))
        setattr(apiCommand, "__doc__", api_cmd_to_ascii(apicmd))

        return apiCommand

    def _keep_device_awake(self, keep_awake):
        if self._keep_device_awake_function:
            self._keep_device_awake_function(keep_awake)

    def set_keep_device_awake_function(self, function):
        """Set a function for keeping device awake

        The function should take one argument, which denotes if the device
        should be kept awake or not. If the value of the argument is zero,
        then the device doesn't need to be kept awake. Any other value means
        that the device should be kept awake.
        """
        self._keep_device_awake_function = function

    def is_open(self):
        return self.conn_handler is not None

    def open(self, daemon: Optional[bool] = None):
        if not self.is_open():
            self.conn.open()
            self._keep_device_awake(0)
            self.conn_handler = BGApiListenerConnHandler(
                self.conn,
                self._handle_low_level_event,
                self.listening_to_host, *self.apis
            )
            if daemon is not None:
                self.conn_handler.daemon = daemon
            self.conn_handler.start()

    def close(self):
        if self.is_open():
            self.conn_handler.stop()
            self.conn.close()
            self.conn_handler = None

    def get_event(self, timeout=0.0):
        """Get single event from the device.

        :param timeout: Timeout in seconds to wait for the event, or None for infinite
        :return: BGEvent object, or None if no event received
        """
        try:
            return self.event_queue.get(timeout=timeout)
        except queue.Empty:
            return None

    def get_events(self, timeout=0.0, max_events=None, max_time=None):
        """Get events from the device.

        :param timeout: Timeout in seconds to wait for single event, or None for infinite
        :param max_events: Maximum number of events to receive, or None for no limit
        :param max_time: Maximum time receiving events, or None for no limit
        :return: List of BGEvent objects
        """
        if timeout == None and max_events == None and max_time == None:
            raise Exception("One of the arguments has to be other than None")

        return list(self.gen_events(timeout=timeout, max_events=max_events, max_time=max_time))

    def gen_events(self, timeout=0.0, max_events=None, max_time=None):
        """Generator retrieving events from the device.

        :param timeout: Timeout in seconds to wait for single event, or None for infinite
        :param max_events: Maximum number of events to receive, or None for no limit
        :param max_time: Maximum time receiving events, or None for no limit
        :return: Generator which generates BGEvent objects
        """
        events = 0
        while (max_events == None or events < max_events) and \
              (max_time == None or max_time > 0):

            if max_time != None:
                start_time = time.time()
                if timeout == None or timeout > max_time:
                    timeout = max_time

            event = self.get_event(timeout)
            if event is None:
                break
            yield event
            events += 1

            if max_time:
                max_time -= time.time() - start_time
