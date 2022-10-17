#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Configurable settings for the IPC layer.

@author: Connor W. Colombo (CMU)
@last-updated: 07/03/2022
"""

settings = {
    'yamcs_username': 'iris',
    'yamcs_password': 'notapassword'  # set the creds during each session.
}


def set_yamcs_credentials(user, password):
    """
    Sets the YAMCS login credentials for this session.
    """
    settings['yamcs_username'] = user
    settings['yamcs_password'] = password

# *TODO: Make all these^ settable via cli args (opts?)
