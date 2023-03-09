#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Configurable settings for the IPC layer.

@author: Connor W. Colombo (CMU)
@last-updated: 02/27/2022
"""

settings = {
    'IP': '127.0.0.1',  # localhost
    # Key used to sign all pickled IPC messages.
    # All process running locally should be given the same session key.
    # (!!TODO: make this settable from console for all IPC apps).
    'SESSION_KEY': 'test-session-key',
    # App Name used in Logs:
    'app_name': 'IpcApp'
}


def set_ip(ip: str):
    """
    Sets the IP for all IPC.
    """
    settings['IP'] = ip

# *TODO: Make all these^ settable via cli args (opts?)
