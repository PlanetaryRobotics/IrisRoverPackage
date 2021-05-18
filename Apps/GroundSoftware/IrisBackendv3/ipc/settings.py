#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Configurable settings for the IPC layer.

@author: Connor W. Colombo (CMU)
@last-updated: 05/18/2020
"""

settings = {
    'IP': '127.0.0.1'  # localhost
}


def set_ip(ip: str):
    """
    Sets the IP for all IPC.
    """
    settings['IP'] = ip

# *TODO: Make all these^ settable via cli args (opts?)
