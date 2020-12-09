# -*- coding: utf-8 -*-

def print_bytearray_hex(ba):
    print(" ".join(["{:02x}".format(x) for x in ba]))